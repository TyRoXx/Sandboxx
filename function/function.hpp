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
			: std::runtime_error("Bad function call")
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
	}

	using detail::throw_on_empty_call;
	using detail::default_value_on_empty_call;

	template <class Signature, class ...Options>
	struct function;

	template <class R, class ...Args, class ...Options>
	struct function<R (Args...), Options...> : private Options...
	{
		function()
		{
		}

		template <class F>
		function(F &&functor)
			: m_impl(new holder<typename std::remove_reference<F>::type>(
						 std::forward<F>(functor)))
		{
		}

		template <class F>
		function &operator = (F &&functor)
		{
			function(std::forward<F>(functor)).swap(*this);
			return *this;
		}

		void swap(function &other)
		{
			m_impl.swap(other.m_impl);
		}

		R operator ()(Args ...args) const
		{
			if (!m_impl)
			{
				return this->template on_empty_call<R>();
			}
			return m_impl->call(std::forward<Args>(args)...);
		}

		explicit operator bool () const
		{
			return !!m_impl;
		}

	private:

		struct holder_base
		{
			virtual ~holder_base()
			{

			}

			virtual R call(Args ...) const = 0;
		};

		template <class F>
		struct holder : holder_base
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

		private:

			F const m_functor;
		};

		std::unique_ptr<holder_base> m_impl;
	};
}


#endif
