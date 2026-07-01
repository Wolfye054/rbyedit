#include <stdint.h>

typedef enum 
{
	SLOW,
	MEDIUM_SLOW,
	MEDIUM_FAST,
	FAST
} GrowthRate;

typedef struct
{
	char *name;
	char *filename;
} Info;

typedef struct
{
	uint8_t hp;
	uint8_t attack;
	uint8_t defense;
	uint8_t speed;
	uint8_t special;
	GrowthRate growth_rate;
} PokemonBaseStats;

Info get_item_info(int id);
Info get_pokemon_info(int id);
PokemonBaseStats get_pokemon_base_stats(int id);
char *get_move_name(int id);
