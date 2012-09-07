#ifndef VALUE_VISITOR_HPP_INCLUDED_HJOJT2RLVND6OVIB
#define VALUE_VISITOR_HPP_INCLUDED_HJOJT2RLVND6OVIB


namespace ptrs
{
	struct local;
	struct element_ptr;
	struct literal;
	struct call;


	struct value_visitor
	{
		virtual ~value_visitor();
		virtual void visit(const local &value) = 0;
		virtual void visit(const element_ptr &value) = 0;
		virtual void visit(const literal &value) = 0;
		virtual void visit(const call &value) = 0;
	};
}


#endif
