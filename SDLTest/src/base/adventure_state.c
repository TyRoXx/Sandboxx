#include "adventure_state.h"
#include "world_text_file.h"
#include "game.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


enum
{
	MapWidth = 12, MapHeight = 8
};

static size_t const BuiltInMap[MapWidth * MapHeight] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	2, 2, 2, 2, 2, 2, 7, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 6, 0, 0, 0, 0, 0,
	3, 3, 3, 3, 3, 3, 5, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static TileKind const TileKinds[] =
{
	{0, 1},
	{1, 1},
	{2, 1},
	{3, 1},
	{4, 0},
	{5, 1},
	{6, 1},
	{7, 1},
};

static void init_demo_grid(TileGrid *g)
{
	size_t y, i;

	for (y = 0; y < MapHeight; ++y)
	{
		size_t x;
		for (x = 0; x < MapWidth; ++x)
		{
			size_t index = y * MapWidth + x;
			g->tiles[index].layers[0] =
				&TileKinds[BuiltInMap[index]];
		}
	}

	for (i = 2; i < MapWidth - 2; ++i)
	{
		g->tiles[5 * MapWidth + i].layers[1] = &TileKinds[4];
	}
}

static int add_demo_entities(World *w)
{
	AppearanceId const a = 8;
	Entity e;

	if (!Entity_init(&e, Vector2i_new(3, 4), a, 1.7f, w))
	{
		return 0;
	}

	e.direction = Dir_North;

	if (!World_add_entity(w, &e))
	{
		Entity_free(&e);
		return 0;
	}

	return 1;
}

static void save_world(World const *world)
{
	FILE * const file = fopen("world.txt", "w");
	if (!file)
	{
		return;
	}
	save_world_to_text(world, TileKinds, file);
	fclose(file);
}

static GameState *AdventureState_create(Game *game)
{
	AdventureState * const adv_state = malloc(sizeof(*adv_state));
	if (!adv_state)
	{
		return 0;
	}

	(void)game;

	{
		TileGrid demo_tiles;

		if (!TileGrid_init(&demo_tiles, 12, 8))
		{
			return 0;
		}

		init_demo_grid(&demo_tiles);

		if (!World_init(&adv_state->world, &demo_tiles) ||
			!add_demo_entities(&adv_state->world))
		{
			goto fail_0;
		}

		save_world(&adv_state->world);

		/*if there is an entity, choose the first one as the avatar*/
		adv_state->avatar = adv_state->world.entities;
		return (GameState *)adv_state;

fail_0:
		TileGrid_free(&demo_tiles);
		return 0;
	}
}

static void AdventureState_destroy(GameState *state)
{
	AdventureState * const adv_state = (AdventureState *)state;
	World_free(&adv_state->world);
	free(state);
}

static void AdventureState_update(GameState *state, unsigned delta)
{
	AdventureState * const adv_state = (AdventureState *)state;
	World_update(&adv_state->world, delta);
}

GameStateDefinition const AdventureStateDef =
{
	AdventureState_create,
	AdventureState_destroy,
	AdventureState_update
};
