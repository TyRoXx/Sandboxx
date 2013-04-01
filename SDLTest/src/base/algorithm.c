#include "algorithm.h"
#include <assert.h>


void for_each(void *begin,
			  void *end,
			  size_t element_size,
			  void (*action)(void *element, void *user),
			  void *user)
{
	char *cbegin = begin;
	char * const cend = end;

	assert(((size_t)(cend - cbegin) % element_size == 0));

	for (; cbegin != cend; cbegin += element_size)
	{
		action(cbegin, user);
	}
}

void *find(void *begin,
		   void *end,
		   size_t element_size,
		   Bool (*predicate)(void *element, void *user),
		   void *user)
{
	char *cbegin = begin;
	char * const cend = end;

	assert(((size_t)(cend - cbegin) % element_size == 0));

	for (; cbegin != cend; cbegin += element_size)
	{
		if (predicate(cbegin, user))
		{
			break;
		}
	}

	return cbegin;
}
