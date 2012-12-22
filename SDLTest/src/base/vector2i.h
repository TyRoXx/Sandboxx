#ifndef VECTOR2I_H
#define VECTOR2I_H


#include <stddef.h>


typedef struct Vector2i
{
	ptrdiff_t x, y;
}
Vector2i;


Vector2i Vector2i_new(ptrdiff_t x, ptrdiff_t y);
void Vector2i_add(Vector2i *dest, Vector2i const *source);
void Vector2i_sub(Vector2i *dest, Vector2i const *source);
void Vector2i_scale(Vector2i *dest, ptrdiff_t scalar);
int Vector2i_equal(Vector2i const *first, Vector2i const *second);

Vector2i const Vector2i_zero;


#endif