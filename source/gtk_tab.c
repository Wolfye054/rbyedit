#include "gtk_rbyedit.h"

static void add_promted_item(GtkButton *button, List *item_list)
{
	GtkWidget *promt_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	GtkWidget *vbox = gtk_window_get_child(GTK_WINDOW(promt_window));
	GtkWidget *dropdown = gtk_widget_get_first_child(vbox);
	GtkWidget *tab_scrolled = g_object_get_data(G_OBJECT(button), "tab-scrolled");

	int selection_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown));
	selection_index++;
	int id;
	for(id = 1;; id++)
	{
		if(get_item_info(id).name && --selection_index == 0)
		{
			break;
		}
	}

	int index = item_list->count++;
	item_list->entries[index].id = id;
	item_list->entries[index].count = 1;
	
	update_item_tab(tab_scrolled, item_list);
	gtk_window_destroy(GTK_WINDOW(promt_window));
}

static void promt_new_item(GtkButton *button, List *item_list)
{
	GtkWidget *promt_window, *main_window;
	GtkWidget *dropdown, *submit_button;
	GtkWidget *vbox;
	GtkWidget *tab_scrolled;
	GtkStringList *item_strings;

	item_strings = gtk_string_list_new(NULL);
	for(int i = 1; i <= 256; i++)
	{
		Info item = get_item_info(i);
		if(item.name)
		{
			gtk_string_list_append(item_strings, item.name);
		}
	}

	main_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	promt_window = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(promt_window), "Choose Item");
	gtk_window_set_modal(GTK_WINDOW(promt_window), TRUE);
	gtk_window_set_transient_for(GTK_WINDOW(promt_window), GTK_WINDOW(main_window));

 	dropdown = gtk_drop_down_new(G_LIST_MODEL(item_strings), NULL);
 	gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dropdown), TRUE);

	tab_scrolled = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_SCROLLED_WINDOW);
	submit_button = gtk_button_new_with_label("Submit");
	g_signal_connect(submit_button, "clicked", G_CALLBACK(add_promted_item), item_list);
	g_object_set_data(G_OBJECT(submit_button), "tab-scrolled", tab_scrolled);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_window_set_child(GTK_WINDOW(promt_window), vbox);
	gtk_box_append(GTK_BOX(vbox), dropdown);
	gtk_box_append(GTK_BOX(vbox), submit_button);

	gtk_window_present(GTK_WINDOW(promt_window));
}

static void delete_item(GtkButton *button, List *item_list)
{
	GtkWidget *item_entry = gtk_widget_get_parent(GTK_WIDGET(button));
	GtkWidget *item_spin_button = g_object_get_data(G_OBJECT(item_entry), "spin-button");

	GtkWidget *tab_scrolled = gtk_widget_get_ancestor(item_entry, GTK_TYPE_SCROLLED_WINDOW);
	int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item_spin_button), "item-index"));

	for(int i = index; i < item_list->count - 1; i++)
	{
		item_list->entries[i] = item_list->entries[i + 1];
	}
	item_list->count--;

	update_item_tab(tab_scrolled, item_list);
}

static void edit_item_count(GtkEditable *item_spin_button, List *item_list) 
{
	int index = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(item_spin_button), "item-index"));
	int count = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(item_spin_button));

	item_list->entries[index].count = count;
}

static void create_pokemon_tab_entry(GtkWidget *tab_vbox, Pokemon *pokemon_group,
	int index, Pokemon *pokemon)
{
	GtkWidget *entry_hbox;
	GtkWidget *pokemon_image, *name_label;

	Info pokemon_info = get_pokemon_info(pokemon->id);

	entry_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_hexpand(entry_hbox, TRUE);

	gchar *full_path = g_build_filename("..", "assets", "pokemon", pokemon_info.filename, NULL);
	GFile *file = g_file_new_for_path(full_path);
	pokemon_image = gtk_picture_new_for_file(file);
	gtk_picture_set_content_fit	(GTK_PICTURE(pokemon_image), GTK_CONTENT_FIT_CONTAIN);
	gtk_widget_set_size_request(pokemon_image, 68, 56);
	gtk_box_append(GTK_BOX(entry_hbox), pokemon_image);
	g_free(full_path);

	name_label = gtk_label_new(pokemon_info.name);
	gtk_box_append(GTK_BOX(entry_hbox), name_label);

	gtk_box_append(GTK_BOX(tab_vbox), entry_hbox);
}

static void create_item_tab_entry(GtkWidget *tab_vbox, List *item_list, int index,
		int id, int count)
{
	GtkWidget *entry_hbox;
	GtkWidget *item_image, *name_label;
	GtkWidget *count_spin_button, *remove_button;

	Info item = get_item_info(id);

	entry_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_hexpand(entry_hbox, TRUE);

	gchar *full_path = g_build_filename("..", "assets", "item", item.filename, NULL);
	item_image = gtk_image_new_from_file(full_path);
	gtk_image_set_pixel_size(GTK_IMAGE(item_image), 64);
	gtk_box_append(GTK_BOX(entry_hbox), item_image);
	g_free(full_path);

	name_label = gtk_label_new(item.name);
	gtk_box_append(GTK_BOX(entry_hbox), name_label);

	count_spin_button = gtk_spin_button_new_with_range(1, 99, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(count_spin_button), count);
	g_signal_connect(count_spin_button, "value-changed", G_CALLBACK(edit_item_count), item_list);
	g_object_set_data(G_OBJECT(count_spin_button), "item-index", GINT_TO_POINTER(index));
	gtk_box_append(GTK_BOX(entry_hbox), count_spin_button);
	g_object_set_data(G_OBJECT(entry_hbox), "spin-button", count_spin_button);

	remove_button = gtk_button_new_from_icon_name("window-close-symbolic");
	g_signal_connect(remove_button, "clicked", G_CALLBACK(delete_item), item_list);
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
		create_item_tab_entry(tab_vbox, item_list, i, item.id, item.count);
	}

	if(item_list->count < 20)
	{
		GtkWidget *new_item_button;
		new_item_button = gtk_button_new_from_icon_name("list-add-symbolic");
		g_signal_connect(new_item_button, "clicked", G_CALLBACK(promt_new_item), item_list);
		gtk_box_append(GTK_BOX(tab_vbox), new_item_button);
	}
}

void update_party_tab(GtkWidget *tab_scrolled, PokemonParty *party)
{
	GtkWidget *tab_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(tab_scrolled), tab_vbox);

	for(int i = 0; i < party->count; i++)
	{
		Pokemon pokemon = party->pokemon[i];
		create_pokemon_tab_entry(tab_vbox, party->pokemon, i, &pokemon);
	}
}

void update_pokemon_box_tab(GtkWidget *tab_scrolled, PokemonBox *boxes)
{
	GtkWidget *tab_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(tab_scrolled), tab_vbox);

	char buffer[10];
	for(int i = 0; i < 12; i++)
	{
		if(boxes[i].count > 0)
		{
			snprintf(buffer, sizeof(buffer), "Box %d", i + 1);
			GtkWidget *box_label = gtk_label_new(buffer);
			gtk_box_append(GTK_BOX(tab_vbox), box_label);

			for(int j = 0; j < boxes[i].count; j++)
			{
				Pokemon pokemon = boxes[i].pokemon[j];
				create_pokemon_tab_entry(tab_vbox, boxes[i].pokemon, i, &pokemon);
			}
		}
	}
}
