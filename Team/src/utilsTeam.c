#include "utilsTeam.h"
extern t_list* entrenadores;
extern t_list* objetivoGlobal;
extern pthread_mutex_t objetivo;
extern pthread_mutex_t requeridos;
extern pthread_mutex_t mutex_ready;
extern t_list* pokemonsRequeridos;
extern t_list* pokemonsDeRepuesto;
extern t_list* especiesNecesarias;
extern t_config* config;
extern t_log* logger;
extern t_queue* ready;
extern int ciclosCPUGlobal;


void iniciarTeam(){ //funciona
	config = leer_config(PATH);
	logger = iniciar_logger(config);
	ready = queue_create();
	entrenadores = list_create();
	objetivoGlobal = list_create();
	ciclosCPUGlobal = 0;
	configurarEntrenadores(config);
	configurarObjetivoGlobal();
	especiesNecesarias = list_create();

	void _agregarEspecie(void* pokemon){
		return agregarEspecie(pokemon, especiesNecesarias);
	}
	list_iterate(objetivoGlobal, (void*)_agregarEspecie);
	//mandar get pokemon de los pokemons de especiesNecesarias

	//suscribirse al broker y reintentar conexion
}

void agregarEspecie(char* pokemon, t_list* especiesNecesarias){//funciona
	bool _mismaEspecie(char* especie){
			return mismaEspecie(especie, pokemon);
					}
	if(!list_any_satisfy(especiesNecesarias, (void*) _mismaEspecie )) list_add(especiesNecesarias, pokemon);
}

void terminarTeam(int conexion, pthread_t* hilo)//revisar memoria y probar si funciona
{
	log_info(logger, "Cantidad total de ciclos %d, Cantidad de cambios de contexto, Cantidad de ciclos CPU", ciclosCPUGlobal); //agregar variables
	void _entrenadorDestroy(void* entrenador){
			return entrenadorDestroy( entrenador);
		}
	for(int i = 0; i<entrenadores->elements_count ; i++){
			//destruir hilos
	}
	pthread_mutex_destroy(&requeridos);
	pthread_mutex_destroy(&objetivo);
	pthread_mutex_destroy(&mutex_ready);

	list_destroy_and_destroy_elements(entrenadores, _entrenadorDestroy);
	list_destroy(objetivoGlobal);
	queue_destroy(ready);
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

	entrenador->pokemonsNoNecesarios = list_duplicate(entrenador->pokemons);
	void _eliminarPokemonsObjetivo(void* pokemon){
		return eliminarPokemonsObjetivo(pokemon, entrenador->pokemonsNoNecesarios);
	}
	list_iterate(entrenador->objetivos, (void*)_eliminarPokemonsObjetivo);
	if(cumpleObjetivoParticular(entrenador)) {
		cambiarEstado(&entrenador, EXIT);
	}

	//	liberar_vector(objetivosEntrenador);
	//	liberar_vector(coordenadas);
	//	liberar_vector(pokemons);

	return entrenador;
}

void eliminarPokemonsObjetivo(char* pokemon, t_list* pokemonsNoNecesarios){
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

	if(!necesitaPokemon(*(entrenador), (*entrenador)->pokemonACapturar->especie))
		list_add((*entrenador)->pokemonsNoNecesarios, (*entrenador)->pokemonACapturar->especie);

	list_add((*entrenador)->pokemons, (*entrenador)->pokemonACapturar->especie);
	pthread_mutex_lock(&objetivo);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	pthread_mutex_unlock(&objetivo);
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
	pthread_mutex_lock(&requeridos);
	list_remove_by_condition(pokemonsRequeridos, (void*)_mismoPokemon);
	pthread_mutex_unlock(&requeridos);
	pthread_mutex_lock(&objetivo);
	removerPokemon((*entrenador)->pokemonACapturar->especie,objetivoGlobal);
	pthread_mutex_unlock(&objetivo);
	free((*entrenador)->pokemonACapturar);
	(*entrenador)->pokemonACapturar = NULL;

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

void atraparPokemon(t_entrenador* entrenador){//terminar y probar
	//enviar mensaje catch
	log_info(logger, "Atrapar %s en la posicion (%d,%d)", entrenador->pokemonACapturar->especie, entrenador->pokemonACapturar->coordx,entrenador->pokemonACapturar->coordy);
	sleep(config_get_int_value(config, "RETARDO_CICLO_CPU"));
	entrenador->CiclosCPU ++;
	ciclosCPUGlobal ++;
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


//	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
//	else cambiarEstado(entrenador, BLOCK);
//	if(cumpleObjetivoParticular(intercambio->entrenador)) cambiarEstado(&(intercambio->entrenador), EXIT);
//	else cambiarEstado(&(intercambio->entrenador), BLOCK);
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
	//pthread_create(&ejecuta, NULL, (void*)ejecutaEntrenadores, (void*)ready);

}

void ejecutaEntrenadores(t_queue* ready){ //agregar semaforos y probar
	while(!cumpleObjetivoGlobal()){
		pthread_mutex_lock(&mutex_ready);
			t_entrenador* entrenador = queue_pop(ready);
			pthread_mutex_unlock(&mutex_ready);
			cambiarEstado(&entrenador, EXEC);
			//activa semaforo exec
			//espera semoforo de entrenador termina exec
		}
}

void llenarColaReady(){ // Funciona

	t_entrenador* entrenadorAPlanificar;
	t_pokemon* pokemon;

	bool tieneEspacioEnInventario(t_entrenador* entrenador){
		return tieneMenosElementos (entrenador->pokemons, entrenador->objetivos);
	}
	while(list_any_satisfy(entrenadores, (void*)tieneEspacioEnInventario)){ //cambiar condicion de while any  entrenador no tiene el inventario lleno

		bool _noEstaPlanificado(void* pokemon){
			return noEstaPlanificado(pokemon);
		}

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
		cambiarEstado(&(entrenadorAPlanificar), READY);
		pthread_mutex_lock(&mutex_ready);
		queue_push(ready, entrenadorAPlanificar);
		pthread_mutex_unlock(&mutex_ready);

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
	int coordx;
	int coordy;

	while((*entrenador)->estado != EXIT){
		if(puedeAtraparPokemon(*entrenador)){
				coordx = (*entrenador)->pokemonACapturar->coordx;
				coordy = (*entrenador)->pokemonACapturar->coordy;
		}else{
				coordx = intercambio->entrenador->coordx;
				coordy = intercambio->entrenador->coordy;
		}

		while((coordx != (*entrenador)->coordx	&& coordy != (*entrenador)->coordy)){
					moverEntrenador(entrenador, coordx, coordy);
					//replanificar?
					}
		if(puedeAtraparPokemon(*entrenador)){
			atraparPokemon(*entrenador);
			cambiarEstado(entrenador, BLOCK);//se blockea hasta que recibe respuesta de si lo capturo o no
			//avisar hilo planificacion que termino de ejecutar ¿¿id de mensaje??
		}else {
			intercambiarPokemon(entrenador);//avisar hilo planificacion que termino de ejecutar
			}
	if(cumpleObjetivoParticular((*entrenador))) cambiarEstado(entrenador, EXIT);
	}
	//pthread_exit?
	return 0;
}

void appeared_pokemon(t_pokemon* pokemonNuevo){
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
	if(necesarios>pokemonsACapturar){
		list_add(pokemonsRequeridos, &pokemonNuevo);
		puts("appeared pokemon");
	}else{
		if(necesarios>0) list_add(pokemonsDeRepuesto, &pokemonNuevo);
	}
	pthread_mutex_unlock(&requeridos);
	//signal sem contador llenarColaReady
}

void process_request(int cod_op, int cliente_fd) { //funciona

	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
//	int id = recv(cliente_fd, &id,sizeof(int),0);

	t_position_and_name* appeared = malloc(sizeof(t_position_and_name));
	t_pokemon* nuevoPokemon = malloc(sizeof(t_pokemon));



		switch (cod_op) {
		case APPEARED_POKEMON:
			appeared = deserializar_position_and_name(buffer);
			nuevoPokemon->coordx = appeared->coordenadas.pos_x;
			nuevoPokemon->coordy = appeared->coordenadas.pos_y;
			nuevoPokemon->especie = appeared->nombre.nombre;
			nuevoPokemon->planificado = false;
			puts("aaaaaa");
			appeared = deserializar_position_and_name(buffer);
			puts(appeared->nombre.nombre);
			appeared_pokemon(nuevoPokemon);
			printf("pokemons requeridos %d", pokemonsRequeridos->elements_count);
			puts("a");
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
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

void socketEscucha(char*IP, char* Puerto){ //funciona
	int servidor = iniciar_servidor(IP,Puerto);
	int i = 1;
	while(1){
		esperar_cliente(servidor);
		i++;
	}
}

void deteccionDeadlock(){ //funciona
	log_info(logger,"Se ha iniciado el algoritmo de deteccion de deadlock");
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

			//agregar cola de ready
			//wait (espera a que termine de moverse el entrenador)
			intercambiarPokemon(&entrenador);
			if(cumpleObjetivoParticular(entrenadorAIntercambiar)){
				puts("cumple objetivo Entrenador a intercambiar");
				cambiarEstado(&entrenadorAIntercambiar, EXIT);
				ID = entrenadorAIntercambiar->ID;
				list_remove_by_condition(entrenadoresDeadlock, (void*)_mismoID);
				}
			else{
				cambiarEstado(&entrenadorAIntercambiar, BLOCK);
			}
			if(cumpleObjetivoParticular(entrenador)) {
				puts("cumple objetivo entrenador");
				cambiarEstado(&entrenador, EXIT);
				ID = entrenador->ID;
				list_remove_by_condition(entrenadoresDeadlock,(void*)_mismoID);
			}
			else{
				cambiarEstado(&entrenador, BLOCK);
			}
		}
	}
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



