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


typedef struct Entity
{
	ptrdiff_t x, y;
	Direction direction;
	Appearance appearance;
	float max_velocity;
	int is_moving;
	float move_progress;
}
Entity;


int Entity_init(
	Entity *e,
	ptrdiff_t x,
	ptrdiff_t y,
	Appearance appearance,
	float max_velocity
	);
void Entity_free(Entity *e);
void Entity_update(Entity *e, unsigned delta);


#endif
