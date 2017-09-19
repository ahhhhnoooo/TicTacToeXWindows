#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

void redraw(Display * display, Window frame_window, GC graphical_context,int* xborder,int* yborder,int* board)
{
	//Draw the border lines
	XDrawLine(display,frame_window,graphical_context, xborder[1],yborder[0],xborder[1],yborder[3]);
	XDrawLine(display,frame_window,graphical_context, xborder[2],yborder[0],xborder[2],yborder[3]);
	XDrawLine(display,frame_window,graphical_context, xborder[0],yborder[1],xborder[3],yborder[1]);
	XDrawLine(display,frame_window,graphical_context, xborder[0],yborder[2],xborder[3],yborder[2]);
	
	for(int x = 0; x < 3; ++x)
	{
		for(int y = 0; y < 3; ++y)
		{
			int index = (y * 3) + x;
			if(board[index] == 1)
			{
				XDrawLine(display,frame_window,graphical_context, xborder[x],yborder[y],xborder[x+1],yborder[y+1]);
				XDrawLine(display,frame_window,graphical_context, xborder[x],yborder[y+1],xborder[x+1],yborder[y]);
			}
			else if(board[index] == -1)
			{
				XDrawArc(display,frame_window,graphical_context, xborder[x],yborder[y],xborder[1],yborder[1],0,360*64);
			}
		}
	}
}

//See tutorial http://www.dreamincode.net/forums/topic/166837-linux-writing-our-first-x-windows-application/
int main (int argc, char *argv[])
{
    Display                 *display;
    Visual                  *visual;
    int                     depth;
    XSetWindowAttributes    frame_attributes;
    Window                  frame_window;
    XFontStruct             *fontinfo;
    XGCValues               gr_values;
    GC                      graphical_context;
    XEvent               event;
    int board[9];
    int xborder[4];
    int yborder[4];
    int turn;

//The first three lines of code open a connection to the X-Windows server and obtain a handle to the display, the visual type of display and the plane depth (Microsoft Windows uses a single plane display) of the display.
    display = XOpenDisplay(NULL);
    visual = DefaultVisual(display, 0);
    depth  = DefaultDepth(display, 0);
    
//The next two lines sets the background colour for the window we are about to open and then opens the window. This is very similar to the Microsoft CreateWindow API call.
    frame_attributes.background_pixel = XWhitePixel(display, 0);

    // create the application window
    frame_window = XCreateWindow(display, XRootWindow(display, 0),
                                 0, 0, 400, 400, 5, depth,
                                 InputOutput, visual, CWBackPixel,
                                 &frame_attributes);

//The next line sets the text in the title bar to 'Hello World Example' and is equivalent to the Microsoft SetWindowText API call.
    XStoreName(display, frame_window, "TicTacToe");

//The next line of code tells the X-Windows server the events that we are interested in for the window we have just created. Remember, because X-Windows is client/server based we have the ability to restrict the events that we receive for an individual window and thus reduce the amount of network traffic.
    XSelectInput(display, frame_window, ExposureMask | StructureNotifyMask|ButtonPressMask);

//The next four lines create a suitable font for our 'Hello World' text, and create a device context suitable for the display and window on that display. These are equivalent to Microsoft CreateFont and GetDC API calls.
    fontinfo = XLoadQueryFont(display, "10x20");
    gr_values.font = fontinfo->fid;
    gr_values.foreground = XBlackPixel(display, 0);
    graphical_context = XCreateGC(display, frame_window, 
                                  GCFont+GCForeground, &gr_values);

//The next line of code actually displays the frame window on the display and is equivalent to the Microsoft ShowWindow API call.
    XMapWindow(display, frame_window);

	xborder[0] = 0;
	yborder[0] = 0;

	//clear the board
	for(int i = 0; i < 9;++i)
	{
		board[i]=0;
	}

	turn = 1;
	
    	while ( 1 ) {
        	XNextEvent(display, (XEvent *)&event);
	        switch ( event.type ) {
		case Expose:
		{
			//Divide window into thirds
		        XWindowAttributes window_attributes;
		        XGetWindowAttributes(display, frame_window, &window_attributes);
			xborder[1] = window_attributes.width/3;
			xborder[2] = window_attributes.width*2/3;
			xborder[3] = window_attributes.width;
			yborder[1] = window_attributes.height/3;
			yborder[2] = window_attributes.height*2/3;
			yborder[3] = window_attributes.height;
	
			redraw(display, frame_window, graphical_context,xborder,yborder,board);
		}
		break;
		case ButtonPress:
		{
			int x = event.xbutton.x / xborder[1];
			int y = event.xbutton.y / yborder[1];
			int index = (y * 3) + x;
			if(board[index] == 0)
			{
				board[index] = turn;
				turn = turn * -1;
				redraw(display, frame_window, graphical_context,xborder,yborder,board);
				
				printf("turn %d\n",turn);
			}
		}
		break;
		default:
		break;
        }
    }
    return(0);
}

