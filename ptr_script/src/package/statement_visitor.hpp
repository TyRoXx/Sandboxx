#ifndef STATEMENT_VISITOR_HPP_INCLUDED_
#define STATEMENT_VISITOR_HPP_INCLUDED_


namespace ptrs
{
	struct block;
	struct conditional;
	struct jump;
	
	
	struct statement_visitor
	{
		virtual ~statement_visitor();
		virtual void visit(const block &statement);
		virtual void visit(const conditional &statement);
		virtual void visit(const jump &statement);
	};
}


#endif
