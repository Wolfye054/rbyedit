#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CHECKSUM_ADDR 0x3523
#define CHECKSUM_START_ADDR 0x2598
#define CHECKSUM_END_ADDR 0x3522

#define PLAYER_NAME_ADDR 0x2598

typedef struct
{
	uint8_t *data;
	int length;
} String;

void set_checksum(FILE* file)
{
	uint8_t checksum = 0;
	uint8_t buffer[1];

	for(int addr = CHECKSUM_START_ADDR; addr <= CHECKSUM_END_ADDR; addr++)
	{
		fseek(file, addr, SEEK_SET);
		fread(buffer, 1, 1, file);
		checksum += buffer[0];
	}

	buffer[0] = ~checksum;
	fseek(file, CHECKSUM_ADDR, SEEK_SET);
	fwrite(buffer, 1, 1, file);
}

void convert_ASCII(String string)
{
	for(int i = 0; i < string.length; i++)
	{
		uint8_t c = string.data[i];
		if((c >= 'A' && c <= 'Z') ||
	   	   (c >= 'a' && c <= 'z'))
		{
			string.data[i] = c + 63;
		}
	}
}

void write_string(FILE* file, int address, String string)
{
	uint8_t term[] = {0x50};
	fseek(file, address, SEEK_SET);
	fwrite(string.data, 1, string.length, file);
	fwrite(term, 1, 1, file);
}

int main(int argc, char *args[])
{
	FILE *file = fopen(args[1], "rb+");
	if(!file)
	{
		printf("no file found\n");
		return 1;
	}

	uint8_t name_data[] = "FFFFFF"; 
	String name;
	name.data = name_data;
	name.length = 6;

	convert_ASCII(name);
	write_string(file, PLAYER_NAME_ADDR, name);

	set_checksum(file); 
	return 0;
}
