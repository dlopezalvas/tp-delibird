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

	sem_init(&new_pokemon_sem,0,0);
	sem_init(&appeared_pokemon_sem,0,0);
	sem_init(&catch_pokemon_sem,0,0);
	sem_init(&caught_pokemon_sem,0,0);
	sem_init(&localized_pokemon_sem,0,0);
	sem_init(&get_pokemon_sem,0,0);
	sem_init(&suscripcion_sem,0,0);


	pthread_t new_pokemon_thread;
	pthread_create(&new_pokemon_thread, NULL, (void*)ejecutar_new_pokemon, NULL);
	//pthread_join(new_pokemon_thread, NULL);

	pthread_t appeared_pokemon_thread;
	pthread_create(&appeared_pokemon_thread, NULL, (void*)ejecutar_appeared_pokemon, NULL);
	//pthread_join(conexionGameBoy, NULL);

	pthread_t catch_pokemon_thread;
	pthread_create(&catch_pokemon_thread, NULL, (void*)ejecutar_catch_pokemon, NULL);
	//pthread_join(conexionGameBoy, NULL);

	pthread_t caught_pokemon_thread;
	pthread_create(&caught_pokemon_thread, NULL, (void*)ejecutar_caught_pokemon, NULL);
	//pthread_join(conexionGameBoy, NULL);

	pthread_t localized_pokemon_thread;
	pthread_create(&localized_pokemon_thread, NULL, (void*)ejecutar_localized_pokemon, NULL);
	//pthread_join(conexionGameBoy, NULL);

	pthread_t get_pokemon_thread;
	pthread_create(&get_pokemon_thread, NULL, (void*)ejecutar_get_pokemon, NULL);
	//pthread_join(conexionGameBoy, NULL);

	pthread_t suscripcion_thread;
	pthread_create(&suscripcion_thread, NULL, (void*)ejecutar_suscripcion, NULL);
	//pthread_join(suscripcion_thread, NULL);

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

