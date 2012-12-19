#include "vector2f.h"
#include <math.h>


void Vector2f_add(Vector2f *dest, Vector2f const *source)
{
	dest->x += source->x;
	dest->y += source->y;
}

void Vector2f_sub(Vector2f *dest, Vector2f const *source)
{
	dest->x -= source->x;
	dest->y -= source->y;
}

void Vector2f_scale(Vector2f *dest, float scalar)
{
	dest->x *= scalar;
	dest->y *= scalar;
}

static float square(float f)
{
	return (f * f);
}

float Vector2f_length_sq(Vector2f const *v)
{
	return square(v->x) + square(v->y);
}

float Vector2f_length(Vector2f const *v)
{
	return (float)sqrt(Vector2f_length_sq(v));
}


Vector2f const Vector2f_zero = {0, 0};
