#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Source.h"
#include "TestSource.h"
#include "TestSourceYUV420.h"
#include "ImageUseCases.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <iostream>
#include "u8x8.h"

#define USE_YUV420_SOURCE

// Callback for GtkGestureClick on the drawing area
static void on_drawing_area_clicked(GtkGestureClick *gesture, int n_press, double x, double y, gpointer user_data)
{
    g_print("Drawing area clicked at (%.0f, %.0f)\n", x, y);
}

class GtkPreviewApplication
{
private:
    GtkWidget *window;
    GtkWidget *draw_area;
    GtkWidget *button;

    int width_;
    int height_;
    GtkApplication *app;
    int status;
    ConvertBitmapUseCase::Buffer draw_buffer;
#ifdef USE_YUV420_SOURCE    
    TestSourceYUV420 testSource;
#else
    TestSource testSource;
#endif
    ConvertYUV420ptoRGBAUseCase convertUseCase;
#ifdef USE_YUV420_SOURCE   
    ConvertBitmapUseCase::Buffer *rgbaBuffer;
#endif
private:
    static gboolean staticTimeout(gpointer user_data)
    {
        static_cast<GtkPreviewApplication *>(user_data)->onTimeout();
        return true; // Continue calling this function
    }

    static void staticActivate(GtkApplication *app, gpointer user_data)
    {
        static_cast<GtkPreviewApplication *>(user_data)->activate();
    }

    static void staticDraw(GtkDrawingArea *drawing_area,
                           cairo_t *cr, int width, int height,
                           gpointer user_data)
    {
        static_cast<GtkPreviewApplication *>(user_data)->draw(cr, width, height);
    }

    void draw(cairo_t *cr,
              int width,
              int height)
    {
        Source::Frame frame = testSource.readFrame();
        auto captureConfig = testSource.getCaptureConfiguration();
#ifdef USE_YUV420_SOURCE
        convertUseCase.convert(*rgbaBuffer,
            {
                .buffer = frame.data,
                .capacity = frame.size,
                .size = frame.size,
                .width = captureConfig.width,
                .height = captureConfig.height
            }
        );
#endif
        // // Write frame.data to a file (for example, as raw YUV420 data)
        // FILE *file = fopen("frame.yuv", "wb");
        // if (file) {
        //     fwrite(frame.data, 1, frame.size, file);
        //     fclose(file);
        // }
        // file = fopen("frame.rgb", "wb");
        // if (file) {
        //     fwrite(rgbaBuffer->buffer, 1, rgbaBuffer->size, file);
        //     fclose(file);
        // }

        cairo_format_t format = CAIRO_FORMAT_ARGB32;
        cairo_surface_t* surface = cairo_image_surface_create_for_data(
#ifdef USE_YUV420_SOURCE            
            rgbaBuffer->buffer,
#else
            frame.data,
#endif
            format, 
            captureConfig.width,
            captureConfig.height, 
            cairo_format_stride_for_width(format, captureConfig.width));
        cairo_set_source_surface(cr, surface, 0, 0);
        // cairo_set_source_rgb(cr, 0.0, 0.5, 0.0);
        cairo_rectangle(cr, 0, 0, captureConfig.width, captureConfig.height);
        cairo_fill(cr);
        cairo_surface_destroy(surface);
    }

    void activate()
    {
        window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(window), "GTK4 C++ Sample");
        gtk_window_set_default_size(GTK_WINDOW(window), 640, 480);
        // g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        GtkWidget *vbox;

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        gtk_window_set_child(GTK_WINDOW(window), vbox);
        // g_signal_connect(G_OBJECT(draw_area_), "draw", G_CALLBACK(&::Draw), this);

        button = gtk_button_new_with_label("Click Me!");

        gtk_box_append(GTK_BOX(vbox), button);
        // To make the button not expand, set its hexpand property to FALSE
        gtk_widget_set_hexpand(button, FALSE);           // Horizontal expand
        gtk_widget_set_halign(button, GTK_ALIGN_CENTER); // Center horizontally within its non-expanded space

        draw_area = gtk_drawing_area_new();
        gtk_widget_set_size_request(draw_area, 200, 200); // Minimum size
        gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), this->staticDraw, this, NULL);
        // Add a GtkGestureClick for button presses on the drawing area
        GtkGesture *gesture = gtk_gesture_click_new();
        // Connect the "pressed" signal for mouse clicks
        g_signal_connect(gesture, "pressed", G_CALLBACK(on_drawing_area_clicked), NULL);
        // Add the gesture to the drawing area
        gtk_widget_add_controller(draw_area, GTK_EVENT_CONTROLLER(gesture));
        gtk_box_append(GTK_BOX(vbox), draw_area);

        // To make the drawing area expand, set its vexpand property to TRUE
        gtk_widget_set_vexpand(draw_area, TRUE); // Vertical expand
        gtk_widget_set_hexpand(draw_area, TRUE); // Horizontal expand (to fill width too)

        gtk_window_present(GTK_WINDOW(window));

        g_timeout_add(33, this->staticTimeout, this);
    }

    void onTimeout()
    {
        gtk_widget_queue_draw(draw_area);
    }
public:
    GtkPreviewApplication(): testSource(u8x8_font_amstrad_cpc_extended_f)
    {
        app = gtk_application_new("org.vsh.gtk4preview", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(&staticActivate), this);
    }

    ~GtkPreviewApplication()
    {
#ifdef USE_YUV420_SOURCE        
        if (rgbaBuffer) {
            delete[] rgbaBuffer->buffer;
            delete rgbaBuffer;
            rgbaBuffer = nullptr;
        }
#endif        
        if (app)
        {
            g_object_unref(app);
        }
    }

    int run(int argc, char *argv[])
    {
        Source::CaptureConfiguration captureConfig = {
            .width = 640,
            .height = 480,
            .fps = 30.0f};
        testSource.startCapturing(captureConfig);
#ifdef USE_YUV420_SOURCE        
        size_t rgbaBufferSize = captureConfig.width * captureConfig.height * 4;
        rgbaBuffer = new ConvertBitmapUseCase::Buffer{
            .buffer = new uint8_t[rgbaBufferSize],
            .capacity = rgbaBufferSize,
            .size = 0,
            .width = 640,
            .height = 480};
#endif            
        status = g_application_run(G_APPLICATION(app), argc, argv);
        return status;
    }
};

int main(int argc, char *argv[])
{
    GtkPreviewApplication app;
    int status = app.run(argc, argv);
    return status;
}