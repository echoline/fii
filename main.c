#include <gtk/gtk.h>
#include <unistd.h>
#include <fcntl.h>
#include "dat.h"
#include "fns.h"

static gboolean
on_draw_textview (GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	cairo_set_source_rgba (cr, 0, 0, 0, 0.85);
	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint (cr);

	return FALSE;
}

static gboolean
on_button_press_event_scrollbar (GtkWidget *widget, GdkEventButton *event,
								gpointer arg)
{
	Data *data = (Data*)arg;

	data->scrolling = TRUE;

	return FALSE;
}

int
main(int argc, char **argv)
{
	GtkWidget *window, *grid, *entry, *textview, *scrollbar;
	PangoFontDescription *fontdesc;
	Data *data;
	GdkScreen *screen;
	GdkVisual *visual;
	gchar title[PATH_MAX];
	GtkEntryCompletion *completion;

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

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), title);
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);

	gtk_widget_set_app_paintable (window, TRUE);
	screen = gdk_screen_get_default ();
	visual = gdk_screen_get_rgba_visual (screen);
	if (visual != NULL && gdk_screen_is_composited (screen)) {
		gtk_widget_set_visual (window, visual);
	}

	grid = gtk_grid_new ();
	gtk_widget_set_hexpand (grid, TRUE);
	gtk_widget_set_vexpand (grid, TRUE);
	gtk_container_add (GTK_CONTAINER (window), grid);

	textview = gtk_text_view_new ();
	g_signal_connect(textview, "draw", G_CALLBACK(on_draw_textview), NULL); 
	gtk_text_view_set_editable (GTK_TEXT_VIEW (textview), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (textview), GTK_WRAP_WORD);
	fontdesc = pango_font_description_from_string ("monospace 10");
	gtk_widget_override_font (textview, fontdesc);
	pango_font_description_free (fontdesc);

	data->scrolled = gtk_scrolled_window_new (NULL, NULL);
	scrollbar = gtk_scrolled_window_get_vscrollbar (GTK_SCROLLED_WINDOW
							(data->scrolled));
	g_signal_connect (scrollbar, "button-press-event",
			G_CALLBACK (on_button_press_event_scrollbar), data);
	gtk_container_add (GTK_CONTAINER (data->scrolled), textview);
	gtk_widget_set_hexpand (data->scrolled, TRUE);
	gtk_widget_set_vexpand (data->scrolled, TRUE);
	gtk_grid_attach (GTK_GRID (grid), data->scrolled, 0, 0, 1, 1);

	data->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
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

	entry = gtk_entry_new ();
	gtk_widget_set_hexpand (entry, TRUE);
	gtk_widget_set_vexpand (entry, FALSE);
	g_signal_connect (entry, "activate", G_CALLBACK (sendircmsg), data);
	gtk_grid_attach (GTK_GRID (grid), entry, 0, 1, 1, 1);

	completion = gtk_entry_completion_new ();
	gtk_entry_set_completion (GTK_ENTRY (entry), completion);

	data->nicks = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_entry_completion_set_model (completion,
					GTK_TREE_MODEL (data->nicks));
	
	gtk_entry_completion_set_text_column (completion, 0);

	gtk_widget_show_all (window);

	gtk_main ();
}
