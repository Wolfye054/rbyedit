#include <stdint.h>

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
} SaveData;

SaveData get_save_data(uint8_t *save);
void update_save(uint8_t *save, SaveData save_data);
