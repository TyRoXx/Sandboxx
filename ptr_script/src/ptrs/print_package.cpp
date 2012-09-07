#include "print_package.hpp"
#include "package/package.hpp"
#include "package/statement.hpp"
#include "package/statement_visitor.hpp"
#include "package/block.hpp"
#include "package/jump.hpp"
#include "package/value.hpp"
#include "package/value_visitor.hpp"
#include "package/local.hpp"
#include "package/literal.hpp"
#include "package/element_ptr.hpp"
#include "package/call.hpp"
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

	namespace
	{
		template <class Types>
		void print_type_ptr_vector(
			std::ostream &os,
			const Types &types
			)
		{
			for (auto i = types.begin(); i != types.end(); ++i)
			{
				const auto &type = **i;
				os << "\t ";
				print_type(os, type);
				os << "\n";
			}
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
		print_type_ptr_vector(os, method.results());
		os << ")";
	}

	void print_structure(
		std::ostream &os,
		const structure &structure
		)
	{
		os << "structure " << structure.full_name() << " {\n";

		const auto &methods = structure.methods();
		for (auto i = methods.begin(); i != methods.end(); ++i)
		{
			const auto &method = **i;
			print_method(os, method);
		}

		const auto &elements = structure.elements();
		for (auto i = elements.begin(); i != elements.end(); ++i)
		{
			const auto &element = **i;
			print_element(os, element);
			os << "\n";
		}

		os << "}";
	}

	void print_element(
		std::ostream &os,
		const element &element
		)
	{
		os << element.name() << ": ";
		print_type(os, element.type());
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
				print_type_ptr_vector(os, type.parameters());
				os << ") -> (\n";
				print_type_ptr_vector(os, type.results());
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

	namespace
	{
		struct value_printer : value_visitor
		{
			std::ostream &os;

			explicit value_printer(std::ostream &os)
				: os(os)
			{
			}

			virtual void visit(const local &value) PTR_SCRIPT_OVERRIDE
			{
				os << "local " << value.id();
			}

			virtual void visit(const element_ptr &value) PTR_SCRIPT_OVERRIDE
			{
				print_value(os, value.object());
				os << "." << value.element_index();
			}

			virtual void visit(const literal &value) PTR_SCRIPT_OVERRIDE
			{
				os << "literal";
			}

			virtual void visit(const call &value) PTR_SCRIPT_OVERRIDE
			{
				print_value(os, value.method());
				os << "(";

				bool comma = false;
				const auto &arguments = value.arguments();
				for (auto i = arguments.begin(); i != arguments.end(); ++i)
				{
					print_value(os, **i);
					if (comma)
					{
						os << ", ";
					}
					else
					{
						comma = true;
					}
				}

				os << ")";
			}
		};
	}

	void print_value(
		std::ostream &os,
		const value &value
		)
	{
		value_printer printer(os);
		value.accept(printer);
	}
}
