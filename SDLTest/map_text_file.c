#include "map_text_file.h"
#include "map.h"
#include <assert.h>
#include <limits.h>
#include <string.h>


static char const * const VersionLine_0 = "Map_v0\n";


static int scan_size_t(FILE *in, size_t *value)
{
	unsigned buf_value;
	if (fscanf(in, "%u", &buf_value) > 0)
	{
		*value = buf_value;
		return 1;
	}
	return 0;
}

static int load_map_from_text_v0(struct Map *map, struct TileKind const *tile_kinds, size_t tile_kind_count, FILE *in, FILE *error_out)
{
	size_t width, height;
	size_t y;

	if (!scan_size_t(in, &width) ||
			!scan_size_t(in, &height))
	{
		fprintf(error_out, "Invalid map size\n");
		return 0;
	}

	if (!TileGrid_init(&map->terrain, width, height))
	{
		fprintf(error_out, "Could not initialize the tile grid\n");
		return 0;
	}

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
					goto fail;
				}

				if (tile_kind_id < UINT_MAX)
				{
					if (tile_kind_id >= tile_kind_count)
					{
						fprintf(stderr, "Invalid tile kind id %u\n", (unsigned)tile_kind_id);
						goto fail;
					}

					TileGrid_get(&map->terrain, x, y)->layers[i] = tile_kinds + tile_kind_id;
				}
			}
		}
	}

	return 1;

fail:
	TileGrid_free(&map->terrain);
	return 0;
}

int load_map_from_text(struct Map *map, struct TileKind const *tile_kinds, size_t tile_kind_count, FILE *in, FILE *error_out)
{
	char version[32];

	assert(map);
	assert(in);

	fgets(version, sizeof(version), in);
	if (!strcmp(version, VersionLine_0))
	{
		return load_map_from_text_v0(map, tile_kinds, tile_kind_count, in, error_out);
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
					fprintf(out, "%u ", (unsigned)kind_id);
				}
				else
				{
					fputs("-1 ", out);
				}
			}

			fputs(" ", out);
		}

		fputs("\n", out);
	}
}

void save_map_to_text(struct Map const *map, struct TileKind const *tile_kinds, FILE *out)
{
	assert(map);
	assert(out);

	fputs(VersionLine_0, out);
	save_tiles_to_text(&map->terrain, tile_kinds, out);
}
