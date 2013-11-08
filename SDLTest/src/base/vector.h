#ifndef VECTOR_H
#define VECTOR_H


#include "bool.h"
#include <stddef.h>
#include <stdio.h>


typedef struct Vector
{
	char *data;
	size_t capacity;
	size_t size;
}
Vector;


void Vector_init(Vector *v);
void Vector_free(Vector *v);
char *Vector_release(Vector *v);
Bool Vector_push_back(Vector *v, void const *element, size_t size);
size_t Vector_size(Vector const *v);
Bool Vector_empty(Vector const *v);
char *Vector_data(Vector const *v);
Bool Vector_reserve(Vector *v, size_t capacity);
Bool Vector_resize(Vector *v, size_t size);
char *Vector_begin(Vector const *v);
char *Vector_end(Vector const *v);
Bool Vector_append_binary_file(Vector *v, FILE *in);


#endif
