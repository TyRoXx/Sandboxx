#include "camera.h"
#include "base/entity.h"
#include <assert.h>
#include <math.h>


int Camera_init(Camera *cam)
{
	assert(cam);
	cam->position = Vector2f_zero;
	return 1;
}

void Camera_free(Camera *cam)
{
	assert(cam);
	(void)cam;
}

void Camera_focus_on(Camera *cam, struct Entity const *target)
{
	assert(cam);
	assert(target);

	cam->position.x = (float)target->position.x + get_entity_offset(target, Dir_East);
	cam->position.y = (float)target->position.y + get_entity_offset(target, Dir_South);
}


float get_move_offset(Direction move_dir, float progress, Direction dir)
{
	long const diff = labs(move_dir - dir);
	switch (diff)
	{
	case 0: return progress - 1;
	case 2: return -progress + 1;
	default: return 0;
	}
}

float get_entity_offset(Entity const *e, Direction dir)
{
	float offset = 0;
	if (e->steps_to_go > 0)
	{
		offset += get_move_offset(e->direction, e->move_progress, dir);
	}
	return offset;
}
