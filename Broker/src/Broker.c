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
#define PATH "/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/BROKER.config"


int main(void) {
	t_config* config;
	iniciar_broker(&config,&logger);
	recibir_mensaje_broker(config);
	terminar_broker(logger,config);
	return EXIT_SUCCESS;
}

void iniciar_broker(t_config** config, t_log** logger){
	*config = leer_config(PATH);
	*logger = iniciar_logger(*config);
	crear_queues();
	terminar_queues();
}


void terminar_broker(t_log* logger, t_config* config)
{
	config_destroy(config);
	log_destroy(logger);
}

