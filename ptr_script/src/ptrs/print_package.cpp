#include "print_package.hpp"
#include "package/package.hpp"
#include "package/statement.hpp"
#include "package/statement_visitor.hpp"
#include "package/value.hpp"
#include "package/value_visitor.hpp"
#include "package/type.hpp"
#include "package/type_visitor.hpp"
#include "package/ptr_type.hpp"
#include "package/method_type.hpp"
#include "package/structure_type.hpp"


namespace ptrs
{
	void print_package(
		std::ostream &os,
		const package &package
		)
	{
		const auto &deps = package.dependencies();
		for (auto i = deps.begin(); i != deps.end(); ++i)
		{
			os << "\tdependency " << *i << "\n";
		}

		const auto &structures = package.structures();
		for (auto i = structures.begin(); i != structures.end(); ++i)
		{
			print_structure(os, **i);
			os << "\n";
		}

		const auto &methods = package.free_methods();
		for (auto i = methods.begin(); i != methods.end(); ++i)
		{
			print_method(os, **i);
			os << "\n";
		}
	}

	void print_method(
		std::ostream &os,
		const method &method
		)
	{
		os << method.name() << " (\n";

		const auto &params = method.parameters();
		for (auto i = params.begin(); i != params.end(); ++i)
		{
			const auto &parameter = **i;
			os << "\t" << parameter.name() << "\n";
		}

		os << ") -> (\n";

		const auto &results = method.results();
		for (auto i = results.begin(); i != results.end(); ++i)
		{
			const auto &result = **i;
			os << "\t ";
			print_type(os, result);
			os << "\n";
		}

		os << ")";
	}

	void print_structure(
		std::ostream &os,
		const structure &structure
		)
	{

	}

	namespace
	{
		struct type_printer : type_visitor
		{
			std::ostream &os;

			explicit type_printer(std::ostream &os)
				: os(os)
			{
			}

			virtual void visit(const ptr_type &type) PTR_SCRIPT_OVERRIDE
			{
				os << "ptr[";
				print_type(os, type.pointee());
				os << "]";
			}

			virtual void visit(const structure_type &type) PTR_SCRIPT_OVERRIDE
			{
				os << "struct " << type.ref().package.dependency_index << "." << type.ref().structure_index;
			}

			virtual void visit(const method_type &type) PTR_SCRIPT_OVERRIDE
			{
				os << "method (\n";

				const auto &params = type.parameters();
				for (auto i = params.begin(); i != params.end(); ++i)
				{
					const auto &parameter = **i;
					os << "\t";
					print_type(os, parameter);
					os << "\n";
				}

				os << ") -> (\n";

				const auto &results = type.results();
				for (auto i = results.begin(); i != results.end(); ++i)
				{
					const auto &result = **i;
					os << "\t ";
					print_type(os, result);
					os << "\n";
				}

				os << ")";
			}
		};
	}

	void print_type(
		std::ostream &os,
		const type &type
		)
	{
		type_printer printer(os);
		type.accept(printer);
	}

	void print_value(
		std::ostream &os,
		const value &value
		)
	{

	}
}
