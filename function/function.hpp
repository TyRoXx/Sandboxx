#ifndef FUNCTION_HPP
#define FUNCTION_HPP


#include <type_traits>
#include <memory>
#include <stdexcept>
#include <cassert>


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
			R on_empty_call() const noexcept(noexcept(R{}))
			{
				return R{};
			}
		};

		template <class R, class ...Args>
		struct holder_base
		{
			virtual ~holder_base() noexcept
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

			virtual R call(Args ...args) const final
			{
				return m_functor(std::forward<Args>(args)...);
			}

			virtual std::unique_ptr<holder_base<R, Args...>> clone() const final
			{
				return std::unique_ptr<holder_base<R, Args...>>{new holder{m_functor}};
			}

		private:

			F const m_functor;
		};


		template <class R, class ...Args>
		struct ptr_to_polymorphic_storage;

		template <class R, class ...Args>
		struct ptr_to_polymorphic_storage<R (Args...)>
		{
			ptr_to_polymorphic_storage() noexcept
			{
			}

			ptr_to_polymorphic_storage(ptr_to_polymorphic_storage &&other) noexcept
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

			bool empty() const noexcept
			{
				return !m_impl;
			}

			bool equals(ptr_to_polymorphic_storage const &other) const noexcept
			{
				return (m_impl == other.m_impl);
			}

			bool less(ptr_to_polymorphic_storage const &other) const noexcept
			{
				return (m_impl < other.m_impl);
			}

			R call(Args ...args) const
			{
				assert(m_impl);
				return m_impl->call(args...);
			}

			void swap(ptr_to_polymorphic_storage &other) noexcept
			{
				m_impl.swap(other.m_impl);
			}

		private:

			std::unique_ptr<holder_base<R, Args...>> m_impl;
		};


		template <class R, class ...Args>
		struct functor_type_base
		{
			virtual R call(void const *storage, Args ...args) const = 0;
			virtual void destroy(void *storage) const = 0;
			virtual void uninitialized_copy(void *dest, void const *source) const = 0;
		};

		template <class F, class R, class ...Args>
		struct flat_functor_type : functor_type_base<R, Args...>
		{
			flat_functor_type() noexcept
			{
			}

			virtual R call(void const *storage, Args ...args) const final
			{
				F const &functor = *reinterpret_cast<F const *>(storage);
				return functor(args...);
			}

			virtual void destroy(void *storage) const final
			{
				F &functor = *reinterpret_cast<F *>(storage);
				functor.~F();
			}

			virtual void uninitialized_copy(void *dest, void const *source) const final
			{
				F * const dest_functor = reinterpret_cast<F *>(dest);
				F const &source_functor = *reinterpret_cast<F const *>(source);
				new (dest_functor) F(source_functor);
			}

			static functor_type_base<R, Args...> const &store(void *storage, F const &functor)
			{
				static flat_functor_type const instance;
				F * const destination = reinterpret_cast<F *>(storage);
				new (destination) F(functor);
				return instance;
			}
		};

		template <class F, class R, class ...Args>
		struct indirect_functor_type : functor_type_base<R, Args...>
		{
			indirect_functor_type() noexcept
			{
			}

			virtual R call(void const *storage, Args ...args) const final
			{
				auto &ptr = *reinterpret_cast<functor_ptr const *>(storage);
				return (*ptr)(args...);
			}

			virtual void destroy(void *storage) const final
			{
				auto &ptr = *reinterpret_cast<functor_ptr *>(storage);
				ptr.~functor_ptr();
			}

			virtual void uninitialized_copy(void *dest, void const *source) const final
			{
				auto * const dest_ptr = reinterpret_cast<functor_ptr *>(dest);
				auto &source_ptr = *reinterpret_cast<functor_ptr const *>(source);
				new (dest_ptr) functor_ptr(new F(*source_ptr));
			}

			static functor_type_base<R, Args...> const &store(void *storage, F const &functor)
			{
				static indirect_functor_type const instance;
				auto * const ptr = reinterpret_cast<functor_ptr *>(storage);
				new (ptr) functor_ptr(new F(functor));
				return instance;
			}

		private:

			typedef std::unique_ptr<F> functor_ptr;
		};

		template <class F, class R, class ...Args>
		functor_type_base<R, Args...> const &store_functor(void *storage, std::size_t storage_size, F const &functor)
		{
			if (sizeof(F) <= storage_size)
			{
				return flat_functor_type<F, R, Args...>::store(storage, functor);
			}
			else
			{
				return indirect_functor_type<F, R, Args...>::store(storage, functor);
			}
		}

		template <std::size_t Size, class R, class ...Args>
		struct small_functor_storage;

		template <std::size_t Size, class R, class ...Args>
		struct small_functor_storage<Size, R (Args...)>
		{
			static constexpr std::size_t min_size = sizeof(std::unique_ptr<char>);
			static constexpr std::size_t size = (Size < min_size) ? min_size : Size;


			small_functor_storage() noexcept
				: m_type(nullptr)
			{
			}

			small_functor_storage(small_functor_storage &&other) noexcept
				: m_type(nullptr)
			{
				swap(other);
			}

			small_functor_storage(small_functor_storage const &other)
				: m_type(other.m_type)
			{
				if (m_type)
				{
					m_type->uninitialized_copy(&m_storage, &other.m_storage);
				}
			}

			template <class F>
			small_functor_storage(F const &functor)
				: m_storage()
				, m_type(&store_functor<F, R, Args...>(&m_storage, size, functor))
			{
			}

			~small_functor_storage() noexcept
			{
				if (m_type)
				{
					m_type->destroy(&m_storage);
				}
			}

			bool empty() const noexcept
			{
				return !m_type;
			}

			bool equals(small_functor_storage const &other) const noexcept
			{
				if (this == &other)
				{
					return true;
				}
				if (!m_type &&
					!other.m_type)
				{
					return true;
				}
				return false;
			}

			R call(Args ...args) const
			{
				assert(m_type);
				return m_type->call(&m_storage, args...);
			}

			void swap(small_functor_storage &other) noexcept
			{
				std::swap(m_storage, other.m_storage);
				std::swap(m_type, other.m_type);
			}

		private:

			typename std::aligned_storage<size>::type m_storage;
			functor_type_base<R, Args...> const *m_type;
		};
	}

	using detail::throw_on_empty_call;
	using detail::default_value_on_empty_call;
	using detail::ptr_to_polymorphic_storage;
	using detail::small_functor_storage;


	template <class Signature, class EmptyCallPolicy, class StoragePolicy>
	struct function;

	template <class R, class ...Args, class EmptyCallPolicy, class StoragePolicy>
	struct function<R (Args...), EmptyCallPolicy, StoragePolicy>
		: private EmptyCallPolicy
		, private StoragePolicy
	{
		typedef EmptyCallPolicy empty_call_policy;
		typedef StoragePolicy storage_policy;


		function() noexcept
		{
		}

		template <class F>
		function(F const &functor)
			: storage_policy(functor)
		{
		}

		function(function const &other)
			: storage_policy(static_cast<storage_policy const &>(other))
		{
		}

		function(function &&other) noexcept
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

		function &operator = (function &&other) noexcept
		{
			swap(other);
			return *this;
		}

		void swap(function &other) noexcept
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
				return empty_call_policy::template on_empty_call<R>();
			}
			return storage_policy::call(args...);
		}

		bool empty() const noexcept
		{
			return storage_policy::empty();
		}

		explicit operator bool () const noexcept
		{
			return !empty();
		}

		friend bool operator == (function const &left, function const &right) noexcept
		{
			return left.equals(right);
		}
	};


	template <class ...T>
	void swap(function<T...> &left, function<T...> &right) noexcept
	{
		left.swap(right);
	}
}


#endif
