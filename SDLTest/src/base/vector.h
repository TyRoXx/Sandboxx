#ifndef VECTOR_H
#define VECTOR_H


#include "bool.h"
#include <stddef.h>


typedef struct Vector
{
	char *data;
	size_t capacity;
	size_t size;
}
Vector;


void Vector_init(Vector *v);
void Vector_free(Vector *v);
Bool Vector_push_back(Vector *v, void const *element, size_t size);
size_t Vector_size(Vector const *v);
char *Vector_data(Vector const *v);
Bool Vector_reserve(Vector *v, size_t capacity);
Bool Vector_resize(Vector *v, size_t size);


#endif
