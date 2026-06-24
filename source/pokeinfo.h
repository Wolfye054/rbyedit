typedef struct
{
	char *name;
	char *filename;
} ItemInfo;

typedef struct
{
	char *name;
	char *filename;
} PokemonInfo;

ItemInfo get_item_info(int id);
PokemonInfo get_pokemon_info(int id);
