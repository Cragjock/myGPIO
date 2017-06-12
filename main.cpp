#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <poll.h>


#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>


///#include "GPIOClass.h"
#include "myGPIO.h"

using namespace std;

#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64
#define SYSFS_GPIO_DIR "/sys/class/gpio"

void sig_handler(int sig);

bool ctrl_c_pressed = false;
int main (void)
{
    /********
    for epoll stuff
    *********/
    struct pollfd fdset[2];
	int nfds = 2;
	int gpio_fd, timeout, rc;
	//char *buf[MAX_BUF];
	unsigned int gpio=16;
	//int len;
	timeout = POLL_TIMEOUT;
    //===============================


	struct sigaction sig_struct;
	sig_struct.sa_handler = sig_handler;
	sig_struct.sa_flags = 0;
	sigemptyset(&sig_struct.sa_mask);

	if (sigaction(SIGINT, &sig_struct, NULL) == -1)
    {
		cout << "Problem with sigaction" << endl;
		exit(1);
	}

	string inputstate;


    myGPIO* gpio26 = new myGPIO(26, OUTPUT, HIGH, FALLING);

    //myGPIO* gpio26 = new myGPIO(26);
    //gpio26->gpio_setdir(OUTPUT);     /// for LED on-off
	//gpio26->gpio_setvalue(HIGH);


	myGPIO* gpio16 = new myGPIO(16);
    gpio16->gpio_setdir(INPUT);      /// for switch state
    gpio16->gpio_set_edge(FALLING);   /// needed this set to for this to work!


	cout << " GPIO pins exported" << endl;
	cout << " Set GPIO pin directions" << endl;

        /*******************
        poll stuff
        ***************************/
    int gfd, len;
	char buf[MAX_BUF];
    len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", 16);
    gfd = open(buf, O_RDONLY | O_NONBLOCK );
	if (gfd < 0)
        {
            perror("gpio/fd_open");
        }
	cout<<"FD="<<gfd<<endl;


    while (1)
        {
            memset((void*)fdset, 0, sizeof(fdset));

            fdset[0].fd = STDIN_FILENO;
            fdset[0].events = POLLIN;

            fdset[1].fd = gfd;
            fdset[1].events = POLLPRI;

            rc = poll(fdset, nfds, timeout);

            if (rc < 0)
            {
                printf("\npoll() failed!\n");
				    	cout << "unexporting pins" << endl;
				    	//gpio26->gpio_unexport();
				    	//gpio16->gpio_unexport();
				    	cout << "deallocating GPIO Objects" << endl;
				    	delete gpio26;
				    	gpio26 = 0;
				    	delete gpio16;
				    	gpio16 =0;

                return -1;
            }

            if (rc == 0)
            {
                printf(".");
            }

            //if (fdset[1].revents & POLLIN)

            if (fdset[1].revents & POLLPRI)
                {
                    lseek(fdset[1].fd, 0, SEEK_SET);
                    len = read(fdset[1].fd, buf, MAX_BUF);
                    printf("\npoll() GPIO %d interrupt occurred\n", gpio);
                }

            if (fdset[0].revents & POLLIN)
            {
                (void)read(fdset[0].fd, buf, 1);
                printf("\npoll() stdin read 0x%2.2X\n", (unsigned int) buf[0]);
            }

            fflush(stdout);
        }

    //======================

	while(0)
	{
		usleep(500000);
		int inputstate = gpio16->gpio_getvalue();
		cout << "Current input pin state is " << inputstate  <<endl;

		if(inputstate == LOW)
		{
			cout << "input pin state is \"Pressed \".\n Will check input pin state again in 20ms "<<endl;
			usleep(20000);
            cout << "Checking again ....." << endl;

            inputstate = gpio16->gpio_getvalue();
		    if(inputstate == LOW)
		    {
		    	cout << "input pin state is definitely \"Pressed\". Turning LED ON" <<endl;
		    	gpio26->gpio_setvalue(HIGH);
		    	cout << " Waiting until pin is unpressed....." << endl;

		    	while (inputstate == LOW)
                {
		    		///gpio16->getval_gpio(inputstate);
		    		inputstate = gpio16->gpio_getvalue();
		    	};
		    	cout << "pin is unpressed" << endl;
		    }
		    else
		    	cout << "input pin state is definitely \"UnPressed\". That was just noise." <<endl;
		}

        gpio26->gpio_setvalue(LOW);     // LED off

        if(ctrl_c_pressed)
				    {
				    	cout << "Ctrl^C Pressed" << endl;
				    	cout << "unexporting pins" << endl;
				    	//gpio26->gpio_unexport();
				    	//gpio16->gpio_unexport();
				    	cout << "deallocating GPIO Objects" << endl;
				    	delete gpio26;
				    	gpio26 = 0;
				    	delete gpio16;
				    	gpio16 =0;
				    	break;
				    }

	}
	cout << "Exiting....." << endl;
	close(gfd);
	return 0;
}

void sig_handler(int sig)
{
	write(0,"\nCtrl^C pressed in sig handler\n",32);
	ctrl_c_pressed = true;
}
