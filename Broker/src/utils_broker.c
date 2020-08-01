
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
	pthread_mutex_init(&buddy_id_mutex,NULL);
	pthread_mutex_init(&ack_queue_mutex,NULL);
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

	//	puts(string_itoa(multihilos->elements_count));

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
		//	puts(string_itoa(cod_op));
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

	send(cliente_fd,&mensaje_id,sizeof(uint32_t),0); //envio ack

	if(es_mensaje_respuesta(cod_op)){
		buffer_broker = deserializar_broker_vuelta(buffer,size);
	}else{
		buffer_broker = deserializar_broker_ida(buffer,size); //si no es de respuesta no tiene correlation id
	}

	void* particion_en_memoria = almacenar_dato(buffer_broker->buffer,buffer_broker->tamanio,cod_op, mensaje_id);

	t_bloque_broker* bloque_broker = malloc(sizeof(t_bloque_broker));

	switch(configuracion_cache->algoritmo_memoria){
	case BS:
		//bloque_broker->particion_buddy = particion_en_memoria;
		break;
	case PARTICIONES:
		bloque_broker->particion = particion_en_memoria;
		break;
	}
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

void socketEscucha(char*ip, char* puerto){
	int servidor = iniciar_servidor(ip,puerto);
	printf("Se creo el socket servidor en el puerto ( %s )", puerto);
	while(1){
		esperar_cliente(servidor);
	}
}

void ejecutar_ACK(){
	while(1){
		sem_wait(&ack_sem);
		pthread_mutex_lock(&ack_queue_mutex);
		t_ack* ack = queue_pop(ACK_COLA);
		pthread_mutex_unlock(&ack_queue_mutex);
		printf("el id que llego %d", ack->id_mensaje);
	}
}

void enviar_mensaje_broker(int cliente_a_enviar,void* a_enviar,int bytes){
	printf("cliente al que se le envia es %d", cliente_a_enviar);
	send(cliente_a_enviar,a_enviar,bytes,0);
}

t_paquete* preparar_mensaje_a_enviar(t_bloque_broker* bloque_broker, op_code codigo_operacion){

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete -> codigo_operacion = codigo_operacion;

	t_buffer* buffer_cargado = malloc(sizeof(t_buffer));
	int size = bloque_broker->particion->tamanio + sizeof(uint32_t);

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

	switch(configuracion_cache->algoritmo_memoria){
	case BS:
		memcpy(stream + offset, (void*)bloque_broker->particion_buddy->base, bloque_broker->particion_buddy->tamanio);
		break;
	case PARTICIONES:
		memcpy(stream + offset, (void*)bloque_broker->particion->base, bloque_broker->particion->tamanio);
	}

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

		//TODO verificar (con identificadores de los procesos) si se mando el mensaje a ese proceso especifico (esperar mail ayudante)
		op_code codigo_operacion = NEW_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		puts("esta por enviar un mensaje");
		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		list_iterate(NEW_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);

	}
}


void ejecutar_appeared_pokemon(){

	while(1){

		sem_wait(&appeared_pokemon_sem);
		pthread_mutex_lock(&appeared_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(APPEARED_POKEMON_COLA);
		pthread_mutex_unlock(&appeared_pokemon_mutex);

		//TODO verificar (con identificadores de los procesos) si se mando el mensaje a ese proceso especifico (esperar mail ayudante)
		op_code codigo_operacion = APPEARED_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		list_iterate(APPEARED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
	}
}

void ejecutar_catch_pokemon(){

	while(1){

		sem_wait(&catch_pokemon_sem);
		pthread_mutex_lock(&catch_pokemon_mutex);
		t_bloque_broker* bloque_broker = queue_pop(CATCH_POKEMON_COLA);
		pthread_mutex_unlock(&catch_pokemon_mutex);

		//TODO verificar (con identificadores de los procesos) si se mando el mensaje a ese proceso especifico (esperar mail ayudante)
		op_code codigo_operacion = CATCH_POKEMON;

		int bytes = 0;
		t_paquete* paquete = preparar_mensaje_a_enviar(bloque_broker, codigo_operacion);

		void* a_enviar = serializar_paquete(paquete, &bytes);

		void _enviar_mensaje_broker(int cliente_a_enviar){
			return enviar_mensaje_broker(cliente_a_enviar, a_enviar, bytes);
		}
		list_iterate(CATCH_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);
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
		list_iterate(CAUGHT_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);

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
		list_iterate(GET_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);

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
		list_iterate(LOCALIZED_POKEMON_QUEUE_SUSCRIPT, (void*)_enviar_mensaje_broker);

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
		puts(string_itoa(mensaje->suscriptor));
		char* log_debug_suscripcion = string_new();
		string_append_with_format(&log_debug_suscripcion ,"DEBUG:El cliente %d se suscribio a la cola %d",mensaje->suscriptor, mensaje_suscripcion->cola);
		pthread_mutex_lock(&logger_mutex);
		log_info(logger,log_debug_suscripcion);
		pthread_mutex_unlock(&logger_mutex);
		switch (mensaje_suscripcion->cola) {
		case NEW_POKEMON:
			puts("se suscribio a new_pokemon");
			ejecutar_new_pokemon_suscripcion(suscriptor);
			break;
		case APPEARED_POKEMON:
			ejecutar_appeared_pokemon_suscripcion(suscriptor);
			break;
		case CATCH_POKEMON:
			ejecutar_catch_pokemon_suscripcion(suscriptor);
			break;
		case CAUGHT_POKEMON:
			ejecutar_caught_pokemon_suscripcion(suscriptor);
			break;
		case GET_POKEMON:
			ejecutar_get_pokemon_suscripcion(suscriptor);
			break;
		case LOCALIZED_POKEMON:
			ejecutar_localized_pokemon_suscripcion(suscriptor);
			break;
			//		case 0:
			//			pthread_exit(NULL);
			//		case -1:
			//			pthread_exit(NULL);
		}
	}
}

void ejecutar_new_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_new_queue_mutex);
	list_add(NEW_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_new_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola NEW_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_appeared_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_appeared_queue_mutex);
	list_add(APPEARED_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_appeared_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola APPEAREAD_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_catch_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_catch_queue_mutex);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_catch_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_caught_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_caught_queue_mutex);
	list_add(CATCH_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_caught_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola CATCH_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_get_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_get_queue_mutex);
	list_add(GET_POKEMON_QUEUE_SUSCRIPT,suscriptor);
	pthread_mutex_unlock(&suscripcion_get_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola GET_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

void ejecutar_localized_pokemon_suscripcion(int suscriptor){
	pthread_mutex_lock(&suscripcion_localized_queue_mutex);
	list_add(LOCALIZED_POKEMON_QUEUE_SUSCRIPT,suscriptor);//Ver si va el & o no
	pthread_mutex_unlock(&suscripcion_localized_queue_mutex);
	pthread_mutex_lock(&logger_mutex);
	log_info(logger,"Se suscribio el proceso, %d ,a la cola LOCALIZED_POKEMON",suscriptor);
	pthread_mutex_unlock(&logger_mutex);
}

//------------MEMORIA------------//
void iniciar_memoria(t_config* config){

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

	particiones_libres = list_create();
	t_particion* aux = malloc(sizeof(t_particion));

	aux->base = (int)memoria_cache;
	aux->tamanio = configuracion_cache->tamanio_memoria;
	aux->id_mensaje = 0;
	aux->ultimo_acceso = time(NULL);

	list_add(particiones_libres, aux);

	particiones_ocupadas = list_create();

	t_particion* particion_libre = list_get(particiones_libres, 0);

	puts(string_itoa(particion_libre->base));

	buddy_id = 0;

	t_particion_buddy* bloque_buddy = malloc(sizeof(t_particion_buddy));

	bloque_buddy->base = (int) memoria_cache;
	bloque_buddy->ocupado = false;
	bloque_buddy->tamanio = configuracion_cache->tamanio_memoria;
	bloque_buddy->id = buddy_id;

	memoria_buddy = list_create();
	//	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add(memoria_buddy,bloque_buddy);
	//	pthread_mutex_lock(&memoria_buddy_mutex);
	//claramente faltan semaforos
}

void* almacenar_dato(void* datos, int tamanio, op_code codigo_op, uint32_t id){

	void* lugar_donde_esta;

	switch(configuracion_cache->algoritmo_memoria){
	case BS:
		//	lugar_donde_esta = almacenar_datos_buddy(datos, tamanio);
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

void ordenar_particiones_libres(){ //no se si anda esto

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->base < particion2->base;
	}

	list_sort(particiones_libres, (void*)_orden);
}

void compactar(){
	int offset = 0;

	ordenar_particiones_libres(); //ordeno entonces puedo ir moviendo una por una al principio de la memoria

	int cantidad_particiones = list_size(particiones_ocupadas) - 1;

	t_particion* aux;

	for(int i = 0; i < cantidad_particiones; i++){
		aux = list_get(particiones_ocupadas, i);
		memcpy(memoria_cache + offset, memoria_cache + aux->base, aux->tamanio);
		aux->base = offset;
		offset+= aux->tamanio;
	}

	list_clean(particiones_libres);

	t_particion* particion_unica = malloc(sizeof(t_particion));
	particion_unica->base = offset;
	particion_unica->tamanio = configuracion_cache->tamanio_memoria - offset; //esto esta bien?
	list_add(particiones_libres, particion_unica);

}

t_particion* buscar_particion_ff(int tamanio_a_almacenar){ //falta ordenar lista

	t_particion* particion_libre;

	bool _puede_almacenar(t_particion* particion){
		return particion->tamanio>= tamanio_a_almacenar;
	}

	ordenar_particiones_libres();

	particion_libre =  list_find(particiones_libres, (void*) _puede_almacenar); //list find agarra el primero que cumpla, asi que el primero que tenga tamanio mayor o igual será

	puts(string_itoa(particion_libre->base));

	return particion_libre;
}

t_particion* particion_libre_ff(int tamanio_a_almacenar){
	t_particion* particion_libre = buscar_particion_ff(tamanio_a_almacenar);

	int contador = 1;

	while(particion_libre == NULL){
		if(contador < configuracion_cache->frecuencia_compact || configuracion_cache->frecuencia_compact == -1){
			//particion_libre = elegir_victima_particiones(tamanio_a_almacenar);
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
			consolidar(elegir_victima_particiones(tamanio_a_almacenar)); //aca se elimina la particion (se pone como libre), se consolida y se vuelve a buscar una particion
			particion_libre = buscar_particion_bf;
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

	bool _es_la_anterior(t_particion* particion){
		return particion->base + particion->tamanio == particion_liberada->base;
	}

	bool _es_la_siguiente(t_particion* particion){
		return particion_liberada->base + particion_liberada->tamanio == particion->base;
	}


	t_particion* p_antes = list_find(particiones_libres, _es_la_anterior); //para no confundir izq y derecha
	t_particion* p_despues = list_find(particiones_libres, _es_la_siguiente);


	if(particion_liberada != NULL){
		if(p_antes != NULL && p_despues != NULL){ //si alguna es null es porque no existe una particion libre que sea anterior/posterior a la que libere
			p_antes->tamanio += particion_liberada->tamanio + p_despues->tamanio; //directamente hago la anterior mas grande (?
			//TODO aca hay que sacar las particiones p_despues y particion_liberada de la lista de particiones liberadas pero ya tengo sueño jaja salu2


		}
	}
}

t_particion* buscar_particion_bf(int tamanio_a_almacenar){ //se puede con fold creo

	int best_fit = -1;

	int cantidad_libres = list_size(particiones_libres) - 1; //para que comience por 0

	t_particion* aux = malloc(sizeof(t_particion));
	t_particion* mayor = malloc(sizeof(t_particion));

	for(int i = 0; i < cantidad_libres; i++){
		aux = list_get(particiones_libres, i);
		if(aux->tamanio >= tamanio_a_almacenar){
			if(best_fit == -1){
				best_fit = i;
				mayor = list_get(particiones_libres, i);
			}else if(mayor->tamanio > aux->tamanio){
				best_fit = i;
			}
		}
	}

	free(aux);
	free(mayor);
	t_particion* best = malloc(sizeof(t_particion));

	if(best_fit == -1){
		best = NULL;
	}else{
		best = list_get(particiones_libres, best_fit);
	}

	return best;

}

t_particion* elegir_victima_particiones(int tamanio_a_almacenar){
	switch(configuracion_cache->algoritmo_reemplazo){
	case LRU:
		return elegir_victima_particiones_LRU(tamanio_a_almacenar);

		//case fifo
	}
}

t_particion* elegir_victima_particiones_LRU(int tamanio_a_almacenar){

	t_particion* particion;

	bool _orden(t_particion* particion1, t_particion* particion2){
		return particion1->ultimo_acceso > particion2->ultimo_acceso;
	}

	list_sort(particiones_ocupadas, (void*)_orden);

	bool _puede_guardar(t_particion* particion){
		return particion->tamanio >= tamanio_a_almacenar;
	}

	particion = list_find(particiones_ocupadas, (void*)_puede_guardar);

	eliminar_particion(particion);

	return particion;

}

void eliminar_particion(t_particion* particion_a_liberar){

	t_particion* particion_nueva_libre = malloc(sizeof(t_particion));

	particion_nueva_libre->base = particion_a_liberar->base;
	particion_nueva_libre->tamanio = particion_a_liberar->tamanio;

	list_add(particiones_libres, particion_nueva_libre);

	bool _es_la_particion(void* particion){
		return particion == particion_a_liberar;
	}

	//TODO: Diana, ver si esta bien con el null (?
	list_remove_and_destroy_by_condition(particiones_libres,_es_la_particion,NULL);

}

void asignar_particion(void* datos, t_particion* particion_libre, int tamanio, op_code codigo_op, uint32_t id){

	memcpy((void*)particion_libre->base, datos, tamanio); //copio a la memoria

	bool _es_la_particion(t_particion* particion){
		return particion == particion_libre;
	}
	list_remove_by_condition(particiones_libres, (void*) _es_la_particion); //esto funca?? saco de la lista la particion (no se si anda haciendo == particion_libre)

	if(particion_libre->tamanio != tamanio){ //si no entro justo (mismo tamanio), significa que queda una nueva particion de menor tamanio libre
		t_particion* particion_nueva = malloc(sizeof(t_particion));
		particion_nueva->base = particion_libre->base + tamanio;
		particion_nueva->tamanio = particion_libre->tamanio - tamanio;
		particion_libre->tamanio = tamanio;

		list_add(particiones_libres, particion_nueva);
	}

	particion_libre->ultimo_acceso = time(NULL);
	particion_libre->cola = codigo_op;
	particion_libre->id_mensaje = id;
	list_add(particiones_ocupadas, particion_libre); //la particion ahora ya no está libre

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
	buffer_broker->id = cid;
	buffer_broker->buffer = stream;

	return buffer_broker;
}


t_particion_buddy* almacenar_datos_buddy(void* datos, int tamanio){

	t_particion_buddy* bloque_buddy_particion = malloc(sizeof(t_particion_buddy));
	bloque_buddy_particion = eleccion_particion_asignada_buddy(datos,tamanio);

	while(bloque_buddy_particion == NULL){

		switch(configuracion_cache->algoritmo_reemplazo){
		case FIFO:
			eleccion_victima_fifo_buddy(tamanio);
			break;
		case LRU:
			eleccion_victima_lru_buddy();
			break;
		}
		bloque_buddy_particion = eleccion_particion_asignada_buddy(datos,tamanio);
	}

	asignar_particion_buddy(bloque_buddy_particion,datos,tamanio);
	return bloque_buddy_particion;
}

void asignar_particion_buddy(t_particion_buddy* bloque_buddy_particion, void* datos, int tamanio){
	memcpy(memoria_cache + bloque_buddy_particion->base, datos, tamanio); //copio a la memoria
	//	bloque_buddy_particion->ocupado = true;

	bool _mismo_id_buddy(void* elemento_lista){
		return remove_by_id(elemento_lista, bloque_buddy_particion->id);//TODO: Cambiar nombre remove a buscar
	}

	pthread_mutex_lock(&memoria_buddy_mutex);
	t_particion_buddy* particion_buddy = list_find(memoria_buddy,_mismo_id_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	particion_buddy->ocupado = true;
}

t_particion_buddy* eleccion_particion_asignada_buddy(void* datos,int tamanio){

	//bloque_elegido = bloque que retorno para guardar los datos
	//bloque_a_partir = bloque que voy a partir en mil pedacitos
	t_particion_buddy* bloque_elegido = malloc(sizeof(t_particion_buddy));
	t_particion_buddy* bloque_a_partir = malloc(sizeof(t_particion_buddy));

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
		list_sort(memoria_buddy, _ordenar_menor_a_mayor);
		bloque_a_partir = memoria_buddy->head->data;
		pthread_mutex_unlock(&memoria_buddy_mutex);

		bool condicion_buddy_particion = validar_condicion_buddy(bloque_a_partir,tamanio);

		//Preguntar si me entra en el bloque, si esta disponible y si el bloque es > a tamanio minimo
		//Si entra divido por 2 y vuelvo a preguntar lo mismo que antes asi hasta llegar al tamanio minimo
		//O hasta llegar a que no entre.
		//Si no entra, lo guardo en la ultima particion que entraba.

		while(condicion_buddy_particion){
			bloque_elegido = generar_particion_buddy(bloque_a_partir);
			condicion_buddy_particion = validar_condicion_buddy(bloque_elegido,tamanio);
		}

		if(bloque_elegido != NULL && !bloque_a_partir->ocupado && bloque_a_partir->tamanio > tamanio)
			bloque_elegido = bloque_a_partir;
	}

	//TODO: Verificar que devuelva null
	return bloque_elegido;
}

bool encontrar_bloque_valido_buddy(t_particion_buddy* bloque_buddy,int tamanio){
	return bloque_buddy->tamanio >= tamanio
			&& !bloque_buddy->ocupado;
}

bool ordenar_menor_a_mayor(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2){
	return bloque_buddy->tamanio < bloque_buddy2->tamanio ;
}

bool validar_condicion_buddy(t_particion_buddy* bloque_buddy,int tamanio){
	int bloque_tamanio_siguiente = bloque_buddy->tamanio / 2;
	return bloque_tamanio_siguiente > tamanio
			&& !bloque_buddy->ocupado
			&& bloque_tamanio_siguiente > configuracion_cache->tamanio_minimo_p;
}

t_particion_buddy* generar_particion_buddy(t_particion_buddy* bloque_buddy){
	uint32_t id_viejo = bloque_buddy->id;

	t_particion_buddy* bloque_buddy2 = malloc(sizeof(t_particion_buddy));
	bloque_buddy2 = bloque_buddy;
	uint32_t nuevo_tamanio = bloque_buddy->tamanio / 2;
	bloque_buddy->tamanio = nuevo_tamanio;
	bloque_buddy2->tamanio = nuevo_tamanio;
	bloque_buddy2->base = bloque_buddy->base + nuevo_tamanio;//TODO: Falta el +1 ?
	bloque_buddy->ocupado = false;
	bloque_buddy2->ocupado = false;
	pthread_mutex_lock(&buddy_id_mutex);
	buddy_id++;
	pthread_mutex_unlock(&buddy_id_mutex);
	bloque_buddy2->id = buddy_id;
	pthread_mutex_lock(&buddy_id_mutex);
	buddy_id++;
	pthread_mutex_unlock(&buddy_id_mutex);
	bloque_buddy->id = buddy_id;

	bool _mismo_id_buddy(t_particion_buddy* bloque_buddy){
		return mismo_id_buddy(bloque_buddy,id_viejo);
	}
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add(memoria_buddy,bloque_buddy2);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_add(memoria_buddy,bloque_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	return bloque_buddy2;
}


bool mismo_id_buddy(t_particion_buddy* bloque_buddy,uint32_t id_viejo){
	return bloque_buddy->id == id_viejo;
}

void eleccion_victima_fifo_buddy(int tamanio){

	bool _sort_byId_memoria_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2){
		return sort_byId_memoria_buddy(bloque_buddy,bloque_buddy2);
	}
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_sort(memoria_buddy,(void*)_sort_byId_memoria_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	bool _eleccion_victima_fifo_a_eliminar(void* bloque_buddy){
		return eleccion_victima_fifo_a_eliminar(bloque_buddy,tamanio);
	}
	//Busco la victima y "elimino"
	pthread_mutex_lock(&memoria_buddy_mutex);
	t_particion_buddy* victima_elegida = list_find(memoria_buddy, (void*)_eleccion_victima_fifo_a_eliminar);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	victima_elegida->ocupado = false;

	//consolidar
	consolidar_buddy(victima_elegida,memoria_buddy);
}

bool remove_by_id(t_particion_buddy* bloque_buddy,uint32_t id_remover){
	return bloque_buddy->id == id_remover;
}

void consolidar_buddy(t_particion_buddy* bloque_buddy_old,t_list* lista_fifo_buddy){

	void _encontrar_y_consolidar_buddy(t_particion_buddy* bloque_buddy){
		return encontrar_y_consolidar_buddy(bloque_buddy,bloque_buddy_old);
	}

	list_iterate(lista_fifo_buddy, (void*)_encontrar_y_consolidar_buddy);
}

//TODO: Reveer esto Lucas
bool validar_condicion_fifo_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy_old){
	return (bloque_buddy->tamanio == bloque_buddy_old->tamanio)
			&& (bloque_buddy_old->base+bloque_buddy_old->tamanio == bloque_buddy->base);
}

void encontrar_y_consolidar_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy_old){
	bool condition_for_buddy = validar_condicion_fifo_buddy(bloque_buddy,bloque_buddy_old);

	if(condition_for_buddy && !bloque_buddy->ocupado){
		t_particion_buddy* bloque_buddy_new = malloc(sizeof(t_particion_buddy));
		bloque_buddy_new->tamanio = bloque_buddy->tamanio * 2;
		bloque_buddy_new->ocupado = false;
		pthread_mutex_lock(&buddy_id_mutex);
		buddy_id++;
		pthread_mutex_unlock(&buddy_id_mutex);
		bloque_buddy_new->id = buddy_id;
		bloque_buddy_new->base = bloque_buddy_old->base;

		//
		bool _mismo_id_buddy1(t_particion_buddy* bloque_buddy){
			return mismo_id_buddy(bloque_buddy,bloque_buddy_old->id);
		}

		bool _mismo_id_buddy2(t_particion_buddy* bloque_buddy){
			return mismo_id_buddy(bloque_buddy,bloque_buddy->id);
		}

		pthread_mutex_lock(&memoria_buddy_mutex);
		list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy1);
		pthread_mutex_unlock(&memoria_buddy_mutex);
		pthread_mutex_lock(&memoria_buddy_mutex);
		list_remove_by_condition(memoria_buddy,(void*)_mismo_id_buddy2);
		pthread_mutex_unlock(&memoria_buddy_mutex);
		//
		pthread_mutex_lock(&memoria_buddy_mutex);
		list_add(memoria_buddy,bloque_buddy_new);
		pthread_mutex_unlock(&memoria_buddy_mutex);
	}
}

bool eleccion_victima_fifo_a_eliminar(t_particion_buddy* bloque_buddy, int tamanio){
	return bloque_buddy->tamanio > tamanio;
}

bool sort_byId_memoria_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2){
	return bloque_buddy->id < bloque_buddy2->id;
}

void eleccion_victima_lru_buddy(){
	bool _sort_by_acceso_memoria_buddy(void* bloque_buddy,void* bloque_buddy2){
		return sort_by_acceso_memoria_buddy(bloque_buddy,bloque_buddy2);
	}
	pthread_mutex_lock(&memoria_buddy_mutex);
	list_sort(memoria_buddy,(void*)sort_by_acceso_memoria_buddy);
	pthread_mutex_unlock(&memoria_buddy_mutex);
	//	t_particion_buddy bloque_a_eliminar = memoria_buddy->head->data;

}

bool sort_by_acceso_memoria_buddy(t_particion_buddy* bloque_buddy,t_particion_buddy* bloque_buddy2){
	return (bloque_buddy->ultimo_acceso) < (bloque_buddy2->ultimo_acceso);
}

//



