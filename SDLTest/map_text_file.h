#ifndef MAP_TEXT_FILE_H
#define MAP_TEXT_FILE_H


#include <stdio.h>


struct Map;
struct TileKind;

int load_map_from_text(struct Map *map, struct TileKind const *tile_kinds, size_t tile_kind_count, FILE *in, FILE *error_out);
void save_map_to_text(struct Map const *map, struct TileKind const *tile_kinds, FILE *out);


#endif
