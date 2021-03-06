#ifndef STATEMENT_VISITOR_HPP_INCLUDED_
#define STATEMENT_VISITOR_HPP_INCLUDED_


namespace ptrs
{
	struct block;
	struct conditional;
	struct jump;
	struct call_statement;
	struct intrinsic;

	
	struct statement_visitor
	{
		virtual ~statement_visitor();
		virtual void visit(const block &statement) = 0;
		virtual void visit(const conditional &statement) = 0;
		virtual void visit(const jump &statement) = 0;
		virtual void visit(const call_statement &statement) = 0;
		virtual void visit(const intrinsic &statement) = 0;
	};
}


#endif
