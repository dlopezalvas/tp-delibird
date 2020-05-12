#include "utils.h"
extern t_list* entrenadores;
extern t_list* objetivoGlobal;
extern pthread_mutex_t semaforo;
extern t_list* pokemonsRequeridos;
extern t_config* config;
extern t_log* logger;

void iniciarTeam(){ //funciona
	config = leer_config(PATH);
	logger = iniciar_logger(config);

	entrenadores = list_create();
	objetivoGlobal = list_create();

	configurarEntrenadores(config);
	configurarObjetivoGlobal();
}

void terminarTeam(int conexion, pthread_t* hilo)//revisar memoria y probar si funciona
{

	void _entrenadorDestroy(void* entrenador){
			return entrenadorDestroy( entrenador);
		}
	for(int i = 0; i<entrenadores->elements_count ; i++){
			//destruir hilos
	}

	list_destroy_and_destroy_elements(entrenadores, _entrenadorDestroy);
	free(objetivoGlobal);
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}

void entrenadorDestroy(t_entrenador * entrenador) { //probar
    free(entrenador);
}

void configurarEntrenadores(){ //funciona

	char** posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

	for(int i=0; posiciones[i];i++){
		t_entrenador* entrenador = crearEntrenador(posiciones[i], pokemonEntrenadores[i], objetivos[i]);
		list_add(entrenadores, entrenador);
	}
	return ;
}

t_entrenador* crearEntrenador(char* posicion, char* pokemonsEntrenador, char* objetivos){ //funciona
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->estado = NEW;
	char** objetivosEntrenador = string_split(objetivos,"|");
	entrenador->objetivos = configurarPokemons(objetivosEntrenador);
	char** coordenadas = string_split(posicion,"|");
	entrenador->coordx = atoi(coordenadas[0]);
	entrenador->coordy = atoi(coordenadas[1]);
	char** pokemons = string_split(pokemonsEntrenador,"|");
	entrenador->pokemons = configurarPokemons(pokemons);
	entrenador->pokemonACapturar = NULL;
	entrenador->intercambio = NULL;
	return entrenador;
}



t_list* configurarPokemons(char** pokemons){ //funciona
	t_list* listaPokemons = list_create();
	for(int i=0; pokemons[i];i++){
		list_add(listaPokemons, (pokemons[i]));
	}

	return listaPokemons;
}



void cambiarEstado (t_entrenador** entrenador,t_estado nuevoEstado){ //funciona
	if(cambioEstadoValido((*entrenador)->estado, nuevoEstado)){
	(*entrenador)->estado = nuevoEstado;
	}else {
		printf("Estado invalido");
	}
}

bool cambioEstadoValido(t_estado estadoViejo,t_estado nuevoEstado){ //funciona
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

bool cumpleObjetivoGlobal(){ //funciona
	bool _esEstadoExit(void* entrenador){
		return esEstadoExit(entrenador);
	}
	return list_all_satisfy(entrenadores,_esEstadoExit);

}

bool esEstadoExit(t_entrenador* entrenador){//funciona
	return entrenador->estado == EXIT;
}

bool cumpleObjetivoParticular (t_entrenador* entrenador){//funciona
	if (tieneMenosElementos (entrenador->objetivos, entrenador->pokemons)) return false;
	bool _criterioOrden(void* elem1 , void* elem2){
		return criterioOrden(elem1, elem2);
	}
	list_sort(entrenador->objetivos, _criterioOrden);
	list_sort(entrenador->pokemons, _criterioOrden);
	return listasIguales( entrenador->objetivos, entrenador->pokemons);
}

bool tieneMenosElementos (t_list* listaChica, t_list* lista ){	//funciona
	if(list_size(listaChica) < list_size(lista)) return true;
	return false;
}

bool listasIguales(t_list* lista1, t_list* lista2){ //funciona
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

bool criterioOrden(char* elem1, char* elem2){ //funciona
	return (0 < strcmp(elem1, elem2));
}

bool puedeAtraparPokemon(t_entrenador* entrenador){ //funciona
	return (entrenador->estado == (NEW || BLOCK) && (tieneMenosElementos (entrenador->pokemons, entrenador->objetivos)));
}

void capturoPokemon(t_entrenador** entrenador){

	list_add((*entrenador)->pokemons, (*entrenador)->pokemonACapturar->especie);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	if(tieneMenosElementos ((*entrenador)->pokemons, (*entrenador)->objetivos)){
		cambiarEstado(entrenador, READY);
	}else{
		if(cumpleObjetivoParticular(*entrenador)){
			cambiarEstado(entrenador, EXIT);
		}
		else{
			cambiarEstado(entrenador, BLOCK);
		}
	}
//Probar si funciona segunda parte
//	bool mismaEspecie(t_pokemon* pokemon1 ){
//				return string_equals_ignore_case((*entrenador)->pokemonACapturar->especie, pokemon1->especie);
//				}
	//list_remove_by_condition(pokemonsRequeridos, (void*)mismaEspecie);
	//removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	//free((*entrenador)->pokemonACapturar);
	//(*entrenador)->pokemonACapturar = NULL;

}


void configurarObjetivoGlobal(){ //funciona
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
		removerPokemon(pokemon,objetivoGlobal);
		free(pokemon);
	}
	free(pokemons);

}

void removerPokemon(char* pokemon, t_list* lista){ //funciona
	bool mismoPokemon(char* pokemon1 ){
			return string_equals_ignore_case(pokemon, pokemon1);
			}
		list_remove_by_condition(lista, (void*)mismoPokemon);
}


uint32_t distancia(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2){ //funciona

	return abs(x2-x1)+abs(y2-y1);

}

void moverEntrenador(t_entrenador** entrenador, uint32_t x, uint32_t y){ //funciona
	int moverse = 1;

	if(x < (*entrenador)->coordx)	moverse = -1;

	while((*entrenador)->coordx != x){
		(*entrenador)->coordx += moverse;
		sleep(config_get_int_value(config,"RETARDO_CICLO_CPU"));
	}

	if(y < (*entrenador)->coordy)	moverse = -1;
	else moverse = 1;

	while((*entrenador)->coordy != y){
		(*entrenador)->coordy += moverse;
		sleep(config_get_int_value(config,"RETARDO_CICLO_CPU"));
	}

}

void atraparPokemon(t_entrenador* entrenador){//terminar y probar
	//enviar mensaje catch
	sleep(config_get_int_value(config, "RETARDO_CICLO_CPU"));
}

void intercambiarPokemon(t_entrenador** entrenador){ // Probar si funciona
	t_intercambio* intercambio = (*entrenador)->intercambio;
	removerPokemon(intercambio->pokemonAEntregar, (*entrenador)->pokemons);
	list_add((*intercambio->entrenador)->pokemons, intercambio->pokemonAEntregar);
	removerPokemon(intercambio->pokemonARecibir, (*intercambio->entrenador)->pokemons);
	list_add((*entrenador)->pokemons, intercambio->pokemonARecibir);
	free(intercambio);
	(*entrenador)->intercambio = NULL;
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
	else cambiarEstado(entrenador, BLOCK);
	if(cumpleObjetivoParticular(*intercambio->entrenador)) cambiarEstado(intercambio->entrenador, EXIT);
	else cambiarEstado(entrenador, BLOCK);
	sleep(config_get_int_value(config,"RETARDO_CICLO_CPU")*5);
}

void planificar(){// Probar si funciona
	char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	if(strcmp(algoritmo, "FIFO") == 0) planificarFIFO();
//	if(strcmp(algoritmo, "RR") == 0) planificarRR();
//	if(strcmp(algoritmo, "SJF-CD") == 0) planificarSJF_CD();
//	if(strcmp(algoritmo, "SJF-SD") == 0) planificarSJF_SD();
}
void planificarFIFO(){// Probar si funciona y agregar hilo sockets
	//pthread_t escuchaSockets;
	pthread_t llenaReady;
	pthread_t ejecuta;
	//hilo que escuche sockets


	//hilo que llena la lista de ready

	t_queue* ready = queue_create();
	pthread_create(&llenaReady, NULL, (void*)llenarColaReady, (void*)ready);

	//hilo que va decidiendo quien ejecuta
	pthread_create(&ejecuta, NULL, (void*)ejecutaEntrenadores, (void*)ready);

}

void ejecutaEntrenadores(t_queue* ready){ //agregar semaforos y probar
	while(!cumpleObjetivoGlobal()){
			t_entrenador* entrenador = queue_pop(ready);
			cambiarEstado(&entrenador, EXEC);
			//activa semaforo exec
			//espera semoforo de entrenador termina exec
		}
}

void llenarColaReady(t_queue* ready){ // Terminar y probar
	t_list* pokemonRequerido;
	ready = queue_create();
	t_entrenador** entrenadorAPlanificar;

	while(!cumpleObjetivoGlobal()){

		pokemonRequerido = list_take_and_remove(pokemonsRequeridos, 1);
		bool _menorDistancia(void* elem1 , void* elem2){
				return menorDistancia(elem1, elem2, pokemonRequerido->head->data);
			}
		list_sort(entrenadores, _menorDistancia);
		//obtenerdireccion del primero que cumple con estado new o block
		//t_entrenador** entrenadorAPlanificar= ??

		(*entrenadorAPlanificar)->pokemonACapturar = pokemonRequerido->head->data;
		cambiarEstado(entrenadorAPlanificar, READY);
		queue_push(ready, *entrenadorAPlanificar);
	}

}

bool menorDistancia(t_entrenador* elem1, t_entrenador* elem2, t_pokemon* pokemon){ //probar
	return (distancia(elem1->coordx, elem1->coordy, pokemon->coordx, pokemon->coordy)<distancia(elem2->coordx, elem2->coordy, pokemon->coordx, pokemon->coordy));
}

void* entrenadorMaster(void* entre){// Probar y agregar semaforos
	t_entrenador** entrenador = entre;
	t_intercambio* intercambio = (*entrenador)->intercambio;
	while((*entrenador)->estado == EXEC){
		if(puedeAtraparPokemon((*entrenador))){
			moverEntrenador(entrenador, (*entrenador)->pokemonACapturar->coordx, (*entrenador)->pokemonACapturar->coordy);
			atraparPokemon(*entrenador);
			cambiarEstado(entrenador, BLOCK);//avisar hilo planificacion que termino de ejecutar ¿¿id de mensaje??
		}else {
			moverEntrenador(entrenador, (*intercambio->entrenador)->coordx , (*intercambio->entrenador)->coordy);
			intercambiarPokemon(entrenador);//avisar hilo planificacion que termino de ejecutar
			}
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
	}
	//pthread_exit?
	return 0;
}

