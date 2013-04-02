#include "vector2i.h"


Vector2i Vector2i_new(ptrdiff_t x, ptrdiff_t y)
{
	Vector2i result;
	result.x = x;
	result.y = y;
	return result;
}

void Vector2i_add(Vector2i *dest, Vector2i const *source)
{
	dest->x += source->x;
	dest->y += source->y;
}

void Vector2i_sub(Vector2i *dest, Vector2i const *source)
{
	dest->x -= source->x;
	dest->y -= source->y;
}

void Vector2i_scale(Vector2i *dest, ptrdiff_t scalar)
{
	dest->x *= scalar;
	dest->y *= scalar;
}

Bool Vector2i_equal(Vector2i const *first, Vector2i const *second)
{
	return
		(first->x == second->x) &&
		(first->y == second->y);
}


Vector2i const Vector2i_zero = {0, 0};
