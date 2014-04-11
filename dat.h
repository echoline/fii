typedef struct Data {
	int in;
	int out;
	int offs;
	GtkWidget *scrolled;
	GtkTextBuffer *buffer;
	gchar *nick;
	GtkListStore *nicks;
	gboolean scrolling;
	GdkRGBA bg;
	GdkRGBA fg;
	GtkWidget *window;
	GtkWidget *textview;
	GtkWidget *entry;
	GtkEntryCompletion *completion;
} Data;
