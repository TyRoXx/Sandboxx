#ifndef ENTITY_H
#define ENTITY_H


#include <stddef.h>


typedef struct Appearance
{
	size_t radius;
	size_t tile_set_id;
}
Appearance;


typedef enum Direction
{
	Dir_North, Dir_West, Dir_South, Dir_East
}
Direction;


typedef struct EntityLink
{
	struct EntityLink *previous, *next;
}
EntityLink;


typedef struct Entity
{
	EntityLink existence;
	ptrdiff_t x, y;
	Direction direction;
	Appearance appearance;
	float max_velocity;
	float move_progress;
}
Entity;


#endif
