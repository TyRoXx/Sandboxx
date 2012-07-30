#include <array>
#include <algorithm>
#include <vector>
#include <random>
#include <iostream>
#include <functional>
using namespace std;

namespace
{
	template <class F, class I>
	bool test_sort_range(F sort_, I begin, I end)
	{
		vector<iterator_traits<I>::value_type> data(begin, end);
		random_shuffle(std::begin(data), std::end(data));
		sort_(std::begin(data), std::end(data));
		return equal(begin, end, std::begin(data));
	}

	template <class F>
	bool test_sort(F sort)
	{
		std::vector<int> test_data(1000);
		std::mt19937 random;
		std::uniform_int_distribution<> dist(-1000, 1000);
		std::generate(begin(test_data), end(test_data), [&](){ return dist(random); });
		std::sort(begin(test_data), end(test_data));

		for (size_t i = 0; i < test_data.size(); ++i)
		{
			if (!test_sort_range(sort, begin(test_data) + i, end(test_data)))
			{
				return false;
			}
		}

		return true;
	}

	template <class F, class N>
	void check_sort(F sort, N name)
	{
		cout << (test_sort(sort) ? "correct" : "incorrect") << ": " << name << endl;
	}


	struct selection_sort
	{
		template <class I>
		void operator ()(I begin, I end) const
		{
		}
	};

	struct std_sort
	{
		template <class I>
		void operator ()(I begin, I end) const
		{
			std::sort(begin, end);
		}
	};
}

int main()
{
	check_sort(selection_sort(), "Selection Sort");
	check_sort(std_sort(), "std::sort");
}
