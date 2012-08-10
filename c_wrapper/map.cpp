#include "map.h"
#include <map>
#include <string>


extern "C"
{
	struct map_t
	{
		std::map<std::string, std::string> instance;
	};


	map_t *map_create()
	{
		return new map_t;
	}

	int map_insert(map_t *map, const char *key, const char *value)
	{
		map->instance[key] = value;
		return 1;
	}

	void map_erase(map_t *map, const char *key)
	{
		map->instance.erase(std::string(key));
	}

	size_t map_size(map_t *map)
	{
		return map->instance.size();
	}

	const char *map_find(map_t *map, const char *key)
	{
		const auto i = map->instance.find(key);
		return (i == map->instance.end()) ? 0 : i->second.c_str();
	}

	void map_destroy(map_t *map)
	{
		delete map;
	}
}
