#ifndef FCT_TREE_HPP
#define FCT_TREE_HPP


#include <string>
#include <vector>
#include <ostream>


namespace fct
{
	struct Tree
	{
		typedef std::vector<Tree> Arguments;


		std::string symbol;
		Arguments arguments;


		Tree();
		Tree(Tree &&other);
		Tree(const Tree &other);
		explicit Tree(std::string symbol);
		explicit Tree(std::string symbol, Arguments arguments);
		Tree &operator = (Tree &&other);
		Tree &operator = (const Tree &other);
		void swap(Tree &other);
	};


	void swap(Tree &left, Tree &right);
	bool operator == (const Tree &left, const Tree &right);
	bool operator != (const Tree &left, const Tree &right);

	std::ostream &operator << (std::ostream &os, const Tree &tree);
}


#endif