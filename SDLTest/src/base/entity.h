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

#define DIR_COUNT 4


typedef struct Entity
{
	Vector2i position;
	Direction direction;
	AppearanceId appearance;
	struct World *world;
}
Entity;

Bool Entity_init(
	Entity *e,
	Vector2i position,
	AppearanceId appearance,
	struct World *world
	);
void Entity_free(Entity *e);


typedef struct Mover
{
	Entity body;
	float max_velocity;
	size_t steps_to_go;
	float move_progress;
}
Mover;

void Mover_init(Mover *m,
				float max_velocity,
				Entity body);
void Mover_free(Mover *m);
Bool Mover_move(Mover *m, size_t steps_to_go);
void Mover_stop(Mover *m);
void Mover_update(Mover *m, unsigned delta);


#endif
