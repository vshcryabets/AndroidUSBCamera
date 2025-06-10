#include <iostream>
#include <string.h>
#include <unistd.h>

// #include "Source.h"
// #include "UvcCamera.h"
// #include "TestSource.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <iostream>

class GtkPreviewApplication
{
private:
    GtkWidget *window;
    GtkWidget *draw_area;
    GtkWidget *button;

    int width_;
    int height_;
    std::unique_ptr<uint8_t[]> draw_buffer_;
    size_t draw_buffer_size_;
    GtkApplication *app;
    int status;

private:
    static void staticActivate(GtkApplication *app, gpointer user_data)
    {
        static_cast<GtkPreviewApplication *>(user_data)->activate();
    }

    void activate()
    {
        window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "GTK4 C++ Sample");
        gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);

        //draw_area_ = gtk_drawing_area_new();
        // g_signal_connect(G_OBJECT(draw_area_), "draw", G_CALLBACK(&::Draw), this);

        button = gtk_button_new_with_label("Click Me!");
        gtk_window_set_child(GTK_WINDOW(window), button);
        gtk_window_present(GTK_WINDOW(window));
    }
public:
    GtkPreviewApplication()
    {
        app = gtk_application_new("org.vsh.gtk4preview", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(&staticActivate), this);
    }

    ~GtkPreviewApplication()
    {
        if (app)
        {
            g_object_unref(app);
        }
    }

    int run(int argc, char *argv[])
    {
        status = g_application_run(G_APPLICATION(app), argc, argv);
        return status;
    }
};

// static void
// draw(GtkWidget *widget, cairo_t *cr)
// {
//     cairo_format_t format = CAIRO_FORMAT_ARGB32;
//     cairo_surface_t *surface = cairo_image_surface_create_for_data(
//         draw_buffer_.get(), format, width_ * 2, height_ * 2,
//         cairo_format_stride_for_width(format, width_ * 2));
//     cairo_set_source_surface(cr, surface, 0, 0);
//     cairo_rectangle(cr, 0, 0, width_ * 2, height_ * 2);
//     cairo_fill(cr);
//     cairo_surface_destroy(surface);
// }

int main(int argc, char *argv[])
{
    GtkPreviewApplication app;
    int status = app.run(argc, argv);
    return status;
}