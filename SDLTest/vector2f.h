#ifndef VECTOR2F_H
#define VECTOR2F_H


typedef struct Vector2f
{
	float x, y;
}
Vector2f;


void Vector2f_add(Vector2f *dest, Vector2f const *source);
void Vector2f_sub(Vector2f *dest, Vector2f const *source);
void Vector2f_scale(Vector2f *dest, float scalar);
float Vector2f_length_sq(Vector2f const *v);
float Vector2f_length(Vector2f const *v);

Vector2f const Vector2f_zero;


#endif
