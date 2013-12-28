#include "function.hpp"
#include "function2.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
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
	measure<tx2::function<void(), tx2::thread_safe_ref_counter>, Content>("tx2 thread-safe");
	measure<tx2::function<void(), tx2::single_thread_ref_counter>, Content>("tx2 single-thread");
	measure<std::function<void()>, Content>("std");
	measure<boost::function<void()>, Content>("boost");
	std::cerr << '\n';
}

template <std::size_t I>
struct integer {};

template <std::size_t From, std::size_t Through>
void measure_different_weights();

template <std::size_t From, std::size_t Through>
void measure_different_weights_impl(std::false_type)
{
}

template <std::size_t From, std::size_t Through>
void measure_different_weights_impl(std::true_type)
{
	static std::size_t const actual_weight = From * 12;
	measure_all<heavy_content<actual_weight>>("heavy " + boost::lexical_cast<std::string>(actual_weight));
	measure_different_weights<From + 1, Through>();
}

template <std::size_t From, std::size_t Through>
void measure_different_weights()
{
	measure_different_weights_impl<From, Through>(std::integral_constant<bool, (From < Through)>());
}

int main()
{
	measure_different_weights<1, 12>();
	measure_all<minimal_content>("minimal");

	tx::function<void(std::string const &)> print_line([](std::string const &s)
	{
		std::cerr << s << '\n';
	});

	print_line("Hello,");
	print_line("world!");

	print_line = [](std::string const &)
	{
	};
}
