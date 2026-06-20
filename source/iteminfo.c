#include "iteminfo.h"

static ItemInfo items[256] =
{
	[1] = {.name="Master Ball", .filename="master.png"},
	[2] = {.name="Ultra Ball", .filename="ultra.png"},
	[3] = {.name="Great Ball", .filename="great.png"},
	[4] = {.name="Poke Ball", .filename="poke.png"},
};

ItemInfo get_item_info(int id)
{
	return items[id];
}
