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

			void swap(ptr_to_polymorphic_storage &other)
			{
				m_impl.swap(other.m_impl);
			}

		private:

			std::unique_ptr<holder_base<R, Args...>> m_impl;
		};


		constexpr size_t functor_storage_size = 32;
		typedef std::aligned_storage<functor_storage_size>::type functor_storage;


		template <class R, class ...Args>
		struct functor_type_base
		{
			virtual R call(functor_storage const &storage, Args ...args) const = 0;
			virtual void destroy(functor_storage &storage) const = 0;
			virtual int compare(functor_storage const &left, functor_storage const &right) const = 0;
			virtual void uninitialized_copy(functor_storage &dest, functor_storage const &source) const = 0;
		};

		template <class F, class R, class ...Args>
		struct flat_functor_type : functor_type_base<R, Args...>
		{
			flat_functor_type()
			{
			}

			virtual R call(functor_storage const &storage, Args ...args) const override
			{
				F const &functor = *reinterpret_cast<F const *>(&storage);
				return functor(args...);
			}

			virtual void destroy(functor_storage &storage) const override
			{
				F &functor = *reinterpret_cast<F *>(&storage);
				functor.~F();
			}

			virtual int compare(functor_storage const &left, functor_storage const &right) const override
			{
				F const &left_functor = *reinterpret_cast<F const *>(&left);
				F const &right_functor = *reinterpret_cast<F const *>(&right);
				if (left_functor < right_functor)
				{
					return -1;
				}
				else if (left_functor > right_functor)
				{
					return 1;
				}
				return 0;
			}

			virtual void uninitialized_copy(functor_storage &dest, functor_storage const &source) const override
			{
				F * const dest_functor = reinterpret_cast<F *>(&dest);
				F const &source_functor = *reinterpret_cast<F const *>(&source);
				new (dest_functor) F(source_functor);
			}

			static functor_type_base<R, Args...> const &store(functor_storage &storage, F const &functor)
			{
				static flat_functor_type const instance;
				F * const destination = reinterpret_cast<F *>(&storage);
				new (destination) F(functor);
				return instance;
			}
		};

		template <class F, class R, class ...Args>
		functor_type_base<R, Args...> const &store_functor(functor_storage &storage, F const &functor)
		{
			if (sizeof(F) <= functor_storage_size)
			{
				return flat_functor_type<F, R, Args...>::store(storage, functor);
			}
			else
			{
				throw std::runtime_error("Not implemented");
			}
		}

		template <class R, class ...Args>
		struct small_functor_storage
		{
			small_functor_storage()
				: m_type(nullptr)
			{
			}

			small_functor_storage(small_functor_storage &&other)
			{
				swap(other);
			}

			small_functor_storage(small_functor_storage const &other)
				: m_type(other.m_type)
			{
				if (m_type)
				{
					m_type->uninitialized_copy(m_storage, other.m_storage);
				}
			}

			template <class F>
			small_functor_storage(F const &functor)
				: m_storage()
				, m_type(&store_functor<F, R, Args...>(m_storage, functor))
			{
			}

			~small_functor_storage()
			{
				if (m_type)
				{
					m_type->destroy(m_storage);
				}
			}

			bool empty() const
			{
				return !m_type;
			}

			bool equals(small_functor_storage const &other) const
			{
				if (m_type == other.m_type)
				{
					return !m_type ||
							m_type->compare(m_storage, other.m_storage) == 0;
				}
				return false;
			}

			bool less(small_functor_storage const &other) const
			{
				if (m_type &&
					m_type == other.m_type)
				{
					return m_type->compare(m_storage, other.m_storage) < 0;
				}
				return (m_type < other.m_type);
			}

			R call(Args ...args) const
			{
				assert(m_type);
				return m_type->call(m_storage, args...);
			}

			void swap(small_functor_storage &other)
			{
				std::swap(m_storage, other.m_storage);
				std::swap(m_type, other.m_type);
			}

		private:

			functor_storage m_storage;
			functor_type_base<R, Args...> const *m_type;
		};
	}

	using detail::throw_on_empty_call;
	using detail::default_value_on_empty_call;
	using detail::ptr_to_polymorphic_storage;
	using detail::small_functor_storage;


	template <class Signature, class EmptyCallPolicy, template <class R, class ...Args> class StoragePolicy>
	struct function;

	template <class R, class ...Args, class EmptyCallPolicy, template <class R2, class ...Args2> class StoragePolicy>
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
			: storage_policy(static_cast<storage_policy const &>(other))
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
