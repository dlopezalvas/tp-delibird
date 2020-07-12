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

t_list* multihilos;

int main(void) {
	t_config* config = leer_config(PATH);


	iniciar_broker(&config,&logger);
//	recibir_mensaje_broker(config);
//	terminar_broker(logger,config);
	return EXIT_SUCCESS;
}

void iniciar_broker(t_config** config, t_log** logger){
	char* ip = config_get_string_value(*config,IP_BROKER);
	char* puerto = config_get_string_value(*config,PUERTO_BROKER);

	*logger = iniciar_logger(*config);
	crear_queues();

	multihilos = list_create();

	socketEscucha(ip,puerto);
}


void terminar_broker(t_log* logger, t_config* config)
{
	config_destroy(config);
	log_destroy(logger);
}

