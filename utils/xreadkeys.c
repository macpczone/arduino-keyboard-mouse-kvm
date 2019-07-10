#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include "keysims-x11.h"

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyUSB0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE;

void* reader_thread (void* fdin);
int wait_flag=TRUE;                    /* TRUE while no signal received */
pthread_mutex_t mutexsum;

int i, res, count = sizeof(keysims)/sizeof(keysims[0]);
unsigned char outcode;
unsigned char buf[255], out[2];

void dumpkeys (Display *display)
{
		KeyCode a;
		for (a = 0; a <= 255; a++) {
				printf( "Key Code number: %d ,", a );
        printf( "KeySymbol: %s,\n", XKeysymToString(XkbKeycodeToKeysym(display, a, 0, 0)));
    }
}

unsigned char getkeycode(KeyCode k)
{
		i = 0;
    while (i < count && k != keysims[i].x11keynum) {
       i++;
    }
   if (i < count) {
      return keysims[i].keyout;
   } else {
      return -1;
   }
}

void sendserial (int fd, unsigned char code, unsigned char pr)
{
		out[0] = pr;
		out[1] = code;
   pthread_mutex_lock (&mutexsum);
 		res = write(fd,&out,2);
            if(res < 0 ) {
                printf("Error Writing. Status=%d \n%s\n",errno, strerror(errno));
                //close(fd);
            }
    pthread_mutex_unlock (&mutexsum);
 }

int main()
{
    Display *display;
    Window window;
    XEvent event;
    Bool ar_set, ar_supp;
    int s;
    pthread_t tid;
    pthread_attr_t attr;

    int fd,c;
    struct termios oldtio,newtio;

    pthread_mutex_init(&mutexsum, NULL);
    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd <0) {perror(MODEMDEVICE); exit(-1); }

    tcgetattr(fd,&oldtio); /* save current port settings */
    cfmakeraw(&newtio); // set struct for new term settings to raw.  See man 'cfmakeraw'
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_lflag = ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_oflag = 0;
//    newtio.c_lflag = ICANON;
//    newtio.c_cc[VMIN]=1;
//    newtio.c_cc[VTIME]=0;
  sleep(2); //required to make flush work, for some reason
  tcflush(fd,TCIOFLUSH);
    tcsetattr(fd,TCSAFLUSH,&newtio);

    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, reader_thread, &fd);

    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(display);

    /* create window */
    window = XCreateSimpleWindow(display, RootWindow(display, s), 10, 10, 200, 200, 1,
                           BlackPixel(display, s), WhitePixel(display, s));

    /* select kind of events we are interested in */
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask );

    /* map (show) the window */
    XMapWindow(display, window);
    XAutoRepeatOff(display);
    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);

        /* keyboard events */
        if (event.type == KeyPress)
        {
            printf( "KeyPress: %d ", event.xkey.keycode );
            printf( "KeySymbol: %s\n", XKeysymToString(XkbKeycodeToKeysym(display, event.xkey.keycode, 0, 0)));
            outcode = getkeycode(event.xkey.keycode);
            if (outcode) {
            		printf( "Output Keycode: %d\n\n", outcode );
            }
            sendserial(fd, outcode, 1);
            /* exit on ESC key press */
            if ( event.xkey.keycode == 0x09 )
                break;
        }
        else if (event.type == KeyRelease)
        {
            printf( "KeyRelease: %d\n", event.xkey.keycode );
            outcode = getkeycode(event.xkey.keycode);
            if (outcode) {
            		printf( "Output Keycode: %d\n\n", outcode );
            }
            sendserial(fd, outcode, 0);
        }


    }

//		dumpkeys(display);

    /* close connection to server */
    XCloseDisplay(display);
    STOP = TRUE;
    pthread_join(tid, NULL);
    pthread_mutex_destroy(&mutexsum);
    tcsetattr(fd,TCSANOW,&oldtio);
    XAutoRepeatOn(display);
    return 0;
}

void* reader_thread (void* fdin)
{
    fd_set readfs;    /* file descriptor set */
    int *fd_ptr = (int*)fdin;
    int fd = *fd_ptr;
           struct timeval tv;
           int retval;

        while (STOP == FALSE) {
          FD_SET(fd, &readfs);  /* set testing for source 1 */
           /* Wait up to five seconds. */
           tv.tv_sec = 5;
           tv.tv_usec = 0;
          retval = select(fd + 1, &readfs, NULL, NULL, &tv);
           if (retval == -1) {
               perror("select()");
               break;
           } else if (retval == 0 && STOP == TRUE) {
           		printf("Exiting thread\n");
           		break;
           } else if (retval > 0) {
    pthread_mutex_lock (&mutexsum);
            res = read(fd,buf,255);
            if(res < 0 ) {
                printf("Error Reading. Status=%d \n%s\n",errno, strerror(errno));
                //close(fd);
            } else {
		            buf[res]=0;
//		          printf("Data in from Arduino:\n");
		            printf("%s", buf);
            }
    pthread_mutex_unlock (&mutexsum);
          }
        }
    pthread_exit(0);
}

