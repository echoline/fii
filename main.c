#include <gtk/gtk.h>
#include <fcntl.h>
#include "dat.h"
#include "fns.h"

int
main(int argc, char **argv)
{
	GtkWidget *window, *grid, *entry, *textview;
	PangoFontDescription *fontdesc;
	Data *data;
	
	gtk_init (&argc, &argv);

	data = g_new0 (Data, 1);

	data->in = open ("in", O_WRONLY);
	data->out = open ("out", O_RDONLY);

	if (data->in <= 0 || data->out <= 0)
		return -1;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	grid = gtk_grid_new ();
	gtk_widget_set_hexpand (grid, TRUE);
	gtk_widget_set_vexpand (grid, TRUE);
	gtk_container_add (GTK_CONTAINER (window), grid);

	textview = gtk_text_view_new ();
	data->scrolled = gtk_scrolled_window_new (NULL, NULL);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD);
	fontdesc = pango_font_description_from_string ("monospace 10");
	gtk_widget_override_font (textview, fontdesc);
	pango_font_description_free (fontdesc);
	gtk_container_add (GTK_CONTAINER (data->scrolled), textview);
	gtk_widget_set_hexpand (data->scrolled, TRUE);
	gtk_widget_set_vexpand (data->scrolled, TRUE);
	gtk_grid_attach (GTK_GRID (grid), data->scrolled, 0, 0, 1, 1);
	data->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
	g_timeout_add (1, (GSourceFunc)readircmsgs, data);

	entry = gtk_entry_new ();
	gtk_widget_set_hexpand (entry, TRUE);
	gtk_widget_set_vexpand (entry, FALSE);
	g_signal_connect (entry, "activate", G_CALLBACK (sendircmsg), data);
	gtk_grid_attach (GTK_GRID (grid), entry, 0, 1, 1, 1);

	gtk_widget_show_all (window);

	gtk_main ();
}
