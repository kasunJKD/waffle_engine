#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>

int main()
{
    Display *display = XOpenDisplay(NULL);
    XEvent event;

    int screen;
    screen = XDefaultScreen(display);

    Window window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                0, 0, 
                                1200, 800,
                                0, 0, 0);

            /* process window close event through event handler so XNextEvent does not fail */
    Atom del_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display, window, &del_window, 1);

    /* select kind of events we are interested in */
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    XMapWindow(display, window);

    int quit = 0;
    while(!quit) {
        XNextEvent(display, &event);

        switch (event.type) {
        case KeyPress: {
            printf("key pressed\n");
        }break;
        }

    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
