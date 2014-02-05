#include <vector>
#include <list>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <set>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>

namespace vsl
{
	typedef std::intptr_t element;

	template <class Container>
	struct tests
	{
		static void push_back(std::size_t n)
		{
			Container c;
			for (std::size_t i = 0; i < n; ++i)
			{
				c.push_back(typename Container::value_type());
			}

			typename Container::value_type sum{};
			for (auto &&e : c)
			{
				sum += e;
			}

			if (sum)
			{
				throw std::logic_error("Cannot happen");
			}
		}

		template <class Generator>
		static void insert_into_set(std::size_t n, Generator const &generate)
		{
			Container c;
			for (std::size_t i = 0; i < n; ++i)
			{
				c.insert(generate());
			}

			typename Container::value_type sum{};
			for (auto &&e : c)
			{
				sum += e;
			}

			if (sum < 0)
			{
				throw std::logic_error("Cannot happen");
			}
		}
	};
}

int main(int argc, char **argv)
{
	if (argc >= 2)
	{
		std::size_t n = 100000000;
		if (argc >= 3)
		{
			n = std::max<std::size_t>(1, boost::lexical_cast<std::size_t>(argv[2]));
		}

		boost::random::mt19937 prng;
		boost::random::uniform_int_distribution<vsl::element> dist(0, std::numeric_limits<vsl::element>::max() / n);
		auto const &&random = [&prng, dist] { return dist(prng); };

		switch (std::toupper(argv[1][0]))
		{
		case 'V':
			vsl::tests<std::vector<vsl::element>>::push_back(n);
			return 0;

		case 'L':
			vsl::tests<std::list<vsl::element>>::push_back(n);
			return 0;

		case 'U':
			vsl::tests<std::unordered_set<vsl::element>>::insert_into_set(n, random);
			return 0;

		case 'S':
			vsl::tests<std::set<vsl::element>>::insert_into_set(n, random);
			return 0;

		default:
			break;
		}
	}

	std::cerr << "Pass either 'V' or 'L' as an argument to run the test on vector or list.\n";
	return 1;
}
