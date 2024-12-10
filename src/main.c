#include <gtk/gtk.h>
#include <stdint.h>
#include "func.c"
#include <omp.h>


// typedef struct 
// {
//   char *COMname;
//   GtkWidget *entry1;
//   GtkWidget *entry2;
// } EntryData;

void thread1 (GtkButton *button, gpointer user_data) 
{
  g_thread_new("FirstCom", firstCOM, user_data);
}

void thread2 (GtkButton *button, gpointer user_data) 
{
  g_thread_new("SecondCom", secondCOM, user_data);
}

void thread3 (GtkButton *button, gpointer user_data) 
{
  g_thread_new("ThirdCom", COMWriter, user_data);
}

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *entry1;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *box;
  GtkWidget *grid;
  EntryData *data;
  EntryDataWrite *dataw;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  grid = gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (window), grid);

  // Создание кнопки для первого потока
  entry1 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry1), "Имя первого COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 0, 0, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry1, TRUE, TRUE, 0);

  entry2 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry2), "Скорость первого COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 0, 1, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry2, TRUE, TRUE, 0);

  data = g_new(EntryData, 1);
  data->entry1 = entry1;
  data->entry2 = entry2;

  button = gtk_button_new_with_label ("Запуск COM");
  g_signal_connect (button, "clicked", G_CALLBACK (thread1), data);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 2, 5, 1);
  
  entry1 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry1), "Имя второго COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 6, 0, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry1, TRUE, TRUE, 0);

  entry2 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry2), "Скорость второго COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 6, 1, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry2, TRUE, TRUE, 0);

  data = g_new(EntryData, 1);
  data->entry1 = entry1;
  data->entry2 = entry2;

  button = gtk_button_new_with_label ("Запуск COM");
  g_signal_connect (button, "clicked", G_CALLBACK (thread2), data);
  gtk_grid_attach (GTK_GRID (grid), button, 6, 2, 5, 1);

  entry1 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry1), "Имя третьего COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 12, 0, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry1, TRUE, TRUE, 0);

  entry2 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry2), "Скорость третьего COM");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 12, 1, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry2, TRUE, TRUE, 0);

  entry3 = gtk_entry_new ();
  gtk_entry_set_placeholder_text (GTK_ENTRY (entry3), "Задержка");
  box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_grid_attach (GTK_GRID (grid), box, 12, 2, 5, 1);
  gtk_box_pack_start (GTK_BOX(box), entry3, TRUE, TRUE, 0);

  dataw = g_new(EntryDataWrite, 1);
  dataw->entry1 = entry1;
  dataw->entry2 = entry2;
  dataw->entry3 = entry3;

  button = gtk_button_new_with_label ("Запуск COM");
  g_signal_connect (button, "clicked", G_CALLBACK (thread3), dataw);
  gtk_grid_attach (GTK_GRID (grid), button, 12, 3, 5, 1);

  button = gtk_button_new_with_label ("Завершить");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_grid_attach (GTK_GRID (grid), button, 0, 4, 17, 1);

  GtkWidget *text_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD); 

  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

  gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 5, 17, 5);
  gtk_widget_set_size_request(scrolled_window, 500, 300);

  global_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

  gtk_widget_show_all (window);

}

int WinMain (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  initialize();

  app = gtk_application_new ("org.gnome.MyApp.Devel", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}