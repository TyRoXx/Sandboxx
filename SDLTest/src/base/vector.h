#ifndef VECTOR_H
#define VECTOR_H


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
int Vector_push_back(Vector *v, void const *element, size_t size);
size_t Vector_size(Vector const *v);
char *Vector_data(Vector const *v);
int Vector_reserve(Vector *v, size_t capacity);
int Vector_resize(Vector *v, size_t size);


#endif
