// fekerr 20180225
// working to create "the dumbest game in the world"
// so my 4-year old son and I can bang on two separate keyboards :)

// http://www.thelinuxdaily.com/2010/05/grab-raw-keyboard-input-from-event-device-node-devinputevent/

// Other notes:
// sudo lsinput (input-utils)
// sudo evemu-record (evemu)

// hybrid C++, working to convert to more C++

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

// Working to convert to C++.
#include <iostream>
#include <cctype>
#include <cstring>
#include <fstream>
using namespace std;

#ifndef FALSE
const int FALSE = 0;
#ifndef TRUE
const int TRUE = !FALSE;
#endif
#endif

// Strings from data defined in linux/input-event-codes.h
// TODO: a better way?

const char *ev_types_str[EV_CNT] =
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

const char *inv_str = "*INV*";

// key map for a few keys from KEY_ in linux/input-event-codes.h
// TODO: a better way?
// "unprintable" currently listed here as underscores

const char ev_keymap_printable[]="__1234567890-=_\tqwertyuiop{}__" \
    "asdfghjkl;\'`_\\zxcvbnm,./";
const size_t KEYLIST_SIZE=sizeof(ev_keymap_printable);

struct keysmain
{
    keysmain();
    int fd0;
    int fd1;
    char *device0;
    char *device1;
    bool switch_onedev;
    int keysbanged[2][KEYLIST_SIZE];
};

keysmain::keysmain()
{
    size_t i; // unsigned for comparison with sizeof. Now was that a silly warning?

    fd0 = -1;
    fd1 = -1;
    device0 = NULL;
    device1 = NULL;
    switch_onedev=FALSE;

    for(i=0; i<KEYLIST_SIZE; ++i)
    {
        keysbanged[0][i]=0;
        keysbanged[1][i]=0;
    }
}

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

const char *map_event(int ev_event)
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

const char map_key(int ev_key)
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

int read_ev(keysmain & progMgmt, int devnum)
{
    struct input_event ev[64];
    const int size = sizeof (struct input_event);
    int i;
    int rd;
    int events;
    int fd;
    char *device;

    fd = progMgmt.fd0;
    device = progMgmt.device0;
    if(devnum == 1)
    {
        fd = progMgmt.fd1;
        device = progMgmt.device1;
    }
    cout << "read_ev: fd, device = " << fd << "," << device << endl;

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
        if(ev[i].type == EV_KEY && ev[i].value == 1)
        {
            //TODO: blah comparison being stupid
            //(I know it's me.)
            cout << "mapkey=" << map_key(ev[i].code) << "," << (int) map_key(ev[i].code) << endl;
            cout << (int) '_' << endl;

            if(map_key(ev[i].code != '_'))
            {
                cout << "111" << endl;
                printf("(%ld.%ld):%s:%d(%c)\n", ev[i].time.tv_sec, ev[i].time.tv_usec,
                        map_event(ev[i].type),
                        ev[i].code, map_key(ev[i].code));
                ++progMgmt.keysbanged[devnum][ev[i].code];
            }
            else
            {
                cout << "222" << endl;
                printf("(%ld.%ld):%s:%d\n", ev[i].time.tv_sec, ev[i].time.tv_usec,
                        map_event(ev[i].type),
                        ev[i].code);
            }
        }

#if 0
        else
        {
            printf("(%ld.%ld):%d(%s):%d:%d\n", ev[i].time.tv_sec, ev[i].time.tv_usec,
                    ev[i].type, map_event(ev[i].type),
                    ev[i].code, 
                    ev[i].value);
        }
#endif

#if 0
        value = ev[i].value;

        if (value != ' ' && ev[i].value == 1 && ev[i].type == 1){ // Only read the key press event
            printf ("(%d):Code[%d]\n", i, (ev[i].code));
        }
#endif
    }
    return rd;
}

void progHelp()
{
    cout << "TODO: help" << endl;
    cout << "*cough* this silly thing requires sudo :p" << endl;

    cout << "Please specify (on the command line) the paths to the two dev event interface devices." << endl;

    cout << "switch -1 for just one device for debug\n"
        << "switch -h for this help\n"
        << endl;
 
}

int args(int argc, char *argv[], keysmain & progMgmt)
{
    int i;
    char name[256] = "Unknown";

    for(i=0; i<argc; ++i)
    {
        cout << i << ": " << argv[i] << "\n" << endl;
        if(i>0)
        {
            if(argv[i][0] == '-' && argv[i][1] == '1')
            {
                progMgmt.switch_onedev=TRUE;
                continue;
            }
            if(argv[i][0] == '-' && argv[i][1] == 'h')
            {
                progHelp();
                continue;
            }
            if(progMgmt.fd0 == -1)
            {
                progMgmt.device0 = argv[i];
                if ((progMgmt.fd0 = open (progMgmt.device0, O_RDONLY | O_NONBLOCK)) == -1)
                {
                    printf ("%s is not a valid device.\n", progMgmt.device0);
                    continue;
                }
                //Print Device0 Name
                ioctl (progMgmt.fd0, EVIOCGNAME (sizeof (name)), name);
                printf ("Reading from : %s (%s)\n", progMgmt.device0, name);

          }
            if(!progMgmt.switch_onedev && progMgmt.fd1 == -1)
            {
                progMgmt.device1 = argv[i];
                if ((progMgmt.fd1 = open (progMgmt.device1, O_RDONLY | O_NONBLOCK)) == -1)
                {
                    printf ("%s is not a valid device.\n", progMgmt.device1);
                    continue;
                }
                //Print Device1 Name
                ioctl (progMgmt.fd1, EVIOCGNAME (sizeof (name)), name);
                printf ("Reading from : %s (%s)\n", progMgmt.device1, name);
            }
        }
    }

    // validate arguments and final status.
    if(argc < 2)
    {
        progHelp();
    }

    //TODO: check states of fd0, fd1 and switch_onedev....
    // null is messing up cout?
#if 0
    cout << "dbg: fd0, device0, device1, fd1, onedev="
        << progMgmt.fd0 << ",\"" << progMgmt.device0 << "\""
        << progMgmt.fd1 << ",\"" << progMgmt.device1 << "\""
        <<  progMgmt.switch_onedev
        << endl << endl;
#endif

    cout << "dbg: fd0," << progMgmt.fd0;
    cout << " device0=" << progMgmt.device0;
    cout << " fd1," << progMgmt.fd1;
    cout << " device1=" << progMgmt.device1;
    cout << " onedev=" << progMgmt.switch_onedev;
    cout << endl << endl;

    return 0; // TODO: ...
}

int main (int argc, char *argv[])
{
    int args_proc;  // return from args processing.
    keysmain progMgmt;
    size_t i;
    size_t j;

    args_proc = args(argc, argv, progMgmt);
    if(args_proc)
    {
        cout << "dbg: err ret from args_proc(): " << args_proc << endl;
    }

    if ((getuid ()) != 0)
        printf ("You are not root! This may not work...\n");
    for(i=0; i<100000000; ++i)
    {
        printf("0");
        read_ev(progMgmt, 0);
        if(!(i%25))
        {
            cout << "Player 0, thou bangst:\n";
            for(j=0;j<KEYLIST_SIZE;++j)
            {
                cout << progMgmt.keysbanged[0][j] << " ";
            }
            cout << endl;
        }
        if(progMgmt.fd1 != -1)
        {
            printf("1");
            read_ev(progMgmt, 1);
            if(!(i%25))
            {
                cout << "Player 1, thou bangst:\n";
                for(j=0;j<KEYLIST_SIZE;++j)
                {
                    cout << progMgmt.keysbanged[1][j] << " ";
                }
                cout << endl;
            }
        }
        sleep(1);
   }

    return 0;
} 
