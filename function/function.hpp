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


		template <class R, class ...Args>
		struct ptr_to_polymorphic_storage
		{
			ptr_to_polymorphic_storage()
			{
			}

			ptr_to_polymorphic_storage(ptr_to_polymorphic_storage &&other)
				: m_impl(std::move(other.m_impl))
			{
			}

			ptr_to_polymorphic_storage(ptr_to_polymorphic_storage const &other)
			{
				if (other.m_impl)
				{
					m_impl = other.m_impl->clone();
				}
			}

			template <class F>
			ptr_to_polymorphic_storage(F const &functor)
				: m_impl(new holder<F, R, Args...>(functor))
			{
			}

			bool empty() const
			{
				return !m_impl;
			}

			bool equals(ptr_to_polymorphic_storage const &other) const
			{
				return (m_impl == other.m_impl);
			}

			bool less(ptr_to_polymorphic_storage const &other) const
			{
				return (m_impl < other.m_impl);
			}

			R call(Args ...args) const
			{
				assert(m_impl);
				return m_impl->call(args...);
			}

		private:

			std::unique_ptr<holder_base<R, Args...>> m_impl;
		};
	}

	using detail::throw_on_empty_call;
	using detail::default_value_on_empty_call;
	using detail::ptr_to_polymorphic_storage;

	template <class Signature, class EmptyCallPolicy, template <class R, class ...Args> class StoragePolicy>
	struct function;

	template <class R, class ...Args, class EmptyCallPolicy, template <class R, class ...Args> class StoragePolicy>
	struct function<R (Args...), EmptyCallPolicy, StoragePolicy>
		: private EmptyCallPolicy
		, private StoragePolicy<R, Args...>
	{
		typedef EmptyCallPolicy empty_call_policy;
		typedef StoragePolicy<R, Args...> storage_policy;


		function()
		{
		}

		template <class F>
		function(F const &functor)
			: storage_policy(functor)
		{
		}

		function(function const &other)
			: storage_policy(other)
		{
		}

		function(function &&other)
			: storage_policy(std::move(static_cast<storage_policy &&>(other)))
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
				function(other).swap(*this);
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
			storage_policy::swap(other);
		}

		R operator ()(Args ...args) const
		{
			if (empty())
			{
				return this->template on_empty_call<R>();
			}
			return storage_policy::call(args...);
		}

		bool empty() const
		{
			return storage_policy::empty();
		}

		explicit operator bool () const
		{
			return !empty();
		}

		friend bool operator == (function const &left, function const &right)
		{
			return left.equals(right);
		}

		friend bool operator < (function const &left, function const &right)
		{
			return left.less(right);
		}
	};


	template <class ...T>
	void swap(function<T...> &left, function<T...> &right)
	{
		left.swap(right);
	}
}


#endif
