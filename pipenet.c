#include "select.h"
#include "user.h"
#include "stat.h"
#include "syscall.h"

char * shargv[] = { "sh", 0 };

/**
 * The basic structure of pipenet reamins the same. However, I now run an infinite for
 * loop that waits for one of 3 conditions to become true. Those conditions and the
 * actions I take when they bcome true are listed below. If more than one condition
 * becomes true, the actions are executed in the order below.
 *
 *	a) Something can be read from the console
		action: read bytes from the console into buf
	b) We have something waiting in buf
		action: write bytes to the tosh pipe
	c) Something can be read from the fromsh pipe
		action: Read the bytes from the pipe and then write them to the console
 *
 * Things to note:
 * 	1) While we use buf for both reading to and from the console, they never conflict with each
 * 		other because we always immediately empty buf after writing to it
 * 	2) While the shell may have many bytes written to the pipe, we only read sizoeof(buf) at
 * 		a time. This allows us to more quickly respond to user input.
 *
*/

void
pipenet(void)
{
    int toshfds[2], fromshfds[2];
    char buf[100];
    int bytesread;			// Used to denote how many bytes are waiting to be
					// read from the console

    int result;				// Used to denote how many bytes were read from the pipe

    int data_ready;			// Used to denote whether we have ata waiting in the pipe.
					// When this is >0, we read form the pipe and write to
					// the console


    int pid;
    fd_set read_fds, write_fds;

    if (pipe(toshfds) != 0)
    {
        printf(2, "Pipe failed!");
        exit();
    }
    if (pipe(fromshfds))
    {
        printf(2, "Pipe failed!");
        exit();
    }

    if ((pid = fork()) == 0) // if child
    {
        close(toshfds[1]);   // close write end
        close(fromshfds[0]); // close read end
        close(0);            // close console stdin
        dup(toshfds[0]);     // dup pipe to stdin
        close(1);            // close console stdout
        close(2);            // close console stderr
        dup(fromshfds[1]);   // dup pipe to stdout
        dup(fromshfds[1]);   // dup pipe to stderr
        close(toshfds[0]);   // close duplicated read fd
        close(fromshfds[1]); // close duplicated write fd
        
        exec(shargv[0], shargv); // invoke shell
    }
    else
    {
        close(toshfds[0]);  // close read end
        close(fromshfds[1]);// close write end
        printf(1,"\n\n ***** Starting PipeNet ***** \n\n");
        for (;;)
        {
	   bytesread = 0;
	   result = 0;


	   data_ready = 0;
	   while(!data_ready) {

	     //zero out the BOYS
	     FD_ZERO(&read_fds); 
	     FD_ZERO(&write_fds); 

	     //populate the BOYS
	     FD_SET(0, &read_fds);
	     FD_SET(fromshfds[0], &read_fds);
	     FD_SET(toshfds[1], &write_fds);
	     FD_SET(1, &write_fds);

	     if(select(12, &read_fds, &write_fds) != 0){
	       printf(1, "Select failed\n");
	       return;
	     }

	     //check conditions
	     if(FD_ISSET(0, &read_fds)) {		//if we can read from console, do it
               //printf(1, "\nReading from console\n");
               bytesread += read(0, buf, sizeof(buf));
	     }
	     if(FD_ISSET(toshfds[1], &write_fds) && bytesread > 0) {	//if we can write to pipe, do it
               //printf(1, "\nWriting to pipe\n");
               write(toshfds[1], buf, bytesread);
	       bytesread = 0;
	     }
	     if(FD_ISSET(fromshfds[0], &read_fds)) {	//if data is ready to read from pipe, do the loop
		data_ready = 1;
		//printf(1, "Data is ready\n");
	     }

	   }

           for (;;)
           {
                //printf(1, "\nReading from pipe\n");
		if(FD_ISSET(fromshfds[0], &read_fds))
                  result = read(fromshfds[0], buf, sizeof(buf)); // read from pipe
                if (result > 0)
                {
                    //printf(1, "\nWriting to console\n");
                    if (write(1, buf, result) < 0) // write data from pipe to console
                    {
                        printf(2, "Write error!");
                        exit();
                    }
		    data_ready = 0;
	  	    break;
                } 
                else if (result == 0) // no data left to read
                {
		    data_ready = 0;
                    break;
                }
                else // error
                {
                    printf(2, "Pipe read error!");
                    exit();
                }
            }
        }
        wait();
    }
}

int
main(void)
{
    pipenet();
    exit();
}
