#include <utility>

namespace
{
	template <class F, class ...Args>
	struct is_callable
	{
	private:

		typedef char (&yes)[2];
		typedef char (&no)[1];

		template <class F2>
		static yes f(
			decltype(
				std::declval<F2 &>()
					(std::declval<Args>()...)
			) *);

		template <class F2>
		static no f(...);

	public:

		enum
		{
			value = (sizeof(f<F>(0)) == sizeof(yes))
		};
	};

	const auto lambda = [](){};
}

static_assert(is_callable<decltype(lambda)>::value, "Lambda is callable");
static_assert(!is_callable<decltype(lambda), int>::value, "");
static_assert(!is_callable<decltype(lambda), int, float>::value, "");

static_assert(is_callable<void ()>::value, "A free function is callable");
static_assert(!is_callable<void (), int>::value, "");
static_assert(!is_callable<void (), int, float>::value, "");

static_assert(!is_callable<void (int)>::value, "");
static_assert(is_callable<void (int), int>::value, "");
static_assert(!is_callable<void (int), int, float>::value, "");

static_assert(!is_callable<void>::value, "void is not callable");
static_assert(!is_callable<void, int, float>::value, "void is not callable");
static_assert(!is_callable<int>::value, "int is not callable");
static_assert(!is_callable<int, int, float>::value, "int is not callable");

static_assert(!is_callable<int (int)>::value, "");
static_assert(is_callable<int (int), int>::value, "");
static_assert(!is_callable<int (int), int, float>::value, "");

int main()
{
}
