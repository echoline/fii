#include <gtk/gtk.h>
#include <string.h>
#include "dat.h"

static void
on_activate_background (GtkMenuItem *item, gpointer arg)
{
	Data *data = (Data*)arg;
	GtkWidget *dialog = gtk_color_chooser_dialog_new ("Background Color",
						GTK_WINDOW (data->window));

	gtk_color_chooser_set_use_alpha (GTK_COLOR_CHOOSER (dialog), TRUE);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
		gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (dialog),
								&data->bg);

	gtk_widget_destroy (dialog);
}

static void
on_activate_foreground (GtkMenuItem *item, gpointer arg)
{
	GtkTextTagTable *table;
	GtkTextTag *tag;
	GtkTextIter start, end;
	Data *data = (Data*)arg;
	GtkWidget *dialog = gtk_color_chooser_dialog_new ("Foreground Color",
						GTK_WINDOW (data->window));

	gtk_color_chooser_set_use_alpha (GTK_COLOR_CHOOSER (dialog), FALSE);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK) {
		gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (dialog),
								&data->fg);

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
entry_append (Data *data, gchar *arg)
{
	const gchar *orig = gtk_entry_get_text (GTK_ENTRY (data->entry));
	gchar *new = g_strdup_printf ("%s%s", orig, arg);

	gtk_entry_set_text (GTK_ENTRY (data->entry), new);
	g_signal_emit_by_name (data->entry, "move-cursor", 
				GTK_MOVEMENT_BUFFER_ENDS, 1, FALSE, NULL);
}

static void
on_activate_bold (GtkMenuItem *item, gpointer arg)
{
	entry_append ((Data*)arg, "\x02");
}

static void
on_activate_italic (GtkMenuItem *item, gpointer arg)
{
	entry_append ((Data*)arg, "\x1D");
}

static void
on_activate_underline (GtkMenuItem *item, gpointer arg)
{
	entry_append ((Data*)arg, "\x1F");
}

static void
on_activate_colors (GtkMenuItem *item, gpointer arg)
{
	entry_append ((Data*)arg, "\x03");
}

static void
on_activate_ctcps (GtkMenuItem *item, gpointer arg)
{
	entry_append ((Data*)arg, "\x01");
}

void
on_populate_popup_entry (GtkEntry *entry, GtkWidget *widget, gpointer arg)
{
	GtkMenuShell *menu = GTK_MENU_SHELL (widget);
	GtkWidget *item;
	
	item = gtk_menu_item_new_with_label ("Bold");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_bold), arg);
	gtk_menu_shell_append (menu, item);

	item = gtk_menu_item_new_with_label ("Italic");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_italic),
									arg);
	gtk_menu_shell_append (menu, item);

	item = gtk_menu_item_new_with_label ("Underline");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_underline),
									arg);
	gtk_menu_shell_append (menu, item);

	item = gtk_menu_item_new_with_label ("Colors");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_colors),
									arg);
	gtk_menu_shell_append (menu, item);

	item = gtk_menu_item_new_with_label ("CTCPs");
	g_signal_connect (item, "activate", G_CALLBACK (on_activate_ctcps),
									arg);
	gtk_menu_shell_append (menu, item);

	gtk_widget_show_all (GTK_WIDGET (menu));
}

