#ifndef _DAEMON
	#define _DAEMON

	#define PID_FILE "/tmp/mydaemon.pid"
	#define READ_FILE  "/etc/shadow"
	
	#define FIFO_NAME "/tmp/mydaemon.fifo"

	#define CMD_START 0
	#define CMD_RESTART 1
	#define CMD_READ 2
	#define CMD_STOP 3


	int start_daemon(void);
	int stop_daemon(void);
	int readfile_daemon(void);
#endif
