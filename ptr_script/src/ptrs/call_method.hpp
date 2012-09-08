#ifndef CALL_METHOD_HPP_INCLUDED_
#define CALL_METHOD_HPP_INCLUDED_


namespace ptrs
{
	struct interpreter;
	struct method;
	
	
	void call_method(
		interpreter &interpreter,
		const method &method
		);
}


#endif
