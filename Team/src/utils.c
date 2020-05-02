#include "utils.h"


void iniciarTeam(void){
	t_config* config = leer_config(TEAM);
	t_log* logger = iniciar_logger(config);
	t_lista entrenadores = NULL;

	configurarEntrenadores(config, &entrenadores);
	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

  	terminar_proceso(1,logger,config);
}


void configurarEntrenadores(t_config* config, t_lista* entrenadores){

	char** posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

	for(int i=0; posiciones[i];i++){
		t_entrenador* entrenador = crearEntrenador(posiciones[i], pokemonEntrenadores[i], objetivos[i]);
		t_nodo* nodo = malloc(sizeof(t_nodo));
		nodo->data = entrenador;
		nodo->sig=NULL;
		agregarEntrenador(entrenadores, nodo);
	}
	return ;
}

t_entrenador* crearEntrenador(char* posicion, char* pokemonsEntrenador, char* objetivos){
	t_entrenador* entrenador= malloc(sizeof(t_entrenador));;
	entrenador->estado = NEW;
	entrenador->objetivos = string_split(objetivos, "|");
	char** coordenadas = string_split(posicion,"|");
	entrenador->coordx = atoi(coordenadas[0]);
	entrenador->coordy = atoi(coordenadas[1]);
	char** pokemons = string_split(pokemonsEntrenador,"|");
	entrenador->pokemons = configurarPokemons(pokemons);
	return entrenador;
}

t_list* configurarPokemons(char** pokemons){
	t_list* listaPokemons = list_create();
	for(int i=0; pokemons[i];i++){
		list_add(listaPokemons, (pokemons[i]));
	}

	return listaPokemons;
}

void agregarEntrenador(t_lista* entrenadores,t_nodo* nodo){

	if(*entrenadores == NULL){
		*entrenadores = nodo;
	}else{
		t_nodo* ultimoNodo = *entrenadores;
		while(ultimoNodo->sig !=NULL) ultimoNodo = ultimoNodo->sig;
		ultimoNodo->sig = nodo;
	}

	return;

}


