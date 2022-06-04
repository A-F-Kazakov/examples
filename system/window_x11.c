#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>

Display *dis;
int screen;
Window win;
GC gc;

void init_x()
{
	unsigned long black, white;

	dis = XOpenDisplay((char *)0);
	screen = DefaultScreen(dis);
	black = BlackPixel(dis, screen);
	white = WhitePixel(dis, screen);

	Window wnd = DefaultRootWindow(dis);

	win = XCreateSimpleWindow(dis, wnd, 0, 0,	200, 300, 5, white, black);

	XSetStandardProperties(dis, win, "My Window", "HI!", None, NULL, 0, NULL);

	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);

	gc=XCreateGC(dis, win, 0, 0);        

	XSetBackground(dis, gc, white);
	XSetForeground(dis, gc, black);

	XClearWindow(dis, win);
	XMapRaised(dis, win);
}

void close_x()
{
	XFreeGC(dis, gc);
	XDestroyWindow(dis, win);
	XCloseDisplay(dis);	
}

int main()
{
	init_x();

	XEvent event;
	KeySym key;
	char text[255];

	while(1)
	{
		XNextEvent(dis, &event);
	
		if(event.type == Expose && event.xexpose.count == 0)
			XClearWindow(dis, win);

		if(event.type == KeyPress && XLookupString(&event.xkey, text, 255, &key, 0) == 1)
		{
			if(text[0] == 'q')
				break;

			printf("You pressed the %c key!\n", text[0]);
		}

		if(event.type == ButtonPress)
			printf("You pressed a button at (%i,%i)\n",
				event.xbutton.x, event.xbutton.y);
	}

	close_x();
}
