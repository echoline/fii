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
	gchar *bg = g_strdup ("transbg");
	gchar *chunk = NULL;
	gchar *tmp = NULL;
	gchar tmpbuf[4] = { 0, 0, 0, 0 };
	gint c, d;
	gint i = 0;
	gboolean postcomma;

	while (buf[i]) {
		if (data->nick && !g_ascii_strncasecmp(&buf[i], data->nick,
						strlen (data->nick))) {
			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			postirc (data, &buf[i], strlen(data->nick), bold,
					italics, underline, "redfg", bg);

			i += strlen (data->nick);
		}

		switch (buf[i]) {
		case 0x0F:
			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			bold = italics = underline = FALSE;
			g_free(bg);
			bg = g_strdup ("transbg");
			g_free (fg);
			fg = g_strdup ("greenfg");

			break;
		case 0x03:
			postcomma = FALSE;

			if (chunk) {
				postirc (data, chunk, strlen(chunk), bold,
					italics, underline, fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			g_free (bg);
			g_free (fg);

			c = d = -1;
			i++;
			if (g_ascii_isdigit(buf[i])) {
				tmpbuf[0] = buf[i];
				tmpbuf[1] = '\0';
				i++;
				if (g_ascii_isdigit(buf[i])) {
					tmpbuf[1] = buf[i];
					tmpbuf[2] = '\0';
					i++;
				}

				c = atoi (tmpbuf);
			} 
			if (buf[i] == ',') {
				i++;
				if (g_ascii_isdigit(buf[i])) {
					tmpbuf[0] = buf[i];
					tmpbuf[1] = '\0';
					i++;
					if (g_ascii_isdigit(buf[i])) {
						tmpbuf[1] = buf[i];
						tmpbuf[2] = '\0';
						i++;
					}

					d = atoi (tmpbuf);
				} else {
					postcomma = TRUE;
				}
			}

			switch (d) {
			default:
			case 0:
				bg = g_strdup ("whitebg");
				break;
			case 1:
				bg = g_strdup ("blackbg");
				break;
			case 2:
				bg = g_strdup ("bluebg");
				break;
			case 3:
				bg = g_strdup ("greenbg");
				break;
			case 4:
				bg = g_strdup ("redbg");
				break;
			case 5:
				bg = g_strdup ("brownbg");
				break;
			case 6:
				bg = g_strdup ("purplebg");
				break;
			case 7:
				bg = g_strdup ("orangebg");
				break;
			case 8:
				bg = g_strdup ("yellowbg");
				break;
			case 9:
				bg = g_strdup ("lightgreenbg");
				break;
			case 10:
				bg = g_strdup ("cyanbg");
				break;
			case 11:
				bg = g_strdup ("lightcyanbg");
				break;
			case 12:
				bg = g_strdup ("lightbluebg");
				break;
			case 13:
				bg = g_strdup ("pinkbg");
				break;
			case 14:
				bg = g_strdup ("graybg");
				break;
			case 15:
				bg = g_strdup ("lightgraybg");
				break;
			}

			switch (c) {
			case 0:
				fg = g_strdup ("whitefg");
				break;
			default:
			case 1:
				fg = g_strdup ("blackfg");
				break;
			case 2:
				fg = g_strdup ("bluefg");
				break;
			case 3:
				fg = g_strdup ("greenfg");
				break;
			case 4:
				fg = g_strdup ("redfg");
				break;
			case 5:
				fg = g_strdup ("brownfg");
				break;
			case 6:
				fg = g_strdup ("purplefg");
				break;
			case 7:
				fg = g_strdup ("orangefg");
				break;
			case 8:
				fg = g_strdup ("yellowfg");
				break;
			case 9:
				fg = g_strdup ("lightgreenfg");
				break;
			case 10:
				fg = g_strdup ("cyanfg");
				break;
			case 11:
				fg = g_strdup ("lightcyanfg");
				break;
			case 12:
				fg = g_strdup ("lightbluefg");
				break;
			case 13:
				fg = g_strdup ("pinkfg");
				break;
			case 14:
				fg = g_strdup ("grayfg");
				break;
			case 15:
				fg = g_strdup ("lightgrayfg");
				break;
			}

			if (postcomma) 
				postirc (data, ",", 1, bold, italics,
					underline, fg, bg);
			i--;
			break;
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
			fg = g_strdup ("whitefg");
			postirc (data, "<", 1, bold, italics, underline, fg, bg);

			if (!nick) {
				g_free (fg);
				fg = g_strdup ("lightbluefg");
				nick++;
			}
			break;
		case '>':
			if (chunk) {
				if (nick < 2 && data->nick &&
				    !g_strcmp0 (data->nick, chunk)) {
					g_free (fg);
					fg = g_strdup ("redfg");
				}

				postirc (data, chunk, strlen(chunk),
					bold, italics, underline,
					fg, bg);
				g_free (chunk);
				chunk = NULL;
			}

			if (nick == 1) {
				g_free (fg);
				fg = g_strdup ("whitefg");
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
			bg = g_strdup ("transbg");
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
