#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP


#include <array>

#include <cassert>

#include <boost/mpl/list.hpp>
#include <boost/mpl/contains.hpp>


namespace sm
{
	template <bool Which, std::size_t First, std::size_t Second>
	struct select;

	template <std::size_t First, std::size_t Second>
	struct select<false, First, Second>
	{
		static constexpr std::size_t value = First;
	};

	template <std::size_t First, std::size_t Second>
	struct select<true, First, Second>
	{
		static constexpr std::size_t value = Second;
	};

	template <std::size_t First, std::size_t Second>
	struct max : select<(First < Second), First, Second>
	{
	};


	template <class ...Types>
	struct largest_sizeof;

	template <class First, class Second, class ...Tail>
	struct largest_sizeof<First, Second, Tail...> : max<sizeof(First), (largest_sizeof<Second, Tail...>::value)>
	{
	};

	template <class First>
	struct largest_sizeof<First>
	{
		static constexpr std::size_t value = sizeof(First);
	};


	template <class ...Types>
	struct largest_alignof;

	template <class First, class Second, class ...Tail>
	struct largest_alignof<First, Second, Tail...> : max<alignof(First), (largest_sizeof<Second, Tail...>::value)>
	{
	};

	template <class First>
	struct largest_alignof<First>
	{
		static constexpr std::size_t value = alignof(First);
	};


	template <class State>
	struct state
	{
	};

	template <class Element, class ...Sequence>
	struct contains;

	template <class Element, class Head>
	struct contains<Element, Head> : std::is_same<Element, Head>
	{
	};

	template <class Element, class First, class Second, class ...Tail>
	struct contains<Element, First, Second, Tail...>
	{
		static constexpr bool value =
				contains<Element, First>::value ||
				contains<Element, Second, Tail...>::value;
	};


	template <class Element, class ...Set>
	struct find;

	template <class Element, class ...Tail>
	struct find<Element, Element, Tail...> : std::integral_constant<size_t, 0>
	{
	};

	template <class Element, class Head, class ...Tail>
	struct find<Element, Head, Tail...>
	    : std::integral_constant<size_t, 1 + find<Element, Tail...>::value>
	{
	};


	template <class ...Types>
	struct nth_destructor;

	template <>
	struct nth_destructor<>
	{
		void operator ()(size_t n, char *object) const
		{
			(void)n;
			(void)object;
			assert(false);
		}
	};

	template <class Head, class ...Tail>
	struct nth_destructor<Head, Tail...>
	{
		void operator ()(size_t n, char *object) const
		{
			if (n == 0)
			{
				Head * const destructible = reinterpret_cast<Head *>(object);
				destructible->~Head();
			}
			else
			{
				nth_destructor<Tail...>()(n - 1, object);
			}
		}
	};


	template <class ...States>
	struct state_machine
	{
		typedef typename std::aligned_storage<
		    largest_sizeof<States...>::value,
		    largest_alignof<States...>::value
		>::type state_storage;
		typedef std::array<state_storage, 2> storages;
		typedef void (*destructor)(void *);


		template <class State, class ...Args>
		state_machine(state<State>, Args &&...args)
			: m_current_state(find<State, States...>::value)
			, m_current_storage(false)
		{
			construct<State>(m_storages[m_current_storage],
							 std::forward<Args>(args)...);
		}

		template <class Entered, class ...Args>
		typename std::enable_if<contains<Entered, States...>::value, void>::type
		enter(Args &&...args)
		{
			bool const next = !m_current_storage;
			construct<Entered>(m_storages[next], std::forward<Args>(args)...);
			destroy(m_storages[m_current_storage]);
			m_current_storage = next;
			m_current_state = find<Entered, States...>::value;
		}

		~state_machine()
		{
			destroy(m_storages[m_current_storage]);
		}

	private:

		storages m_storages;
		unsigned char m_current_state;
		bool m_current_storage;


		template <class State, class ...Args>
		void construct(state_storage &storage, Args &&...args)
		{
			new (reinterpret_cast<State *>(&storage))
					State(std::forward<Args>(args)...);
		}

		void destroy(state_storage &storage)
		{
			nth_destructor<States...>()(m_current_state,
			                            reinterpret_cast<char *>(&storage));
		}
	};
}


#endif
