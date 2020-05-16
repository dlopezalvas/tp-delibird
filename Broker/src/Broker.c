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
	//inicio logger y config
		t_config* config = leer_config(PATH);;
		//t_log* logger = iniciar_logger(config);
		//iniciar_broker(&config,&logger);

		char* ip = config_get_string_value(config,IP_BROKER);
		char* puerto = config_get_string_value(config,PUERTO_BROKER);

		iniciar_servidor(ip,puerto);
		printf("Se creo el socket servidor en el puerto ( %s )", puerto);
//	t_config* config;
//	iniciar_broker(&config,&logger);
//	recibir_mensaje_broker(config);
//	terminar_broker(logger,config);
//	return EXIT_SUCCESS;
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

