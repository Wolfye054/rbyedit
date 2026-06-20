#include "gtk_rbyedit.h"

// static void delete_item_list_entry(GtkButton *button, GtkWidget *entry)
// {
// 	int index;
// 	index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(entry), "index"));
// 
// 	for(int i = index; i < save_data.bag.count - 1; i++)
// 	{
// 		save_data.bag.entries[i] = save_data.bag.entries[i+1];
// 	}
// 	save_data.bag.count--;
// 
// 	bag_list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
// 	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(bag_list_scrolled), bag_list_vbox);
// 
// 	create_item_list_entries_from_save_data(bag_list_vbox, save_data);
// }

static void promt_new_item()
{
	return;
}

static void delete_item_tab_entry()
{
	return;
}

static void edit_item_count(GtkEditable *item_spin_button, List *item_list) 
{
	GtkWidget *item_entry = gtk_widget_get_ancestor(GTK_WIDGET(item_spin_button), GTK_TYPE_BOX);
	GtkWidget *tab_vbox = gtk_widget_get_ancestor(GTK_WIDGET(item_entry), GTK_TYPE_BOX);

	int i;

	GtkWidget *child = gtk_widget_get_first_child(tab_vbox);
	for(i = 0; child; i++)
	{
		if(child == item_entry)
		{
			break;
		}

		child = gtk_widget_get_next_sibling(child);
	}

	int count = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(item_spin_button));
	item_list->entries[i].count = count;
}

static void create_item_tab_entry(GtkWidget *tab_vbox, List *item_list, int id, int count)
{
	GtkWidget *entry_hbox;
	GtkWidget *item_image, *name_label;
	GtkWidget *count_spin_button, *remove_button;

	ItemInfo item = get_item_info(id);

	entry_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_hexpand(entry_hbox, TRUE);

	gchar *full_path = g_build_filename("..", "assets", item.filename, NULL);
	item_image = gtk_image_new_from_file(full_path);
	gtk_image_set_pixel_size(GTK_IMAGE(item_image), 64);
	gtk_box_append(GTK_BOX(entry_hbox), item_image);
	g_free(full_path);

	name_label = gtk_label_new(item.name);
	gtk_box_append(GTK_BOX(entry_hbox), name_label);

	count_spin_button = gtk_spin_button_new_with_range(1, 99, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(count_spin_button), count);
	g_signal_connect(count_spin_button, "value-changed", G_CALLBACK(edit_item_count), item_list);
	gtk_box_append(GTK_BOX(entry_hbox), count_spin_button);

	remove_button = gtk_button_new_from_icon_name("window-close-symbolic");
	g_signal_connect(remove_button, "clicked", G_CALLBACK(delete_item_tab_entry), entry_hbox);
	gtk_box_append(GTK_BOX(entry_hbox), remove_button);

	gtk_box_append(GTK_BOX(tab_vbox), entry_hbox);
}

void update_item_tab(GtkWidget *tab_scrolled, List *item_list)
{
	GtkWidget *tab_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(tab_scrolled), tab_vbox);

	for(int i = 0; i < item_list->count; i++)
	{
		ListEntry item = item_list->entries[i];
		create_item_tab_entry(tab_vbox, item_list, item.id, item.count);
	}

	GtkWidget *new_item_button;
	new_item_button = gtk_button_new_from_icon_name("list-add-symbolic");
	g_signal_connect(new_item_button, "clicked", G_CALLBACK(promt_new_item), NULL);
	gtk_box_append(GTK_BOX(tab_vbox), new_item_button);
}
