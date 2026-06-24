#include <stdint.h>

typedef struct
{
	uint8_t id;
	uint16_t current_hp;
	uint8_t pc_level;
	uint8_t status;
	uint8_t type1;
	uint8_t type2;
	uint8_t catch_rate;
	uint8_t move1_id;
	uint8_t move2_id;
	uint8_t move3_id;
	uint8_t move4_id;
	uint16_t og_trainer_id;
	uint32_t xp;
	uint16_t hp_stat_xp;
	uint16_t attack_stat_xp;
	uint16_t defense_stat_xp;
	uint16_t speed_stat_xp;
	uint16_t special_stat_xp;
	uint16_t iv;
	uint8_t move1_pp;
	uint8_t move2_pp;
	uint8_t move3_pp;
	uint8_t move4_pp;
	uint8_t level;
	uint16_t max_hp;
	uint16_t attack;
	uint16_t defense;
	uint16_t speed;
	uint16_t special;
	char *nickname;
	char *og_trainer_name;
} Pokemon;

typedef struct
{
	int count;
	Pokemon *pokemon;
} PokemonParty;

typedef struct
{
	int id;
	int count;
} ListEntry;

typedef struct
{
	int count;
	ListEntry *entries;
} List;

typedef struct
{
	char *player_name;
	char *rival_name;
	int money;
	List bag;
	List box_items;
	PokemonParty party;
} SaveData;

SaveData get_save_data(uint8_t *save);
void update_save(uint8_t *save, SaveData save_data);
