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
	AppearanceId appearance,
	struct World *world
	)
{
	e->position = position;
	e->direction = Dir_South;
	e->appearance = appearance;
	e->world = world;
	return 1;
}

void Entity_free(Entity *e)
{
	(void)e;
}


void Mover_init(Mover *m,
				float max_velocity,
				Entity body)
{
	m->max_velocity = max_velocity;
	m->body = body;
	m->move_progress = 0;
	m->steps_to_go = 0;
}

void Mover_free(Mover *m)
{
	Entity_free(&m->body);
}

Bool Mover_move(Mover *m, size_t steps_to_go)
{
	if (m->steps_to_go > 0)
	{
		return 0;
	}

	if (!is_possible_step(&m->body, m->body.direction))
	{
		return 0;
	}

	add_direction_vector(&m->body.position, m->body.direction, 1);

	m->steps_to_go = steps_to_go;
	m->move_progress = 0;
	return 1;
}

void Mover_stop(Mover *m)
{
	assert(m);
	assert(m->steps_to_go > 0);

	m->steps_to_go = 1;
}

void Mover_update(Mover *m, unsigned delta)
{
	if (m->steps_to_go > 0)
	{
		m->move_progress += m->max_velocity * (float)delta / 1000.0f;
		while (m->move_progress >= 1)
		{
			if (m->steps_to_go != (size_t)-1)
			{
				--(m->steps_to_go);
				if (m->steps_to_go == 0)
				{
					break;
				}
			}

			if (!is_possible_step(&m->body, m->body.direction))
			{
				m->steps_to_go = 0;
				break;
			}

			add_direction_vector(&m->body.position, m->body.direction, 1);

			m->move_progress -= 1;
		}
	}
}
