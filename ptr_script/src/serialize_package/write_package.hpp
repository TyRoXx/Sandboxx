#ifndef WRITE_PACKAGE_HPP_INCLUDED_
#define WRITE_PACKAGE_HPP_INCLUDED_


namespace ptrs
{
	struct package;
	
	
	namespace serialization
	{
		struct sink;
		
		
		void write_package(
			sink &sink,
			const package &package
			);
	}
}


#endif
