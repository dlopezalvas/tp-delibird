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
t_queue* ready;

int main(void) {

//	pthread_mutex_init (&semaforo,NULL);
	pokemonsRequeridos = list_create();
	iniciarTeam(&logger);
	pthread_t hilo[entrenadores->elements_count];
	//socketEscucha("127.0.0.2", "5002");


	/*Prueba intercambiar
	t_entrenador* entrenador1 = entrenadores->head->data;
	t_entrenador* entrenador2 = entrenadores->head->next->data;


	printf("Prueba: %s \n", entrenador1->pokemons->head->data);
	printf("Prueba: %s \n", entrenador1->pokemons->head->next->data);


	t_intercambio* inter = malloc(sizeof(t_intercambio));
	inter->pokemonAEntregar = "Pikachu";
	inter->pokemonARecibir = "Charmander";
	inter->entrenador = entrenador2;

	entrenador2 = inter->entrenador;
	printf("%d", entrenador2->coordx);
	printf("%d", entrenador2->coordy);
	puts("llego");

	entrenador1->intercambio = inter;

	inter = entrenador1->intercambio;
	printf("%s",inter->pokemonAEntregar);
	printf("%s",inter->pokemonARecibir);
	intercambiarPokemon(&entrenador1);
	printf("%s \n", entrenador1->pokemons->head->data);
	printf("%s \n", entrenador1->pokemons->head->next->data);
	printf("%s \n", entrenador1->pokemons->head->next->next->data);
	printf("%s \n", entrenador2->pokemons->head->data);
	printf("%s \n", entrenador2->pokemons->head->next->data);

*************************FIN Prueba Intercambiar***********************
*/




//	t_link_element * aux = entrenadores->head;
//	for(int j=0; j<entrenadores->elements_count; j++){
//		pthread_create(&hilo[j],NULL, entrenadorMaster, (void*)(&aux->data));
//		aux = aux->next;
//		//join o detatch del hilo ??
//	}

	//planificar();
	sleep(1);
	terminarTeam(1, hilo);
	puts("termina");
	return EXIT_SUCCESS;
}
