#include "entity.h"
#include "world.h"
#include <assert.h>


static void add_direction_vector(
	Vector2i *dest,
	Direction dir,
	ptrdiff_t amount)
{
	assert(dest);
	switch (dir)
	{
	case Dir_North: dest->y -= amount; break;
	case Dir_West:  dest->x -= amount; break;
	case Dir_South: dest->y += amount; break;
	case Dir_East:  dest->x += amount; break;
	}
}

static int is_possible_step(
	Entity const *entity,
	Direction dir
	)
{
	Vector2i new_pos = entity->position;
	add_direction_vector(&new_pos, dir, 1);

	return World_is_walkable(entity->world, &new_pos);
}


Bool Entity_init(
	Entity *e,
	Vector2i position,
	Appearance appearance,
	float max_velocity,
	struct World *world
	)
{
	e->position = position;
	e->direction = Dir_South;
	e->appearance = appearance;
	e->max_velocity = max_velocity;
	e->steps_to_go = 0;
	e->world = world;
	return 1;
}

void Entity_free(Entity *e)
{
	(void)e;
}

void Entity_update(Entity *e, unsigned delta)
{
	if (e->steps_to_go > 0)
	{
		e->move_progress += e->max_velocity * (float)delta / 1000.0f;
		while (e->move_progress >= 1)
		{
			if (e->steps_to_go != (size_t)-1)
			{
				--(e->steps_to_go);
				if (e->steps_to_go == 0)
				{
					break;
				}
			}

			if (!is_possible_step(e, e->direction))
			{
				e->steps_to_go = 0;
				break;
			}

			add_direction_vector(&e->position, e->direction, 1);

			e->move_progress -= 1;
		}
	}
}

void Entity_stop(Entity *e)
{
	assert(e);
	assert(e->steps_to_go > 0);

	e->steps_to_go = 1;
}

Bool Entity_move(Entity *e, size_t steps_to_go)
{
	if (e->steps_to_go > 0)
	{
		return 0;
	}

	if (!is_possible_step(e, e->direction))
	{
		return 0;
	}

	add_direction_vector(&e->position, e->direction, 1);

	e->steps_to_go = steps_to_go;
	e->move_progress = 0;
	return 1;
}
