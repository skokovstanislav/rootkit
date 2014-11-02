
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h> 
#include <signal.h>
#include <errno.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "descriptions.h"

int run_daemon(void);
int init_daemon(void);
void sigcatcher(int sig);


int start_daemon(void){
	pid_t pid;
	int status;
	struct stat st_file;
	
	status = stat(PID_FILE, &st_file);
	if(status == NULL){
		printf("Deamon exists!\n");
		return 1;
	}

	pid = fork();

	if(pid == -1){
		perror("Error");
		return 1;
	}else if(pid == 0){
		pid = getpid();
		if(init_daemon() == 1)
			return 1;
		run_daemon();
		exit(EXIT_SUCCESS);
	}
	printf("%s", "Started\n");
	return 0;
}


int stop_daemon(void){
	pid_t pid;
	int fd;

	if((fd=open(PID_FILE, O_RDONLY)) == -1){
		printf("Deamon not runing\n");
		return 1;
	}

	if(read(fd, &pid, sizeof(pid_t)) == -1)
		printf("Not read file!");
		goto error;
	
	if(close(fd) == -1)
		printf("Not close file");
		goto error;

	if(kill(pid, SIGTERM) == -1)
		printf("Not kill");
		goto error;
	
	printf("%s", "Stoped\n");
	
	return 0;
error:
	perror("Error");
	return 1;
}


int readfile_daemon(void){
	int fd;
	pid_t pid;

	if((fd=open(PID_FILE, O_RDONLY)) == -1){
		printf("Deamon not runing\n");
		return 1;
	}

	if(read(fd, &pid, sizeof(pid_t)) == -1)
		goto error;
	
	if(close(fd) == -1)
		goto error;

	if(kill(pid, SIGUSR1) == -1)
		goto error;
	
	return 0;

error:
	perror("Error");
	return 1;
}



int init_daemon(void)
{
	int fd, status;
	pid_t pid;

	/* Change the file mode mask */
	umask(077);

	pid = getpid();
	
	openlog("mydeamon", LOG_PID, LOG_USER);
	syslog(LOG_INFO|LOG_USER|LOG_ODELAY, "%s\n", "Start daemon");

	/* Create a new SID for the child process */
	if(setsid() < 0)
		goto error;

	/* Change the current working directory */
	if(chdir("/") < 0)
		goto error;

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	
	fd = open(PID_FILE, O_RDWR | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR);
	if(fd == -1){
		syslog(LOG_NOTICE, "Error: %s\n", strerror(errno));
		return 1;
	}
	status = write(fd, &pid, sizeof(pid_t));
	close(fd);
	return 0;

error:
	syslog(LOG_NOTICE, "Error: %s\n", strerror(errno));
	return 1;
}

int run_daemon(void)
{
	int sig, status, fd;
	sigset_t waitset;

	sigemptyset(&waitset);
	sigaddset(&waitset, SIGTERM);
	sigaddset(&waitset, SIGUSR1);
	sigaddset(&waitset, SIGTSTP);
	sigprocmask(SIG_BLOCK, &waitset, NULL);

	while((status==sigwait(&waitset, &sig)) == 0){
		switch(sig){
		case SIGTERM:
			if(unlink(PID_FILE) == -1)
				goto error;
			syslog(LOG_INFO|LOG_USER|LOG_ODELAY, "%s\n", "Stop daemon");
			return 0;
		case SIGTSTP:
		case SIGUSR1:
			if((fd = open(READ_FILE, O_RDONLY)) == -1)
				syslog(LOG_NOTICE, "Error: %s\n", strerror(errno));
			break;
		}
	}

	return 0;

error:
	syslog(LOG_NOTICE, "Error: %s\n", strerror(errno));
	return 1;
}