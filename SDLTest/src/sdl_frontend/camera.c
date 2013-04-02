#include "camera.h"
#include "base/entity.h"
#include <assert.h>
#include <stdlib.h>


Bool Camera_init(Camera *cam)
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

void Camera_focus_on(Camera *cam, struct Mover const *target)
{
	assert(cam);
	assert(target);

	cam->position.x = (float)target->body.position.x + get_mover_offset(target, Dir_East);
	cam->position.y = (float)target->body.position.y + get_mover_offset(target, Dir_South);
}


float get_move_offset(Direction move_dir, float progress, Direction dir)
{
	int const diff = abs((int)move_dir - (int)dir);
	assert(diff >= 0);
	assert(diff <= 3);
	switch (diff)
	{
	case 0: return progress - 1;
	case 2: return -progress + 1;
	default: return 0;
	}
}

float get_mover_offset(Mover const *e, Direction dir)
{
	float offset = 0;

	assert(e);

	if (e->steps_to_go > 0)
	{
		offset += get_move_offset(e->body.direction, e->move_progress, dir);
	}
	return offset;
}
