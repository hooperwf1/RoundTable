#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "logging.h"
#include "config.h"
#include "communication.h"

int main(){
	atexit(log_close);

	struct fig_ConfigData config = {0};
	fig_readConfig("example_config.conf", &config);
	log_editConfig(config.useFile, config.logDirectory);

	int sock = com_startServerSocket(&config, 0);

	close(sock);
	return 0;
}
