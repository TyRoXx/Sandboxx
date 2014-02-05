#include <vector>
#include <list>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace vsl
{
	typedef std::intptr_t element;

	template <class Container>
	struct tests
	{
		static void run_all(std::size_t n)
		{
			push_back(n);
		}

	private:

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
	};
}

int main(int argc, char **argv)
{
	if (argc >= 2)
	{
		std::size_t n = 100000000;
		if (argc >= 3)
		{
			n = boost::lexical_cast<std::size_t>(argv[2]);
		}
		switch (std::toupper(argv[1][0]))
		{
		case 'V':
			vsl::tests<std::vector<vsl::element>>::run_all(n);
			return 0;

		case 'L':
			vsl::tests<std::list<vsl::element>>::run_all(n);
			return 0;

		default:
			break;
		}
	}

	std::cerr << "Pass either 'V' or 'L' as an argument to run the test on vector or list.\n";
	return 1;
}
