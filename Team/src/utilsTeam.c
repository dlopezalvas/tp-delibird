#include "utilsTeam.h"
extern t_list* entrenadores;
extern t_list* objetivoGlobal;
extern pthread_mutex_t semaforo;
extern t_list* pokemonsRequeridos;
extern t_config* config;
extern t_log* logger;
extern t_queue* ready;


void iniciarTeam(){ //funciona
	config = leer_config(PATH);
	logger = iniciar_logger(config);
	ready = queue_create();
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
	list_destroy(objetivoGlobal);
	queue_destroy(ready);
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------");
	log_destroy(logger);
}

void entrenadorDestroy(t_entrenador * entrenador) { //probar con valgrind
	list_destroy(entrenador->objetivos);
    list_destroy(entrenador->pokemons);
    free(entrenador);
}

void configurarEntrenadores(){ //funciona

	char** posiciones = config_get_array_value(config, "POSICIONES_ENTRENADORES");
	char** pokemonEntrenadores = config_get_array_value(config, "POKEMON_ENTRENADORES");
	char** objetivos = config_get_array_value(config, "OBJETIVOS_ENTRENADORES");

	for(int i=0; posiciones[i];i++){
		t_entrenador* entrenador = crearEntrenador(posiciones[i], pokemonEntrenadores[i], objetivos[i], i);
		list_add(entrenadores, entrenador);
	}

//	liberar_vector(posiciones);
//	liberar_vector(pokemonEntrenadores);
//	liberar_vector(objetivos);

	return ;
}

t_entrenador* crearEntrenador(char* posicion, char* pokemonsEntrenador, char* objetivos, int ID){ //funciona
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	entrenador->ID = ID;
	entrenador->estado = NEW;
	char** objetivosEntrenador = string_split(objetivos,"|");
	entrenador->objetivos = configurarPokemons(objetivosEntrenador);
	char** coordenadas = string_split(posicion,"|");
	entrenador->coordx = atoi(coordenadas[0]);
	entrenador->coordy = atoi(coordenadas[1]);
	char** pokemons = string_split(pokemonsEntrenador,"|");
	entrenador->pokemons = configurarPokemons(pokemons);
	entrenador->pokemonACapturar = NULL;
	entrenador->pokemonsNoNecesarios = list_create();
	entrenador->intercambio = NULL;

	//	liberar_vector(objetivosEntrenador);
	//	liberar_vector(coordenadas);
	//	liberar_vector(pokemons);

	return entrenador;
}



t_list* configurarPokemons(char** pokemons){ //funciona //objetivo global
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
		if(nuevoEstado == READY || nuevoEstado == BLOCK || nuevoEstado == EXIT) return true;
		else return false;
		break;
	case BLOCK:
		if(nuevoEstado == READY || nuevoEstado == BLOCK || nuevoEstado == EXIT) return true;
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

void capturoPokemon(t_entrenador** entrenador){ // ejecuta luego de que capturo un pokemon

	if(!necesitaPokemon(*(entrenador)))//probar si funciona
		list_add((*entrenador)->pokemonsNoNecesarios, (*entrenador)->pokemonACapturar->especie);

	list_add((*entrenador)->pokemons, (*entrenador)->pokemonACapturar->especie);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	if(tieneMenosElementos ((*entrenador)->pokemons, (*entrenador)->objetivos)){
		cambiarEstado(entrenador, BLOCK);
	}else{
		if(cumpleObjetivoParticular(*entrenador)){
			cambiarEstado(entrenador, EXIT);
		}
		else{
			cambiarEstado(entrenador, BLOCK);
		}
	}
//Probar si funciona segunda parte
	bool _mismoPokemon(t_pokemon* pokemon ){
				return mismoPokemon(pokemon,(*entrenador)->pokemonACapturar);
				}
	list_remove_by_condition(pokemonsRequeridos, (void*)_mismoPokemon);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	free((*entrenador)->pokemonACapturar);
	(*entrenador)->pokemonACapturar = NULL;

}

bool mismoPokemon(t_pokemon* pokemon,t_pokemon* pokemon2){

	return(pokemon->coordx == pokemon2->coordx && pokemon->coordy == pokemon2->coordy &&
			string_equals_ignore_case(pokemon->especie, pokemon2->especie) &&
				pokemon->planificado == true);

}

bool necesitaPokemon(t_entrenador* entrenador){
	bool _mismaEspecie(char* especie){
			return mismaEspecie(especie, entrenador->pokemonACapturar->especie);
					}

	int necesarios = list_count_satisfying(entrenador->objetivos, (void*)_mismaEspecie);
	int capturados = list_count_satisfying(entrenador->pokemons, (void*)_mismaEspecie);

	if(necesarios == 0) return false;
	if(necesarios > capturados) return true;
	else return false;
	}


bool mismaEspecie(char* especie,char* especie2){

	return(string_equals_ignore_case(especie, especie2));

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
//		free(pokemon);
	}
	free(pokemons);

}

void removerPokemon(char* pokemon, t_list* lista){ //funciona
	bool mismoPokemon(char* pokemon1 ){
			return string_equals_ignore_case(pokemon, pokemon1);
			}
		list_remove_by_condition(lista, (void*)mismoPokemon);
}


int distancia(int x1, int y1, int x2, int y2){ //funciona

	return abs(x2-x1)+abs(y2-y1);

}

void moverEntrenador(t_entrenador** entrenador, int x, int y){ //funciona
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

void intercambiarPokemon(t_entrenador** entrenador){ // Funciona

	t_intercambio* intercambio = (*entrenador)->intercambio;
	printf("%s",intercambio->pokemonAEntregar);
	removerPokemon(intercambio->pokemonAEntregar, (*entrenador)->pokemons);
	removerPokemon(intercambio->pokemonAEntregar, (*entrenador)->pokemonsNoNecesarios);
	list_add(intercambio->entrenador->pokemons, intercambio->pokemonAEntregar);
	removerPokemon(intercambio->pokemonARecibir, intercambio->entrenador->pokemons);
	removerPokemon(intercambio->pokemonARecibir, intercambio->entrenador->pokemonsNoNecesarios);
	list_add((*entrenador)->pokemons, intercambio->pokemonARecibir);
	(*entrenador)->intercambio = NULL;
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
	else cambiarEstado(entrenador, BLOCK);
	if(cumpleObjetivoParticular(intercambio->entrenador)) cambiarEstado(&(intercambio->entrenador), EXIT);
	else cambiarEstado(&(intercambio->entrenador), BLOCK);
	sleep(config_get_int_value(config,"RETARDO_CICLO_CPU")*5);

}

void planificar(){//funciona
	char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	if(strcmp(algoritmo, "FIFO") == 0) planificarFIFO();
//	if(strcmp(algoritmo, "RR") == 0) planificarRR();
//	if(strcmp(algoritmo, "SJF-CD") == 0) planificarSJF_CD();
//	if(strcmp(algoritmo, "SJF-SD") == 0) planificarSJF_SD();
}
void planificarFIFO(){// funciona
	//pthread_t escuchaSockets;
	pthread_t llenaReady;
	pthread_t ejecuta;
	//hilo que escuche sockets


	//hilo que llena la lista de ready

	pthread_create(&llenaReady, NULL, (void*)llenarColaReady, NULL);

	//hilo que va decidiendo quien ejecuta
	//pthread_create(&ejecuta, NULL, (void*)ejecutaEntrenadores, (void*)ready);

}

void ejecutaEntrenadores(t_queue* ready){ //agregar semaforos y probar
	while(!cumpleObjetivoGlobal()){
			t_entrenador* entrenador = queue_pop(ready);
			cambiarEstado(&entrenador, EXEC);
			//activa semaforo exec
			//espera semoforo de entrenador termina exec
		}
}

void llenarColaReady(){ // Funciona

	t_entrenador* entrenadorAPlanificar;
	t_pokemon* pokemon;
	int i = 0;
	while(i == 0){ //cambiar condicion de while por !cumpleObjetivoGlobal()

		bool _noEstaPlanificado(void* pokemon){
			return noEstaPlanificado(pokemon);
		}


		pokemon = list_find(pokemonsRequeridos, _noEstaPlanificado);
		bool _menorDistancia(void* elem1 , void* elem2){
				return menorDistancia(elem1, elem2, pokemon);
			}
		list_sort(entrenadores, _menorDistancia);
		bool _estadoNewoBlock(void* entrenador){
						return estadoNewOBlock(entrenador);
					}
		entrenadorAPlanificar = list_find(entrenadores, _estadoNewoBlock);
		entrenadorAPlanificar->pokemonACapturar = pokemon;
		pokemon->planificado = true;
		cambiarEstado(&(entrenadorAPlanificar), READY);
		queue_push(ready, entrenadorAPlanificar);


		t_entrenador* entrenadorPrueba = queue_pop(ready);
		printf("Entrenador sacado de ready \n");

		t_pokemon* pokemonPlanificado = pokemonsRequeridos->head->data;
		if(pokemonPlanificado->planificado){
			printf("planificado \n");
		}


		printf("%s ", entrenadorPrueba->pokemonACapturar->especie);
		printf("%d ", entrenadorPrueba->pokemonACapturar->coordx);
		printf("%d \n\n", entrenadorPrueba->pokemonACapturar->coordy);
		i++;
	}

}

bool noEstaPlanificado(t_pokemon* pokemon){

	return !pokemon->planificado;

}

bool estadoNewOBlock(t_entrenador* entrenador){ //funciona
	return entrenador->estado == NEW || entrenador->estado == BLOCK;
}


bool menorDistancia(t_entrenador* elem1, t_entrenador* elem2, t_pokemon* pokemon){ //funciona
	return (distancia(elem1->coordx, elem1->coordy, pokemon->coordx, pokemon->coordy)<distancia(elem2->coordx, elem2->coordy, pokemon->coordx, pokemon->coordy));
}

void* entrenadorMaster(void* entre){// Probar y agregar semaforos
	t_entrenador** entrenador = entre;
	t_intercambio* intercambio = (*entrenador)->intercambio;
	while((*entrenador)->estado != EXIT){
		if(puedeAtraparPokemon((*entrenador))){
			moverEntrenador(entrenador, (*entrenador)->pokemonACapturar->coordx, (*entrenador)->pokemonACapturar->coordy);
			atraparPokemon(*entrenador);
			cambiarEstado(entrenador, BLOCK);//se blockea hasta que recibe respuesta de si lo capturo o no
			//avisar hilo planificacion que termino de ejecutar ¿¿id de mensaje??
		}else {
			moverEntrenador(entrenador, intercambio->entrenador->coordx , intercambio->entrenador->coordy);
			intercambiarPokemon(entrenador);//avisar hilo planificacion que termino de ejecutar
			}
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
	}
	//pthread_exit?
	return 0;
}

void appeared_pokemon(t_pokemon* pokemonNuevo){ //Agregar Verificacion
	//Verifica que lo necesite
	pokemonNuevo->planificado = false;
	list_add(pokemonsRequeridos, pokemonNuevo);

//	t_pokemon* pokemonPrueba = (pokemonsRequeridos->head->data);
//	printf("Pokemon de lista de Requeridos \n");
//	printf("%s ", (pokemonPrueba)->especie);
//	printf("%d ", pokemonPrueba->coordx);
//	printf("%d \n\n", pokemonPrueba->coordy);

	//signal sem contador llenarColaReady
}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
//	int id = recv(cliente_fd, &id,sizeof(int),0);

	t_position_and_name* get_pokemon = malloc(sizeof(t_position_and_name));

		switch (cod_op) {
		case APPEARED_POKEMON:
			get_pokemon = deserializar_position_and_name(buffer);
			puts(get_pokemon->nombre.nombre);
			//appeared_pokemon;

			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}


void esperar_cliente(int servidor){
	pthread_t thread;
	struct sockaddr_in direccion_cliente;

	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&cliente);
	pthread_detach(thread);
}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void socketEscucha(char*IP, char* Puerto){
	int servidor = iniciar_servidor(IP,Puerto);
	while(1){
		esperar_cliente(servidor);
	}

}

