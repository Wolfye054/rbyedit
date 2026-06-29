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
} PokemonBaseStats;

ItemInfo get_item_info(int id);
PokemonInfo get_pokemon_info(int id);
