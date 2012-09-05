#include "interpreter.hpp"
#include "package/package.hpp"


namespace ptrs
{
	void start()
	{
		package::structure_vector structures;
		package::method_vector methods;
		package::dependency_vector deps;

		package p(std::move(structures), std::move(methods), std::move(deps));

		interpreter::package_by_id packages;
		packages[guid()] = &p;

		interpreter inter(packages);
		inter.call(0);
	}
}


int main()
{
	ptrs::start();
}
