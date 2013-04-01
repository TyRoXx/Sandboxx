#ifndef ENTITY_H
#define ENTITY_H


#include "vector2i.h"
#include "bool.h"


struct World;


typedef size_t AppearanceId;


typedef enum Direction
{
	Dir_North,
	Dir_West,
	Dir_South,
	Dir_East
}
Direction;


typedef struct Entity
{
	Vector2i position;
	Direction direction;
	AppearanceId appearance;
	float max_velocity;
	size_t steps_to_go;
	float move_progress;
	struct World *world;
}
Entity;


Bool Entity_init(
	Entity *e,
	Vector2i position,
	AppearanceId appearance,
	float max_velocity,
	struct World *world
	);
void Entity_free(Entity *e);
void Entity_update(Entity *e, unsigned delta);
Bool Entity_move(Entity *e, size_t steps_to_go);
void Entity_stop(Entity *e);


#endif
