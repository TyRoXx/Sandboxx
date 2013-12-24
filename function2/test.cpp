#include "function.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <functional>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <array>

template <class Test>
void measure_test_duration(std::string const &test_name, Test const &test)
{
	auto const begin = std::chrono::high_resolution_clock::now();
	test();
	auto const end = std::chrono::high_resolution_clock::now();
	std::cerr << test_name << ": " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << '\n';
}

static void light_content()
{
}

struct heavy_content
{
	std::array<long, 20> weight;

	void operator ()() const
	{
	}
};

template <class FunctorEraser>
void run_test()
{
	std::vector<FunctorEraser> fs;
	fs.resize(1000000);
	//std::fill(begin(fs), end(fs), light_content);
	std::fill(begin(fs), end(fs), heavy_content());
	auto fs2 = fs;
}

template <class FunctorEraser>
void measure(std::string const &test_name)
{
	return measure_test_duration(test_name, run_test<FunctorEraser>);
}

int main()
{
	tx::function<void(std::string const &)> print_line([](std::string const &s)
	{
		std::cerr << s << '\n';
	});

	print_line("Hello,");
	print_line("world!");

	print_line = [](std::string const &s)
	{
	};

	measure<tx::function<void()>>("tx");
	measure<std::function<void()>>("std");
	measure<boost::function<void()>>("boost");
}
