#include "utilsTeam.h"
t_list* entrenadores;
t_list* objetivoGlobal;
t_list* pokemonsRequeridos;
t_list* pokemonsDeRepuesto;
t_list* especiesNecesarias;
t_list* IDs_get_pokemon;
t_list* ready;
t_queue* colaAppearedPokemon;
t_queue* colaLocalizedPokemon;
t_queue* colaCaughtPokemon;

pthread_mutex_t objetivo, requeridos, mutex_ready, mutex_ejecutar, mutex_cola_appeared_pokemon, mutex_cola_caught_pokemon, mutex_cola_localized_pokemon;
sem_t conexiones, sem_deteccionDeadlock, sem_ready, sem_ejecutar, semAppeared, semCaught, semLocalized;

t_config* config;
t_log* logger;
//t_queue* ready;
int ciclosCPUGlobal;

char* estado[5];
int cambiosDeContexto;


void iniciarTeam(){
	pthread_mutex_init(&objetivo, NULL);
	pthread_mutex_init(&requeridos, NULL);
	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_ejecutar, NULL);
	pthread_mutex_init(&mutex_cola_appeared_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_caught_pokemon, NULL);
	pthread_mutex_init(&mutex_cola_localized_pokemon, NULL);
	sem_init(&conexiones, 0,0);
	sem_init(&sem_ready,0,0);
	sem_init(&sem_ejecutar,0,0);
	sem_init(&sem_deteccionDeadlock, 0, 0);
	sem_init(&semAppeared,0,0);
	sem_init(&semLocalized,0,0);
	sem_init(&semCaught,0,0);
	estado[1] = "NEW";
	estado[2] = "READY";
	estado[3] ="EXEC";
	estado[4] = "BLOCK";
	estado[5] ="EXIT";
	cambiosDeContexto = 0;

	entrenadores = list_create();
	objetivoGlobal = list_create();
	especiesNecesarias = list_create();
	pokemonsRequeridos = list_create();
	pokemonsDeRepuesto = list_create();
	IDs_get_pokemon = list_create();
	ready = list_create();
	colaAppearedPokemon = queue_create();
	colaCaughtPokemon = queue_create();
	colaLocalizedPokemon = queue_create();

	ciclosCPUGlobal = 0;

	config = leer_config(PATH);
	logger = iniciar_logger(config);

	configurarEntrenadores(config);
	configurarObjetivoGlobal();

	void _agregarEspecie(void* pokemon){
		return agregarEspecie(pokemon, especiesNecesarias);
	}
	list_iterate(objetivoGlobal, (void*)_agregarEspecie);

	pthread_t conexionBroker;
	pthread_create(&conexionBroker, NULL, (void*)crearConexiones, NULL);
	pthread_join(conexionBroker, NULL);

	pthread_t recibirAppearedPokemon;
	pthread_create(&recibirAppearedPokemon, NULL, (void*)appeared_pokemon, NULL);
//	pthread_join(recibirAppearedPokemon, NULL);

	pthread_t conexionGameBoy;
	pthread_create(&conexionGameBoy, NULL, (void*)connect_gameboy, NULL);
	pthread_join(conexionGameBoy, NULL);
//	pthread_t thread_deadlock;
//	pthread_create(&thread_deadlock, NULL, (void*)deteccionDeadlock, NULL);
//	pthread_detach(thread_deadlock);

//	pthread_t hiloEntrenador[entrenadores->elements_count];
//	t_link_element * aux = entrenadores->head;
	//	for(int j=0; j<entrenadores->elements_count; j++){
	//		pthread_create(&hiloEntrenador[j],NULL, entrenadorMaster, (void*)(&aux->data));
	//		aux = aux->next;
	//		//join o detatch del hilo ??
	//	}

	//suscribirse al broker y reintentar conexion
}

void agregarEspecie(char* pokemon, t_list* especiesNecesarias){//funciona
	bool _mismaEspecie(char* especie){
			return mismaEspecie(especie, pokemon);
					}
	if(!list_any_satisfy(especiesNecesarias, (void*) _mismaEspecie )) list_add(especiesNecesarias, pokemon);
}

void terminarTeam(int conexion)//revisar memoria y probar si funciona
{
	log_info(logger, "Cantidad total de ciclos %d, Cantidad de cambios de contexto %d, Cantidad de ciclos CPU", ciclosCPUGlobal, cambiosDeContexto); //agregar variables
	void _entrenadorDestroy(void* entrenador){
			return entrenadorDestroy( entrenador);
		}
	for(int i = 0; i<entrenadores->elements_count ; i++){
			//destruir hilos
	}
	pthread_mutex_destroy(&requeridos);
	pthread_mutex_destroy(&objetivo);
	pthread_mutex_destroy(&mutex_ready);
	pthread_mutex_destroy(&mutex_ejecutar);

	list_destroy_and_destroy_elements(entrenadores, _entrenadorDestroy);
	list_destroy(objetivoGlobal);
	list_destroy(ready);
	config_destroy(config);
	//liberar_conexion(conexion);
	log_info(logger,"-----------LOG END--------"); //borrar
	log_destroy(logger);
}

void entrenadorDestroy(t_entrenador * entrenador) { //probar con valgrind
	list_destroy(entrenador->objetivos);
    list_destroy(entrenador->pokemons);
    list_destroy(entrenador->pokemonsNoNecesarios);
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
	entrenador->estado = BLOCK;
	char** objetivosEntrenador = string_split(objetivos,"|");
	entrenador->objetivos = configurarPokemons(objetivosEntrenador);
	char** coordenadas = string_split(posicion,"|");
	entrenador->coordx = atoi(coordenadas[0]);
	entrenador->coordy = atoi(coordenadas[1]);
	char** pokemons = string_split(pokemonsEntrenador,"|");
	entrenador->pokemons = configurarPokemons(pokemons);
	entrenador->pokemonACapturar = NULL;
	entrenador->intercambio = NULL;
	entrenador->CiclosCPU = 0;
	entrenador->catch_id = 0;
	pthread_mutex_init(&(entrenador->mutex), NULL);
	pthread_mutex_lock(&(entrenador->mutex));

	entrenador->pokemonsNoNecesarios = list_duplicate(entrenador->pokemons);
	void _eliminarPokemonsObjetivo(void* pokemon){
		return eliminarPokemonsObjetivoParticular(pokemon, entrenador->pokemonsNoNecesarios);
	}
	list_iterate(entrenador->objetivos, (void*)_eliminarPokemonsObjetivo);
	if(cumpleObjetivoParticular(entrenador)) {
		cambiarEstado(&entrenador, EXIT, "cumplio su objetivo particular");
	}

	//	liberar_vector(objetivosEntrenador);
	//	liberar_vector(coordenadas);
	//	liberar_vector(pokemons);

	return entrenador;
}

void eliminarPokemonsObjetivoParticular(char* pokemon, t_list* pokemonsNoNecesarios){
	bool _mismaEspecie(void* pokemonNoNecesario){
		return mismaEspecie(pokemonNoNecesario, pokemon);
	}
	list_remove_by_condition(pokemonsNoNecesarios, _mismaEspecie);
}

t_list* configurarPokemons(char** pokemons){ //funciona //objetivo global
	t_list* listaPokemons = list_create();

	for(int i=0; pokemons[i];i++){
		list_add(listaPokemons, (pokemons[i]));
	}

	return listaPokemons;
}

void cambiarEstado (t_entrenador** entrenador,t_estado nuevoEstado, char* razonDeCambio){ //funciona
	if((*entrenador)->estado == EXEC) cambiosDeContexto ++;
	if(cambioEstadoValido((*entrenador)->estado, nuevoEstado)){
	(*entrenador)->estado = nuevoEstado;
	log_info(logger, "Se cambio al entrenador %d a la cola %s porque %s", (*entrenador)->ID, estado[(*entrenador)->estado], razonDeCambio);
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
//	bool _criterioOrden(void* elem1 , void* elem2){
//		return criterioOrden(elem1, elem2);
//	}
//	list_sort(entrenador->objetivos, _criterioOrden);
//	list_sort(entrenador->pokemons, _criterioOrden);
	return listasIguales(entrenador->objetivos, entrenador->pokemons);
}

bool tieneMenosElementos (t_list* listaChica, t_list* lista ){	//funciona
	if(list_size(listaChica) < list_size(lista)) return true;
	return false;
}

bool listasIguales(t_list* lista1, t_list* lista2){ //funciona
	bool _criterioOrden(void* elem1 , void* elem2){
			return criterioOrden(elem1, elem2);
		}
	list_sort(lista1, _criterioOrden);
	list_sort(lista2, _criterioOrden);
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


bool mismoPokemon(t_pokemon* pokemon,t_pokemon* pokemon2){

	return(pokemon->coordx == pokemon2->coordx && pokemon->coordy == pokemon2->coordy &&
			string_equals_ignore_case(pokemon->especie, pokemon2->especie) &&
				pokemon->planificado == true);

}

bool necesitaPokemon(t_entrenador* entrenador, char* pokemon){ //funciona
	bool _mismaEspecie(char* especie){
			return mismaEspecie(especie, pokemon);
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

void moverEntrenador(t_entrenador** entrenador, int x, int y){ //probar si funciona
	int moverse = 1;

	if((*entrenador)->coordx != x){
		if(x < (*entrenador)->coordx)	moverse = -1;
		(*entrenador)->coordx += moverse;
		sleep(config_get_int_value(config,"RETARDO_CICLO_CPU"));
		ciclosCPUGlobal ++;
		(*entrenador)->CiclosCPU ++;
		log_info(logger,"Se ha movido al entrenador %d a la posicion (%d,%d)", (*entrenador)->ID,(*entrenador)->coordx, (*entrenador)->coordy);
	}else{
		if((*entrenador)->coordy != y){
			if(y < (*entrenador)->coordy)	moverse = -1;
			(*entrenador)->coordy += moverse;
			sleep(config_get_int_value(config,"RETARDO_CICLO_CPU"));
			(*entrenador)->CiclosCPU ++;
			ciclosCPUGlobal ++;
			log_info(logger,"Se ha movido al entrenador %d a la posicion (%d,%d)", (*entrenador)->ID,(*entrenador)->coordx, (*entrenador)->coordy);
		}
	}
}



void intercambiarPokemon(t_entrenador** entrenador){ // Funciona
	t_intercambio* intercambio = (*entrenador)->intercambio;
	removerPokemon(intercambio->pokemonAEntregar, (*entrenador)->pokemons);
	removerPokemon(intercambio->pokemonAEntregar, (*entrenador)->pokemonsNoNecesarios);
	if(!necesitaPokemon(intercambio->entrenador, intercambio->pokemonAEntregar))
		list_add(intercambio->entrenador->pokemonsNoNecesarios, intercambio->pokemonAEntregar);
	list_add(intercambio->entrenador->pokemons, intercambio->pokemonAEntregar);
	removerPokemon(intercambio->pokemonARecibir, intercambio->entrenador->pokemons);
	removerPokemon(intercambio->pokemonARecibir, intercambio->entrenador->pokemonsNoNecesarios);
	if(!necesitaPokemon((*entrenador), intercambio->pokemonARecibir))
		list_add((*entrenador)->pokemonsNoNecesarios, intercambio->pokemonARecibir);
	list_add((*entrenador)->pokemons, intercambio->pokemonARecibir);
	log_info(logger,"Se ha realizado un intercambio entre el entrenador %d y el entrenador %d", (*entrenador)->ID, intercambio->entrenador->ID);
	(*entrenador)->intercambio = NULL;


//	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT, "cumplio su objetivo particular");
//	else cambiarEstado(entrenador, BLOCK, "termino un intercambio");
//	if(cumpleObjetivoParticular(intercambio->entrenador)) cambiarEstado(&(intercambio->entrenador), EXIT, "cumplio su objetivo particular");
//	else cambiarEstado(&(intercambio->entrenador), BLOCK, "termino un intercambio");
	sleep(config_get_int_value(config,"RETARDO_CICLO_CPU")*5);
	(*entrenador)->CiclosCPU += 5;
	ciclosCPUGlobal += 5;
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
	pthread_create(&ejecuta, NULL, (void*)ejecutaEntrenadores, NULL);

}

void ejecutaEntrenadores(){ //agregar semaforos y probar
	while(!cumpleObjetivoGlobal()){
			sem_wait(&sem_ejecutar);
			pthread_mutex_lock(&mutex_ejecutar);
			pthread_mutex_lock(&mutex_ready);
			t_entrenador* entrenador = ready->head->data;//list_get(ready, 0);
			pthread_mutex_unlock(&mutex_ready);
			cambiarEstado(&entrenador, EXEC, "va a ejecutar");
			//activa semaforo exec
			pthread_mutex_unlock(&(entrenador->mutex));
			pthread_mutex_lock(&mutex_ejecutar);
			if(entrenadoresTienenElInventarioLleno()){ //probar
				sem_post(&sem_deteccionDeadlock);
			}
		}
	pthread_exit(NULL);
}

void llenarColaReady(){ // probar

	t_entrenador* entrenadorAPlanificar;
	t_pokemon* pokemon;

//	bool puedePlanificar(t_entrenador* entrenador){
//		return tieneMenosElementos (entrenador->pokemons, entrenador->objetivos) && entrenador->pokemonACapturar == NULL;
//	}
	while(!entrenadoresTienenElInventarioLleno()){

		bool _noEstaPlanificado(void* pokemon){
			return noEstaPlanificado(pokemon);
		}
		sem_wait(&sem_ready);
		pthread_mutex_lock(&requeridos);
		pokemon = list_find(pokemonsRequeridos, _noEstaPlanificado);
		pthread_mutex_unlock(&requeridos);
		bool _menorDistancia(void* elem1 , void* elem2){
				return menorDistancia(elem1, elem2, pokemon);
			}
		list_sort(entrenadores, _menorDistancia);
		bool _estadoNewoBlock(void* entrenador){
						return estadoNewOBlock(entrenador);
					}
		entrenadorAPlanificar = list_find(entrenadores, _estadoNewoBlock);
		pthread_mutex_lock(&requeridos);
		entrenadorAPlanificar->pokemonACapturar = pokemon;
		pokemon->planificado = true;
		pthread_mutex_unlock(&requeridos);
		cambiarEstado(&(entrenadorAPlanificar), READY, "se le asigno el pokemon a atrapar");
		pthread_mutex_lock(&mutex_ready);
		list_add(ready, entrenadorAPlanificar);
		pthread_mutex_unlock(&mutex_ready);
		sem_post(&sem_ejecutar);
	}
	return;
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
	t_intercambio* intercambio;
	int coordx;
	int coordy;

	while((*entrenador)->estado != EXIT){ //agregar semaforo para que no pueda entrar al ciclo.
		pthread_mutex_lock(&((*entrenador)->mutex));
		intercambio = (*entrenador)->intercambio;
		if(puedeAtraparPokemon(*entrenador)){
				coordx = (*entrenador)->pokemonACapturar->coordx;
				coordy = (*entrenador)->pokemonACapturar->coordy;
		}else{
				coordx = intercambio->entrenador->coordx;
				coordy = intercambio->entrenador->coordy;
		}
		pthread_mutex_unlock(&((*entrenador)->mutex));
		while((coordx != (*entrenador)->coordx	&& coordy != (*entrenador)->coordy)){
			pthread_mutex_lock(&((*entrenador)->mutex));
			moverEntrenador(entrenador, coordx, coordy);
			pthread_mutex_unlock(&mutex_ejecutar);
					}
		if(puedeAtraparPokemon(*entrenador)){
			pthread_mutex_lock(&((*entrenador)->mutex));
			cambiarEstado(entrenador, BLOCK, "esta esperando el resultado de intentar atrapar pokemon");
			catch_pokemon(config_get_string_value(config, "IP_BROKER"), config_get_string_value(config, "PUERTO_BROKER"), entrenador);
			pthread_mutex_unlock(&mutex_ejecutar);
		}else {
			intercambiarPokemon(entrenador);
			}
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT, "cumplio su objetivo particular");
	}
	//pthread_exit(NULL); agregar el hilo
	return 0;
}


//void atraparPokemon(t_entrenador* entrenador){//terminar y probar
//	//enviar mensaje catch
//	log_info(logger, "Atrapar %s en la posicion (%d,%d)", entrenador->pokemonACapturar->especie, entrenador->pokemonACapturar->coordx,entrenador->pokemonACapturar->coordy);
//	sleep(config_get_int_value(config, "RETARDO_CICLO_CPU"));
//	entrenador->CiclosCPU ++;
//	ciclosCPUGlobal ++;
//}




void process_request(int cod_op, int cliente_fd) { //funciona

	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);

	t_position_and_name* appeared;
	t_localized_pokemon* localized;
	t_caught_pokemon* caught;

	bool _mismaEspecie(char* especie1){
		return mismaEspecie(especie1, appeared->nombre.nombre);
	}
	bool _tieneMismoIDCatch(void* entrenador){
				return tienemismoIdCatch(entrenador, caught->correlation_id);
			}

	switch (cod_op) {
	case APPEARED_POKEMON:
		appeared = deserializar_position_and_name(buffer);
		puts(appeared->nombre.nombre);
		puts(string_itoa(appeared->id));
		if(list_any_satisfy(objetivoGlobal, (void*)_mismaEspecie)){
			log_info(logger, "Mensaje %d Appeared_pokemon %s %d %d", appeared->id, appeared->nombre.nombre, appeared->coordenadas.pos_x, appeared->coordenadas.pos_y);
			pthread_mutex_lock(&mutex_cola_appeared_pokemon);
			queue_push(colaAppearedPokemon,appeared);
			pthread_mutex_unlock(&mutex_cola_appeared_pokemon);
			sem_post(&semAppeared);
		}
		break;

	case CAUGHT_POKEMON:
		caught = deserializar_caught_pokemon(buffer);
		if(list_any_satisfy(entrenadores, (void*)_tieneMismoIDCatch)){
			log_info(logger, "Mensaje Caught_pokemon %d %d", caught->caught, caught->correlation_id);
			//caught_pokemon(); //usar capturo_pokemon
			pthread_mutex_lock(&mutex_cola_caught_pokemon);
			queue_push(colaCaughtPokemon,caught);
			pthread_mutex_unlock(&mutex_cola_caught_pokemon);
			sem_post(&semCaught);
		}

		break;

	case LOCALIZED_POKEMON:

		puts("recibe mensaje");
		localized = deserializar_localized_pokemon(buffer);
		bool mismoIdMensaje(int* ID){
			return  (localized->correlation_id == *ID);
		}
		if(list_any_satisfy(especiesNecesarias, (void*)_mismaEspecie) && list_any_satisfy(IDs_get_pokemon, (void*)mismoIdMensaje)){
			char* mensaje = string_new();
			string_append_with_format(&mensaje, "Mensaje %d localized_pokemon %s %d", localized->id, localized->nombre.nombre, localized->cantidad);
			coordenadas_pokemon* coord;
			for(int i = 0; i<localized->cantidad; i++){
				coord = list_get(localized->listaCoordenadas, i);
				string_append_with_format(&mensaje, " %d %d", coord->pos_x, coord->pos_y);
			}
			string_append_with_format(&mensaje, " correlation id: %d", localized->correlation_id);
			log_info(logger, mensaje);
			pthread_mutex_lock(&mutex_cola_localized_pokemon);
			queue_push(colaLocalizedPokemon,localized);
			pthread_mutex_unlock(&mutex_cola_localized_pokemon);
			sem_post(&semLocalized);
		}
		puts(localized->nombre.nombre);
		puts("deserializo");

		break;
		//		case 0:
		//			//pthread_exit(NULL);
		//		case -1:
		//			//pthread_exit(NULL);
	}

}

void esperar_cliente(int servidor){ //funciona
	pthread_t thread;
	struct sockaddr_in direccion_cliente;

	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);

	pthread_create(&thread,NULL,(void*)serve_client,&cliente);
	pthread_detach(thread);
}

void serve_client(int* socket) //funciona
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}





void socketEscucha(char* ip, char* puerto){ //funciona
	int servidor = iniciar_servidor(ip, puerto);
	int i = 1;
	while(1){
		esperar_cliente(servidor);
		i++;
	}
}

void deteccionDeadlock(){ //funciona
	sem_wait(&sem_deteccionDeadlock); //probar
	sem_wait(&sem_deteccionDeadlock);
	log_info(logger,"Se ha iniciado el algoritmo de deteccion de deadlock");
	int cantDeadlocks = 0;
	bool _puedeEstarEnDeadlock(void* entrenador){
		return puedeEstarEnDeadlock(entrenador);
	}
	t_list* entrenadoresDeadlock = list_create();
	entrenadoresDeadlock = list_filter(entrenadores, _puedeEstarEnDeadlock);

	t_entrenador* entrenador;
	t_entrenador* entrenadorAIntercambiar;
	int ID;
	char* pokemon;
	t_intercambio* intercambio;

	bool _tienePokemonNoNecesario(void* entrenador){
		return tienePokemonNoNecesario(entrenador, pokemon);
	}

	bool _necesitaPokemon(void* pokemon){
		return necesitaPokemon(entrenador, pokemon);
	}
	bool _mismoID(t_entrenador* entrenador1){
		return mismoID(entrenador1, ID);
	}

	while(list_size(entrenadoresDeadlock) > 0){

		entrenador = entrenadoresDeadlock->head->data;
		while(list_size(entrenador->pokemonsNoNecesarios) > 0 ){
			pokemon = list_find(entrenador->objetivos, _necesitaPokemon); //devuelve el primer pokemon que necesita el entrenador
			entrenadorAIntercambiar = list_find(entrenadoresDeadlock, _tienePokemonNoNecesario); //devuelve entrenador que tenga al pokemon en pokemons no necesarios
			intercambio = malloc(sizeof(t_intercambio));
			intercambio->entrenador = entrenadorAIntercambiar;
			intercambio->pokemonARecibir = pokemon;
			intercambio->pokemonAEntregar = entrenador->pokemonsNoNecesarios->head->data;
			entrenador->intercambio = intercambio;
			printf("%s \n",intercambio->pokemonAEntregar);
			printf("%s \n",intercambio->pokemonARecibir);

			if(necesitaPokemon(intercambio->entrenador, intercambio->pokemonAEntregar)) cantDeadlocks++;

			list_add(ready, entrenador);
			//wait (espera a que termine de moverse el entrenador)
			intercambiarPokemon(&entrenador);
			if(cumpleObjetivoParticular(entrenadorAIntercambiar)){
				puts("cumple objetivo Entrenador a intercambiar");
				cambiarEstado(&entrenadorAIntercambiar, EXIT, "cumplio su objetivo particular");
				ID = entrenadorAIntercambiar->ID;
				list_remove_by_condition(entrenadoresDeadlock, (void*)_mismoID);
				}
			else{
				cambiarEstado(&entrenadorAIntercambiar, BLOCK, "termino de intercambiar pokemon");
			}
			if(cumpleObjetivoParticular(entrenador)) {
				puts("cumple objetivo entrenador");
				cambiarEstado(&entrenador, EXIT, "cumplio su objetivo particular");
				ID = entrenador->ID;
				list_remove_by_condition(entrenadoresDeadlock,(void*)_mismoID);
			}
			else{
				cambiarEstado(&entrenador, BLOCK, "termino de intercambiar pokemon");
			}
		}
	}
	log_info(logger, "Cantidad de deadlocks Detectados: %d Cantidad de deadlocks Resueltos: %d", cantDeadlocks, cantDeadlocks);
}

bool tienePokemonNoNecesario(t_entrenador* entrenador, char* pokemon){ //funciona
	bool _mismaEspecie(char* especie){
		return mismaEspecie(especie, pokemon);
		}
	return list_any_satisfy(entrenador->pokemonsNoNecesarios, (void*)_mismaEspecie);
}

bool puedeEstarEnDeadlock(t_entrenador* entrenador){//funciona
	return (entrenador->estado == BLOCK &&(entrenador->pokemons->elements_count ==  entrenador->objetivos->elements_count));
}

bool mismoID(t_entrenador* entrenador, int ID){ //funciona
	return entrenador->ID == ID;
}

void connect_appeared(){
	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"APPEARED_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_team(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);

	puts("envia mensaje");

	int size = 0;
	t_position_and_name* appeared;
	bool _mismaEspecie(char* especie1){
		return mismaEspecie(especie1, appeared->nombre.nombre);
	}
	int cod_op;

	while(!(cumpleObjetivoGlobal())){

		if(recv(socket_broker, &cod_op, sizeof(int), MSG_WAITALL) == 0){
			log_info(logger,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);
		}
		void* buffer = recibir_mensaje(socket_broker,&size);

		if(cod_op == APPEARED_POKEMON){

			puts("recibe mensaje");
			appeared = deserializar_position_and_name(buffer);
			if(list_any_satisfy(objetivoGlobal, (void*)_mismaEspecie)){
				log_info(logger, "Mensaje Appeared_pokemon %s %d %d", appeared->nombre.nombre, appeared->coordenadas.pos_x, appeared->coordenadas.pos_y);
				//appeared_pokemon(appeared) ; //hacer dentro de un hilo?
				pthread_mutex_lock(&mutex_cola_appeared_pokemon);
				queue_push(colaAppearedPokemon,appeared);
				pthread_mutex_unlock(&mutex_cola_appeared_pokemon);
				sem_post(&semAppeared);
			}
			puts(appeared->nombre.nombre);
			puts("deserializo");
		}
	}

	liberar_conexion(socket_broker); //hacer en finalizar team?

//	free(mensaje -> parametros);
//	free(mensaje);
}
void get_pokemon(char*especie, int socket_broker, t_list* IDs){
	op_code codigo_operacion = GET_POKEMON;
	puts(especie);
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
	char* linea_split = string_new();
	string_append_with_format(&linea_split, "%s,%s", especie, "0");
	puts(linea_split);
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = string_split(linea_split, ",");
	puts("antes enviar");
	enviar_mensaje(mensaje, (socket_broker));
	uint32_t id;
	recv(socket_broker, &id, sizeof(uint32_t), MSG_WAITALL);
	list_add(IDs, &id);
	puts(string_itoa(id));
}

void connect_localized_pokemon(){

	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"LOCALIZED_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_team(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);
	puts("envia mensaje");


	void _get_pokemon(void* especie){
		return get_pokemon(especie, socket_broker, IDs_get_pokemon);
	}

	list_iterate(especiesNecesarias, (void*)_get_pokemon); //poner if es la primera vez que lo ejecuta??
	int size = 0;
	t_localized_pokemon* localized;
	bool _mismaEspecie(char* especie1){
		return mismaEspecie(especie1, localized->nombre.nombre);
	}
	bool mismoIdMensaje(int* ID){
			return  (localized->correlation_id == *ID);
		}
	int cod_op;

	while(!(cumpleObjetivoGlobal())){

		if(recv(socket_broker, &cod_op, sizeof(int), MSG_WAITALL) == 0){
			log_info(logger,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);

		}
		void* buffer = recibir_mensaje(socket_broker,&size);

		if(cod_op == LOCALIZED_POKEMON){

			puts("recibe mensaje");
			localized = deserializar_localized_pokemon(buffer);
			if(list_any_satisfy(especiesNecesarias, (void*)_mismaEspecie) && list_any_satisfy(IDs_get_pokemon, (void*)mismoIdMensaje)){
				char* mensaje = string_new();
				string_append_with_format(&mensaje, "Mensaje %d localized_pokemon %s %d", localized->id, localized->nombre.nombre, localized->cantidad);
				coordenadas_pokemon* coord;
				for(int i = 0; i<localized->cantidad; i++){
					coord = list_get(localized->listaCoordenadas, i);
					string_append_with_format(&mensaje, " %d %d", coord->pos_x, coord->pos_y);
				}
				string_append_with_format(&mensaje, " correlation id: %d", localized->correlation_id);
				log_info(logger, mensaje);
				pthread_mutex_lock(&mutex_cola_localized_pokemon);
				queue_push(colaLocalizedPokemon,localized);
				pthread_mutex_unlock(&mutex_cola_localized_pokemon);
				sem_post(&semLocalized);
			}
			puts(localized->nombre.nombre);
			puts("deserializo");
		}
	}

	liberar_conexion(socket_broker); //hacer en finalizar team?

	//	free(mensaje -> parametros);
	//	free(mensaje);
}

void connect_caught_pokemon(){
	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"CAUGHT_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_team(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);
	puts("envia mensaje");

	int size = 0;
	t_caught_pokemon* caught;

	bool _tieneMismoIDCatch(void* entrenador){
		return tienemismoIdCatch(entrenador, caught->correlation_id);
	}
	int cod_op;

	while(!(cumpleObjetivoGlobal())){

		if(recv(socket_broker, &cod_op, sizeof(int), MSG_WAITALL) == 0){
			log_info(logger,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);
		}
		void* buffer = recibir_mensaje(socket_broker,&size);
		if(cod_op == CAUGHT_POKEMON){
			puts("recibe mensaje");
			caught = deserializar_caught_pokemon(buffer);
			if(list_any_satisfy(entrenadores, (void*)_tieneMismoIDCatch)){
				log_info(logger, "Mensaje Caught_pokemon %d %d", caught->caught, caught->id);
				pthread_mutex_lock(&mutex_cola_caught_pokemon);
				queue_push(colaCaughtPokemon,caught);
				pthread_mutex_unlock(&mutex_cola_caught_pokemon);
				sem_post(&semCaught);
			}
		}
	}
}


bool tienemismoIdCatch(t_entrenador* entrenador, uint32_t correlation_id){
	return entrenador->catch_id == correlation_id;
}

void connect_gameboy(){
	socketEscucha(config_get_string_value(config, "IP_TEAM"), config_get_string_value(config, "PUERTO_TEAM"));
}

int iniciar_cliente_team(char* ip, char* puerto){
	struct sockaddr_in direccion_servidor;

	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = inet_addr(ip);
	direccion_servidor.sin_port = htons(atoi(puerto));
	int cliente = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(cliente, (void*) &direccion_servidor, sizeof(direccion_servidor)) !=0){
		log_info(logger, "No se pudo realizar la conexion");
		liberar_conexion(cliente);
		sem_post(&conexiones);
		pthread_exit(NULL);
	}else{

	log_info(logger,"Se ha establecido una conexion con el proceso Broker");
	return cliente;
	}
}


void catch_pokemon(char* ip, char* puerto, t_entrenador** entrenador){ //probar

	struct sockaddr_in direccion_servidor;
	op_code codigo_operacion = CATCH_POKEMON;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));
	uint32_t ID;
	char* linea_split = string_new();
	string_append_with_format(&linea_split, "%s,%d,%d", (*entrenador)->pokemonACapturar->especie,(*entrenador)->pokemonACapturar->coordx, (*entrenador)->pokemonACapturar->coordy);

	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = string_split(linea_split,",");;

	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = inet_addr(ip);
	direccion_servidor.sin_port = htons(atoi(puerto));

	int socket_broker = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(socket_broker, (void*) &direccion_servidor, sizeof(direccion_servidor)) !=0){
		log_info(logger, "Se atrapara al pokemon por default porque no se pudo conectar con el Broker");
		capturoPokemon(entrenador);
		liberar_conexion(socket_broker);
		pthread_mutex_unlock(&((*entrenador)->mutex));
		return;
	}else{
		enviar_mensaje(mensaje, socket_broker);
		if(recv(socket_broker, &ID, sizeof(uint32_t), MSG_WAITALL) == 0){
			log_info(logger, "Se atrapara al pokemon por default porque no se pudo conectar con el Broker");
			liberar_conexion(socket_broker);
			capturoPokemon(entrenador);
			pthread_mutex_unlock(&((*entrenador)->mutex));
			return;
		}
		(*entrenador)->catch_id = ID;
		liberar_conexion(socket_broker);
	}
	log_info(logger, "Atrapar %s en la posicion (%d,%d)", (*entrenador)->pokemonACapturar->especie, (*entrenador)->pokemonACapturar->coordx,(*entrenador)->pokemonACapturar->coordy);
	sleep(config_get_int_value(config, "RETARDO_CICLO_CPU"));
	(*entrenador)->CiclosCPU ++;
	ciclosCPUGlobal ++;
	free(mensaje);
	pthread_mutex_lock(&(*entrenador)->mutex);
}

void crearConexiones(){
	int tiempoReconexion = config_get_int_value(config, "TIEMPO_RECONEXION");
	//pthread_t appeared_pokemon_thread;
	pthread_t localized_pokemon_thread;
	//pthread_t caught_pokemon_thread;
	while(!entrenadoresTienenElInventarioLleno()){
//		pthread_create(&appeared_pokemon_thread,NULL,(void*)connect_appeared,NULL);
//		pthread_detach(appeared_pokemon_thread);
		pthread_create(&localized_pokemon_thread,NULL,(void*)connect_localized_pokemon,NULL);
		pthread_detach(localized_pokemon_thread);
//		pthread_create(&caught_pokemon_thread,NULL,(void*)connect_caught_pokemon,NULL);
//		pthread_detach(caught_pokemon_thread);
//		sem_wait(&conexiones);
//		sem_wait(&conexiones);
		sem_wait(&conexiones);
		sleep(tiempoReconexion);
		log_info(logger, "Inicio Reintento de todas las conexiones");

	}
	sem_destroy(&conexiones);
	return;
}

bool entrenadoresTienenElInventarioLleno(){
	bool _tieneInventarioLlenoOEstaEnExit(void* entrenador){
		return tieneInventarioLlenoOEstaEnExit(entrenador);
	}
	return list_all_satisfy(entrenadores, (void*)_tieneInventarioLlenoOEstaEnExit);
}


bool tieneInventarioLlenoOEstaEnExit(t_entrenador* entrenador){
	return (puedeEstarEnDeadlock(entrenador) || entrenador->estado == EXIT);
	}
void appeared_pokemon(){

	while(!(cumpleObjetivoGlobal())){

		sem_wait(&semAppeared);
		pthread_mutex_lock(&mutex_cola_appeared_pokemon);
		t_position_and_name* appeared = queue_pop(colaAppearedPokemon);
		pthread_mutex_unlock(&mutex_cola_appeared_pokemon);
		t_pokemon* pokemonNuevo = malloc(sizeof(t_pokemon));
		pokemonNuevo->coordx = appeared->coordenadas.pos_x;
		pokemonNuevo->coordy = appeared->coordenadas.pos_y;
	//	pokemonNuevo->especie = malloc(appeared->nombre.largo_nombre);
		pokemonNuevo->especie =	appeared->nombre.nombre;
		pokemonNuevo->planificado = false;
		bool _mismaEspecie(char* especie){
					return mismaEspecie(especie, pokemonNuevo->especie);
							}
		bool _mismaPokemon(t_pokemon* pokemon){
					return mismaEspecie(pokemon->especie, pokemonNuevo->especie);
				}

		if(list_any_satisfy(especiesNecesarias, (void*)_mismaEspecie))
			list_remove_by_condition(especiesNecesarias, (void*)_mismaEspecie); //para marcar que ya nos llego un appeared de esta especie

		pthread_mutex_lock(&objetivo);
		int necesarios = list_count_satisfying(objetivoGlobal, (void*)_mismaEspecie);
		pthread_mutex_unlock(&objetivo);
		pthread_mutex_lock(&requeridos);
		int pokemonsACapturar = list_count_satisfying(pokemonsRequeridos, (void*)_mismaPokemon);
		pthread_mutex_unlock(&requeridos);
		if(necesarios>pokemonsACapturar){
			pthread_mutex_lock(&requeridos);
			list_add(pokemonsRequeridos, pokemonNuevo);
			pthread_mutex_unlock(&requeridos);
			sem_post(&sem_ready);
			puts("appeared pokemon");
		}else{
			if(necesarios>0) list_add(pokemonsDeRepuesto, &pokemonNuevo);
		}

		printf("%d", pokemonsRequeridos->elements_count);
	}
}

void localized_pokemon(){
	t_localized_pokemon* localized;
	t_position_and_name* appeared;
	void _llenarAppearedPokemon(void* coordenada){
		return llenarAppearedPokemon(coordenada, localized, appeared);
	}

	while(!(cumpleObjetivoGlobal())){
		sem_wait(&semLocalized);
		pthread_mutex_lock(&mutex_cola_localized_pokemon);
		localized = queue_pop(colaLocalizedPokemon);
		pthread_mutex_unlock(&mutex_cola_localized_pokemon);
		list_iterate(localized->listaCoordenadas, (void*)_llenarAppearedPokemon);

	}
}
void llenarAppearedPokemon(coordenadas_pokemon* coord ,t_localized_pokemon* localized, t_position_and_name* appeared){
	appeared = malloc(sizeof(t_position_and_name));
	appeared->coordenadas.pos_x = coord->pos_x;
	appeared->coordenadas.pos_y = coord->pos_y;
	appeared->nombre.nombre = localized->nombre.nombre;
	appeared->nombre.largo_nombre = localized->nombre.largo_nombre;
	pthread_mutex_lock(&mutex_cola_appeared_pokemon);
	queue_push(colaAppearedPokemon,appeared);
	pthread_mutex_unlock(&mutex_cola_appeared_pokemon);
	sem_post(&semAppeared);
}

void caught_pokemon(){
	t_entrenador* entrenador;
	t_caught_pokemon* caught;
	bool _tieneMismoIDCatch(void* entrenador){
			return tienemismoIdCatch(entrenador, caught->correlation_id);
		}
	while(!(cumpleObjetivoGlobal())){
		sem_wait(&semCaught);
		pthread_mutex_lock(&mutex_cola_caught_pokemon);
		caught = queue_pop(colaCaughtPokemon);
		pthread_mutex_unlock(&mutex_cola_caught_pokemon);
		if(caught->caught){
			entrenador = list_find(entrenadores, (void*)_tieneMismoIDCatch);
			capturoPokemon(&entrenador);
		}
		else{
			noCapturoPokemon(&entrenador);
		}
		pthread_mutex_unlock(&(entrenador->mutex));
	}
}
void capturoPokemon(t_entrenador** entrenador){ // ejecuta luego de que capturo un pokemon

	if(!necesitaPokemon(*(entrenador), (*entrenador)->pokemonACapturar->especie))
		list_add((*entrenador)->pokemonsNoNecesarios, (*entrenador)->pokemonACapturar->especie);

	list_add((*entrenador)->pokemons, (*entrenador)->pokemonACapturar->especie);
	pthread_mutex_lock(&objetivo);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	pthread_mutex_unlock(&objetivo);
	if(tieneMenosElementos ((*entrenador)->pokemons, (*entrenador)->objetivos)){
		cambiarEstado(entrenador, BLOCK, "capturo un pokemon");
	}else{
		if(cumpleObjetivoParticular(*entrenador)){
			cambiarEstado(entrenador, EXIT, "cumplio su objetivo particular");
		}
		else{
			cambiarEstado(entrenador, BLOCK, "capturo un pokemon y no puede capturar mas pokemons");
		}
	}
//Probar si funciona segunda parte
	bool _mismoPokemon(t_pokemon* pokemon ){
				return mismoPokemon(pokemon,(*entrenador)->pokemonACapturar);
				}
	pthread_mutex_lock(&requeridos);
	list_remove_by_condition(pokemonsRequeridos, (void*)_mismoPokemon);
	pthread_mutex_unlock(&requeridos);
	pthread_mutex_lock(&objetivo);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	pthread_mutex_unlock(&objetivo);
	free((*entrenador)->pokemonACapturar);
	(*entrenador)->pokemonACapturar = NULL;

}

void noCapturoPokemon(t_entrenador** entrenador){//Probar

	bool _mismoPokemon(t_pokemon* pokemon ){
				return mismoPokemon(pokemon,(*entrenador)->pokemonACapturar);
				}
	pthread_mutex_lock(&requeridos);
	list_remove_by_condition(pokemonsRequeridos, (void*)_mismoPokemon);
	pthread_mutex_unlock(&requeridos);
	free((*entrenador)->pokemonACapturar);
	(*entrenador)->pokemonACapturar = NULL;
}


