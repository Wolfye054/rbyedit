#include "gtk_rbyedit.h"

GFile *file;
uint8_t *save;
gsize length;
SaveData save_data;

GtkWidget *main_window;
GtkWidget *player_name_entry, *rival_name_entry, *money_entry;
GtkWidget *bag_list_scrolled, *pc_list_scrolled;
GtkWidget *bag_list_vbox, *pc_list_vbox;

static GtkWidget *create_save_edit_entry(GtkWidget *save_edits_vbox, char *name)
{
	GtkWidget *save_edit_vbox, *label, *entry;
	
	save_edit_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	label = gtk_label_new(name);
	entry = gtk_entry_new();
	gtk_widget_set_name(entry, name);
	gtk_box_append(GTK_BOX(save_edit_vbox), label);
	gtk_box_append(GTK_BOX(save_edit_vbox), entry);
	gtk_box_append(GTK_BOX(save_edits_vbox), save_edit_vbox);

	return entry;
}

// static void create_prompted_item(GtkButton *button, GtkWidget *window)
// {
// 	GtkWidget *box = gtk_window_get_child(GTK_WINDOW(window));
// 	GtkWidget *dropdown = gtk_widget_get_first_child(box);
// 
// 	int selection_index = gtk_drop_down_get_selected(GTK_DROP_DOWN(dropdown));
// 	selection_index++;
// 	int id;
// 	for(id = 1;; id++)
// 	{
// 		if(get_item_info(id).name && --selection_index == 0)
// 		{
// 			break;
// 		}
// 	}
// 
// 	int index = save_data.bag.count++;
// 	save_data.bag.entries[index].id = id;
// 	save_data.bag.entries[index].count = 1;
// 	update_item_tab(bag_list_scrolled, &save_data.bag);
// 
// 	gtk_window_destroy(GTK_WINDOW(window));
// }
// 
// static void promt_new_item(GtkButton *button, GtkWidget *list_vbox)
// {
// 	GtkWidget *window;
// 	GtkWidget *dropdown;
// 	GtkWidget *submit_button;
// 	GtkStringList *items;
// 
// 	items = gtk_string_list_new(NULL);
// 	for(int i = 1; i <= 256; i++)
// 	{
// 		if(get_item_info(i).name)
// 		{
// 			gtk_string_list_append(items, get_item_info(i).name);
// 		}
// 	}
// 
// 	dropdown = gtk_drop_down_new(G_LIST_MODEL(items), NULL);
// 	gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dropdown), TRUE);
// 
// 	window = gtk_window_new();
// 	gtk_window_set_title(GTK_WINDOW(window), "Choose Item");
// 	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
// 	gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(main_window));
// 
// 	submit_button = gtk_button_new_with_label("Submit");
// 	g_signal_connect(submit_button, "clicked", G_CALLBACK(create_prompted_item), window);
// 
// 	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
// 	gtk_window_set_child(GTK_WINDOW(window), box);
// 	gtk_box_append(GTK_BOX(box), dropdown);
// 	gtk_box_append(GTK_BOX(box), submit_button);
// 
// 	gtk_window_present(GTK_WINDOW(window));
// }

static void save_file()
{
	if(!save) return;

	save_data.player_name = gtk_editable_get_text(GTK_EDITABLE(player_name_entry));
	save_data.rival_name = gtk_editable_get_text(GTK_EDITABLE(rival_name_entry));
	save_data.money = atoi(gtk_editable_get_text(GTK_EDITABLE(money_entry)));

	update_save(save, save_data);
	g_file_replace_contents(file, (char *)save, length, NULL,
			0, G_FILE_CREATE_REPLACE_DESTINATION, NULL, NULL,
			NULL);
}

static void load_file(GObject *file_dialog, GAsyncResult *result, gpointer window)
{
	if(save)
	{
		g_free(save);
		save = NULL;
	}
	if(file)
	{
		g_object_unref(file);
		file = NULL;
	}

	GError *error = NULL;
	char str[12];

	file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(file_dialog), result, &error);

	if(!file)
	{
		return;
	}

	g_file_load_contents(file, NULL, (char **)&save, &length, NULL, &error); 

	if(error)
	{
		GtkAlertDialog *dialog = gtk_alert_dialog_new("An unexpected error occurred");
		gtk_alert_dialog_set_detail (dialog, error->message);
		gtk_alert_dialog_show (dialog, GTK_WINDOW (window));
		g_object_unref (dialog);
	}

	g_object_unref(file_dialog);

	save_data = get_save_data(save);

	gtk_editable_set_text(GTK_EDITABLE(player_name_entry), save_data.player_name);
	gtk_editable_set_text(GTK_EDITABLE(rival_name_entry), save_data.rival_name);

	snprintf(str, sizeof(str), "%d", save_data.money);
	gtk_editable_set_text(GTK_EDITABLE(money_entry), str);

	update_item_tab(bag_list_scrolled, &save_data.bag);
}

static void open_file(GtkWindow *window)
{
	GtkFileDialog *file_dialog;

	file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_open(file_dialog, window, 0, load_file, window);
}

static void app_activate(GApplication *app)
{
	GtkWidget *window;
	GtkWidget *vbox, *hbox;
	GtkWidget *save_edits_vbox;
	GtkWidget *toolbar;
	GtkWidget *openf_button, *savef_button;
	GtkWidget *notebook;
	GtkWidget *bag_tab_label, *pc_tab_label;

	window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "rby edit");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
	main_window = window;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
	gtk_window_set_child(GTK_WINDOW(main_window), vbox);

	toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	gtk_widget_add_css_class(toolbar, "toolbar");
	gtk_box_append(GTK_BOX(vbox), toolbar);

	openf_button = gtk_button_new_from_icon_name("document-open");
	g_signal_connect_swapped(openf_button, "clicked", G_CALLBACK(open_file), window);
	gtk_box_append(GTK_BOX(toolbar), openf_button);

	savef_button = gtk_button_new_from_icon_name("document-save");
	g_signal_connect(savef_button, "clicked", G_CALLBACK(save_file), NULL);
	gtk_box_append(GTK_BOX(toolbar), savef_button);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_hexpand(hbox, TRUE);
	gtk_widget_set_vexpand(hbox, TRUE);
	gtk_box_append(GTK_BOX(vbox), hbox);

	save_edits_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_start(save_edits_vbox, 16);
	gtk_box_append(GTK_BOX(hbox), save_edits_vbox);

	player_name_entry = create_save_edit_entry(save_edits_vbox, "Player Name");
	rival_name_entry = create_save_edit_entry(save_edits_vbox, "Rival Name");
	money_entry = create_save_edit_entry(save_edits_vbox, "Money");

	notebook = gtk_notebook_new();
	gtk_widget_set_hexpand(notebook, TRUE);
	gtk_widget_set_vexpand(notebook, TRUE);
	gtk_widget_set_margin_bottom(notebook, 16);
	gtk_widget_set_margin_start(notebook, 16);
	gtk_widget_set_margin_end(notebook, 16);
	gtk_box_append(GTK_BOX(hbox), notebook);

	bag_tab_label = gtk_label_new("Bag Items");
	bag_list_scrolled = gtk_scrolled_window_new();
	bag_list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(bag_list_scrolled), bag_list_vbox);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), bag_list_scrolled, bag_tab_label);

	pc_tab_label = gtk_label_new("Pc Items");
	pc_list_scrolled = gtk_scrolled_window_new();
	pc_list_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(pc_list_scrolled), pc_list_vbox);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pc_list_scrolled, pc_tab_label);


	gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **args)
{
	GtkApplication *app;
	int status;

	app = gtk_application_new("com.github.Wolfye054.rbyedit", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(app_activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, args);

	return status;
}
