// http://www.thelinuxdaily.com/2010/05/grab-raw-keyboard-input-from-event-device-node-devinputevent/
//
// fekerr 20180205 pi
// fekerr laptop 20180216 - manually updated
// fekerr hsw 20180217

// TODO: find if all these includes are really needed.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

#include <unistd.h> // for sleep().

// Strings from data defined in linux/input-event-codes.h
// TODO: a better way?

char *ev_types_str[EV_CNT] =
{
    "EV_SYN", // 0x00
    "EV_KEY", // 0x01
    "EV_REL", // 			0x02
    "EV_ABS", //			0x03
    "EV_MSC", // 			0x04
    "EV_SW", //			    0x05

    //06  07  08  09  0A  0B  0C  0D  0E, 0F, 10
    "", "", "", "", "", "", "", "", "", "", "",

    "EV_LED", //		0x11
    "EV_SND", //			0x12
    "", // 0x13
    "EV_REP", // 			0x14
    "EV_FF", //		    	0x15
    "EV_PWR", //			0x16
    "EV_FF_STATUS", //		0x17

    //18, 19, 1a, 1b, 1c, 1e
    "", "", "", "", "", "",
    
    "EV_MAX" //			0x1f
};

char *inv_str = "*INV*";

// key map for a few keys from KEY_ in linux/input-event-codes.h
// TODO: a better way?
// "unprintable" currently listed here as underscores

char ev_keymap_printable[]="__123456789-=_\tqwertyuiop{}__" \
    "asdfghjkl;\'__zxcvbnm,./";



//TODO: original lacked \n etc.
void handler (int sig)
{
    printf ("\nexiting...(%d)\n", sig);
    exit (0);
}

void perror_exit (char *error)
{
    perror (error);
    handler (9);
}

char *map_event(int ev_event)
{
    if (ev_event > 0 && ev_event < EV_MAX) 
    {
        return ev_types_str[ev_event];
    }
    else
    {
        return inv_str;
    }
}

char map_key(int ev_key)
{
    int ev_keymap_printable_len = strlen(ev_keymap_printable); // TODO: a better way?

    if(ev_key > 0 && ev_key < ev_keymap_printable_len)
    {
        return ev_keymap_printable[ev_key];
    }
    else
    {
        return '!';
    }
}

int read_ev(int fd, struct input_event ev[], char *device)
{
    int i;
    int rd;
    int size = sizeof (struct input_event); // TODO: const int
    int events;

    if ((rd = read (fd, ev, size * 64)) < size)
    {
        printf(".");
        return rd;
    //    perror_exit ("read()"); // TODO: can I pass %s for device to perror_exit()?
    }

    events=rd/size;
    printf("Read %d, %d events from device %s.\n", rd, events, device);

    for(i=0; i < events; ++i)
    {
        printf("(%ld.%ld):%d:%d:%d\n", ev[i].time.tv_sec, ev[i].time.tv_usec, ev[i].type, ev[i].code, ev[i].value);
#if 0
        value = ev[i].value;

        if (value != ' ' && ev[i].value == 1 && ev[i].type == 1){ // Only read the key press event
            printf ("(%d):Code[%d]\n", i, (ev[i].code));
        }
#endif
    }
    return rd;
}

int main (int argc, char *argv[])
{
    struct input_event ev[64];
    int fd0;
    int fd1;
//    int rd;
//    int value;
//    int size = sizeof (struct input_event);
    char name[256] = "Unknown";
    char *device0 = NULL;
    char *device1 = NULL;
//    int events;
//    int i; //fek

    //Setup check
    if (argc < 2){
        printf("Please specify (on the command line) the paths to the two dev event interface devices.\n");
        exit (0);
    }

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");

    if (argc > 1)
        device0 = argv[1];
    if (argc > 2)
        device1 = argv[2];

    //Open Device
    //TODO: nonblocking: O_RDONLY | O_NONBLOCK
    if ((fd0 = open (device0, O_RDONLY | O_NONBLOCK)) == -1)
        printf ("%s is not a valid device.\n", device0);
    if ((fd1 = open (device1, O_RDONLY | O_NONBLOCK)) == -1)
        printf ("%s is not a valid device.\n", device1);

    //Print Device0 Name
    ioctl (fd0, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading from : %s (%s)\n", device0, name);

    //Print Device1 Name
    ioctl (fd1, EVIOCGNAME (sizeof (name)), name);
    printf ("Reading from : %s (%s)\n", device1, name);

    while (1){
        printf("0");
        read_ev(fd0, ev, device0);
        printf("1");
        read_ev(fd1, ev, device1);
        sleep(1);
   }

    return 0;
} 
