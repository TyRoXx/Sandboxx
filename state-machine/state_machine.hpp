#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP


#include <array>

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
			: m_current_dtor(&destroy_impl<State>)
			, m_current(false)
		{
			construct<State>(m_storages[m_current],
							 std::forward<Args>(args)...);
		}

		template <class Entered, class ...Args>
		typename std::enable_if<contains<Entered, States...>::value, void>::type
		enter(Args &&...args)
		{
			bool const next = !m_current;
			construct<Entered>(m_storages[next], std::forward<Args>(args)...);
			destroy(m_storages[m_current]);
			m_current = next;
			m_current_dtor = &destroy_impl<Entered>;
		}

		~state_machine()
		{
			destroy(m_storages[m_current]);
		}

	private:

		storages m_storages;
		destructor m_current_dtor;
		bool m_current;


		template <class State, class ...Args>
		void construct(state_storage &storage, Args &&...args)
		{
			new (reinterpret_cast<State *>(&storage))
					State(std::forward<Args>(args)...);
		}

		void destroy(state_storage &storage)
		{
			m_current_dtor(&storage);
		}

		template <class Destroyed>
		static void destroy_impl(void *storage)
		{
			auto * const state = static_cast<Destroyed *>(storage);
			state->~Destroyed();
		}
	};
}


#endif
