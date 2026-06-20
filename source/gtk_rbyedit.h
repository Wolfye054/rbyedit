#include <gtk/gtk.h>
#include "rbyedit.h"
#include "iteminfo.h"

typedef struct
{
	GtkWidget *main_window;
	GtkWidget *player_name_entry, *rival_name_entry, *money_entry;
	GtkWidget *bag_list_scrolled, *pc_list_scrolled;
} AppWidgets;

void update_item_tab(GtkWidget *tab_scrolled, List *item_list);
