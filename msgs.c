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

static void
postirc (Data *data, gchar *buf, gint r, gboolean bold, gboolean italics,
	gboolean underline, gchar *fg, gchar *bg)
{
	GtkTextIter end;
	GtkAdjustment *adj;

	gtk_text_buffer_get_end_iter (data->buffer, &end);
	gtk_text_buffer_insert_with_tags_by_name (data->buffer, &end, buf, r,
						bold ? "bold" : "unbold",
							fg, bg, NULL);

	adj = gtk_scrolled_window_get_vadjustment (
			GTK_SCROLLED_WINDOW (data->scrolled));
	gtk_adjustment_set_value (adj, gtk_adjustment_get_upper (adj));
	gtk_scrolled_window_set_vadjustment (
			GTK_SCROLLED_WINDOW (data->scrolled), adj);
}

static gboolean
parseirc (Data *data, gchar *buf, gint r)
{
	gboolean bold = FALSE;
	gboolean italics = FALSE;
	gboolean underline = FALSE;
	gint nick = FALSE;
	gchar *fg = g_strdup ("greenfg");
	gchar *bg = g_strdup ("whitebg");
	gchar *chunk = NULL;
	gchar *tmp = NULL;
	gint i = 0;

	while (buf[i]) {
		switch (buf[i]) {
		case 0x02:
			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			bold = !bold;
			break;
		case '<':
			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			g_free (fg);
			fg = g_strdup ("blackfg");
			postirc (data, "<", 1, bold, italics, underline, fg, bg);

			if (!nick) {
				g_free (fg);
				fg = g_strdup ("bluefg");
				nick++;
			}
			break;
		case '>':
			if (chunk) {
				if (data->nick && !g_strcmp0 (data->nick, chunk)) {
					g_free (fg);
					fg = g_strdup ("redfg");
				}

				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			if (nick == 1) {
				g_free (fg);
				fg = g_strdup ("blackfg");
				nick++;
			}

			postirc (data, ">", 1, bold, italics, underline, fg, bg);

			break;
		case '\n':
			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			bold = italics = underline = nick = FALSE;
			g_free(bg);
			bg = g_strdup ("whitebg");
			g_free (fg);
			fg = g_strdup ("greenfg");

			postirc (data, "\n", 1, bold, italics, underline, fg, bg);
			break;
		default:
			if (chunk == NULL) {
				chunk = g_strdup_printf ("%c", buf[i]);
			} else {
				tmp = g_strdup_printf ("%s%c", chunk, buf[i]);
				g_free (chunk);
				chunk = tmp;
			}

			break;
		}

		i++;
	}
}

gboolean
readircmsgs (gpointer *arg)
{
	Data *data = (Data*)arg;
	gint r;
	gchar buf[0x10000];

	r = read (data->out, buf, sizeof (buf));
	if (r > 0) {
		buf[r] = '\0';
		data->offs += r;

		parseirc (data, buf, r);
	} else {
		close (data->out);
		data->out = open ("out", O_RDONLY);
		lseek (data->out, data->offs, SEEK_SET);
	}

	return TRUE;
}
