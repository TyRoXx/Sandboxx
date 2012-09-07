#include "write_package.hpp"
#include "sink.hpp"
#include "package/package.hpp"
#include "package/structure.hpp"
#include "package/method.hpp"
#include "package/element.hpp"


namespace ptrs
{
	namespace serialization
	{
		namespace
		{
			void write_method(
				sink &sink,
				const method &method
				)
			{
				sink.text(method.name());
				sink.line();
			}

			void write_element(
				sink &sink,
				const element &element
				)
			{
				sink.identifier(element.name());
				sink.line();
			}

			void write_structure(
				sink &sink,
				const structure &structure
				)
			{
				sink.identifier(structure.full_name());
				sink.line();

				{
					const auto &methods = structure.methods();
					for (auto i = methods.begin(); i != methods.end(); ++i)
					{
						write_method(sink, **i);
					}
				}

				{
					const auto &elements = structure.elements();
					for (auto i = elements.begin(); i != elements.end(); ++i)
					{
						write_element(sink, **i);
					}
				}

				sink.line();
			}
		}


		void write_package(
			sink &sink,
			const package &package
			)
		{
			{
				const auto &deps = package.dependencies();
				sink.integer64(deps.size());
				sink.line();

				for (auto i = deps.begin(); i != deps.end(); ++i)
				{
					sink.guid(*i);
				}
			}

			sink.line();

			{
				const auto &structures = package.structures();
				sink.integer64(structures.size());
				sink.line();

				for (auto i = structures.begin(); i != structures.end(); ++i)
				{
					write_structure(sink, **i);
				}
			}

			sink.line();
		}
	}
}
