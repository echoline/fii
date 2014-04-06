#include <gtk/gtk.h>
#include <string.h>
#include <fcntl.h>
#include "dat.h"
#include "fns.h"

void
sendircmsg (GtkEntry *entry, gpointer arg)
{
	Data *data = (Data*)arg;
	const gchar *text = gtk_entry_get_text (entry);

	if (write (data->in, text, strlen (text)) < 0 ||
	    write (data->in, "\n", 1) < 0) {
		gtk_main_quit();
	}

	gtk_entry_set_text (entry, "");
}

gboolean
readircmsgs (gpointer *arg)
{
	GtkTextIter end;
	Data *data = (Data*)arg;
	int r;
	gchar buf[0x10000];
	GtkAdjustment *adj;

	r = read (data->out, buf, sizeof (buf));
	if (r > 0) {
		gtk_text_buffer_get_end_iter (data->buffer, &end);
		gtk_text_buffer_insert (data->buffer, &end, buf, r);

		adj = gtk_scrolled_window_get_vadjustment (
				GTK_SCROLLED_WINDOW (data->scrolled));
		gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj));
		gtk_scrolled_window_set_vadjustment (
				GTK_SCROLLED_WINDOW (data->scrolled), adj);

		data->offs += r;
	} else {
		close (data->out);
		data->out = open ("out", O_RDONLY);
		lseek (data->out, data->offs, SEEK_SET);
	}

	return TRUE;
}
