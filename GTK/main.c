#include <gtk/gtk.h>

static void get_line (GtkButton *button, gpointer user_data)
{
  GtkEntry *entry = GTK_ENTRY (user_data);
  const gchar *text = gtk_entry_get_text (entry);
  g_print ("You write: %s\n", text);
}

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *entry;
  GtkWidget *box;
  GtkWidget *grid;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);

  grid = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (window), grid);

  entry = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry), "Имя первого COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 0, 0, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry, TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("Запуск COM");
  g_signal_connect (button, "clicked", G_CALLBACK (get_line), entry);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 5, 1);
  
  gtk_widget_show_all (window);
}

int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gnome.MyApp.Devel", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}