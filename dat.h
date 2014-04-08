typedef struct Data {
	int in;
	int out;
	int offs;
	GtkWidget *scrolled;
	GtkTextBuffer *buffer;
	gchar *nick;
	GtkListStore *nicks;
	gboolean scrolling;
} Data;

typedef struct ListCBData {
	gboolean found;
	gchar *nick;
} ListCBData;
