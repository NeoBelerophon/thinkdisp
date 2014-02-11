#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>
#include <string.h>
// killdisp script for external monitors, part of thinkdisp
// we want this compiled because we'll eventually use setuid for no
// password prompt

int startsWith(const char *pre, const char *str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}


void turnoffVirtualDisplays()
{
	int o;
	Display *display;
	int screen_num;
	int display_width;
	int display_height;
	Window root;
	XRRScreenResources  *res;
	int screen;
	char *display_name = NULL;
	char buffer[200];

	/* connect to X server */
	if ( (display= XOpenDisplay(display_name)) == NULL )
	{
		(void) fprintf( stderr, "cannot connect to X server %s\n", XDisplayName(display_name));
		exit( -1 );
	}

	screen = DefaultScreen (display);
	root = RootWindow (display, screen);
	res = XRRGetScreenResources(display, root);

	for (o = 0; o < res->noutput; o++)
	{
		XRROutputInfo	*output_info = XRRGetOutputInfo (display, res, res->outputs[o]);
		if(startsWith("VIRTUAL", output_info->name) && output_info->connection == 0 )
		{
			sprintf(buffer, "xrandr --output %s --off", output_info->name);
			system(buffer);
		}
	}


	XCloseDisplay(display);

}


int main()
{
    setuid(0);
    printf("WARNING: must run as root, else you'll get a segfault\n");
    printf("turning off display\n");
    turnoffVirtualDisplays();

    printf("killing screenclone\n");
    system("pkill screenclone");
    
    printf("killing X server...\n");
    printf("running X servers are:\n");

    //print running X servers
    FILE *fp;
    fp = popen("pgrep X", "r");
    char line[200];
    fgets(line, sizeof line, fp);
    printf("%s", line);
    pclose(fp);


    //get # running X servers
    FILE *fp2;
    fp2 = popen("pgrep -c X", "r");
    char line2[200];
    fgets(line2, sizeof line2, fp2);
    printf("%s", line2);
    pclose(fp2);

    int count = atoi(line2);
    printf("%d", count);

    //if more than one X server, kill the latest one
    if (count > 1) {
        system("sudo kill $(pgrep -n X)");
        sleep(3);
    }
   
    //insert bbswitch and shut off the nvidia card 
    system("sudo rmmod nvidia");
    sleep(1);
    system("sudo modprobe bbswitch");
    //write OFF to bbswitch
    FILE *bbswitch;
    bbswitch = fopen("/proc/acpi/bbswitch", "w");
    fprintf(bbswitch, "OFF");
    fclose(bbswitch);
	
	return 0;
}
