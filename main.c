#include <gtk/gtk.h>
#include <unistd.h>
#include <fcntl.h>
#include "dat.h"
#include "fns.h"

static gboolean
on_draw_textview (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	Data *data = (Data*)user_data;

	cairo_set_source_rgba (cr, data->bg.red, data->bg.green, data->bg.blue,
					data->bg.alpha);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);

	return FALSE;
}

static gboolean
on_key_press_event_entry (GtkEntry *entry, GdkEventKey *event,
						gpointer user_data)
{
	Data *data = (Data*)user_data;
	GtkTreeModel *empty = GTK_TREE_MODEL (gtk_list_store_new (1,
							G_TYPE_STRING));

	if (event->keyval == GDK_KEY_Tab) {
		gtk_entry_completion_set_model (data->completion,
						GTK_TREE_MODEL (data->nicks));
		gtk_entry_completion_insert_prefix (data->completion);
		gtk_entry_completion_complete (data->completion);

		return TRUE;
	}

	gtk_entry_completion_set_model (data->completion, empty);
	g_object_unref (empty);

	return FALSE;
}

static gboolean
nick_match_func (GtkEntryCompletion *completion, const gchar *unused, 
					GtkTreeIter *iter, gpointer user_data)
{
	Data *data = (Data*)user_data;
	gchar *nick;
	const gchar *text = gtk_entry_get_text (GTK_ENTRY (data->entry));
	gchar *tmptext = g_strdup (text);;
	gchar *newtext;
	gchar *key = tmptext;
	gchar *ptr = g_strrstr (tmptext, " ");

	if (ptr) {
		*ptr = '\0';
		key = ptr + 1;
	}

	gtk_tree_model_get (GTK_TREE_MODEL (data->nicks), iter, 0, &nick, -1);

	if (!g_ascii_strncasecmp (nick, key, g_utf8_strlen (key, -1))) {
		if (ptr)
			newtext = g_strdup_printf ("%s %s", tmptext, nick);
		else
			newtext = g_strdup (nick);

		g_free (tmptext);

		gtk_entry_set_text (GTK_ENTRY (data->entry), newtext);
		g_signal_emit_by_name (data->entry, "move-cursor",
					GTK_MOVEMENT_BUFFER_ENDS, 1, FALSE,
					NULL);

		g_free (newtext);
		return TRUE;
	}
	
	return FALSE;
}

int
main(int argc, char **argv)
{
	GtkWidget *grid;
	PangoFontDescription *fontdesc;
	Data *data;
	GdkScreen *screen;
	GdkVisual *visual;
	gchar title[PATH_MAX];
	gint status;
	GtkTextIter end;

	getcwd (title, PATH_MAX-1);

	gtk_init (&argc, &argv);
	
	data = g_new0 (Data, 1);

	if (argc > 1) {
		data->nick = g_strdup (argv[1]);
	}

	data->in = open ("in", O_WRONLY);
	data->out = open ("out", O_RDONLY);

	if (data->in <= 0 || data->out <= 0)
		return -1;

	data->bg = (GdkRGBA){0, 0, 0, 0.85};
	data->fg = (GdkRGBA){1, 1, 1, 1};

	data->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (data->window), title);
	g_signal_connect (data->window, "destroy", G_CALLBACK (gtk_main_quit),
									NULL);
	gtk_window_set_default_size (GTK_WINDOW (data->window), 800, 480);

	gtk_widget_set_app_paintable (data->window, TRUE);
	screen = gdk_screen_get_default ();
	visual = gdk_screen_get_rgba_visual (screen);
	if (visual != NULL && gdk_screen_is_composited (screen)) {
		gtk_widget_set_visual (data->window, visual);
	}

	grid = gtk_grid_new ();
	gtk_widget_set_hexpand (grid, TRUE);
	gtk_widget_set_vexpand (grid, TRUE);
	gtk_container_add (GTK_CONTAINER (data->window), grid);

	data->textview = gtk_text_view_new ();
	g_signal_connect (data->textview, "draw", G_CALLBACK(on_draw_textview),
									data);
	g_signal_connect (data->textview, "populate-popup",
			G_CALLBACK (on_populate_popup_textview), data);
	gtk_text_view_set_editable (GTK_TEXT_VIEW (data->textview), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (data->textview),
							GTK_WRAP_WORD_CHAR);
	fontdesc = pango_font_description_from_string ("monospace 10");
	gtk_widget_override_font (data->textview, fontdesc);
	pango_font_description_free (fontdesc);

	data->scrolled = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_add (GTK_CONTAINER (data->scrolled), data->textview);
	gtk_widget_set_hexpand (data->scrolled, TRUE);
	gtk_widget_set_vexpand (data->scrolled, TRUE);
	gtk_grid_attach (GTK_GRID (grid), data->scrolled, 0, 0, 1, 1);

	data->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (
							data->textview));
	gtk_text_buffer_get_end_iter (data->buffer, &end);
	gtk_text_buffer_create_mark (data->buffer, "end", &end, FALSE);
	gtk_text_buffer_create_tag(data->buffer, "defaultfg", "foreground-rgba", &data->fg, NULL); 
	gtk_text_buffer_create_tag(data->buffer, "redfg", "foreground", "red", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "orangefg", "foreground", "orange", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "yellowfg", "foreground", "yellow", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "greenfg", "foreground", "green", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "bluefg", "foreground", "blue", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "purplefg", "foreground", "purple", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "blackfg", "foreground", "black", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "whitefg", "foreground", "white", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "grayfg", "foreground", "gray", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "brownfg", "foreground", "brown", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightgreenfg", "foreground", "light green", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "cyanfg", "foreground", "cyan", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightcyanfg", "foreground", "light cyan", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightbluefg", "foreground", "light blue", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightgrayfg", "foreground", "light gray", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "pinkfg", "foreground", "pink", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "redbg", "background", "red", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "orangebg", "background", "orange", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "yellowbg", "background", "yellow", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "greenbg", "background", "green", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "bluebg", "background", "blue", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "purplebg", "background", "purple", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "blackbg", "background", "black", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "whitebg", "background", "white", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "transbg", "background-rgba", ((GdkRGBA){0,0,0,0}), NULL); 
	gtk_text_buffer_create_tag(data->buffer, "graybg", "background", "gray", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "brownbg", "background", "brown", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightgreenbg", "background", "light green", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "cyanbg", "background", "cyan", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightcyanbg", "background", "light cyan", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightbluebg", "background", "light blue", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "lightgraybg", "background", "light gray", NULL); 
	gtk_text_buffer_create_tag(data->buffer, "italics", "style", PANGO_STYLE_ITALIC, NULL);
	gtk_text_buffer_create_tag(data->buffer, "unitalics", "style", PANGO_STYLE_NORMAL, NULL);
	gtk_text_buffer_create_tag(data->buffer, "bold", "weight", PANGO_WEIGHT_BOLD, NULL);
	gtk_text_buffer_create_tag(data->buffer, "unbold", "weight", PANGO_WEIGHT_NORMAL, NULL);
	gtk_text_buffer_create_tag(data->buffer, "underline", "underline", PANGO_UNDERLINE_SINGLE);
	gtk_text_buffer_create_tag(data->buffer, "ununderline", "underline", PANGO_UNDERLINE_NONE);
	g_timeout_add (1, (GSourceFunc)readircmsgs, data);

	data->entry = gtk_entry_new ();
	gtk_widget_set_hexpand (data->entry, TRUE);
	gtk_widget_set_vexpand (data->entry, FALSE);
	g_signal_connect (data->entry, "activate", G_CALLBACK (sendircmsg),
								data);
	g_signal_connect (data->entry, "populate-popup",
			G_CALLBACK (on_populate_popup_entry), data);
	g_signal_connect (data->entry, "key-press-event",
			G_CALLBACK (on_key_press_event_entry), data);
	gtk_grid_attach (GTK_GRID (grid), data->entry, 0, 1, 1, 1);

	data->completion = gtk_entry_completion_new ();
	gtk_entry_set_completion (GTK_ENTRY (data->entry), data->completion);

	gtk_entry_completion_set_popup_completion (data->completion, FALSE);
	gtk_entry_completion_set_inline_completion (data->completion, TRUE);
	gtk_entry_completion_set_minimum_key_length (data->completion, 1);
	gtk_entry_completion_set_match_func (data->completion, nick_match_func,
								data, NULL);
	data->nicks = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_entry_completion_set_model (data->completion, NULL);
	
	gtk_entry_completion_set_text_column (data->completion, 0);

	gtk_widget_show_all (data->window);

	gtk_main ();
}
