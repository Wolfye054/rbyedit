#include "rbyedit.h"
#include <stdlib.h>
#include <string.h>

#define CHECKSUM_ADDR 0x3523
#define CHECKSUM_START_ADDR 0x2598
#define CHECKSUM_END_ADDR 0x3522

#define PLAYER_NAME_ADDR 0x2598
#define RIVAL_NAME_ADDR 0x25F6
#define MONEY_ADDR 0x25F3
#define BAG_ADDR 0x25C9
#define BOX_ITEMS_ADDR 0x27E6
#define POKE_PARTY_ADDR 0x2F2C
#define POKE_BOXES_ADDR_1 0x4000
#define POKE_BOXES_ADDR_2 0x6000
#define CURRENT_BOX_ADDR 0x284C
#define CURRENT_BOX_DATA_ADDR 0x30C0

// TODO: finish character sets for the ascii conversion functions

static uint8_t ascii_to_rby(uint8_t c)
{
	if((c >= 'A' && c <= 'Z') ||
   	   (c >= 'a' && c <= 'z'))
	{
		return c + 63;
	}

	return c;
}

static uint8_t rby_to_ascii(uint8_t c)
{
	if((c >= 0x80 && c <= 0x99) ||
   	   (c >= 0xA0 && c <= 0xB9))
	{
		return c - 63;
	}

	return c;
}

static uint16_t get_int16(uint8_t *save, int address)
{
	uint16_t value = *(uint16_t *)(save + address);
	value = __builtin_bswap16(value);
	return value;
}

static uint32_t get_int24(uint8_t *save, int address)
{
	// TODO: make this and get_int16 safe
	uint32_t value = *(uint32_t *)(save + address);
	value = __builtin_bswap32(value);
	value = value >> 8;
	return value;
}

static void set_derived_values(Pokemon *pokemon)
{
	return;
}

static Pokemon get_pokemon(uint8_t *save, int address)
{
	Pokemon pokemon;
	pokemon.id = save[address];
	pokemon.current_hp = get_int16(save, address + 0x01);
	pokemon.pc_level = save[address + 0x03];
	pokemon.status =  save[address + 0x04];
	pokemon.type1 = save[address + 0x05];
	pokemon.type2 = save[address + 0x06];
	pokemon.catch_rate = save[address + 0x07];
	pokemon.move1_id = save[address + 0x08];
	pokemon.move2_id = save[address + 0x09];
	pokemon.move3_id = save[address + 0x0A];
	pokemon.move4_id = save[address + 0x0B];
	pokemon.og_trainer_id = get_int16(save, address + 0x0C);
	pokemon.xp = get_int24(save, address + 0X0E);
	pokemon.hp_xp = get_int16(save, address + 0x11);
	pokemon.attack_xp = get_int16(save, address + 0x13);
	pokemon.defense_xp = get_int16(save, address + 0x15);
	pokemon.speed_xp = get_int16(save, address + 0x17);
	pokemon.special_xp = get_int16(save, address + 0x19);

	// each iv is four bits in size, and hp iv is composed of the
	// least significant bit of attack, defense, speed, and special ivs
	// in that order.
	uint16_t iv_values = get_int16(save, address + 0x1B);
	
	// TODO: check if these are the correct positions for iv values
	uint8_t attack_iv = (iv_values >> 12) & 0x0F;
	uint8_t defense_iv = (iv_values >> 8) & 0x0F;
	uint8_t speed_iv = (iv_values >> 4) & 0x0F;
	uint8_t special_iv = iv_values & 0x0F;

	pokemon.hp_iv =
		((attack_iv & 0x01) << 3)  |
		((defense_iv & 0x01) << 2) |
		((speed_iv & 0x01) << 1)   |
		(special_iv & 0x01);

	pokemon.attack_iv = attack_iv;
	pokemon.defense_iv = defense_iv;
	pokemon.speed_iv = speed_iv;
	pokemon.special_iv = special_iv;

	pokemon.move1_pp = save[address + 0x1D];
	pokemon.move2_pp = save[address + 0x1E];
	pokemon.move3_pp = save[address + 0x1F];
	pokemon.move4_pp = save[address + 0x20];

	// nickname and trainer name are stored seperatly from the pokemon data,
	// so the caller needs to set them.
	pokemon.nickname = NULL;
	pokemon.og_trainer_name = NULL;

	set_derived_values(&pokemon);

	return pokemon;
}

static List get_list(uint8_t *save, int address, int size)
{
	List list;
	list.count = save[address];
	list.entries = malloc(size * sizeof(ListEntry));

	address++;
	for(int i = 0; i < list.count; i++)
	{
		ListEntry entry;
		entry.id = save[address];
		entry.count = save[address + 1];
		list.entries[i] = entry;
		address += 2;
	}

	return list;
}

static int get_money(uint8_t *save)
{
	uint32_t bcd = get_int24(save, MONEY_ADDR);
	
	int money = 0;
	int multiplier = 1;

	while(bcd > 0)
	{
		money += multiplier * (bcd & 0x0F);
		bcd = bcd >> 4;
		multiplier *= 10;
	}

	return money;
}

static char *get_string(uint8_t *save, int address)
{
	int size;
	for(size = 0; save[address + size] != 0x50; size++);

	char *name = malloc(size + 1);

	int i;
	for(i = 0; i < size; i++)
	{
		name[i] = rby_to_ascii(save[address + i]);
	}
	name[i] = '\0';

	return name;
}

static PokemonParty get_party(uint8_t *save)
{
	PokemonParty party;
	int address;

	party.count = save[POKE_PARTY_ADDR];
	party.pokemon = malloc(6 * sizeof(Pokemon));

	address = POKE_PARTY_ADDR + 0x08;
	for(int i = 0; i < party.count; i++)
	{
		party.pokemon[i] = get_pokemon(save, address);
		address += 44;
	}
	
	address = POKE_PARTY_ADDR + 0x110;
	for(int i = 0; i < party.count; i++)
	{
		party.pokemon[i].og_trainer_name = get_string(save, address);
		address += 11;
	}

	address = POKE_PARTY_ADDR + 0x152;
	for(int i = 0; i < party.count; i++)
	{
		party.pokemon[i].nickname = get_string(save, address);
		address += 11;
	}

	return party;
}

static PokemonBox get_pokemon_box(uint8_t *save, int address)
{
	PokemonBox box;
	box.count = save[address];
	box.pokemon = malloc(20 * sizeof(Pokemon));

	address += 0x16;
	for(int i = 0; i < box.count; i++)
	{
		box.pokemon[i] = get_pokemon(save, address);
		address += 0x21;
	}

	return box;
}

static PokemonBox *get_pokemon_boxes(uint8_t *save)
{
	// TODO: currently getting trash data for derived values.
	PokemonBox *boxes = malloc(12 * sizeof(PokemonBox));

	// only the first 7 bits represent the box number, bit 8 checks whether the player
	// has changed boxes before.
	int active_box = save[CURRENT_BOX_ADDR] & 0x7F;

	int address = POKE_BOXES_ADDR_1;
	for(int i = 0; i < 12; i++)
	{
		// the boxes are split up in different banks and we need to jump
		// to the next one when we get to index 6
		if(i == 6) address = POKE_BOXES_ADDR_2;

		if(i == active_box)
		{
			boxes[i] = get_pokemon_box(save, CURRENT_BOX_DATA_ADDR);
		}
		else
		{
			boxes[i] = get_pokemon_box(save, address);
		}

		address += 0x462;
	}

	return boxes;
}

static void set_checksum(uint8_t *save)
{
	uint8_t checksum = 0;

	for(int addr = CHECKSUM_START_ADDR; addr <= CHECKSUM_END_ADDR; addr++)
	{
		checksum += save[addr];
	}

	save[CHECKSUM_ADDR] = ~checksum;
}

static void set_money(uint8_t *save, uint32_t amount)
{
	if(amount > 999999) amount = 999999;
	
	uint32_t bcd = 0;
	int shift = 0;
	while(amount > 0)
	{
		bcd |= (amount % 10) << shift;
		amount /= 10;
		shift += 4;
	}

	save[MONEY_ADDR] = bcd >> 16;
	save[MONEY_ADDR + 1] = bcd >> 8;
	save[MONEY_ADDR + 2] = bcd;
}

static void write_list(uint8_t *save, int address, List list)
{
	save[address] = list.count;
	int addr = address + 1;

	if(list.count == 0)
	{
		save[addr] = 0xFF;
	}
	else
	{
		for(int i = 0; i < list.count; i++)
		{
			ListEntry entry = list.entries[i];
			save[addr] = entry.id;
			save[addr + 1] = entry.count;
			addr += 2;
		}
	}

	save[address + (2 * list.count) + 1] = 0XFF;
}

static void write_string(uint8_t *save, int address, char *string)
{
	int i;
	for(i = 0; string[i] != '\0'; i++)
	{
		save[address + i] = ascii_to_rby(string[i]);
	}

	save[address + i] = 0x50;
}

void update_save(uint8_t *save, SaveData save_data)
{
	set_money(save, save_data.money);
	write_string(save, PLAYER_NAME_ADDR, save_data.player_name);
	write_string(save, RIVAL_NAME_ADDR, save_data.rival_name);
	write_list(save, BAG_ADDR, save_data.bag);
	write_list(save, BOX_ITEMS_ADDR, save_data.box_items);
	set_checksum(save);
}

SaveData get_save_data(uint8_t *save)
{
	// TODO: check if the passed save is a valid rby save file.
	// TODO: free old save_data memory
	SaveData save_data;
	
	save_data.rival_name = get_string(save, RIVAL_NAME_ADDR);
	save_data.player_name = get_string(save, PLAYER_NAME_ADDR);
	save_data.money = get_money(save);
	save_data.bag = get_list(save, BAG_ADDR, 20);
	save_data.box_items = get_list(save, BOX_ITEMS_ADDR, 50);
	save_data.party = get_party(save);
	save_data.pokemon_boxes = get_pokemon_boxes(save);

	return save_data;
}
