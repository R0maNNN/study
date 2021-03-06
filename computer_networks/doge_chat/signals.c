#include "signals.h"

void send_msg() {
	GtkTextIter start, end;
	gchar *msg;

	gtk_text_buffer_get_bounds(buffer_send, &start, &end);
	msg = gtk_text_buffer_get_text(buffer_send, &start, &end, FALSE);

	gtk_text_buffer_insert_at_cursor(buffer_chat, "Me: ", 4);
	gtk_text_buffer_insert_at_cursor(buffer_chat, msg, strlen(msg));
	gtk_text_buffer_insert_at_cursor(buffer_chat, "\n", 1);

	for (char *g = strtok(msg, "$"); g != NULL; g = strtok(NULL, "$")) {

		Type type;

		if (!strcmp(g, "!list")) {
			type = TYPE__list;
		} else if (!strcmp(g, "!dc")) {
			type = TYPE__dc;
		} else {
			type = TYPE__text;
		}

		char **smsg = (char **)alloca(sizeof(char*));
		smsg[0] = g;
		send_message(socket_fd, smsg, 1, type);
	}

	gtk_text_buffer_delete(buffer_send, &start, &end);
}

void disconnect_user() {
	if (socket_fd) {
		shutdown(socket_fd, 1);
		close(socket_fd);
	}
}

void save_dialog_open() {
	GtkWidget *dialog;
	GtkFileChooser *chooser;

	FILE *f;
	char *filename, *msg;

	GtkTextIter start, end;

	dialog = gtk_file_chooser_dialog_new(
										"Save File",
										GTK_WINDOW(window),
										GTK_FILE_CHOOSER_ACTION_SAVE,
										"Cancel",
										GTK_RESPONSE_CANCEL,
										"Save",
										GTK_RESPONSE_ACCEPT,
										NULL);
	chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);
	gtk_file_chooser_set_current_name (chooser, "History.txt");

	switch (gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		case GTK_RESPONSE_ACCEPT:
			filename = gtk_file_chooser_get_filename(chooser);
			f = fopen(filename, "w");

			gtk_text_buffer_get_bounds(buffer_chat, &start, &end);
			msg = (char *) gtk_text_buffer_get_text(buffer_chat, &start, &end,
				FALSE);

			fprintf(f, "%s", msg);
			fclose(f);
			g_free(filename);
	}

	gtk_widget_destroy (dialog);
}


void login_dialog_open() {
	switch (gtk_dialog_run(dialog_login))
	{
		case GTK_RESPONSE_OK:
			login = (char *) gtk_entry_buffer_get_text(buffer_login);
			snprintf(main_title, 50, "%s - %s", _TITLE_, login);
			gtk_window_set_title(GTK_WINDOW(window), main_title);
			break;
	}

	gtk_widget_hide(GTK_WIDGET(dialog_login));
}

void login_dialog_cancel() {
	gtk_dialog_response(dialog_login, GTK_RESPONSE_CANCEL);
}

void login_dialog_ok() {
	gtk_dialog_response(dialog_login, GTK_RESPONSE_OK);
}

void connect_dialog_open() {
	char state[50];
	switch (gtk_dialog_run(dialog_connect))
	{
		case GTK_RESPONSE_OK:
			disconnect_user();
			clean_buffer_chat();
			socket_fd = clients->fd = init_connect((char *) gtk_entry_buffer_get_text(buffer_ip),
					atoi(gtk_entry_buffer_get_text(buffer_port)));
			send_message(socket_fd, &login, 1, TYPE__login);
			if (pthread_create(&listen_thread, NULL, listener, &id) != 0) {
				perror("Creating the listener thread");
			}
			snprintf(state, 50, "Online (%s:%d)", ip_addr, port_num);
			gtk_label_set_text(label_state, state);
			break;
	}
	gtk_label_set_text(label_waddr, "");
	gtk_widget_hide(GTK_WIDGET(dialog_connect));
}

void connect_dialog_cancel() {
	gtk_dialog_response(dialog_connect, GTK_RESPONSE_CANCEL);
}

void connect_dialog_connect() {
	int _port_num = atoi(gtk_entry_buffer_get_text(buffer_port));
	char *_ip_addr = (char *) gtk_entry_buffer_get_text(buffer_ip);
	if (inet_addr(_ip_addr) == INADDR_NONE || _port_num < 1024 || _port_num > 65536) {
		gtk_label_set_text(label_waddr, "Wrong IP or port");
		return;
	}
	ip_addr = _ip_addr;
	port_num = _port_num;
	gtk_dialog_response(dialog_connect, GTK_RESPONSE_OK);
}

void about_dialog_open() {
	switch (gtk_dialog_run(GTK_DIALOG(dialog_about)))
	{
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide(GTK_WIDGET(dialog_about));
			break;
	}
}
void about_dialog_close() {
	gtk_dialog_response(GTK_DIALOG(dialog_about), GTK_RESPONSE_CANCEL);
}

gboolean login_entry_key_press(GtkWidget* widget, GdkEventKey* pKey, gpointer userdata) {
	if (pKey->type == GDK_KEY_PRESS) {
			switch (pKey->keyval) {
			case GDK_KEY_Return:
				login_dialog_ok();
				return TRUE;
			}
		}
		return FALSE;
}

gboolean connect_entry_key_press(GtkWidget* widget, GdkEventKey* pKey, gpointer userdata) {
	if (pKey->type == GDK_KEY_PRESS) {
			switch (pKey->keyval) {
			case GDK_KEY_Return:
				connect_dialog_connect();
				return TRUE;
			}
		}
		return FALSE;
}

gboolean send_area_key_press(GtkWidget* widget, GdkEventKey* pKey, gpointer userdata) {
	if (pKey->type == GDK_KEY_PRESS) {
		switch (pKey->keyval) {
		case GDK_KEY_Return:
			send_msg();
			return TRUE;
		}
	}
	return FALSE;
}
