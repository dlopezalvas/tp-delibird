
#include "utils_broker.h"

extern t_list* multihilos;

int proceso_valido(char*procesos_validos,char* proceso){

	char* s = strstr(procesos_validos,proceso);

	if(s != NULL) return 1;
	return 0;
}

void log_suscribir_mensaje_queue(char* proceso,char* queue){
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Proceso: %s se suscribio a la cola: %s", proceso, queue);
	pthread_mutex_unlock(&logger_mutex);
}

void crear_queues(void){
	unique_message_id = 0;
	NEW_POKEMON_QUEUE = list_create();
	APPEARED_POKEMON_QUEUE = list_create();
	CATCH_POKEMON_QUEUE = list_create();
	CAUGHT_POKEMON_QUEUE = list_create();
	GET_POKEMON_QUEUE = list_create();
	LOCALIZED_POKEMON_QUEUE = list_create();
	NEW_POKEMON_QUEUE_SUSCRIPT = list_create();
	APPEARED_POKEMON_QUEUE_SUSCRIPT = list_create();
	CATCH_POKEMON_QUEUE_SUSCRIPT = list_create();
	CAUGHT_POKEMON_QUEUE_SUSCRIPT = list_create();
	GET_POKEMON_QUEUE_SUSCRIPT = list_create();
	LOCALIZED_POKEMON_QUEUE_SUSCRIPT = list_create();
	NEW_POKEMON_COLA = queue_create();
	APPEARED_POKEMON_COLA = queue_create();
	CATCH_POKEMON_COLA = queue_create();
	CAUGHT_POKEMON_COLA = queue_create();
	GET_POKEMON_COLA = queue_create();
	LOCALIZED_POKEMON_COLA = queue_create();
	SUSCRIPCION_COLA = queue_create();
	ACK_COLA = queue_create();
	IDS_RECIBIDOS = list_create();

	pthread_mutex_init(&new_pokemon_mutex,NULL);
	pthread_mutex_init(&appeared_pokemon_mutex,NULL);
	pthread_mutex_init(&catch_pokemon_mutex,NULL);
	pthread_mutex_init(&caught_pokemon_mutex,NULL);
	pthread_mutex_init(&localized_pokemon_mutex,NULL);
	pthread_mutex_init(&get_pokemon_mutex,NULL);
	pthread_mutex_init(&suscripcion_mutex,NULL);
	pthread_mutex_init(&new_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&appeared_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&catch_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&caught_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&localized_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&get_pokemon_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_new_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_get_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_caught_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_localized_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_catch_queue_mutex,NULL);
	pthread_mutex_init(&suscripcion_appeared_queue_mutex,NULL);
	pthread_mutex_init(&multhilos_mutex,NULL);
	pthread_mutex_init(&logger_mutex,NULL);
	pthread_mutex_init(&memoria_buddy_mutex,NULL);
	pthread_mutex_init(&id_fifo_mutex,NULL);
	pthread_mutex_init(&ack_queue_mutex,NULL);
	pthread_mutex_init(&ids_recibidos_mtx,NULL);
}

void terminar_queues(void){
	list_destroy(NEW_POKEMON_QUEUE);
	list_destroy(APPEARED_POKEMON_QUEUE);
	list_destroy(CATCH_POKEMON_QUEUE);
	list_destroy(CAUGHT_POKEMON_QUEUE);
	list_destroy(GET_POKEMON_QUEUE);
	list_destroy(NEW_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(APPEARED_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(CATCH_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(CAUGHT_POKEMON_QUEUE_SUSCRIPT);
	list_destroy(GET_POKEMON_QUEUE_SUSCRIPT);
}

void esperar_cliente(int servidor){

	struct sockaddr_in direccion_cliente;

	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);
	pthread_t hilo;

	pthread_mutex_lock(&multhilos_mutex);
	list_add(multihilos, &hilo);
	pthread_mutex_unlock(&multhilos_mutex);

	pthread_create(&hilo,NULL,(void*)serve_client,cliente);
	pthread_detach(hilo);

}

void serve_client(int socket){
	int rec;
	int cod_op;
	while(1){
		rec = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);
		if(rec == -1 || rec == 0 ){
			cod_op = -1;
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se desconecto el proceso con id: %d",socket);
			pthread_mutex_unlock(&logger_mutex);
			pthread_exit(NULL);
		}
		puts("recibi un mensaje");
		printf("codigo: %d\n", cod_op);
		process_request(cod_op, socket);
	}
}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se conecto el proceso con id: %d",cliente_fd);
	pthread_mutex_unlock(&logger_mutex);
	if(cod_op == SUSCRIPCION){
		t_mensaje_broker* mensaje_suscripcion = malloc(sizeof(t_mensaje_broker));
		mensaje_suscripcion->buffer = buffer;
		mensaje_suscripcion->suscriptor = cliente_fd;
		pthread_mutex_lock(&suscripcion_mutex);
		queue_push(SUSCRIPCION_COLA, mensaje_suscripcion);
		pthread_mutex_unlock(&suscripcion_mutex);
		sem_post(&suscripcion_sem);
	}else if(cod_op == ACK){
		t_ack* ack = deserializar_ack(buffer);
		printf("id %d, proceso %d\n", ack->id_mensaje, ack->id_proceso);
		pthread_mutex_lock(&ack_queue_mutex);
		queue_push(ACK_COLA, ack);
		pthread_mutex_unlock(&ack_queue_mutex);
		sem_post(&ack_sem);
	}else{
		suscribir_mensaje(cod_op,buffer,cliente_fd,size);
	}


}

int suscribir_mensaje(int cod_op,void* buffer,int cliente_fd,uint32_t size){

	t_buffer_broker* buffer_broker = malloc(sizeof(t_buffer_broker));
	pthread_mutex_lock(&unique_id_mutex);
	unique_message_id++;
	pthread_mutex_unlock(&unique_id_mutex);

	uint32_t mensaje_id = unique_message_id;

	if(cod_op != GET_POKEMON){
		send(cliente_fd,&mensaje_id,sizeof(uint32_t),0); //envio ack
	}else{
		enviar_ack(cliente_fd, mensaje_id, 0);
	}

	if(es_mensaje_respuesta(cod_op)){
		buffer_broker = deserializar_broker_vuelta(buffer,size);
	}else{
		buffer_broker = deserializar_broker_ida(buffer,size); //si no es de respuesta no tiene correlation id
	}

	t_bloque_broker* bloque_broker = malloc(sizeof(t_bloque_broker));

	bloque_broker->particion = almacenar_dato(buffer_broker->buffer,buffer_broker->tamanio,cod_op, mensaje_id);
	bloque_broker->procesos_recibidos = list_create();
	bloque_broker->id = mensaje_id;
	bloque_broker->correlation_id = buffer_broker->correlation_id;

	printf("id: %d cid: %d\n", bloque_broker->id, bloque_broker->correlation_id);

	switch (cod_op) {
	case NEW_POKEMON:
		pthread_mutex_lock(&new_pokemon_mutex);
		queue_push(NEW_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&new_pokemon_mutex);
		sem_post(&new_pokemon_sem);
		break;
	case APPEARED_POKEMON:
		pthread_mutex_lock(&appeared_pokemon_mutex);
		queue_push(APPEARED_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&appeared_pokemon_mutex);
		sem_post(&appeared_pokemon_sem);
		break;
	case CATCH_POKEMON:
		pthread_mutex_lock(&catch_pokemon_mutex);
		queue_push(CATCH_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&catch_pokemon_mutex);
		sem_post(&catch_pokemon_sem);
		break;
	case CAUGHT_POKEMON:
		pthread_mutex_lock(&caught_pokemon_mutex);
		queue_push(CAUGHT_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&caught_pokemon_mutex);
		sem_post(&caught_pokemon_sem);
		break;
	case GET_POKEMON:
		pthread_mutex_lock(&get_pokemon_mutex);
		queue_push(GET_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&get_pokemon_mutex);
		sem_post(&get_pokemon_sem);
		break;
	case LOCALIZED_POKEMON:
		pthread_mutex_lock(&localized_pokemon_mutex);
		queue_push(LOCALIZED_POKEMON_COLA,bloque_broker);
		pthread_mutex_unlock(&localized_pokemon_mutex);
		sem_post(&localized_pokemon_sem);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}

	return bloque_broker->id;
}

bool es_mensaje_respuesta(op_code cod_op){
	return cod_op == APPEARED_POKEMON || cod_op == LOCALIZED_POKEMON || cod_op == CAUGHT_POKEMON;
}

void ejecutar_ACK(){
	while(1){
		sem_wait(&ack_sem);
		pthread_mutex_lock(&ack_queue_mutex);
		t_ack* ack = queue_pop(ACK_COLA);
		pthread_mutex_unlock(&ack_queue_mutex);
		printf("el id que llego %d", ack->id_mensaje);

		bool _buscar_por_id(t_bloque_broker* bloque){
			return bloque->id == ack->id_mensaje;
		}

		pthread_mutex_lock(&ids_recibidos_mtx);
		t_bloque_broker* bloque_broker = list_find(IDS_RECIBIDOS, (void*)_buscar_por_id);

		pthread_mutex_lock(&(bloque_broker->mtx));
		list_add(bloque_broker->procesos_recibidos, ack->id_proceso);
		pthread_mutex_unlock(&(bloque_broker->mtx));

		pthread_mutex_unlock(&ids_recibidos_mtx);
		//TODO ver si es necesario ignorar mensajes
	}
}

bool buscar_por_id(t_bloque_broker* bloque, int id_mensaje){
	return bloque->id == id_mensaje;

}

void enviar_mensaje_broker(int cliente_a_enviar,void* a_enviar,int bytes){
	printf("cliente al que se le envia es %d", cliente_a_enviar);
	send(cliente_a_enviar,a_enviar,bytes,0);
}

t_paquete* preparar_mensaje_a_enviar(t_bloque_broker* bloque_broker, op_code codigo_operacion){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete -> codigo_operacion = codigo_operacion;

	t_buffer* buffer_cargado = malloc(sizeof(t_buffer));

	int size = 0;

	size = bloque_broker->particion->tamanio + sizeof(uint32_t);
	bloque_broker->particion->ultimo_acceso = time(NULL);

	if(es_mensaje_respuesta(codigo_operacion)){
		size+= sizeof(uint32_t);
	}

	buffer_cargado->size = size;
	void* stream = malloc(buffer_cargado->size);

	int offset = 0;
	memcpy(stream + offset, &bloque_broker->id, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	if(es_mensaje_respuesta(codigo_operacion)){
		memcpy(stream + offset, &bloque_broker->correlation_id, sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}
	memcpy(stream + offset, (void*)bloque_broker->particion->base, bloque_broker->particion->tamanio);
	bloque_broker->particion->ultimo_acceso = time(NULL);


	buffer_cargado->stream = stream;

	paquete -> buffer = buffer_cargado;

	return paquete;
}

void ejecutar_new_pokemon(){

	while(1){

		sem_wait(&new_pokemon_sem);
		pthread_mutex_lock(&new_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(NEW_POKEMON_COLA);
		pthread_mutex_unlock(&new_pokemon_mutex);

		op_code codigo_operacion = NEW_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		int id_mensaje = bloque_broker->id;

		puts("esta por enviar un mensaje");
		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}

		pthread_mutex_lock(&suscripcion_new_queue_mutex);
		list_iterate(NEW_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_new_queue_mutex);

	}
}

void ejecutar_appeared_pokemon(){

	while(1){

		sem_wait(&appeared_pokemon_sem);
		pthread_mutex_lock(&appeared_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(APPEARED_POKEMON_COLA);
		pthread_mutex_unlock(&appeared_pokemon_mutex);

		op_code codigo_operacion = APPEARED_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		pthread_mutex_lock(&suscripcion_appeared_queue_mutex);
		list_iterate(APPEARED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_appeared_queue_mutex);
		puts("envio appeared_pokemon");
	}
}

void ejecutar_catch_pokemon(){

	while(1){

		sem_wait(&catch_pokemon_sem);
		pthread_mutex_lock(&catch_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(CATCH_POKEMON_COLA);
		pthread_mutex_unlock(&catch_pokemon_mutex);

		op_code codigo_operacion = CATCH_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		pthread_mutex_lock(&suscripcion_catch_queue_mutex);
		list_iterate(CATCH_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_catch_queue_mutex);
	}
}

void ejecutar_caught_pokemon(){

	while(1){

		sem_wait(&caught_pokemon_sem);
		pthread_mutex_lock(&caught_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(CAUGHT_POKEMON_COLA);
		pthread_mutex_unlock(&caught_pokemon_mutex);

		op_code codigo_operacion = CAUGHT_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);
		puts("aca envio un caught");

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		pthread_mutex_lock(&suscripcion_caught_queue_mutex);
		list_iterate(CAUGHT_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_caught_queue_mutex);

	}
}

void ejecutar_get_pokemon(){

	while(1){

		sem_wait(&get_pokemon_sem);
		pthread_mutex_lock(&get_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(GET_POKEMON_COLA);
		pthread_mutex_unlock(&get_pokemon_mutex);

		op_code codigo_operacion = GET_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		pthread_mutex_lock(&suscripcion_get_queue_mutex);
		list_iterate(GET_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_get_queue_mutex);

	}
}

void ejecutar_localized_pokemon(){

	while(1){

		sem_wait(&localized_pokemon_sem);
		pthread_mutex_lock(&localized_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(LOCALIZED_POKEMON_COLA);
		pthread_mutex_unlock(&localized_pokemon_mutex);

		op_code codigo_operacion = LOCALIZED_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		pthread_mutex_lock(&suscripcion_localized_queue_mutex);
		list_iterate(LOCALIZED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
		pthread_mutex_unlock(&suscripcion_localized_queue_mutex);

	}
}

void ejecutar_suscripcion(){

	while(1){

		sem_wait(&suscripcion_sem);
		pthread_mutex_lock(&suscripcion_mutex);
		t_mensaje_broker* mensaje = queue_pop(SUSCRIPCION_COLA);
		pthread_mutex_unlock(&suscripcion_mutex);

		void* buffer = mensaje->buffer;

		t_suscripcion* mensaje_suscripcion = deserializar_suscripcion(buffer);

		int suscriptor = mensaje->suscriptor;
		puts(string_itoa(suscriptor));
		char* log_debug_suscripcion = string_new();
		string_append_with_format(&log_debug_suscripcion ,"DEBUG:El cliente %d se suscribio a la cola %d",mensaje->suscriptor, mensaje_suscripcion->cola);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,log_debug_suscripcion);
		pthread_mutex_unlock(&logger_mutex);
		switch (mensaje_suscripcion->cola) {
		case NEW_POKEMON:
			pthread_mutex_lock(&suscripcion_new_queue_mutex);
			list_add(NEW_POKEMON_QUEUE_SUSCRIPT,suscriptor);
			pthread_mutex_unlock(&suscripcion_new_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola NEW_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		case APPEARED_POKEMON:
			pthread_mutex_lock(&suscripcion_appeared_queue_mutex);
			list_add(APPEARED_POKEMON_QUEUE_SUSCRIPT,suscriptor);
			pthread_mutex_unlock(&suscripcion_appeared_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola APPEAREAD_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		case CATCH_POKEMON:
			pthread_mutex_lock(&suscripcion_catch_queue_mutex);
			list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,suscriptor);
			pthread_mutex_unlock(&suscripcion_catch_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		case CAUGHT_POKEMON:
			pthread_mutex_lock(&suscripcion_caught_queue_mutex);
			list_add(CAUGHT_POKEMON_QUEUE_SUSCRIPT,suscriptor);
			pthread_mutex_unlock(&suscripcion_caught_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola CAUGHT_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		case GET_POKEMON:
			pthread_mutex_lock(&suscripcion_get_queue_mutex);
			list_add(GET_POKEMON_QUEUE_SUSCRIPT,suscriptor);
			pthread_mutex_unlock(&suscripcion_get_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola GET_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		case LOCALIZED_POKEMON:
			pthread_mutex_lock(&suscripcion_localized_queue_mutex);
			list_add(LOCALIZED_POKEMON_QUEUE_SUSCRIPT,suscriptor);//Ver si va el & o no
			pthread_mutex_unlock(&suscripcion_localized_queue_mutex);
			pthread_mutex_lock(&logger_mutex);
			log_info(logger,"Se suscribio el proceso, %d ,a la cola LOCALIZED_POKEMON",suscriptor);
			pthread_mutex_unlock(&logger_mutex);
			break;
		}
		enviar_faltantes(suscriptor, mensaje_suscripcion);
	}
}

void enviar_faltantes(int suscriptor, t_suscripcion* mensaje_suscripcion){

	bool _buscar_por_proceso(int proceso){
		return proceso == mensaje_suscripcion->id_proceso;
	}

	bool _falta_enviar(t_bloque_broker* bloque){
		pthread_mutex_lock(&(bloque->mtx));
		bool entregado = list_any_satisfy(bloque->procesos_recibidos, (void*) _buscar_por_proceso); //esto da true si el mensaje ya fue enviado al proceso
		bool es_de_cola = bloque->particion->cola == mensaje_suscripcion->cola;
		pthread_mutex_lock(&(bloque->mtx));
		return !entregado && es_de_cola;
	}

	pthread_mutex_lock(&ids_recibidos_mtx);

	t_list* mensajes_de_cola = list_filter(IDS_RECIBIDOS, (void*)_falta_enviar); //tengo los mensajes que no le mande a ese proceso

	if(!list_is_empty(mensajes_de_cola)){

		void _enviar_mensaje_faltante(t_bloque_broker* bloque){
			t_paquete* paquete = preparar_mensaje_a_enviar(bloque, mensaje_suscripcion->cola);
			int bytes = 0;
			void* a_enviar = serializar_paquete(paquete, &bytes);
			return enviar_mensaje_broker(suscriptor, a_enviar, bytes);
		}

		list_iterate(mensajes_de_cola, (void*)_enviar_mensaje_faltante); //TODO esto se me hace re falopa, hay que ver que este bien
	}

	pthread_mutex_unlock(&ids_recibidos_mtx);
}

//------------MEMORIA------------//
void iniciar_memoria(){

	configuracion_cache = malloc(sizeof(t_config_cache));

	configuracion_cache->tamanio_memoria = config_get_int_value(config, TAMANO_MEMORIA);
	configuracion_cache->tamanio_minimo_p = config_get_int_value(config, TAMANO_MINIMO_PARTICION);

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_MEMORIA), "BS")) configuracion_cache->algoritmo_memoria = BS;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_MEMORIA), "PARTICIONES")) configuracion_cache->algoritmo_memoria = PARTICIONES;

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_REEMPLAZO), "FIFO")) configuracion_cache->algoritmo_reemplazo = FIFO;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_REEMPLAZO), "LRU")) configuracion_cache->algoritmo_reemplazo = LRU;

	if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_PARTICION_LIBRE), "FF")) configuracion_cache->algoritmo_part_libre = FIRST_FIT;
	else if(string_equals_ignore_case(config_get_string_value(config, ALGORITMO_PARTICION_LIBRE), "BF")) configuracion_cache->algoritmo_part_libre = BEST_FIT;

	configuracion_cache->frecuencia_compact = config_get_int_value(config, FRECUENCIA_COMPACTACION);

	memoria_cache = malloc(configuracion_cache->tamanio_memoria);

	id_fifo = 0;

	particiones = list_create();
	t_particion* aux = malloc(sizeof(t_particion));

	aux->base = (int)memoria_cache;
	aux->tamanio = configuracion_cache->tamanio_memoria;
	aux->id_mensaje = 0;
	aux->ultimo_acceso = time(NULL);
	aux->id = id_fifo;

	pthread_mutex_lock(&lista_particiones_mtx);
	list_add(particiones, aux);
	pthread_mutex_unlock(&lista_particiones_mtx);

	t_particion* bloque_buddy = malloc(sizeof(t_particion));

	bloque_buddy->base = (int) memoria_cache;
	bloque_buddy->ocupado = false;
	bloque_buddy->tamanio = configuracion_cache->tamanio_memoria;
	bloque_buddy->id = id_fifo;
	bloque_buddy->id_mensaje = 0;
	bloque_buddy->ultimo_acceso = time(NULL);

	memoria_buddy = list_create();
	list_add(memoria_buddy,bloque_buddy);
	//claramente faltan semaforos
}

void* almacenar_dato(void* datos, int tamanio, op_code codigo_op, uint32_t id){

	void* lugar_donde_esta;

	switch(configuracion_cache->algoritmo_memoria){
	case BS:
		lugar_donde_esta = almacenar_datos_buddy(datos, tamanio,codigo_op,id);
		break;
	case PARTICIONES:
		lugar_donde_esta = almacenar_dato_particiones(datos, tamanio, codigo_op, id);
		break;
	}

	return lugar_donde_esta;
}

t_particion* almacenar_dato_particiones(void* datos, int tamanio, op_code codigo_op, uint32_t id){

	t_particion* particion_libre;

	switch(configuracion_cache->algoritmo_part_libre){
	case FIRST_FIT:
		particion_libre = particion_libre_ff(tamanio);
		break;
	case BEST_FIT:
		particion_libre = particion_libre_bf(tamanio);
	}

	asignar_particion(datos, particion_libre, tamanio, codigo_op, id);

	return particion_libre;
}

void ordenar_particiones(){ //no se si anda esto

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->base < particion2->base;
	}

	list_sort(particiones, (void*)_orden);
}

void compactar(){
	int offset = 0;
	t_particion* aux;

//	bool _es_la_particion(t_particion* particion){
//		return particion->base == aux->base;
//	}
	pthread_mutex_lock(&lista_particiones_mtx);

	ordenar_particiones(); //ordeno entonces puedo ir moviendo una por una al principio de la memoria

	t_list* particiones_ocupadas = list_filter(particiones, (void*)esta_ocupada);

	int cantidad_p_ocupadas = list_size(particiones_ocupadas);

	pthread_mutex_lock(&memoria_cache_mtx);
	for(int i = 0; i < cantidad_p_ocupadas; i++){
		aux = list_get(particiones_ocupadas, i);
		memcpy(memoria_cache + offset, (void*)aux->base, aux->tamanio);
		aux->base = (int)memoria_cache + offset;
		offset+= aux->tamanio;
	}
	pthread_mutex_unlock(&memoria_cache_mtx);

	t_list* particiones_aux = list_filter(particiones, (void*)esta_ocupada);

	list_destroy(particiones);

	particiones = particiones_aux;

	t_particion* particion_unica = malloc(sizeof(t_particion));
	particion_unica->base = (int) memoria_cache + offset;
	particion_unica->tamanio = configuracion_cache->tamanio_memoria - offset; //esto esta bien?
	particion_unica->id_mensaje = 0;
	particion_unica->ultimo_acceso = time(NULL);
	particion_unica->ocupado = false;
	list_add(particiones, particion_unica);
	pthread_mutex_unlock(&lista_particiones_mtx); //otro mutex grande :(

	list_destroy(particiones_ocupadas); //esto no deberia borrar los elementos, si los borra entonces sacar(?

}

t_particion* buscar_particion_ff(int tamanio_a_almacenar){ //falta ordenar lista

	t_particion* particion_libre;

	bool _puede_almacenar_y_esta_libre(t_particion* particion){
		return particion->tamanio>= tamanio_a_almacenar && !particion->ocupado;
	}

	pthread_mutex_lock(&lista_particiones_mtx);
	ordenar_particiones();

	particion_libre =  list_find(particiones, (void*) _puede_almacenar_y_esta_libre); //list find agarra el primero que cumpla, asi que el primero que tenga tamanio mayor o igual será

	if(particion_libre != NULL){
		particion_libre->ocupado = true; //si devuelve algo ya lo pongo como ocupado asi ningun otro hilo puede agarrar la misma particion
		pthread_mutex_lock(&id_fifo_mutex);
		id_fifo++;
		pthread_mutex_unlock(&id_fifo_mutex);
		particion_libre->id = id_fifo;
	}

	pthread_mutex_unlock(&lista_particiones_mtx);
	return particion_libre;
}

t_particion* particion_libre_ff(int tamanio_a_almacenar){
	t_particion* particion_libre = buscar_particion_ff(tamanio_a_almacenar);

	int contador = 1;

	while(particion_libre == NULL){
		if(contador < configuracion_cache->frecuencia_compact || configuracion_cache->frecuencia_compact == -1){
			consolidar(elegir_victima_particiones(tamanio_a_almacenar)); //aca se elimina la particion (se pone como libre), se consolida y se vuelve a buscar una particion
			particion_libre = buscar_particion_ff(tamanio_a_almacenar);
			contador++;
		}else{
			compactar();
			particion_libre = buscar_particion_ff(tamanio_a_almacenar);
			contador = 0;
		}
	}

	return particion_libre;
}

t_particion* particion_libre_bf(int tamanio_a_almacenar){

	t_particion* particion_libre = buscar_particion_bf(tamanio_a_almacenar);

	int contador = 1;

	while(particion_libre == NULL){
		if(contador < configuracion_cache->frecuencia_compact || configuracion_cache->frecuencia_compact == -1){
			consolidar(elegir_victima_particiones()); //aca se elimina la particion (se pone como libre), se consolida y se vuelve a buscar una particion
			particion_libre = buscar_particion_bf(tamanio_a_almacenar);
			contador++;
		}else{
			compactar();
			particion_libre = buscar_particion_bf(tamanio_a_almacenar);
			contador = 0;
		}
	}

	return particion_libre;
}

void consolidar(t_particion* particion_liberada){

	//cuando busco la anterior y la siguiente tambien me fijo que esten libres, si no la encuentra (porque no tiene siguiente/anterior o porque esta ocupada) no consolida

	bool _es_la_anterior(t_particion* particion){
		return particion->base + particion->tamanio == particion_liberada->base && !particion->ocupado;
	}

	bool _es_la_siguiente(t_particion* particion){
		return particion_liberada->base + particion_liberada->tamanio == particion->base && !particion->ocupado;
	}

	bool _es_la_particion(t_particion* particion){
		return particion_liberada->base == particion->base;
	}

	pthread_mutex_lock(&lista_particiones_mtx); //bloqueo aca porque puede pasar que otro hilo quiera ocupar una de las libres antes/mientras esta consolidando

	t_particion* p_anterior = list_find(particiones,(void*) _es_la_anterior);
	t_particion* p_siguiente = list_find(particiones,(void*) _es_la_siguiente);


	if(particion_liberada != NULL){
		if(p_anterior != NULL && p_siguiente != NULL){ //si encuentra particiones libres por los dos lados consolida las 3
			p_anterior->tamanio += particion_liberada->tamanio + p_siguiente->tamanio; //directamente hago la anterior mas grande (?
			list_remove_by_condition(particiones, (void*) _es_la_siguiente);
			list_remove_by_condition(particiones, (void*) _es_la_particion);
		}else if(p_anterior != NULL && p_siguiente == NULL){ //solo encontro de un lado una particion vacia, consolida solo 2
			p_anterior->tamanio += particion_liberada->tamanio;
			list_remove_by_condition(particiones, (void*) _es_la_particion);
		}else if(p_anterior == NULL && p_siguiente != NULL){
			particion_liberada->tamanio += p_siguiente->tamanio;
			list_remove_by_condition(particiones, (void*) _es_la_siguiente);
		}
	}

	pthread_mutex_unlock(&lista_particiones_mtx); // re largo el mutex, preguntar que onda aca (?

}

t_particion* buscar_particion_bf(int tamanio_a_almacenar){ //se puede con fold creo

	t_particion* best;

	bool _ordenar_por_tamanio(t_particion* particion_menor, t_particion* particion_mayor){
		return particion_menor->tamanio < particion_mayor->tamanio;
	}

	bool _la_mejor(t_particion* particion){
		return particion->tamanio >= tamanio_a_almacenar && !particion->ocupado; //commo esta ordenada de menor a mayor, la primera que encuentre que tenga tamanio
	}																			 //mayor o igual (y este vacia) será la mejor

	pthread_mutex_lock(&lista_particiones_mtx);

	list_sort(particiones, (void*)_ordenar_por_tamanio); //ordeno de menor a mayor

	best = list_find(particiones, (void*)_la_mejor);

	if(best != NULL){
		best->ocupado = true; //si devuelve algo ya lo pongo como ocupado asi ningun otro hilo puede agarrar la misma particion
		pthread_mutex_lock(&id_fifo_mutex);
		id_fifo++;
		pthread_mutex_unlock(&id_fifo_mutex);
		best->id = id_fifo;
	}

	pthread_mutex_unlock(&lista_particiones_mtx);
	return best;

}

t_particion* elegir_victima_particiones(int tamanio_a_almacenar){
	switch(configuracion_cache->algoritmo_reemplazo){
	case LRU:
		return elegir_victima_particiones_LRU();
	case FIFO:
		return elegir_victima_particiones_FIFO();
	}
}

bool esta_ocupada(t_particion* particion){
	return particion->ocupado;
}

t_particion* elegir_victima_particiones_FIFO(){
	t_particion* particion;

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->id < particion2->id;
	}

	pthread_mutex_lock(&lista_particiones_mtx);
	list_sort(particiones, (void*)_orden);

	particion = list_find(particiones, (void*)esta_ocupada); //las ordeno por LRU y agarro la primera en la lista que este ocupada

	particion->ocupado = false;
	particion->cola = 0;
	particion->id_mensaje = 0;
	particion->ultimo_acceso = time(NULL); //importa esto aca??

	pthread_mutex_unlock(&lista_particiones_mtx);

	return particion;
}

t_particion* elegir_victima_particiones_LRU(){

	t_particion* particion;

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->ultimo_acceso < particion2->ultimo_acceso;
	}

	pthread_mutex_lock(&lista_particiones_mtx);
	list_sort(particiones, (void*)_orden);

	particion = list_find(particiones, (void*)esta_ocupada); //las ordeno por LRU y agarro la primera en la lista que este ocupada

	particion->ocupado = false;
	particion->cola = 0;
	particion->id_mensaje = 0;
	particion->ultimo_acceso = time(NULL); //importa esto aca??

	pthread_mutex_unlock(&lista_particiones_mtx);

	return particion;

}

void asignar_particion(void* datos, t_particion* particion_libre, int tamanio, op_code codigo_op, uint32_t id){

	pthread_mutex_lock(&memoria_cache_mtx);
	memcpy((void*)particion_libre->base, datos, tamanio); //copio a la memoria
	pthread_mutex_unlock(&memoria_cache_mtx);


	if(particion_libre->tamanio != tamanio){ //si no entro justo (mismo tamanio), significa que queda una nueva particion de menor tamanio libre
		t_particion* particion_nueva = malloc(sizeof(t_particion));
		particion_nueva->base = particion_libre->base + tamanio;
		particion_nueva->tamanio = particion_libre->tamanio - tamanio;
		particion_nueva->id_mensaje = 0;
		particion_nueva->ultimo_acceso = time(NULL);
		particion_nueva->ocupado = false;
		particion_libre->tamanio = tamanio;

		pthread_mutex_lock(&lista_particiones_mtx);
		list_add(particiones, particion_nueva);
		pthread_mutex_unlock(&lista_particiones_mtx);
	}

	particion_libre->ultimo_acceso = time(NULL); //ya viene de antes con el bit de ocupado en true asi que nadie lo va a elegir (no hace falta semaforo)
	particion_libre->cola = codigo_op;
	particion_libre->id_mensaje = id;

}

t_buffer_broker* deserializar_broker_ida(void* buffer, uint32_t size){ //eso hay que probarlo que onda


	t_buffer_broker* buffer_broker = malloc(sizeof(t_buffer_broker));

	uint32_t tamanio_mensaje = size - sizeof(uint32_t);

	buffer_broker->buffer = malloc(sizeof(tamanio_mensaje)); //??????
	buffer_broker->tamanio = tamanio_mensaje;
	int offset = 0;
	int id = 0;

	void* stream = malloc(tamanio_mensaje);

	memcpy(&id, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream, buffer + offset, tamanio_mensaje);

	buffer_broker->id = id;
	buffer_broker->correlation_id = 0; //no tiene corr id
	buffer_broker->buffer = stream;

	return buffer_broker;
}

t_buffer_broker* deserializar_broker_vuelta(void* buffer, uint32_t size){

	t_buffer_broker* buffer_broker = malloc(sizeof(t_buffer_broker));

	uint32_t tamanio_mensaje = size - sizeof(uint32_t)*2;

	buffer_broker->buffer = malloc(sizeof(tamanio_mensaje));
	buffer_broker->tamanio = tamanio_mensaje;
	int offset = 0;
	int id = 0;
	int cid = 0;

	void* stream = malloc(tamanio_mensaje);

	memcpy(&id, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&cid, buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream, buffer + offset, tamanio_mensaje);

	buffer_broker->id = id;
	buffer_broker->correlation_id = cid;
	buffer_broker->buffer = stream;

	return buffer_broker;
}


t_particion* almacenar_datos_buddy(void* datos, int tamanio,op_code cod_op,uint32_t id_mensaje){

	t_particion* bloque_buddy_particion = malloc(sizeof(t_particion));

	bloque_buddy_particion = eleccion_particion_asignada_buddy(datos,tamanio);

	while(bloque_buddy_particion == NULL){

		puts("while reemplazo fifo");

		switch(configuracion_cache->algoritmo_reemplazo){
		case FIFO:
			eleccion_victima_fifo_buddy(tamanio);
			puts("elimino victima fifo");
			break;
		case LRU:
			eleccion_victima_lru_buddy(tamanio);
			break;
		}

		bloque_buddy_particion = eleccion_particion_asignada_buddy(datos,tamanio);

	}

	asignar_particion_buddy(bloque_buddy_particion,datos,tamanio,cod_op,id_mensaje);

	return bloque_buddy_particion;
}

void asignar_particion_buddy(t_particion* bloque_buddy_particion, void* datos, int tamanio,op_code cod_op,uint32_t id_mensaje){
	if(bloque_buddy_particion == NULL) puts("null");
	if(datos == NULL) puts("null");
	printf("%p, %d",bloque_buddy_particion->base, bloque_buddy_particion->id);

	memcpy((void*)bloque_buddy_particion->base, datos, tamanio); //copio a la memoria
	//	bloque_buddy_particion->ocupado = true;

	bool _mismo_id_buddy(void* elemento_lista){
		return remove_by_id(elemento_lista, bloque_buddy_particion->id);//TODO: Cambiar nombre remove a buscar
	}

	pthread_mutex_lock(&memoria_buddy_mutex);
	t_particion* particion_buddy = list_find(memoria_buddy,_mismo_id_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	particion_buddy->ocupado = true;
	pthread_mutex_lock(&id_fifo_mutex);
	id_fifo++;
	pthread_mutex_unlock(&id_fifo_mutex);
	particion_buddy->id = id_fifo;
	particion_buddy->id_mensaje = id_mensaje;
	particion_buddy->cola = cod_op;
	particion_buddy->ultimo_acceso = time(NULL);
}

t_particion* eleccion_particion_asignada_buddy(void* datos,int tamanio){

	//bloque_elegido = bloque que retorno para guardar los datos
	//bloque_a_partir = bloque que voy a partir en mil pedacitos
	t_particion* bloque_elegido = malloc(sizeof(t_particion));
	t_particion* bloque_a_partir = malloc(sizeof(t_particion));

	bool _validar_condicion_buddy(void* bloque_buddy){
		return encontrar_bloque_valido_buddy(bloque_buddy,tamanio);
	}

	pthread_mutex_lock(&memoria_buddy_mutex);
	t_list* lista_bloques_validos = list_filter(memoria_buddy, (void*)_validar_condicion_buddy);

	pthread_mutex_unlock(&memoria_buddy_mutex);

	if(!list_is_empty(lista_bloques_validos))
	{

		bool _ordenar_menor_a_mayor(void* bloque_buddy,void* bloque_buddy2){
			return ordenar_menor_a_mayor(bloque_buddy,bloque_buddy2);
		}

		pthread_mutex_lock(&memoria_buddy_mutex);
		list_sort(lista_bloques_validos, _ordenar_menor_a_mayor);
		bloque_a_partir = lista_bloques_validos->head->data;
		pthread_mutex_unlock(&memoria_buddy_mutex);

		bool condicion_buddy_particion = validar_condicion_buddy(bloque_a_partir,tamanio);

		//Preguntar si me entra en el bloque, si esta disponible y si el bloque es > a tamanio minimo
		//Si entra divido por 2 y vuelvo a preguntar lo mismo que antes asi hasta llegar al tamanio minimo
		//O hasta llegar a que no entre.
		//Si no entra, lo guardo en la ultima particion que entraba.

		while(condicion_buddy_particion){
			bloque_elegido = generar_particion_buddy(bloque_a_partir);
			condicion_buddy_particion = validar_condicion_buddy(bloque_elegido,tamanio);
			puts("entra al while");
		}
		// que pasa si nunca entra al while y no tiene ningun bloque elegido??
		if(bloque_elegido != NULL && !bloque_a_partir->ocupado && bloque_a_partir->tamanio > tamanio){
			puts("entra al iffffff");
			bloque_elegido = bloque_a_partir;
			puts(string_itoa(bloque_elegido->id_mensaje));
		}
	}
	else{
		bloque_elegido = NULL;
	}
	return bloque_elegido;
}

bool encontrar_bloque_valido_buddy(t_particion* bloque_buddy,int tamanio){
	return ((!(bloque_buddy->ocupado)) &&
			(bloque_buddy->tamanio) >= tamanio) ;
}

bool ordenar_menor_a_mayor(t_particion* bloque_buddy,t_particion* bloque_buddy2){
	return bloque_buddy->tamanio < bloque_buddy2->tamanio ;
}

bool validar_condicion_buddy(t_particion* bloque_buddy,int tamanio){

	int bloque_tamanio_siguiente = bloque_buddy->tamanio / 2;
	return bloque_tamanio_siguiente > tamanio
			&& !bloque_buddy->ocupado
			&& bloque_tamanio_siguiente >= configuracion_cache->tamanio_minimo_p;
}

t_particion* generar_particion_buddy(t_particion* bloque_buddy){
	uint32_t id_viejo = bloque_buddy->id;
	uint32_t base_vieja = bloque_buddy->base;

	bool _mismo_id_buddy(t_particion* bloque_buddy){
		return mismo_id_buddy(bloque_buddy,id_viejo);
	}
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);

	t_particion* bloque_buddy2 = malloc(sizeof(t_particion));

	uint32_t nuevo_tamanio = bloque_buddy->tamanio / 2;
	bloque_buddy->tamanio = nuevo_tamanio;
	bloque_buddy2->tamanio = nuevo_tamanio;
	bloque_buddy2->base = base_vieja + nuevo_tamanio;
	bloque_buddy->ocupado = false;
	bloque_buddy->base = base_vieja;
	bloque_buddy2->ocupado = false;
	pthread_mutex_lock(&id_fifo_mutex);
	id_fifo++;
	pthread_mutex_unlock(&id_fifo_mutex);
	bloque_buddy2->id = id_fifo;
	pthread_mutex_lock(&id_fifo_mutex);
	id_fifo++;
	pthread_mutex_unlock(&id_fifo_mutex);
	bloque_buddy->id = id_fifo;
	bloque_buddy->id_mensaje = 0;
	bloque_buddy->ultimo_acceso = time(NULL);
	bloque_buddy2->id_mensaje = 0;
	bloque_buddy2->ultimo_acceso = time(NULL);

	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add(memoria_buddy,bloque_buddy2);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add(memoria_buddy,bloque_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	return bloque_buddy2;
}


bool mismo_id_buddy(t_particion* bloque_buddy,uint32_t id_viejo){
	return bloque_buddy->id == id_viejo;
}

void eleccion_victima_fifo_buddy(int tamanio){

	bool _sort_byId_memoria_buddy(t_particion* bloque_buddy,t_particion* bloque_buddy2){
		return sort_byId_memoria_buddy(bloque_buddy,bloque_buddy2);
	}

	bool _esta_ocupada(t_particion* bloque_buddy){
		return bloque_buddy->ocupado;
	}

	pthread_mutex_lock(&memoria_buddy_mutex);
	list_sort(memoria_buddy,(void*)_sort_byId_memoria_buddy);
	t_particion* victima_elegida = list_find(memoria_buddy, (void*)_esta_ocupada);
	pthread_mutex_unlock(&memoria_buddy_mutex);

	victima_elegida->ocupado = false;
	victima_elegida->id_mensaje = 0;
	victima_elegida->cola = 0;

	//consolidar
	consolidar_buddy(victima_elegida,memoria_buddy);
}

bool remove_by_id(t_particion* bloque_buddy,uint32_t id_remover){
	return bloque_buddy->id == id_remover;
}

void consolidar_buddy(t_particion* bloque_buddy_old,t_list* lista_fifo_buddy){

	bool _validar_condicion_fifo_buddy(t_particion* bloque_buddy){

		return validar_condicion_fifo_buddy(bloque_buddy, bloque_buddy_old);
	}

	t_particion* buddy = list_find(lista_fifo_buddy,(void*)_validar_condicion_fifo_buddy);

	//list_iterate(lista_fifo_buddy, (void*)_encontrar_y_consolidar_buddy);
	while(buddy != NULL){
	bloque_buddy_old = encontrar_y_consolidar_buddy(buddy, bloque_buddy_old);
	if(bloque_buddy_old != NULL)
	buddy = list_find(lista_fifo_buddy,(void*)_validar_condicion_fifo_buddy);
	else buddy = NULL;
	}
}

bool validar_condicion_fifo_buddy(t_particion* bloque_buddy,t_particion* bloque_buddy_old){
	return (bloque_buddy->tamanio == bloque_buddy_old->tamanio)
			&& ((bloque_buddy->base - (int)memoria_cache) == ((bloque_buddy_old->base -(int)memoria_cache) ^ bloque_buddy->tamanio)) &&
			((bloque_buddy_old->base - (int)memoria_cache) == (((bloque_buddy->base - (int)memoria_cache)) ^ bloque_buddy_old->tamanio));
}

t_particion*  encontrar_y_consolidar_buddy(t_particion* bloque_buddy,t_particion* bloque_buddy_old){

	if(!bloque_buddy->ocupado){

		puts("encontro buddy libre");

		t_particion* bloque_buddy_new = malloc(sizeof(t_particion));
		bloque_buddy_new->tamanio = bloque_buddy_old->tamanio * 2;
		bloque_buddy_new->ocupado = false;
		bloque_buddy_new->id_mensaje = 0;
		bloque_buddy_new->cola = 0;
		bloque_buddy_new->ultimo_acceso = time(NULL);

		puts("armo nuevo buddy");

//		pthread_mutex_lock(&id_fifo_mutex);
//		id_fifo++;
//		pthread_mutex_unlock(&id_fifo_mutex);
//		bloque_buddy_new->id = id_fifo;

		if(bloque_buddy_old->base < bloque_buddy->base){

		bloque_buddy_new->base = bloque_buddy_old->base;

		}else{

			bloque_buddy_new->base = bloque_buddy->base;

		}

		puts("puso la base del buddy");

		//
		bool _mismo_id_buddy1(t_particion* bloque_buddy_1){
			return mismo_id_buddy(bloque_buddy_1,bloque_buddy_old->id);
		}

		bool _mismo_id_buddy2(t_particion* bloque_buddy_2){
			return mismo_id_buddy(bloque_buddy_2,bloque_buddy->id);
		}

		pthread_mutex_lock(&memoria_buddy_mutex);

		t_particion* bloque_a_remover_1 = list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy1);
		printf("base en entero: %d, tamaño: %d, base en hhhhexa: %p \n ", (bloque_a_remover_1->base - (int)memoria_cache) , bloque_a_remover_1->tamanio, bloque_a_remover_1->base );
		pthread_mutex_unlock(&memoria_buddy_mutex);
		pthread_mutex_lock(&memoria_buddy_mutex);
		bloque_a_remover_1 = list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy2);
		printf("base en entero: %d, tamaño: %d, base en hhhhexa: %p \n ", (bloque_a_remover_1->base - (int)memoria_cache) , bloque_a_remover_1->tamanio, bloque_a_remover_1->base );
		pthread_mutex_unlock(&memoria_buddy_mutex);
		//
		pthread_mutex_lock(&memoria_buddy_mutex);
		list_add(memoria_buddy,bloque_buddy_new);
		pthread_mutex_unlock(&memoria_buddy_mutex);

		return bloque_buddy_new;
		puts("termino consolidacion");

	}

	return NULL;
}

//bool eleccion_victima_fifo_a_eliminar(t_particion* bloque_buddy, int tamanio){
//	return bloque_buddy->tamanio >= tamanio;
//}

bool sort_byId_memoria_buddy(t_particion* bloque_buddy,t_particion* bloque_buddy2){
	return bloque_buddy->id < bloque_buddy2->id;
}

bool sort_by_acceso_memoria_buddy(t_particion* bloque_buddy,t_particion* bloque_buddy2){
	return (bloque_buddy->ultimo_acceso) < (bloque_buddy2->ultimo_acceso);
}

void eleccion_victima_lru_buddy(int tamanio){

	bool _orden(t_particion* bloque_buddy,t_particion* bloque_buddy2){
		return bloque_buddy->ultimo_acceso < bloque_buddy2->ultimo_acceso;
	}

	bool _esta_ocupada(t_particion* bloque_buddy){
		return bloque_buddy->ocupado;
	}

	pthread_mutex_lock(&memoria_buddy_mutex);
	list_sort(memoria_buddy,(void*)_orden);
	t_particion* victima_elegida = list_find(memoria_buddy, (void*)_esta_ocupada);
	pthread_mutex_unlock(&memoria_buddy_mutex);

	victima_elegida->ocupado = false;
	victima_elegida->id_mensaje = 0;
	victima_elegida->cola = 0;

	//consolidar
	consolidar_buddy(victima_elegida,memoria_buddy);

}



