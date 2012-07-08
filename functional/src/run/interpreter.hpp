#ifndef FCT_INTERPRETER_HPP
#define FCT_INTERPRETER_HPP


#include "program/tree.hpp"
#include "object.hpp"
#include <memory>


namespace fct
{
	struct Interpreter
	{
		std::unique_ptr<Object> evaluate(const Tree &program);
		void pushSymbol(std::string name, std::unique_ptr<Object> value);
		void popSymbol();
		Object *findSymbol(const std::string &name) const;

	private:

		typedef std::vector<std::pair<std::string, std::unique_ptr<Object>>> Symbols;


		Symbols m_symbols;
	};
}


#endif