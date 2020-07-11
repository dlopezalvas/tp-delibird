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
pthread_mutex_t objetivo; // esto se usa adentro de entrenadorMaster()
t_list* pokemonsRequeridos; //para que se vaya llenando a medida que aparecen los pokemon
pthread_mutex_t requeridos;
t_list* especiesNecesarias;
t_config* config;
t_log* logger;
t_queue* ready;
pthread_mutex_t mutex_ready;
int ciclosCPUGlobal;
t_list* pokemonsDeRepuesto;
sem_t sem_ready;
sem_t sem_ejecutar;
pthread_mutex_t mutex_ejecutar;


int main(void) {

	iniciarTeam();
	pthread_t hiloEntrenador[entrenadores->elements_count];
	//socketEscucha("127.0.0.2", "5002"); //ip broker puerto broker

	//Intento de envio de mensaje

//		t_mensaje* mensaje = malloc(sizeof(t_mensaje));
//		mensaje -> tipo_mensaje = APPEARED_POKEMON;
//		char* hola = "gastooooon 1 2 3";
//		char** hola2 = string_split(hola," ");
//
//		mensaje -> parametros = hola2;
//		mensaje -> id = 2;
//		int socket_broker = iniciar_cliente("127.0.0.1","6009");//IP y Puerto del broker
//		enviar_mensaje(mensaje, socket_broker);
	//

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


/*Prueba capturo pokemon

		t_entrenador* entrenador = entrenadores->head->data;
		entrenador->pokemonACapturar = malloc(sizeof(t_pokemon));
		entrenador->pokemonACapturar->especie = "Pidgey";
		capturoPokemon(&(entrenador));
		printf("%s", entrenador->pokemonsNoNecesarios->head->data);


		*************************FIN Prueba capturo pokemon***********************/

//Prueba necesita pokemon

//	t_entrenador* entrenador = entrenadores->head->next->data;
//
//	if(list_is_empty(entrenador->pokemonsNoNecesarios)) puts("esta vacia\n");
//	else{
//	printf("%d \n", entrenador->pokemonsNoNecesarios->elements_count);
//	printf("%s \n",entrenador->pokemonsNoNecesarios->head->data);
//	}
	//	if(tienePokemonNoNecesario(entrenador, "Squirtle")) puts("No necesita squirtle");
//	if(puedeEstarEnDeadlock(entrenador)) puts("puede estar en deadlock");


//*************************FIN Prueba necesita pokemon***********************/

//	deteccionDeadlock();
//	if(puedeEstarEnDeadlock(entrenadores->head->data)) puts("puede estar en deadlock");
//	if(cumpleObjetivoGlobal()) puts("cumple objetivo global");


//	t_link_element * aux = entrenadores->head;
//	for(int j=0; j<entrenadores->elements_count; j++){
//		pthread_create(&hiloEntrenador[j],NULL, entrenadorMaster, (void*)(&aux->data));
//		aux = aux->next;
//		//join o detatch del hilo ??
//	}

	//planificar();

	terminarTeam(1, hiloEntrenador);
	puts("termina");
	return EXIT_SUCCESS;
}
