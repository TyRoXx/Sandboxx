#include "world_text_file.h"
#include "world.h"
#include "algorithm.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>


static char const * const VersionLine_1 = "World_v1\n";


static Bool scan_size_t(FILE *in, size_t *value)
{
	unsigned buf_value;
	if (fscanf(in, "%u", &buf_value) > 0)
	{
		*value = buf_value;
		return 1;
	}
	return 0;
}

static Bool load_world_from_text_v1(struct World *world, struct TileKind const *tile_kinds, size_t tile_kind_count, FILE *in, FILE *error_out)
{
	size_t width, height;

	assert(world);
	assert(tile_kinds);
	assert(in);
	assert(error_out);

	if (!scan_size_t(in, &width) ||
		!scan_size_t(in, &height))
	{
		fprintf(error_out, "Invalid map size\n");
		return 0;
	}

	if (!TileGrid_init(&world->tiles, width, height))
	{
		fprintf(error_out, "Could not initialize the tile grid\n");
		return 0;
	}

	{
		size_t y;
		for (y = 0; y < height; ++y)
		{
			size_t x;
			for (x = 0; x < width; ++x)
			{
				size_t i;
				for (i = 0; i < TILE_LAYER_COUNT; ++i)
				{
					size_t tile_kind_id;
					if (!scan_size_t(in, &tile_kind_id))
					{
						fprintf(stderr, "Expected tile kind id\n");
						goto fail_1;
					}

					if (tile_kind_id < UINT_MAX)
					{
						if (tile_kind_id >= tile_kind_count)
						{
							fprintf(stderr, "Invalid tile kind id %u\n", (unsigned)tile_kind_id);
							goto fail_1;
						}

						TileGrid_get(&world->tiles, x, y)->layers[i] = tile_kinds + tile_kind_id;
					}
				}
			}
		}
	}

	{
		size_t entity_count;
		size_t i;
		if (!scan_size_t(in, &entity_count))
		{
			goto fail_1;
		}

		Vector_init(&world->movers);

		for (i = 0; i < entity_count; ++i)
		{
			int x, y;
			int direction;
			AppearanceId app;
			Entity entity;
			Mover mover;

			if (fscanf(in, "%d%d%d", &x, &y, &direction) != 3 ||
				!scan_size_t(in, &app))
			{
				goto fail_0;
			}

			if (Entity_init(&entity, Vector2i_new(x, y), app, world))
			{
				Mover_init(&mover, 1.7f, entity);

				if (Vector_push_back(&world->movers, &mover, sizeof(mover)))
				{
					continue;
				}

				Mover_free(&mover);
			}

			goto fail_0;
		}
	}

	return 1;

fail_0:
	free_movers(&world->movers);

fail_1:
	TileGrid_free(&world->tiles);
	return 0;
}

Bool load_world_from_text(struct World *world, struct TileKind const *tile_kinds, size_t tile_kind_count, FILE *in, FILE *error_out)
{
	char version[32];

	assert(world);
	assert(in);

	fgets(version, sizeof(version), in);
	if (!strcmp(version, VersionLine_1))
	{
		return load_world_from_text_v1(world, tile_kinds, tile_kind_count, in, error_out);
	}
	else
	{
		fprintf(error_out, "Unknown map version line\n");
		return 0;
	}
}


static void save_tiles_to_text(TileGrid const *tiles, struct TileKind const *tile_kinds, FILE *out)
{
	size_t const width = tiles->width;
	size_t const height = tiles->height;
	size_t y;

	fprintf(out, "%u %u\n", (unsigned)width, (unsigned)height);

	for (y = 0; y < height; ++y)
	{
		size_t x;
		for (x = 0; x < width; ++x)
		{
			size_t i;
			for (i = 0; i < TILE_LAYER_COUNT; ++i)
			{
				TileKind const * const layer = TileGrid_get(tiles, x, y)->layers[i];
				if (layer)
				{
					size_t const kind_id = (size_t)(layer - tile_kinds);
					fprintf(out, "%3u ", (unsigned)kind_id);
				}
				else
				{
					fputs(" -1 ", out);
				}
			}

			fputs(" ", out);
		}

		fputs("\n", out);
	}
}

static void save_entity_to_text(void *element, void *user)
{
	Mover const * const mover = element;
	FILE * const out = user;

	assert(mover);
	assert(out);

	fprintf(out, "%d %d\n",	(int)mover->body.position.x, (int)mover->body.position.y);
	fprintf(out, "%d\n", (int)mover->body.direction);
	fprintf(out, "%u\n", (unsigned)mover->body.appearance);

	fputs("\n", out);
}

static void save_entities_to_text(struct World const *world, FILE *out)
{
	fprintf(out, "%u\n\n", (unsigned)Vector_size(&world->movers));

	for_each(Vector_begin(&world->movers),
			 Vector_end(&world->movers),
			 sizeof(Mover),
			 save_entity_to_text,
			 out
			 );
}

void save_world_to_text(struct World const *world, struct TileKind const *tile_kinds, FILE *out)
{
	assert(world);
	assert(out);

	fputs(VersionLine_1, out);
	save_tiles_to_text(&world->tiles, tile_kinds, out);
	save_entities_to_text(world, out);
}
