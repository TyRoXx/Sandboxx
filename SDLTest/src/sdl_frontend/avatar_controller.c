#include "avatar_controller.h"
#include "base/game.h"
#include <assert.h>
#include <string.h>


Bool AvatarController_init(AvatarController *a, struct Mover *avatar)
{
	assert(a);
	assert(avatar);
	a->avatar = avatar;
	memset(a->is_direction_key_down, 0, sizeof(a->is_direction_key_down));
	return 1;
}

void AvatarController_free(AvatarController *a)
{
	(void)a;
}

void AvatarController_handle_input(AvatarController *a,
								   Direction dir,
								   int is_down)
{
	Mover * const avatar = a->avatar;
	if (!avatar)
	{
		return;
	}

	if (is_down)
	{
		a->is_direction_key_down[dir] = 1;
		if (avatar->steps_to_go == 0)
		{
			avatar->body.direction = dir;
			Mover_move(avatar, (size_t)-1);
		}
	}
	else
	{
		int * const previous = &a->is_direction_key_down[dir];
		if (*previous)
		{
			*previous = 0;

			if ((avatar->body.direction == dir) &&
				(avatar->steps_to_go > 0))
			{
				Mover_stop(avatar);
			}
		}
	}
}

void AvatarController_update(AvatarController *a)
{
	Mover * const avatar = a->avatar;
	if (!avatar)
	{
		return;
	}

	if (avatar->steps_to_go == 0)
	{
		int input_dir = 0;
		while ((input_dir < 4) &&
			!a->is_direction_key_down[input_dir])
		{
			++input_dir;
		}
		if (input_dir < 4)
		{
			avatar->body.direction = (Direction)input_dir;
			Mover_move(avatar, (size_t)-1);
		}
	}
}
