/*
 ============================================================================
 Name        : GameCard.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
//#include "GameCard.h"

#include<../CommonsMCLDG/utils.h>

int main(void) {
	char* ip;
	char* puerto;

	t_log* logger;
	t_config* config;
	config = leer_config();

	logger = iniciar_logger(config);
	log_info(logger,"-----------LOG START--------");



	ip = config_get_string_value(config,"IP_BROKER");
	puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_info(logger,puerto);
	log_info(logger,ip);

	log_destroy(logger);
	config_destroy(config);

}
