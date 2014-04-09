#include <gtk/gtk.h>
#include <string.h>
#include "dat.h"
#include "fns.h"

static void
on_background_red (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->bg.red = gtk_range_get_value (range);
}

static void
on_background_green (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->bg.green = gtk_range_get_value (range);
}

static void
on_background_blue (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->bg.blue = gtk_range_get_value (range);
}

static void
on_background_alpha (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->bg.alpha = gtk_range_get_value (range);
}

static void
on_activate_background (GtkMenuItem *item, gpointer arg)
{
	Data *data = (Data*)arg;
	GtkWidget *scale;
	GdkRGBA orig = data->bg;
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("Background",
					GTK_WINDOW (data->window),
					GTK_DIALOG_MODAL,
					"_OK", GTK_RESPONSE_ACCEPT,
					"_Cancel", GTK_RESPONSE_REJECT, NULL);
	GtkWidget *content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_background_red), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.red);
	gtk_container_add (GTK_CONTAINER (content), scale);

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_background_green), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.green);
	gtk_container_add (GTK_CONTAINER (content), scale);

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_background_blue), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.blue);
	gtk_container_add (GTK_CONTAINER (content), scale);

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_background_alpha), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.alpha);
	gtk_container_add (GTK_CONTAINER (content), scale);

	gtk_widget_show_all (content);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_REJECT)
		data->bg = orig;

	gtk_widget_destroy (dialog);
}

static void
on_foreground_red (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->fg.red = gtk_range_get_value (range);
}

static void
on_foreground_green (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->fg.green = gtk_range_get_value (range);
}

static void
on_foreground_blue (GtkRange *range, gpointer arg)
{
	Data *data = (Data*)arg;
	data->fg.blue = gtk_range_get_value (range);
}
static void
on_activate_foreground (GtkMenuItem *item, gpointer arg)
{
	GtkTextIter start, end;
	Data *data = (Data*)arg;
	GtkWidget *scale;
	GdkRGBA orig = data->fg;
	GtkTextTagTable *table;
	GtkTextTag *tag;
	GtkWidget *dialog = gtk_dialog_new_with_buttons ("Background",
					GTK_WINDOW (data->window),
					GTK_DIALOG_MODAL,
					"_OK", GTK_RESPONSE_ACCEPT,
					"_Cancel", GTK_RESPONSE_REJECT, NULL);
	GtkWidget *content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_foreground_red), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.red);
	gtk_container_add (GTK_CONTAINER (content), scale);

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_foreground_green), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.green);
	gtk_container_add (GTK_CONTAINER (content), scale);

	scale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL,
						0, 1, 0.01);
	g_signal_connect (scale, "value-changed",
				G_CALLBACK (on_foreground_blue), arg);
	gtk_range_set_value (GTK_RANGE (scale), orig.blue);
	gtk_container_add (GTK_CONTAINER (content), scale);

	gtk_widget_show_all (content);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_REJECT)
		data->fg = orig;
	else {
		table = gtk_text_buffer_get_tag_table (data->buffer);
		tag = gtk_text_tag_table_lookup (table, "defaultfg");
		gtk_text_tag_table_remove (table, tag);

		gtk_text_buffer_create_tag (data->buffer, "defaultfg",
						"foreground-rgba", &data->fg);

		gtk_text_buffer_get_start_iter (data->buffer, &start);
		gtk_text_buffer_get_end_iter (data->buffer, &end);
		gtk_text_buffer_delete (data->buffer, &start, &end);
		data->offs = 0;
	}	

	gtk_widget_destroy (dialog);
}

void
on_populate_popup_textview (GtkTextView *textview, GtkWidget *widget,
				gpointer arg)
{
	GtkMenuShell *menu = GTK_MENU_SHELL (widget);
	GtkWidget *item;
	
	item = gtk_menu_item_new_with_label ("Background");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_background),
							arg);
	gtk_menu_shell_append (menu, item);

	item = gtk_menu_item_new_with_label ("Foreground");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_foreground),
							arg);
	gtk_menu_shell_append (menu, item);

	gtk_widget_show_all (GTK_WIDGET (menu));
}

static void
on_activate_bold (GtkMenuItem *item, gpointer arg)
{
	Data *data = (Data*)arg;
	const gchar *orig = gtk_entry_get_text (GTK_ENTRY (data->entry));
	gchar *new = g_strdup_printf ("%s\002", orig);

	gtk_entry_set_text (GTK_ENTRY (data->entry), new);
	g_signal_emit_by_name (data->entry, "move-cursor", 
				GTK_MOVEMENT_BUFFER_ENDS, 1, FALSE, NULL);
}

void
on_populate_popup_entry (GtkEntry *entry, GtkWidget *widget, gpointer arg)
{
	GtkMenuShell *menu = GTK_MENU_SHELL (widget);
	GtkWidget *item;
	
	item = gtk_menu_item_new_with_label ("Bold");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_bold), arg);
	gtk_menu_shell_append (menu, item);
	gtk_widget_show (item);
}

