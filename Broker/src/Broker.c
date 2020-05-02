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
#define LOG_FILE "LOF_FILE"
#define LOG_NOMBRE_APLICACION "NOMBRE_APLICACION"
#define BROKER_CONFIG "Broker.config"
int main(void) {

	t_log* logger;
	t_config* config;
	iniciar_broker(&logger,&config);
}

void iniciar_broker(t_config** config, t_log** logger){
	*config = leer_config();
	*logger = iniciar_logger(*config);
}

t_log* iniciar_logger(t_config* config)
{
	config = leer_config();
	char* nombre_archivo = config_get_string_value(config,LOG_FILE);
	char* nombre_aplicacion = config_get_string_value(config,LOG_NOMBRE_APLICACION);
	t_log* logger = log_create(nombre_archivo,nombre_aplicacion,0,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(void)
{
	t_config* config = config_create(BROKER_CONFIG);
	return config;
}

void terminar_broker(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	log_info(logger,"-----------LOG END--------");
	//liberar_conexion(conexion);
	log_destroy(logger);
}

