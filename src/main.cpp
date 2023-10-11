#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>

int main()
{
    Display *display = XOpenDisplay(NULL);
    XEvent ev;

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
    XSelectInput(display, window, ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask);

	XStoreName(display, window, "waffle engine");

	XClearWindow(display, window);
	XMapRaised(display, window);

	XWindowAttributes attribs;
    XGetWindowAttributes(display, window, &attribs);
	
    // Resize window
    unsigned int change_values = CWWidth | CWHeight;
    XWindowChanges values;
    values.width = 800;
    values.height = 600;
    XConfigureWindow(display, window, change_values, &values);

    int x, y;
    char str[25] = {0}; 
    KeySym keysym = 0;
    int len = 0;

    int quit = 0;
    while(!quit) {
       XNextEvent(display, &ev);
		switch(ev.type) {
			case Expose: {
				XGetWindowAttributes(display, window, &attribs);
			}break;
			case KeymapNotify:
                XRefreshKeyboardMapping(&ev.xmapping);
            break;
            case KeyPress:
                len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
                if (len > 0) {
                    std::cout << "Key pressed: " << str << " - " << len << " - " << keysym <<'\n';
                }
                if (keysym == XK_Escape) {
                    quit = 1;
                }
            break;
            case KeyRelease:
                len = XLookupString(&ev.xkey, str, 25, &keysym, NULL);
                if (len > 0) {
                    std::cout << "Key released: " << str << " - " << len << " - " << keysym <<'\n';
                }
            break;
			case ButtonPress:
				if (ev.xbutton.button == 1) {
					std::cout << "Left mouse down\n";
				}
				else if (ev.xbutton.button == 2) {
					std::cout << "Middle mouse down\n";
				}
				else if (ev.xbutton.button == 3) {
					std::cout << "Right mouse down\n";
				}
				else if (ev.xbutton.button == 4) {
					std::cout << "Mouse scroll up\n";
				} 
				else if (ev.xbutton.button == 5) {
					std::cout << "Mouse scroll down\n";
				}
			break;
			case ButtonRelease:
				if (ev.xbutton.button == 1) {
					std::cout << "Left mouse up\n";
				}
				else if (ev.xbutton.button == 2) {
					std::cout << "Middle mouse up\n";
				}
				else if (ev.xbutton.button == 3) {
					std::cout << "Right mouse up\n";
				}
			break;
			case MotionNotify:
				x = ev.xmotion.x;
				y = ev.xmotion.y;
				std::cout << "Mouse X:" << x << ", Y: " << y << "\n";
			break;
			case EnterNotify:
				std::cout << "Mouse enter\n";
			break;
			case LeaveNotify:
				std::cout << "Mouse leave\n";
			break;
		} 
    }

    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
