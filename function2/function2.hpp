#ifndef FUNCTION2_HPP
#define FUNCTION2_HPP

#include <boost/atomic.hpp>
#include <cassert>
#include <boost/type_traits/decay.hpp>
#include <boost/intrusive_ptr.hpp>

namespace tx2
{
	struct thread_safe_ref_counter
	{
		thread_safe_ref_counter()
			: m_refs(0)
		{
		}

		void increment()
		{
			m_refs.fetch_add(1, boost::memory_order_relaxed);
		}

		bool decrement_is_zero_now()
		{
			if (m_refs.fetch_sub(1, boost::memory_order_release) == 1)
			{
				boost::atomic_thread_fence(boost::memory_order_acquire);
				return true;
			}
			return false;
		}

	private:

		boost::atomic<std::ptrdiff_t> m_refs;
	};

	struct single_thread_ref_counter
	{
		single_thread_ref_counter()
			: m_refs(0)
		{
		}

		void increment()
		{
			++m_refs;
		}

		bool decrement_is_zero_now()
		{
			assert(m_refs);
			--m_refs;
			return (m_refs == 0);
		}

	private:

		std::ptrdiff_t m_refs;
	};

	template <class Signature, class RefCounter = thread_safe_ref_counter>
	struct function;

	template <class RefCounter, class R, class ...Args>
	struct function<R(Args...), RefCounter>
	{
		typedef R result_type;

		function() BOOST_NOEXCEPT
		{
		}

		function(function &&other) BOOST_NOEXCEPT
		{
			swap(other);
		}

		function(function const &other) BOOST_NOEXCEPT
			: m_content(other.m_content)
		{
		}

		template <class F>
		explicit function(F &&f)
			: m_content(new functor<typename boost::decay<F>::type>(std::forward<F>(f)))
		{
		}

		template <class F>
		function &operator = (F &&f)
		{
			*this = function(std::forward<F>(f));
			return *this;
		}

		function &operator = (function &&other) BOOST_NOEXCEPT
		{
			swap(other);
			return *this;
		}

		function &operator = (function const &other) BOOST_NOEXCEPT
		{
			m_content = other.m_content;
			return *this;
		}

		R operator ()(Args ...args) const
		{
			assert(m_content);
			return m_content->call(args...);
		}

		void swap(function &other) BOOST_NOEXCEPT
		{
			m_content.swap(other.m_content);
		}

		explicit operator bool() const BOOST_NOEXCEPT
		{
			return !!m_content;
		}

	private:

		struct callable
		{
			RefCounter counter;

			callable()
			{
			}

			virtual ~callable()
			{
			}

			virtual R call(Args... args) const = 0;
		};

		friend void intrusive_ptr_add_ref(callable *ca)
		{
			ca->counter.increment();
		}

		friend void intrusive_ptr_release(callable *ca)
		{
			if (ca->counter.decrement_is_zero_now())
			{
				delete ca;
			}
		}

		template <class F>
		struct functor : callable
		{
			template <class G>
			explicit functor(G &&f)
				: m_f(std::forward<G>(f))
			{
			}

			virtual R call(Args... args) const override
			{
				return m_f(args...);
			}

		private:

			F m_f;
		};
		
		boost::intrusive_ptr<callable> m_content;
	};
}


#endif
