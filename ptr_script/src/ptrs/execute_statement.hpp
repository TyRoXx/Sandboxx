#ifndef EXECUTE_STATEMENT_HPP_INCLUDED_
#define EXECUTE_STATEMENT_HPP_INCLUDED_


namespace ptrs
{
	struct interpreter;
	struct statement;
	
	
	void execute_statement(
		interpreter &interpreter,
		const statement &statement
		);
}


#endif
