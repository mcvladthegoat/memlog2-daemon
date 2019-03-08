#define MAXLINE 250 //used for /proc/* reading
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

static void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("memlog2d", LOG_PID, LOG_DAEMON);
}

int main()
{
    skeleton_daemon();

    while (1)
    {
        FILE *meminfo = fopen("/proc/meminfo", "r");
	FILE *output = fopen("/var/log/memtestlog", "a");
        if (meminfo == NULL || output == NULL) {
		syslog(LOG_NOTICE, "memlog2d failed :(");
                break;
        }
        char buffer[MAXLINE];
	for (int i = 0; i < 3; ++i) {
	        fgets(buffer, MAXLINE, meminfo);
		fputs(buffer, output);		
        }
	fclose(meminfo);
	fclose(output);
        syslog (LOG_NOTICE, "memlog2d started.");
        sleep (1); //delay 60s
    }

    syslog (LOG_NOTICE, "memlog2d terminated.");
    closelog();
    return EXIT_SUCCESS;
}
