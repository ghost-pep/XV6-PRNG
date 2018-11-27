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
 * a) Something can be read from the console
 *		action: read bytes from the console into buf
 * b) We have something waiting in buf
 *    action: write bytes to the tosh pipe
 * c) Something can be read from the fromsh pipe
 *    action: Read the bytes from the pipe and then write them to the console
 *
 * Things to note:
 * 	1) While we use buf for both reading to and from the console, they never conflict with each
 * 		other because we always immediately empty buf after writing to it
 * 	2) While the shell may have many bytes written to the pipe, we only read sizoeof(buf) at
 * 		a time. This allows us to more quickly respond to user input.
 */
void
pipenet(void)
{
#define BUF_SIZE 100
#define CTR_SIZE 112    // Must be a multiple of 16
  int toshfds[2], fromshfds[2], toe[2], frome[2];
  char buf[BUF_SIZE];
  //char encryptedBuf[BUF_SIZE];
  u_int8_t counter[CTR_SIZE];
  random((char*)counter, CTR_SIZE);
  //u_int32_t KEY[4] = {0x1E1E1E1E,0x87654321,0xDEADBEEF,0xABCBACED};

  int bytesread;			// Used to denote how many bytes are waiting to be
        // read from the console

  int result;				// Used to denote how many bytes were read from the pipe

  int pid;
  fd_set read_fds, write_fds;

  if (pipe(toshfds) != 0) {
      printf(2, "Pipe failed!");
      exit();
  }
  if (pipe(fromshfds)) {
      printf(2, "Pipe failed!");
      exit();
  }
  if (pipe(frome)) {
      printf(2, "Pipe failed!");
      exit();
  }
  if (pipe(toe)) {
      printf(2, "Pipe failed!");
      exit();
  }


  if ((pid = fork()) == 0) { // if sh

    close(toe[0]); 	// close unneeded pipes
    close(toe[1]);
    close(frome[0]);
    close(frome[1]);

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
  } else if((pid = fork()) == 0) {
    // e process
    //printf(1, "e started\n");
    close(toshfds[0]);
    close(fromshfds[1]);

    close(toe[1]);
    close(frome[0]);

    for(;;){
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	FD_SET(toe[0], &read_fds);
	FD_SET(fromshfds[0], &read_fds);

	if (select(20, &read_fds, &write_fds) != 0) {
	  printf(1, "Select failed\n");
	  return;
	}//printf(1, "e select returned\n");


	if (FD_ISSET(toe[0], &read_fds)) {		// read from pipenet
	  //printf(1, "%d reading from pipenet, writing to sh\n", toe[0]);
	  bytesread = read(toe[0], buf, sizeof(buf));
//printf(1, "string: %s\n", buf);
	  write(toshfds[1], buf, bytesread);
	}


	if (FD_ISSET(fromshfds[0], &read_fds)) {	// read from sh
	  //printf(1, "%d reading from sh, writing to pipenet\n", frome[0]);
	  result = read(fromshfds[0], buf, sizeof(buf));
	  write(frome[1], buf, result);
        }
      
   } 
  } else {
    //printf(1, "pipenet started\n");
    //pipenet
    close(toshfds[0]);
    close(toshfds[1]);
    close(fromshfds[0]);
    close(fromshfds[1]);

    close(toe[0]);
    close(frome[1]);

    //printf(1,"\n\n ***** Starting PipeNet ***** \n\n");
    for (;;) {
        //zero out the BOYS
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

	//populate the BOYS
	FD_SET(0, &read_fds);
	FD_SET(frome[0], &read_fds);

        if (select(20, &read_fds, &write_fds) != 0) {
          //printf(1, "Select failed\n");
          return;
        }//printf(1, "pipenet select returned\n");

	//check conditions
	if (FD_ISSET(0, &read_fds)) {		//if we can read from console, read console and write e
	  //printf(1, "%d reading from console, writing to e\n", toe[0]);
          bytesread = read(0, buf, sizeof(buf));
	  write(toe[1], buf, bytesread);
	}

	if (FD_ISSET(frome[0], &read_fds)) {	//if data is ready to read from e, read e and write console
	  //printf(1, "%d reading from e\n", frome[0]);
	  result = read(frome[0], buf, sizeof(buf));
	  write(1, buf, result);
	}
    }
    wait();
    wait();
  }
}

int
main(void)
{
  pipenet();
  exit();
}
