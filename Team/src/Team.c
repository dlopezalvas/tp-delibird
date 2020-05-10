/*
 ============================================================================
 Name        : Team.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Team.h"


//no olvidarse de sincronizar las variables globales
t_list* entrenadores;
t_list* objetivoGlobal;
pthread_mutex_t semaforo; // esto se usa adentro de entrenadorMaster()
t_list* mapaPokemons; //para que se vaya llenando a medida que aparecen los pokemon

int main(void) {
	t_config* config;
	t_log* logger;
	pthread_mutex_init (&semaforo,NULL);

	pthread_t hilo;

	iniciarTeam(&config, &logger);

//	t_entrenador* tmp;
//
//	tmp->coordx = 2;
//	tmp->coordy = 2;
//
//	moverEntrenador(config,tmp,4,4);
//
//	printf("%d",tmp->coordx);

	//conectarse a las colas del broker
	//armar un socket de escucha


	// ordenarla por la mas cercana al pokemon que aparecio
	// planificar el entrenador mas cercano y que pueda atrapar pokemons

//	for(int i=0; i<list_size(entrenadores); i++){
//
//
//		pthread_t hilo[i];
//
//		pthread_create(&hilo[i],NULL,entrenadorMaster(),NULL);
//
//
//
//	}

	terminarTeam(1,logger, config);
	puts("termina");
	return EXIT_SUCCESS;
}


