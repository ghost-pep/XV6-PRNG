#include "types.h"
#include "user.h"
#include "stat.h"
#include "syscall.h"
#include "select.h"

int
test0(void)
{
   fd_set s;
   int nfds = 32;
   FD_ZERO(&s);
   if (s != 0)
   {   
        printf(1, "FD_ZERO FAILED: s = %x\n", s);
	printf(1, "Test 0 FAILED.\n");
        return 1;
   }
   for (int fd=0; fd<nfds; fd++)
   {
       FD_SET(fd, &s);
       if (!FD_ISSET(fd, &s))
       {
           printf(1, "FD_SET FAILED; s = %x\n", s);
	   printf(1, "Test 0 FAILED.\n");
           return 1;
       }
       FD_CLR(fd, &s);
       if (FD_ISSET(fd, &s))
       {
           printf(1, "FD_CLR FAILED: s = %x\n", s);
	   printf(1, "Test 0 FAILED.\n");
           return 1;
       }
   }
   
   printf(1,"Test 0 was successful.\n");
   return 0;
}

int
test1(void)
{
    fd_set s;
    int nfds = 5;
    int fds[2];
    
    FD_ZERO(&s);

    pipe(fds);

    if (fork() == 0)
    {
        close(fds[1]);
        fd_set readfds, writefds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fds[0],&readfds);
        if (select(nfds, &readfds, &writefds) == 0)//child didn't return
        {
            close(fds[0]);
            if (!FD_ISSET(fds[0],&readfds))
            {
                printf(1, "Child: select returned but read fd not set!\n");
		printf(1, "Test 1 FAILED\n.");
                return 1;
            }
        }

        exit();

    } 
    else
    {
        close(fds[0]);
        fd_set readfds, writefds;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(fds[1],&writefds);
        if (select(nfds, &readfds, &writefds) == 0)
        {
            close(fds[1]);
            if (!FD_ISSET(fds[1],&writefds))
            {
                printf(1, "Parent: select returned but write fd not set!\n");
		printf(1, "Test 1 FAILED\n.");
                wait();
                return 1;
            }
        }
        wait();
    }
    printf(1,"Test 1 was successful.\n");
    return 0;
}
/* I'm assuming that success is: 
 *  0 - nothing set
 *  1 - write set
 *  2 - write set
*/
int
test2(void)
{
    fd_set s;
    int nfds = 3;
    int successfd0, successfd1, successfd2;
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(0,&readfds);
    FD_SET(1,&writefds);
    FD_SET(2,&writefds);
    if (select(nfds, &readfds, &writefds) == 0)
    {
        for (int fd=0; fd<nfds; fd++)
        {
            if (FD_ISSET(fd,&readfds))
            {
                //printf(1, "Console read fd %d set\n", fd);
            }
            else
            {
                //printf(1, "Console read fd %d not set\n", fd);
		if(fd == 0) successfd0 = 1;
		if(fd == 1) successfd1 = 1;
		if(fd == 2) successfd2 = 1;
            }
            if (FD_ISSET(fd,&writefds))
            {
                //printf(1, "Console write fd %d set\n", fd);
		if(fd == 1) successfd1 = successfd1 && 1;
		if(fd == 2) successfd2 = successfd2 && 1;
            }
            else
            {
                //printf(1, "Console write fd %d not set\n", fd);
		if(fd == 0) successfd0 = successfd0 && 1;
            }
        }
    }
    if(successfd0 && successfd1 && successfd2)
      printf(1, "Test 2 was successful.\n");
    else 
      printf(1, "Test FAILED.\n");
    return 0;
}

int
test3(void)
{
    fd_set s;
    int nfds = 3;
   
    int success = 1;
 
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(0,&readfds);
    if (select(nfds, &readfds, &writefds) == 0)
    {
        for (int fd=0; fd<nfds; fd++)
        {
            if (FD_ISSET(fd,&readfds))
            {
                //printf(1, "Console read fd %d set\n", fd);
		if(fd != 0) success = 0;
            }
            else
            {
                //printf(1, "Console read fd %d not set to read\n", fd);
		if(fd == 0) success = 0;
            }
            if (FD_ISSET(fd,&writefds))
            {
                //printf(1, "Console write fd %d set\n", fd);	
		success = 0;
            }
            else
            {
                //printf(1, "Console write fd %d not set\n", fd);
            }

        }
    }

    if(success) printf(1, "Test 3 was successful.\n");
    else printf(1, "Test 3 was NOT successful.\n");

    return 0;
}

int
test3p(void)
{
    fd_set s;
    int nfds = 5;
    int fds[2];
    char wbuf[32] = "I am a write buffer\n";
    char rbuf[32];
   
    int success = 1;
 
    pipe(fds);
    
    FD_ZERO(&s);

    fd_set readfds, writefds;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    if (fork() == 0)
    {
        FD_SET(fds[0],&readfds);
        //printf(1, "Selecting on Read\n");
        if (select(nfds, &readfds, &writefds) == 0)
        {
            for (int fd=0; fd<nfds; fd++)
            {
                if (FD_ISSET(fd,&readfds))
                {
                    //printf(1, "fd %d set read\n", fd);
		    if(fd != 3) success = 0;
                }
                else
                {
                    //printf(1, "fd %d not set to read\n", fd);
		    if(fd == 3) success = 0;
                }
                if (FD_ISSET(fd,&writefds))
                {
                    //printf(1, "fd %d set write\n", fd);
		    success = 0;
                }
                else
                {
                    //printf(1, "fd %d not set to write\n", fd);
                }
            }
            read(fds[0],rbuf,32);
            write(1,rbuf,32);
        }
	exit();
    }
    else
    {
        //printf(1, "Sleeping for 5\n");
        sleep(5);
        //printf(1, "Writing to pipe\n");
        write(fds[1],wbuf,32);
        wait();
    }

    if(success) printf(1, "Test 3p was successful.\n");
    else printf(1, "Test 3p FAILED.\n");

    return 0;
}



int
main(void)
{

    printf(1, "test0\n");
    test0();

    printf(1, "\ntest1\n");
    test1();

    printf(1, "\ntest2\n");
    test2();

    printf(1, "\ntest3p\n");
    test3p();

    printf(1, "\ntest3\n");
    test3();
    exit();
}
