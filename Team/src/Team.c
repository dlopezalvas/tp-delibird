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
t_list* pokemonsRequeridos; //para que se vaya llenando a medida que aparecen los pokemon
t_config* config;
t_log* logger;

int main(void) {

	pthread_mutex_init (&semaforo,NULL);
	iniciarTeam(&logger);
	pthread_t hilo[entrenadores->elements_count];

	t_entrenador** entrenador = &(entrenadores->head->data);
	printf("x:%d y:%d\n", (*entrenador)->coordx, (*entrenador)->coordx);
	t_pokemon* pokemon= malloc (sizeof(t_pokemon));
	pokemon->coordx = 1;
	pokemon->coordy = 3;
	pokemon->especie = "Pikachu";
	(*entrenador)->pokemonACapturar = pokemon;
	moverEntrenador(entrenador, 2,6);
	entrenador = (&entrenadores->head->data);
	printf("x:%d y:%d\n", (*entrenador)->coordx, (*entrenador)->coordy);



	t_link_element * aux = entrenadores->head;
	for(int j=0; j<entrenadores->elements_count; j++){
		pthread_create(&hilo[j],NULL, entrenadorMaster, (void*)(&aux->data));
		aux = aux->next;
		//join o detatch del hilo ??
	}

	//planificar();
	sleep(1);
	terminarTeam(1, hilo);
	puts("termina");
	return EXIT_SUCCESS;
}
