#include "function.hpp"
#include "function2.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <functional>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

template <class Test>
void measure_test_duration(std::string const &test_name, Test const &test)
{
	auto const begin = std::chrono::high_resolution_clock::now();
	test();
	auto const end = std::chrono::high_resolution_clock::now();
	std::cerr << test_name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << '\n';
}

struct minimal_content
{
	void operator ()() const
	{
	}
};

template <std::size_t Size>
struct heavy_content
{
	char weight[Size];

	heavy_content()
	{
	}

	void operator ()() const
	{
	}
};

template <class FunctorEraser, class Content>
void run_test()
{
	std::vector<FunctorEraser> fs;
	fs.resize(1000000);
	std::fill(begin(fs), end(fs), Content());
	auto fs2 = fs;
	std::random_shuffle(begin(fs), end(fs));
}

template <class FunctorEraser, class Content>
void measure(std::string const &test_name)
{
	return measure_test_duration(test_name, run_test<FunctorEraser, Content>);
}

template <class Content>
void measure_all(std::string const &content_name)
{
	std::cerr << content_name << "\n";
	measure<tx::function<void()>, Content>("tx");
	measure<tx2::function<void()>, Content>("tx2");
	measure<std::function<void()>, Content>("std");
	measure<boost::function<void()>, Content>("boost");
	std::cerr << '\n';
}

int main()
{
	measure_all<heavy_content<12>>("heavy 12");
	measure_all<heavy_content<24>>("heavy 24");
	measure_all<heavy_content<48>>("heavy 48");
	measure_all<heavy_content<96>>("heavy 96");
	measure_all<minimal_content>("minimal");

	tx::function<void(std::string const &)> print_line([](std::string const &s)
	{
		std::cerr << s << '\n';
	});

	print_line("Hello,");
	print_line("world!");

	print_line = [](std::string const &s)
	{
	};
}
