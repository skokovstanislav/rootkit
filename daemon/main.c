#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h> 


#include "descriptions.h"

static const char cmd_opt[] = "c:h";
static const char* cmd_opt_params[] = {"start", "restart", "read", "stop", NULL};

int main(int argc, char** argv)
{
	int ch = 0, i, status;
	int command_number = -1;

	while((ch = getopt(argc, argv, cmd_opt)) != -1){
		switch(ch){
		case 'c':
			for(i = 0; cmd_opt_params[i] != NULL; i++){
				if(strcmp(optarg, cmd_opt_params[i]) == 0){
					command_number = i;
					break;
				}
			}
			if(command_number == -1){
				printf("%s invalid parammetr -- '%s'\n", argv[0], optarg);
				goto error;
			}
			break;
		}
	}

	switch(command_number){
	case CMD_START:
		if(start_daemon() == 1)
			goto error;
		break;
	case CMD_STOP:
		if(stop_daemon() == 1)
			goto error;
		break;
	case CMD_READ:
		if(readfile_daemon() == 1)
			goto error;
		break;
	case CMD_RESTART:
		if(stop_daemon() == 1)
			goto error;
		if(start_daemon() == 1)
			goto error;
		break;
	}

	return 0;

error:
	exit(EXIT_FAILURE);
}
