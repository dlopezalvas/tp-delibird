/*
 ============================================================================
 Name        : Broker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include "Broker.h"
#define LOG_FILE "LOG_FILE"
#define LOG_NOMBRE_APLICACION "NOMBRE_APLICACION"
#define BROKER_CONFIG "Broker.config"

int main(void) {

	t_log* logger;
	t_config* config;
	iniciar_broker(&config,&logger);
	terminar_broker(logger,config);
	return EXIT_SUCCESS;
}

void iniciar_broker(t_config** config, t_log** logger){
	*config = leer_config(BROKER);
	*logger = iniciar_logger(*config);
}


void terminar_broker(t_log* logger, t_config* config)
{
	config_destroy(config);
	log_destroy(logger);
}

