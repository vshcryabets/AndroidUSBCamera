#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Source.h"
#include "TestSource.h"
#include "TestSourceYUV420.h"
#include "ImageUseCases.h"
#include "PullToPushSource.h"
#include "UvcSource.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <iostream>
#include "u8x8.h"

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

    bool convertToRgba = false;
    int width_;
    int height_;
    GtkApplication *app;
    int status;
    ConvertBitmapUseCase::Buffer draw_buffer;
    std::shared_ptr<auvc::OwnBufferFrame> lastFrame;
    std::shared_ptr<PullToPushSource> pullToPush;
    std::shared_ptr<PullSource> testSource;

    std::shared_ptr<ConvertBitmapUseCase> convertUseCase;
    ConvertBitmapUseCase::Buffer *rgbaBuffer = nullptr;
    Source::ProducingConfiguration captureConfig = {
        .width = 640,
        .height = 480,
        .fps = 10.0f};

private:

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
        if (lastFrame == nullptr) {
            // No frame to draw
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_rectangle(cr, 0, 0, width, height);
            cairo_fill(cr);
            return;
        }
        if (convertToRgba) {
            convertUseCase->convert(*rgbaBuffer,
                {
                    .buffer = lastFrame->getData(),
                    .capacity = lastFrame->getSize(),
                    .size = lastFrame->getSize(),
                    .width = captureConfig.width,
                    .height = captureConfig.height
                }
            );
        }

        cairo_format_t format = CAIRO_FORMAT_ARGB32;
        auto* buffer = lastFrame->getData();
        if (convertToRgba) {
            buffer = rgbaBuffer->buffer;
        }
        cairo_surface_t* surface = cairo_image_surface_create_for_data(
            buffer,
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

        GtkWidget *vbox;

        vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        gtk_window_set_child(GTK_WINDOW(window), vbox);

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
        testSource->startProducing(captureConfig).get();
        pullToPush->startProducing({}).get();
    }

public:
    GtkPreviewApplication(): testSource(nullptr)
    {
        app = gtk_application_new("org.vsh.gtk4preview", G_APPLICATION_DEFAULT_FLAGS);
        g_signal_connect(app, "activate", G_CALLBACK(&staticActivate), this);
    }

    ~GtkPreviewApplication()
    {
        this->pullToPush->stopProducing().get();
        this->pullToPush->close().get();
        this->testSource->stopProducing().get();
        this->testSource->close().get();
        if (rgbaBuffer) {
            delete[] rgbaBuffer->buffer;
            delete rgbaBuffer;
            rgbaBuffer = nullptr;
        }
        if (app)
        {
            g_object_unref(app);
        }
    }

    int run(int argc, char *argv[])
    {
        // Parse command line arguments for --testSourceYUV420
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--testSourceYUV420") == 0) {
                testSource = std::make_shared<TestSourceYUV420>(u8x8_font_amstrad_cpc_extended_f);
                convertUseCase = std::make_shared<ConvertYUV420ptoRGBAUseCase>();
                convertToRgba = true;
            } else if (strcmp(argv[i], "--uvcSource") == 0) {
                auto uvcSource = std::make_shared<UvcSource>();
                std::cout << "Opening /dev/video0" << std::endl;
                uvcSource->open(UvcSource::OpenConfiguration{
                    .dev_name = "/dev/video0"
                });
                testSource = uvcSource;
                convertUseCase = std::make_shared<ConvertYUYVtoRGBAUseCase>();
                convertToRgba = true;
            } else if (strcmp(argv[i], "--testSourceRGB") == 0) {
                testSource = std::make_shared<TestSource>(u8x8_font_amstrad_cpc_extended_f);
            }
        }
        if (convertToRgba) {
            size_t rgbaBufferSize = captureConfig.width * captureConfig.height * 4;
            rgbaBuffer = new ConvertBitmapUseCase::Buffer{
                .buffer = new uint8_t[rgbaBufferSize],
                .capacity = rgbaBufferSize,
                .size = 0,
                .width = 640,
                .height = 480};
        }
        if (testSource == nullptr) {
            std::cerr << "No test source specified. Use --testSourceYUV420 or --testSourceRGB" << std::endl;
            testSource = std::make_shared<TestSource>(u8x8_font_amstrad_cpc_extended_f);
        }

        pullToPush = std::make_shared<PullToPushSource>();
        PullToPushSource::OpenConfiguration config;
        config.pullSource = testSource;
        config.frameCallback = [&](const auvc::Frame &frame) {
            // Handle the frame data from the pull source
            if (lastFrame == nullptr) {
                lastFrame = std::make_shared<auvc::OwnBufferFrame>(frame.getWidth(),
                 frame.getHeight(),
                  frame.getFormat(), 
                  frame.getSize(), 
                  frame.getTimestamp());
            }
            *lastFrame = frame;
            gtk_widget_queue_draw(draw_area);
        };
        pullToPush->open(config);

        // Filter out our custom options before passing to GTK
        int gtk_argc = 1;
        char *gtk_argv[argc];
        gtk_argv[0] = argv[0];
        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--testSourceYUV420") != 0 && 
                strcmp(argv[i], "--testSourceRGB") != 0 &&
                strcmp(argv[i], "--uvcSource") != 0) {
            gtk_argv[gtk_argc++] = argv[i];
            }
        }
        status = g_application_run(G_APPLICATION(app), gtk_argc, gtk_argv);
        return status;
    }
};



int main(int argc, char *argv[])
{
    GtkPreviewApplication app;
    int status = app.run(argc, argv);
    return status;
}

