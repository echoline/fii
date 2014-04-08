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
	GtkWidget *entry;
} Data;

typedef struct ListCBData {
	gboolean found;
	gchar *nick;
} ListCBData;
