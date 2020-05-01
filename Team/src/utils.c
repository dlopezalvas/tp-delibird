#include "utils.h"


void iniciarTeam(void){
	t_config* config = leer_config();
	t_log* logger = iniciar_logger(config);

	t_list* entrenadores = list_create();

	configurarEntrenadores(config, entrenadores);


	char *ip = config_get_string_value(config,IP_BROKER);
	char *puerto = config_get_string_value(config,PUERTO_BROKER);

  	log_info(logger,puerto);
  	log_info(logger,ip);

  	terminarTeam(1,logger,config);
}



t_log* iniciar_logger(t_config* config)
{
	config = leer_config();
	char* nombre_archivo = config_get_string_value(config,"LOG_FILE");
	char* nombre_aplicacion = config_get_string_value(config,"NOMBRE_APLICACION");
	t_log* logger = log_create(nombre_archivo,nombre_aplicacion,0,LOG_LEVEL_INFO);
	return logger;
}

t_config* leer_config(void)
{
	t_config* config = config_create("Team.config");
	return config;
}


void terminarTeam(int conexion, t_log* logger, t_config* config)
{
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}

//void configurarEntrenadores(t_config* config, t_lista* entrenadores){
//
//	char** posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
//	char** pokemonEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
//	char** objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");
//
//	for(int i=0; posiciones[i];i++){
//		t_entrenador* entrenador = crearEntrenador(posiciones[i], pokemonEntrenadores[i], objetivos[i]);
//		t_nodo* nodo = malloc(sizeof(t_nodo));
//		nodo->data = entrenador;
//		nodo->sig=NULL;
//		agregarEntrenador(entrenadores, nodo);
//	}
//	return ;
//}

void configurarEntrenadores(t_config* config, t_list* entrenadores){

	char** posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

	for(int i=0; posiciones[i];i++){
		t_entrenador* entrenador = crearEntrenador(posiciones[i], pokemonEntrenadores[i], objetivos[i]);
		list_add(entrenadores, entrenador);
	}
	return ;
}

static t_entrenador* crearEntrenador(char* posicion, char* pokemonsEntrenador, char* objetivos){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
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


t_entrenador* cambiarEstado (t_entrenador* entrenador,t_estado nuevoEstado){
	if(cambioEstadoValido(entrenador->estado, nuevoEstado)){
	entrenador->estado = nuevoEstado;
	return entrenador;
	}else {
		printf("Estado invalido");
		return entrenador;
	}
}

bool cambioEstadoValido(t_estado estadoViejo,t_estado nuevoEstado){
	switch (estadoViejo){
	case NEW:
		if(nuevoEstado == READY) return true;
		else return false;
		break;
	case READY:
		if(nuevoEstado == EXEC) return true;
		else return false;
		break;
	case EXEC:
		if(nuevoEstado == (READY || BLOCK || EXIT)) return true;
		else return false;
		break;
	case BLOCK:
		if(nuevoEstado == (READY || EXIT)) return true;
		else return false;
		break;
	case EXIT:
		return false;
		break;
	return true;
	}
	return false;
}

bool cumpleObjetivoGlobal(t_list* entrenadores){
	bool _cumpleObjetivoParticular(void* entrenador){
		return cumpleObjetivoParticular(entrenadores->head->data);
	}
	return list_all_satisfy(entrenadores,_cumpleObjetivoParticular);

}

bool cumpleObjetivoParticular(t_entrenador* entrenador){
	return entrenador->estado == EXIT;
}

