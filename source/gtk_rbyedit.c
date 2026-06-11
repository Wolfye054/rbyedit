#include <gtk/gtk.h>
#include "rbyedit.c"

static void load_file(GtkFileDialog *file_dialog, GAsyncResult *result)
{
	GFile *file;
	GError *error = NULL;
	char *data = NULL;
	gsize length = 0;

	file = gtk_file_dialog_open_finish(file_dialog, result, &error);
	g_file_load_contents(file, NULL, &data, &length, NULL, &error); 

	g_object_unref(file_dialog);
	g_object_unref(file);

	SaveData save_data;
	save_data = get_save_data((uint8_t *)data);
	g_print("Player name: %s\nRival name: %s\nMoney: %d\n", save_data.player_name,
			save_data.rival_name, save_data.money);
}

static void open_file(GtkWindow *window)
{
	GtkFileDialog *file_dialog;

	file_dialog = gtk_file_dialog_new();
	gtk_file_dialog_open(file_dialog, window, 0, (GAsyncReadyCallback)load_file, NULL);
}

static GtkWidget *create_save_edit(char *name)
{
	GtkWidget *save_edit_vbox, *label, *entry;
	
	save_edit_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	label = gtk_label_new(name);
	entry = gtk_entry_new();
	gtk_box_append(GTK_BOX(save_edit_vbox), label);
	gtk_box_append(GTK_BOX(save_edit_vbox), entry);

	return save_edit_vbox;
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
	gtk_box_append(GTK_BOX(toolbar), savef_button);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_append(GTK_BOX(vbox), hbox);

	save_edits_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_append(GTK_BOX(hbox), save_edits_vbox);

	gtk_box_append(GTK_BOX(save_edits_vbox), create_save_edit("Player Name"));
	gtk_box_append(GTK_BOX(save_edits_vbox), create_save_edit("Rival Name"));
	gtk_box_append(GTK_BOX(save_edits_vbox), create_save_edit("Money"));

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
