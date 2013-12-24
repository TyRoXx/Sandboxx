#ifndef FUNCTION2_HPP
#define FUNCTION2_HPP

#include <boost/atomic.hpp>
#include <cassert>
#include <boost/type_traits/decay.hpp>
#include <boost/intrusive_ptr.hpp>

namespace tx2
{
	template <class Signature>
	struct function;

	template <class R, class ...Args>
	struct function<R(Args...)>
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
			boost::atomic<std::size_t> refs;

			callable()
				: refs(0)
			{
			}

			virtual ~callable()
			{
			}

			virtual R call(Args... args) const = 0;
		};

		friend void intrusive_ptr_add_ref(callable *ca)
		{
			ca->refs.fetch_add(1, boost::memory_order_relaxed);
		}

		friend void intrusive_ptr_release(callable *ca)
		{
			if (ca->refs.fetch_sub(1, boost::memory_order_release) == 1)
			{
				boost::atomic_thread_fence(boost::memory_order_acquire);
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
