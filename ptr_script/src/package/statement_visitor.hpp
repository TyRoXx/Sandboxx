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
		virtual void accept(const block &statement);
		virtual void accept(const conditional &statement);
		virtual void accept(const jump &statement);
	};
}


#endif
