#include "utils.h"


void iniciarTeam(t_config** config, t_log** logger, t_list** entrenadores,t_list** objetivoGlobal){
	*config = leer_config();
	*logger = iniciar_logger(*config);

	*entrenadores = list_create();
	*objetivoGlobal = list_create();

	configurarEntrenadores(*config, *entrenadores);
	configurarObjetivoGlobal(*entrenadores, *objetivoGlobal);
//	printf("%d \n", list_size(*objetivoGlobal));
//	printf("%s \n", (*objetivoGlobal)->head->data);
//	printf("%s \n", (*objetivoGlobal)->head->next->data);
//	printf("%s \n", (*objetivoGlobal)->head->next->next->data);
//	if(cumpleObjetivoParticular((*entrenadores)->head->data)) puts("son iguales");
//	else puts("no son iguales");
//
//	if(puedeAtraparPokemon((*entrenadores)->head->data)) puts("puedeAtraparPokemon");


//	char *ip = config_get_string_value(*config,IP_BROKER);
//	char *puerto = config_get_string_value(*config,PUERTO_BROKER);
//
//  	log_info(*logger,puerto);
//  	log_info(*logger,ip);
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


void terminarTeam(int conexion, t_log* logger, t_config* config, t_list* entrenadores, t_list* objetivoGlobal)
{

	void _entrenadorDestroy(void* entrenador){
			return entrenadorDestroy( entrenador);
		}

	list_destroy_and_destroy_elements(entrenadores, _entrenadorDestroy);
	free(objetivoGlobal);
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}

void entrenadorDestroy(t_entrenador * entrenador) {
    free(entrenador);
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

t_entrenador* crearEntrenador(char* posicion, char* pokemonsEntrenador, char* objetivos){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->estado = NEW;
	char** objetivosEntrenador = string_split(objetivos,"|");
	entrenador->objetivos = configurarPokemons(objetivosEntrenador);
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
	bool _esEstadoExit(void* entrenador){
		return esEstadoExit(entrenadores->head->data);
	}
	return list_all_satisfy(entrenadores,_esEstadoExit);

}

bool esEstadoExit(t_entrenador* entrenador){
	return entrenador->estado == EXIT;
}

bool cumpleObjetivoParticular (t_entrenador* entrenador){
	if (tieneMenosElementos (entrenador->objetivos, entrenador->pokemons)) return false;
	bool _criterioOrden(void* elem1 , void* elem2){
		return criterioOrden(elem1, elem2);
	}
	list_sort(entrenador->objetivos, _criterioOrden);
	list_sort(entrenador->pokemons, _criterioOrden);
	return listasIguales( entrenador->objetivos, entrenador->pokemons);
}

bool tieneMenosElementos (t_list* listaChica, t_list* lista ){	//usar list_size o elements_count
	if(list_size(listaChica) < list_size(lista)) return true;
	return false;
}

bool listasIguales(t_list* lista1, t_list* lista2){
	t_link_element* list1 = lista1->head;
	t_link_element* list2 = lista2->head;
	while(list1){
		if(string_equals_ignore_case(list1->data, list2->data)) {
		 list1 = list1->next;
		 list2 = list2->next;
		}
		else return false;
	}
	return true;
}

bool criterioOrden(char* elem1, char* elem2){
	return (0 < strcmp(elem1, elem2));
}

bool puedeAtraparPokemon(t_entrenador* entrenador){
	return (entrenador->estado == (NEW || BLOCK) && (tieneMenosElementos (entrenador->pokemons, entrenador->objetivos)));
}

void capturoPokemon(t_entrenador* entrenador, char* pokemon, t_list* objetivoGlobal){
	list_add(entrenador->pokemons, pokemon);
	removerPokemon(pokemon,objetivoGlobal);
	if(tieneMenosElementos (entrenador->pokemons, entrenador->objetivos)){
		cambiarEstado(entrenador, READY);
	}else{
		if(cumpleObjetivoParticular(entrenador)){
			cambiarEstado(entrenador, EXIT);
		}
		else{
			cambiarEstado(entrenador, BLOCK);
		}
	}
}

void configurarObjetivoGlobal(t_list* entrenadores, t_list* objetivoGlobal){
	t_link_element *entrenador = entrenadores->head;
	t_link_element *aux = NULL;
	char* pokemon = NULL;
	t_list* pokemons = list_create();
	while(entrenador!= NULL){
		aux = entrenador->next;
		t_entrenador* entre = entrenador->data;
		list_add_all(objetivoGlobal, entre->objetivos);
		list_add_all(pokemons, entre->pokemons);
		entrenador = aux;
	}

	while(!(list_is_empty(pokemons))){

		pokemon = list_remove(pokemons, 0);
		removerPokemon( pokemon,objetivoGlobal);
		free(pokemon);
	}
	free(pokemons);

}



void removerPokemon(char* pokemon, t_list* lista){
	bool mismoPokemon(char* pokemon1 ){
			return string_equals_ignore_case(pokemon, pokemon1);
			}
		list_remove_by_condition(lista, (void*)mismoPokemon);
}



