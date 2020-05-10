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

	iniciarTeam(&config, &logger);

	pthread_t hilo[entrenadores->elements_count];

	t_entrenador* tmp = malloc (sizeof(t_entrenador));

	tmp->coordx = 2;
	tmp->coordy = 2;

	moverEntrenador(config,tmp,4,4);

	printf("%d %d",tmp->coordx,tmp->coordy);

	//conectarse a las colas del broker
	//armar un socket de escucha


	// ordenarla por la mas cercana al pokemon que aparecio
	// planificar el entrenador mas cercano y que pueda atrapar pokemons

	for(int i=0; i<entrenadores->elements_count; i++){

		//pthread_create(&hilo[i],NULL, saludar(),NULL);
		//pthread_join(hilo[i], NULL);

	}

	terminarTeam(1,logger, config, &hilo);
	puts("termina");
	return EXIT_SUCCESS;
}

//void saludar(){
//	puts("soy un hilo");
//	//pthread_exit(NULL);
//}
