#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <cairo/cairo.h>

int write_png(XImage *img, const char* filename)
{
    int height = img->height;
    int width = img->width;
    unsigned char *data = (unsigned char *) img->data;

    int format = CAIRO_FORMAT_RGB24;
    int stride;
    cairo_surface_t *surface;

    stride = cairo_format_stride_for_width (format, width);
    surface = cairo_image_surface_create_for_data (data, format,
                                                   width, height,
                                                   stride);

    return cairo_surface_write_to_png (surface, filename);
}

Pixmap GetRootPixmap(Display* display, Window *root)
{
    Pixmap currentRootPixmap;
    Atom act_type;
    int act_format;
    unsigned long nitems, bytes_after;
    unsigned char *data = NULL;
    Atom _XROOTPMAP_ID;

    _XROOTPMAP_ID = XInternAtom(display, "_XROOTPMAP_ID", False);

    if (XGetWindowProperty(display, *root, _XROOTPMAP_ID, 0, 1, False,
                XA_PIXMAP, &act_type, &act_format, &nitems, &bytes_after,
                &data) == Success) {

        if (data) {
            currentRootPixmap = *((Pixmap *) data);
            XFree(data);
        }
    }

    return currentRootPixmap;
}

int main(int argc, const char *argv[])
{
    Window root;
    Display* display;
    XWindowAttributes attrs;
    Pixmap bg;
    XImage* img;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return 1;
    }

    display = XOpenDisplay(getenv("DISPLAY"));

    if (display == NULL) {
        fprintf(stderr, "cannot connect to X server %s\n",
                getenv("DISPLAY") ? getenv("DISPLAY") : "(default)");
        return 1;
    }

    root = RootWindow(display, DefaultScreen(display));
    XGetWindowAttributes(display, root, &attrs);


    bg = GetRootPixmap(display, &root);
    img = XGetImage(display, bg, 0, 0, attrs.width, attrs.height, ~0, ZPixmap);
    XFreePixmap(display, bg);

    if (!img) {
        fprintf(stderr, "Can't create ximage\n");
        return 1;
    }

    if (img->depth != 24) {
        fprintf(stderr, "Image depth is %d. Must be 24 bits.\n", img->depth);
        return 1;
    }

    if (write_png(img, argv[1])) {
        printf("JPEG file successfully written to %s\n", argv[1]);
    }
    XDestroyImage(img);

    return 0;
}
