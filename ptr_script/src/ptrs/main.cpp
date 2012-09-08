#include "interpreter.hpp"
#include "package/package.hpp"
#include "package/method.hpp"
#include "package/block.hpp"
#include "package/structure_type.hpp"
#include "package/intrinsic.hpp"
#include "serialize_package/text_sink.hpp"
#include "serialize_package/write_package.hpp"
#include "print_package.hpp"
#include "call_method.hpp"
#include <iostream>
#include <fstream>


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

		const structure_ref uint_ref(package_ref(), 0);
		{
			method::result_vector results;
			results.push_back(std::unique_ptr<type>(new structure_type(
				uint_ref)));

			std::unique_ptr<method> negate(new method(
				"-",
				method::parameter_vector(),
				std::move(results),
				std::unique_ptr<statement>(new intrinsic)
				));

			structure::method_vector methods;
			methods.push_back(std::move(negate));

			std::unique_ptr<structure> structure(new structure(
				"uint",
				std::move(methods),
				structure::element_vector()));

			structures.push_back(std::move(structure));
		}

		{
			structure::element_vector elements;
			elements.push_back(std::unique_ptr<element>(new element(
				std::unique_ptr<type>(new structure_type(uint_ref)), "real")));
			elements.push_back(std::unique_ptr<element>(new element(
				std::unique_ptr<type>(new structure_type(uint_ref)), "imag")));

			std::unique_ptr<structure> structure(new structure(
				"complex",
				structure::method_vector(),
				std::move(elements)));

			structures.push_back(std::move(structure));
		}

		const package p(
			std::move(deps),
			std::move(structures),
			std::move(methods));

		print_package(std::cout, p);

		{
			std::ofstream file("package.txt");
			serialization::text_sink sink(file);
			serialization::write_package(sink, p);
		}

		interpreter::package_by_id packages;
		packages[guid()] = &p;

		interpreter inter(packages);
		call_method(
			inter,
			*p.free_methods()[0]);
	}
}


int main()
{
	ptrs::start();
}
