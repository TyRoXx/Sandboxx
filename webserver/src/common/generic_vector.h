#ifndef WS_GENERIC_VECTOR_H
#define WS_GENERIC_VECTOR_H


#include "config.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>


#define WS_GEN_VECTOR(name, element_type) \
	element_type * name ## _begin; \
	element_type * name ## _end; \
	element_type * name ## _allocated

#define WS_GEN_VECTOR_CREATE(reference) do { \
		reference ## _begin = \
		reference ## _end = \
		reference ## _allocated = 0; \
	} while (0)

#define WS_GEN_VECTOR_DESTROY(reference) do { \
		free( reference ## _begin ); \
	} while (0)

#define WS_GEN_VECTOR_RESERVE(reference, capacity, success) do { \
		void *new_begin; \
		const size_t new_capacity = (capacity); \
		const size_t old_size = WS_GEN_VECTOR_SIZE(reference); \
		if (WS_GEN_VECTOR_CAPACITY(reference) >= new_capacity) { (success) = true; break; } \
		new_begin = realloc( WS_GEN_VECTOR_DATA(reference), (new_capacity * sizeof(*WS_GEN_VECTOR_DATA(reference)))); \
		if (!new_begin) { (success) = false; break; } \
		WS_GEN_VECTOR_BEGIN(reference) = new_begin; \
		WS_GEN_VECTOR_END(reference) = WS_GEN_VECTOR_BEGIN(reference) + old_size; \
		(reference ## _allocated) = (WS_GEN_VECTOR_BEGIN(reference) + new_capacity); \
		(success) = true; \
	} while (0)

#define WS_GEN_VECTOR_RESIZE(reference, size, success) do { \
	const size_t local_size = (size); \
	WS_GEN_VECTOR_RESERVE(reference, local_size, (success)); \
	if ((success)) { \
	WS_GEN_VECTOR_END(reference) = (WS_GEN_VECTOR_BEGIN(reference) + local_size); \
	} \
	} while (0)

#define WS_GEN_VECTOR_GROW(reference, min_capacity, success) do { \
		const size_t min_cap_copy = (min_capacity); \
		WS_GEN_VECTOR_RESERVE(reference, \
			(min_cap_copy > WS_GEN_VECTOR_CAPACITY(reference)) ? (min_cap_copy * 2) : min_cap_copy, (success)); \
	} while (0)

#define WS_GEN_VECTOR_PUSH_BACK(reference, element, success) do { \
		WS_GEN_VECTOR_GROW(reference, (WS_GEN_VECTOR_SIZE(reference) + 1), success); \
		if (!(success)) break; \
		*WS_GEN_VECTOR_END(reference) = element; \
		++(WS_GEN_VECTOR_END(reference)); \
	} while (0)

#define WS_GEN_VECTOR_POP_BACK(reference) do { \
		assert(!WS_GEN_VECTOR_EMPTY(reference)); \
		--WS_GEN_VECTOR_END(reference); \
	} while (0)

#define WS_GEN_VECTOR_SIZE(reference) (size_t)(WS_GEN_VECTOR_END(reference) - WS_GEN_VECTOR_BEGIN(reference))

#define WS_GEN_VECTOR_EMPTY(reference) (WS_GEN_VECTOR_BEGIN(reference) == WS_GEN_VECTOR_END(reference))

#define WS_GEN_VECTOR_CAPACITY(reference) (size_t)((reference ## _allocated) - WS_GEN_VECTOR_BEGIN(reference))

#define WS_GEN_VECTOR_DATA(reference) (reference ## _begin)

#define WS_GEN_VECTOR_BEGIN(reference) (reference ## _begin)

#define WS_GEN_VECTOR_END(reference) (reference ## _end)

#define WS_GEN_VECTOR_ASSIGN(reference, begin, end, success) do { \
	size_t const new_size = ((end) - (begin)); \
	WS_GEN_VECTOR_RESIZE(reference, new_size, success); \
	if ((success)) { \
	memmove(WS_GEN_VECTOR_DATA(reference), begin, (new_size * sizeof(*WS_GEN_VECTOR_DATA(reference)))); \
	} \
	} while (0)

#define WS_GEN_VECTOR_CLEAR(reference) do { \
	WS_GEN_VECTOR_END(reference) = WS_GEN_VECTOR_BEGIN(reference); \
	} while (0)

#define WS_GEN_VECTOR_APPEND_RANGE(reference, begin, end, success) do { \
	size_t const appended_size = ((end) - (begin)); \
	size_t i = WS_GEN_VECTOR_SIZE(reference); \
	size_t size = (i + appended_size); \
	size_t j = 0; \
	WS_GEN_VECTOR_RESIZE(reference, size, success); \
	if (!(success)) break; \
	for (; i < size; ++i, ++j) { \
		WS_GEN_VECTOR_DATA(reference)[i] = begin[j]; \
	} \
	} while (0)

#define WS_GEN_VECTOR_FRONT(reference) (WS_GEN_VECTOR_BEGIN(reference)[0])
#define WS_GEN_VECTOR_BACK(reference) (WS_GEN_VECTOR_END(reference)[-1])


#endif
