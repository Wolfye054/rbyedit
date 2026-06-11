#include <gtk/gtk.h>
#include "rbyedit.c"

GFile *file;
uint8_t *save;
gsize length;
SaveData save_data;
GtkWidget *player_name_entry;
GtkWidget *rival_name_entry;
GtkWidget *money_entry;

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

static void load_file(GtkFileDialog *file_dialog, GAsyncResult *result)
{
	if(save) g_free(save);

	GError *error = NULL;
	char str[12];

	file = gtk_file_dialog_open_finish(file_dialog, result, &error);
	g_file_load_contents(file, NULL, (char **)&save, &length, NULL, &error); 

	g_object_unref(file_dialog);

	save_data = get_save_data(save);

	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(player_name_entry)),
			save_data.player_name, -1);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(rival_name_entry)),
			save_data.rival_name, -1);

	snprintf(str, sizeof(str), "%d", save_data.money);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(money_entry)),
			str, -1);
}

static void open_file(GtkWindow *window)
{
	GtkFileDialog *file_dialog;

	file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_open(file_dialog, window, 0, (GAsyncReadyCallback)load_file, NULL);
}

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

static void app_activate(GApplication *app)
{
	GtkWidget *window;
	GtkWidget *vbox, *hbox;
	GtkWidget *save_edits_vbox;
	GtkWidget *toolbar;
	GtkWidget *openf_button, *savef_button;

	window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "rby edit");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_window_set_child(GTK_WINDOW(window), vbox);

	toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	gtk_box_append(GTK_BOX(vbox), toolbar);

	openf_button = gtk_button_new_with_label("open file");
	g_signal_connect_swapped(openf_button, "clicked", G_CALLBACK(open_file), window);
	gtk_box_append(GTK_BOX(toolbar), openf_button);

	savef_button = gtk_button_new_with_label("save file");
	g_signal_connect(savef_button, "clicked", G_CALLBACK(save_file), NULL);
	gtk_box_append(GTK_BOX(toolbar), savef_button);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_append(GTK_BOX(vbox), hbox);

	save_edits_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(hbox), save_edits_vbox);

	player_name_entry = create_save_edit_entry(save_edits_vbox, "Player Name");
	rival_name_entry = create_save_edit_entry(save_edits_vbox, "Rival Name");
	money_entry = create_save_edit_entry(save_edits_vbox, "Money Name");

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
