#include <gtk/gtk.h>
#include <stdint.h>
#include "func.c"


// typedef struct 
// {
//   char *COMname;
//   GtkWidget *entry1;
//   GtkWidget *entry2;
// } EntryData;

gpointer startobr (gpointer user_data)
{
   prog_start(NULL, user_data);
}

void start_thread (GtkButton *button, gpointer user_data) {
    g_thread_new("prog_start_thread", startobr, user_data);
}

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *name1;
  GtkWidget *speed1;
  GtkWidget *name2;
  GtkWidget *speed2;
  GtkWidget *name3;
  GtkWidget *speed3;
  GtkWidget *zaderzka;
  GtkWidget *box;
  GtkWidget *grid;
  EntryData *data;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);

    grid = gtk_grid_new ();
    gtk_container_add (GTK_CONTAINER (window), grid);

    name1 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (name1), "Имя первого COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 0, 0, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), name1, TRUE, TRUE, 0);

    speed1 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (speed1), "Скорость первого COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 0, 1, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), speed1, TRUE, TRUE, 0);

    name2 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (name2), "Имя второго COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 6, 0, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), name2, TRUE, TRUE, 0);

    speed2 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (speed2), "Скорость второго COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 6, 1, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), speed2, TRUE, TRUE, 0);

    name3 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (name3), "Имя третьего COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 12, 0, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), name3, TRUE, TRUE, 0);

    speed3 = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (speed3), "Скорость третьего COM");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 12, 1, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), speed3, TRUE, TRUE, 0);

    zaderzka = gtk_entry_new ();
    gtk_entry_set_placeholder_text (GTK_ENTRY (zaderzka), "Задержка");
    box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_grid_attach (GTK_GRID (grid), box, 12, 2, 5, 1);
    gtk_box_pack_start (GTK_BOX (box), zaderzka, TRUE, TRUE, 0);

    data = g_new(EntryData, 1);
    data->name1 = name1;
    data->speed1 = speed1;
    data->name2 = name2;
    data->speed2 = speed2;
    data->name3 = name3;
    data->speed3 = speed3;
    data->zaderzka = zaderzka;

    button = gtk_button_new_with_label ("Запуск COM-портов");
    g_signal_connect (button, "clicked", G_CALLBACK (start_thread), data);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 3, 17, 1);

    button = gtk_button_new_with_label ("Завершить");
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
    gtk_grid_attach (GTK_GRID (grid), button, 0, 4, 17, 1);

    GtkWidget *text_view = gtk_text_view_new ();
    gtk_text_view_set_editable(GTK_TEXT_VIEW (text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD); 

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER (scrolled_window), text_view);

    gtk_grid_attach(GTK_GRID (grid), scrolled_window, 0, 5, 17, 5);
    gtk_widget_set_size_request(scrolled_window, 500, 300);

    global_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (text_view));

  gtk_widget_show_all (window);

}

int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  initialize();

  app = gtk_application_new ("org.gnome.MyApp.Devel", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  gtk_main ();

  return status;
}