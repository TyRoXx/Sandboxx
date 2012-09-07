#ifndef TYPE_VISITOR_HPP_INCLUDED_EDDY965Z6E7TBUJY
#define TYPE_VISITOR_HPP_INCLUDED_EDDY965Z6E7TBUJY


namespace ptrs
{
	struct ptr_type;
	struct structure_type;
	struct method_type;


	struct type_visitor
	{
		virtual ~type_visitor();
		virtual void visit(const ptr_type &type) = 0;
		virtual void visit(const structure_type &type) = 0;
		virtual void visit(const method_type &type) = 0;
	};
}


#endif
