#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include <type_traits>
#include <memory>
#include <stdexcept>


namespace exp
{
	struct bad_function : std::runtime_error
	{
		bad_function()
			: std::runtime_error("Call on empty exp::function")
		{

		}
	};

	namespace detail
	{
		struct throw_on_empty_call
		{
			template <class R>
			R on_empty_call() const
			{
				throw bad_function();
			}
		};

		struct default_value_on_empty_call
		{
			template <class R>
			R on_empty_call() const
			{
				return R{};
			}
		};

		template <class R, class ...Args>
		struct holder_base
		{
			virtual ~holder_base()
			{
			}

			virtual R call(Args ...) const = 0;
			virtual std::unique_ptr<holder_base> clone() const = 0;
		};

		template <class F, class R, class ...Args>
		struct holder : holder_base<R, Args...>
		{
			template <class G>
			explicit holder(G &&functor)
				: m_functor(std::forward<G>(functor))
			{
			}

			virtual R call(Args ...args) const override
			{
				return m_functor(std::forward<Args>(args)...);
			}

			virtual std::unique_ptr<holder_base<R, Args...>> clone() const override
			{
				return std::unique_ptr<holder_base<R, Args...>>{new holder{m_functor}};
			}

		private:

			F const m_functor;
		};
	}

	using detail::throw_on_empty_call;
	using detail::default_value_on_empty_call;

	template <class Signature, class EmptyCallPolicy>
	struct function;

	template <class R, class ...Args, class EmptyCallPolicy>
	struct function<R (Args...), EmptyCallPolicy> : private EmptyCallPolicy
	{
		function()
		{
		}

		template <class F>
		function(F const &functor)
			: m_impl(new detail::holder<F, R, Args...>(
						 functor))
		{
		}

		function(function const &other)
		{
			if (other.m_impl)
			{
				m_impl = other.m_impl->clone();
			}
		}

		function(function &&other)
			: m_impl(std::move(other.m_impl))
		{
		}

		template <class F>
		function &operator = (F const &functor)
		{
			function(functor).swap(*this);
			return *this;
		}

		function &operator = (function const &other)
		{
			if (this != &other)
			{
				if (other.m_impl)
				{
					m_impl = other.m_impl->clone();
				}
				else
				{
					m_impl.reset();
				}
			}
			return *this;
		}

		function &operator = (function &&other)
		{
			swap(other);
			return *this;
		}

		void swap(function &other)
		{
			if (this == &other)
			{
				return;
			}
			m_impl.swap(other.m_impl);
		}

		R operator ()(Args ...args) const
		{
			if (!m_impl)
			{
				return this->EmptyCallPolicy::template on_empty_call<R>();
			}
			return m_impl->call(std::forward<Args>(args)...);
		}

		bool empty() const
		{
			return !m_impl;
		}

		explicit operator bool () const
		{
			return !!m_impl;
		}

		friend bool operator == (function const &left, function const &right)
		{
			return (left.m_impl == right.m_impl);
		}

		friend bool operator < (function const &left, function const &right)
		{
			return (left.m_impl < right.m_impl);
		}

	private:

		std::unique_ptr<detail::holder_base<R, Args...>> m_impl;
	};


	template <class ...T>
	void swap(function<T...> &left, function<T...> &right)
	{
		left.swap(right);
	}
}


#endif
