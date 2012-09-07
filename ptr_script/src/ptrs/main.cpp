#include "interpreter.hpp"
#include "package/package.hpp"
#include "package/method.hpp"
#include "package/block.hpp"
#include "print_package.hpp"
#include <iostream>


namespace ptrs
{
	void start()
	{
		package::structure_vector structures;
		package::method_vector methods;
		package::dependency_vector deps;

		{
			std::unique_ptr<statement> body(new block(
				block::statement_vector(),
				true));

			methods.push_back(std::unique_ptr<method>(new method(
				"main",
				method::parameter_vector(),
				method::result_vector(),
				std::move(body))));
		}

		const package p(
			std::move(deps),
			std::move(structures),
			std::move(methods));

		print_package(std::cout, p);

		interpreter::package_by_id packages;
		packages[guid()] = &p;

		interpreter inter(packages);
		inter.call(*p.free_methods()[0]);
	}
}


int main()
{
	ptrs::start();
}
