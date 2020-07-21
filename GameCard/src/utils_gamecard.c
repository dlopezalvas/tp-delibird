#include "utils_gamecard.h"

//creacion de archivos y directorios (checkpoint 3)

void crear_tall_grass(t_config* config){
	pto_montaje = config_get_string_value(config,PUNTO_MONTAJE_TALLGRASS);

	mkdir(pto_montaje, 0777);

	char* path_metadata = string_new();

	string_append(&path_metadata, pto_montaje);
	string_append(&path_metadata,"/Metadata/Metadata.bin");

	t_config* config_metadata = config_create(path_metadata);

	metadata_fs = malloc(sizeof(t_metadata));

	metadata_fs->block_size = config_get_int_value(config_metadata, BLOCK_SIZE);
	metadata_fs->blocks = config_get_int_value(config_metadata, BLOCKS);
	metadata_fs->magic_number = config_get_string_value(config_metadata, MAGIC_NUMBER);

	config_destroy(config_metadata);
	//free(path_metadata);

	crear_bitmap(pto_montaje);

	sem_metadatas = list_create();
	pthread_mutex_init(&lista_metadatas_mtx, NULL);

	pokemones = list_create();
	pthread_mutex_init(&pokemones_mtx, NULL);

	//free(metadata_fs);

}

//metadata es el inodo
//
//void crear_metadata(char* punto_montaje){
//
//	char* path_metadata = string_new();
//
//	string_append(&path_metadata, punto_montaje);
//	string_append(&path_metadata,"/Metadata");
//
//	mkdir(path_metadata, 0777);
//
//	string_append(&path_metadata, "/Metadata.bin");
//
//	FILE * metadata = fopen(path_metadata, "w+");
//
//	fprintf(metadata, "BLOCK_SIZE=64\n");
//	fprintf(metadata, "BLOCKS=4096\n");
//	fprintf(metadata, "MAGIC_NUMBER=TALL_GRASS\n");
//
//	fclose(metadata);
//
//	free(path_metadata);
//
//}

void crear_bitmap(char* punto_montaje){

	char* path_bitarray = string_new();

	string_append_with_format(&path_bitarray, "%s/Metadata/Bitmap.bin", punto_montaje);

	int blocks = metadata_fs->blocks/8;

	int bitarray_file = open(path_bitarray, O_RDWR | O_CREAT, 0700);  //uso open porque necesito el int para el mmap

	ftruncate(bitarray_file, blocks);

	char* mapeo_bitarray = mmap(0, blocks, PROT_WRITE | PROT_READ, MAP_SHARED, bitarray_file, 0);

	//ver errores en mapeo

	bitarray = bitarray_create_with_mode(mapeo_bitarray, blocks, LSB_FIRST);

	for(int i = 0; i < blocks; i++){
		bitarray_clean_bit(bitarray, i);
	}

	msync(bitarray, sizeof(bitarray), MS_SYNC);

	pthread_mutex_init(&bitarray_mtx, NULL);

	close(bitarray_file);

	//free(path_bitarray);
}

void agregar_pokemon_mapa(t_buffer* buffer){

	t_new_pokemon* pokemon = deserializar_new_pokemon(buffer);

	char* path_pokemon = string_new();
	string_append_with_format(&path_pokemon, "%s/Files/%s", pto_montaje, pokemon->nombre.nombre);

	if(existe_pokemon(path_pokemon)){
		puts("actualizo");
		actualizar_nuevo_pokemon(pokemon);
	}else{
		crear_pokemon(pokemon);
	}

}

void capturar_pokemon(t_position_and_name* pokemon){

	char* path_pokemon = string_new();
	string_append_with_format(&path_pokemon, "%s/Files/%s", pto_montaje, pokemon->nombre.nombre);
	if(existe_pokemon(path_pokemon)){
		puts("existe"); //TODO
	}else{
		puts("informar error por logs???");
	}
}

void crear_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append_with_format(&path_pokemon, "%s/Files/%s", pto_montaje, pokemon->nombre.nombre);

	mkdir(path_pokemon, 0777); //creo el directorio

	string_append(&path_pokemon, "/Metadata.bin");

	FILE* metadata = fopen(path_pokemon, "w+"); //creo su metadata

	pthread_mutex_lock(&lista_metadatas_mtx);
	pthread_mutex_lock(&pokemones_mtx);

	int index_semaforo = list_size(sem_metadatas); //el index va a ser la cantidad de elementos en la lista - 1 (lista comienza en 0), que es lo mismo que tener el tamanio antes de agregarlo a la lista

	if(index_semaforo != list_size(pokemones)){
		perror("No coinciden los semaforos con la cantidad de pokemones");
	}

	pthread_mutex_t pokemon_mtx;

	pthread_mutex_init(&pokemon_mtx, NULL);


	list_add(sem_metadatas, &pokemon_mtx);
	list_add(pokemones, pokemon->nombre.nombre);

	pthread_mutex_unlock(&pokemones_mtx);
	pthread_mutex_unlock(&lista_metadatas_mtx);


	pthread_mutex_lock(&pokemon_mtx); //cambia en la lista tambien?

	fprintf(metadata, "DIRECTORY=N\n"); //no lo escribe esto???????
	fprintf(metadata, "OPEN=Y\n"); //lo marco como abierto
	fclose(metadata);

	pthread_mutex_unlock(&pokemon_mtx);

	char* datos = string_new();
	string_append_with_format(&datos, "%d-%d=%d", pokemon->coordenadas.pos_x, pokemon->coordenadas.pos_y, pokemon->cantidad);

	int tamanio = strlen(datos);

	int cantidad_bloques = ceil((float) tamanio / (float) metadata_fs->block_size);

	char** bloques_a_escribir = buscar_bloques_libres(cantidad_bloques);

	//no necesito semaforo porque ya lo marque como abierto, entonces no puede entrar otro hilo al metadata

	FILE* actualizar_metadata = fopen(path_pokemon, "r+");

	fprintf(actualizar_metadata, "BLOCKS=[");



	int j;

	for(j = 0; j < (cantidad_bloques - 1); j++){
		fprintf(actualizar_metadata, "%s", bloques_a_escribir[j]);
		fprintf(actualizar_metadata, ",");
	}

	fprintf(actualizar_metadata, "%s", bloques_a_escribir[j]); //imprimo el ultimo sin la coma
	fprintf(actualizar_metadata, "]\n");

	fclose(actualizar_metadata);

	t_config* config_aux = config_create(path_pokemon);

	config_set_value(config_aux, SIZE, string_itoa(tamanio));

	int offset = 0;

	int i;

	for(i = 0; i < cantidad_bloques; i++){
		escribir_bloque(&offset, datos, bloques_a_escribir[i], &tamanio);
	}


	cerrar_archivo(config_aux, path_pokemon, pokemon->nombre.nombre);

	config_destroy(config_aux);

	//liberar_vector(bloques_a_escribir);
//	free(datos);
//	free(path_pokemon);
}

void actualizar_nuevo_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append_with_format(&path_pokemon, "%s/Files/%s/Metadata.bin", pto_montaje, pokemon->nombre.nombre);

	t_config* config_pokemon = config_create(path_pokemon);

	if(!archivo_abierto(config_pokemon)){

		abrir_archivo(config_pokemon, path_pokemon, pokemon->nombre.nombre);

		char** blocks = config_get_array_value(config_pokemon, BLOCKS);

		int tamanio_total = config_get_int_value(config_pokemon, SIZE);

		char** datos = leer_archivo(blocks, tamanio_total);

		t_list* lista_datos = transformar_a_lista(datos);

		t_config* config_datos = transformar_a_config(datos);

		char* posicion = string_new();

		string_append_with_format(&posicion, "%d-%d", pokemon->coordenadas.pos_x, pokemon->coordenadas.pos_y);

		if(config_has_property(config_datos, posicion)){
			char* nueva_cantidad_posicion = string_itoa(config_get_int_value(config_datos, posicion) + 1); //a la cantidad que ya hay, le sumo el nuevo pokemon

			int i = 0;

			while(!(comienza_con(posicion, list_get(lista_datos, i)))) {  //Se que existe la posicion, entonces recorro hasta encontrarla
				i++; 													 //al salir del while me queda el valor de i como la posicion de la lista que quiero cambiar
			}

			string_append_with_format(&posicion, "=%s", nueva_cantidad_posicion);

			list_replace(lista_datos, i, posicion); //posicion ahora es un string completo (posicion = cantidad)

		}else{
			string_append(&posicion, "=1"); //si no tiene pokemones en esa posicion, cargo solamente 1 (el pokemon nuevo)
			list_add(lista_datos, posicion);
		}

		guardar_archivo(lista_datos, config_pokemon, path_pokemon);

	//	cerrar_archivo(config_pokemon, path_pokemon, pokemon->nombre.nombre);

		liberar_vector(blocks);
		liberar_vector(datos);
		//list_destroy
		config_destroy(config_datos);

	}else{
		//sleep(config_get_int_value(config_gamecard,TIEMPO_DE_REINTENTO_OPERACION));
		//actualizar_nuevo_pokemon(pokemon);
		puts("reintentar");
	}
}

//void actualizar_quitar_pokemon(t_position_and_name* pokemon){
//
//	char* path_pokemon = string_new();
//	string_append_with_format(&path_pokemon, "%s/Files/%s/Metadata.bin", pto_montaje, pokemon->nombre.nombre);
//
//	int index_sem_metadata = index_pokemon(pokemon->nombre.nombre);
//
//	pthread_mutex_t* semaforo_pokemon = list_get(sem_metadatas, index_sem_metadata);
//
//	pthread_mutex_lock(semaforo_pokemon);
//	t_config* config_pokemon = config_create(path_pokemon);
//	pthread_mutex_unlock(semaforo_pokemon);
//
//	if(!archivo_abierto(config_pokemon)){
//
//		abrir_archivo(config_pokemon, path_pokemon, pokemon->nombre.nombre);
//
//		char** blocks = config_get_array_value(config_pokemon, BLOCKS);
//
//		int tamanio_total = config_get_int_value(config_pokemon, SIZE);
//
//		char** datos = leer_archivo(blocks, tamanio_total);
//
//		t_list* lista_datos = transformar_a_lista(datos);
//
//		t_config* config_datos = transformar_a_config(datos);
//
//		char* posicion = string_new();
//
//		string_append_with_format(&posicion, "%d-%d", pokemon->coordenadas.pos_x, pokemon->coordenadas.pos_y);
//
//		if(config_has_property(config_datos, posicion)){
//
//			int i = 0;
//
//			while(!(comienza_con(posicion, list_get(lista_datos, i)))) {  //Se que existe la posicion, entonces recorro hasta encontrarla
//				i++; 													 //al salir del while me queda el valor de i como la posicion de la lista que quiero cambiar
//			}
//
//			char* nueva_cantidad_posicion = string_itoa(config_get_int_value(config_datos, posicion) - 1);
//
//			if(nueva_cantidad_posicion == 0){
//				list_remove(lista_datos, i);
//			}else{
//				string_append_with_format(&posicion, "=%s", nueva_cantidad_posicion);
//
//				list_replace(lista_datos, i, posicion); //posicion ahora es un string completo (posicion = cantidad)
//			}
//
//		}else{
//			puts("otro error que ni idea si va en log o que :(");
//		}
//
//		guardar_archivo(lista_datos, config_pokemon, path_pokemon);
//
//		liberar_vector(blocks);
//		liberar_vector(datos);
//		//list_destroy
//		config_destroy(config_datos);
//
//	}else{
//		//sleep(config_get_int_value(config_gamecard,TIEMPO_DE_REINTENTO_OPERACION));
//		//actualizar_nuevo_pokemon(pokemon);
//		puts("reintentar");
//	}
//}

void guardar_archivo(t_list* lista_datos, t_config* config_pokemon, char* path_pokemon){

	//cantidad de bloques = tamanio real en bytes / tamanio de cada bloque redondeado hacia arriba

	int cantidad_bloques_antes = ceil((float)config_get_int_value(config_pokemon, SIZE) / (float)metadata_fs->block_size);

	int tamanio_nuevo = list_fold(lista_datos, 0, (void*) calcular_tamanio) - 1; //el -1 porque el ultimo elemento (ultima linea del archivo) no tiene \n

	int cantidad_bloques_actuales = ceil((float)tamanio_nuevo / (float)metadata_fs->block_size);

	int offset = 0;

	char* datos = transformar_a_dato(lista_datos, tamanio_nuevo);

	char** bloques = config_get_array_value(config_pokemon, BLOCKS);
	char* bloques_guardar = string_new();
	string_append(&bloques_guardar, "[");

	if(cantidad_bloques_antes <= cantidad_bloques_actuales){ //si tienen el mismo tamanio, solo vuelvo a copiar los datos en los mismos bloques
		int i;

		for(i = 0; i < cantidad_bloques_antes -1; i++){
			escribir_bloque(&offset, datos, bloques[i], &tamanio_nuevo);
			string_append_with_format(&bloques_guardar, "%s,", bloques[i]);
		}
		escribir_bloque(&offset, datos, bloques[i], &tamanio_nuevo);
		string_append_with_format(&bloques_guardar, "%s", bloques[i]);

		if(cantidad_bloques_antes < cantidad_bloques_actuales){ //si es mayor tamanio tengo que pedir mas bloques
			string_append(&bloques_guardar, ",");
			int bloques_a_pedir = cantidad_bloques_actuales - cantidad_bloques_antes;
			char** bloques_nuevos = buscar_bloques_libres(bloques_a_pedir); //falta verificar ver error si no hay disponibles

			puts(bloques_nuevos[0]);
			puts(bloques_guardar);

			int j;

			for(j = 0; j < bloques_a_pedir -1;j++){
				escribir_bloque(&offset, datos, bloques_nuevos[j], &tamanio_nuevo);
				string_append_with_format(&bloques_guardar, "%s,", bloques_nuevos[j]);
			}
			escribir_bloque(&offset, datos, bloques_nuevos[j], &tamanio_nuevo);
			string_append_with_format(&bloques_guardar, "%s", bloques_nuevos[j]);

			//liberar_vector(bloques_nuevos);
		}

	string_append(&bloques_guardar, "]");

	}else{ //tengo que borrar bloques
		puts("borrar");
	}

	config_set_value(config_pokemon, SIZE, string_itoa(offset));

	config_set_value(config_pokemon, OPEN, NO);

	config_set_value(config_pokemon, BLOCKS, bloques_guardar);

	config_save_in_file(config_pokemon, path_pokemon);

	config_destroy(config_pokemon);

	liberar_vector(bloques);
	free(datos);

}

char** buscar_bloques_libres(int cantidad){
	char** bloques = malloc(cantidad);

	for(int i = 0; i < cantidad; i++){
		int aux = bloque_libre();
		if(aux == -1){
			perror("No se encontró bloque libre");
		}else{
			bloques[i] = string_itoa(aux);
		}
	}

	return bloques;
}

int bloque_libre(){
	for(int i = 0; i < metadata_fs->blocks; i++){
		if(!bitarray_test_bit(bitarray,i)){
			pthread_mutex_lock(&bitarray_mtx);
			bitarray_set_bit(bitarray,i);
			msync(bitarray, sizeof(bitarray), MS_SYNC);
			pthread_mutex_unlock(&bitarray_mtx);
			return i;
		}
	}

	return -1;
}

void escribir_bloque(int* offset, char* datos, char* bloque, int* tamanio){

		char* path_blocks = string_new();
		string_append_with_format(&path_blocks, "%s/Blocks/%s.bin", pto_montaje, bloque);

		int tamanio_a_escribir = minimo_entre(metadata_fs->block_size, *tamanio); //si es mayor o igual al block_size escribo el bloque entero, si es menor escribo los bytes que quedan por escribir

		FILE* fd_bloque = fopen(path_blocks, "w+");

		fseek(fd_bloque, 0, SEEK_SET);

		fwrite(datos + *offset, tamanio_a_escribir, 1, fd_bloque);
		*offset += tamanio_a_escribir;
		*tamanio -= tamanio_a_escribir;

		fclose(fd_bloque);
		free(path_blocks);
}

int minimo_entre (int nro1, int nro2){
	if (nro1 >= nro2){
		return nro2;
	}
	return nro1;
}

char* transformar_a_dato(t_list* lista_datos, int tamanio){
	char* datos = string_new();

	int cantidad_lineas = list_size(lista_datos) - 1; //la ultima linea no tiene \n

	int i;

	for (i=0; i < cantidad_lineas; i++){
		string_append_with_format(&datos, "%s\n", list_get(lista_datos, i));
	}

	string_append(&datos, list_get(lista_datos, i)); //ultima linea

	datos[tamanio] = '\0';

	return datos;
}

int calcular_tamanio(int acc, char* linea){ //func para el fold en guardar archivo

	int tamanio_linea = strlen(linea) + 1;

	return acc + tamanio_linea;
}

bool comienza_con(char* posicion, char* linea){

	char** posicion_cantidad = string_split(linea, "=");

	bool es_la_posicion = string_equals_ignore_case(posicion_cantidad[0], posicion);

	liberar_vector(posicion_cantidad);

	return es_la_posicion;
}

// Conviene mas con lista o config?????
t_list* transformar_a_lista(char** lineas){
	int i = 0;
	t_list* lista_datos = list_create();

	while(lineas[i]!=NULL){
		list_add(lista_datos, lineas[i]);
		i++;
	}

	return lista_datos;
}

t_config* transformar_a_config(char** lineas){
	t_config* config_datos = config_create(pto_montaje);

	int i = 0;

	while(lineas[i]!=NULL){ //por cada "posicion", hay una linea del vector "lineas" (separado por \n)
		char** key_valor = string_split(lineas[i], "=");
		config_set_value(config_datos, key_valor[0], key_valor[1]); //separo la posicion de la cantidad (a traves del =)y seteo como key la posicion con su valor cantidad
		liberar_vector(key_valor);
		i++;
	}
	return config_datos;
}

char** leer_archivo(char** blocks, int tamanio_total){ //para leer el archivo, si su tamanio es mayor a block_size del metadata tall grass, leo esa cantidad, si no leo el tamanio que tiene

	char* path_blocks = string_new();

	string_append_with_format(&path_blocks, "%s/Blocks/", pto_montaje);

	int i;

	int cantidad = cantidad_bloques(blocks);

	char* datos = malloc(tamanio_total + 1);

	int offset = 0;

	for(i = 0; i < cantidad; i++){

		//armo el path de cada bloque que voy a leer
		char* bloque_especifico = string_new();
		string_append_with_format(&bloque_especifico, "%s%s.bin", path_blocks, blocks[i]);

		FILE* bloque = fopen(bloque_especifico, "r");

		if(tamanio_total > metadata_fs->block_size){
			fread(datos + offset, sizeof(char), metadata_fs->block_size, bloque);
			offset+= metadata_fs->block_size;
			tamanio_total -= metadata_fs->block_size;

		}else{
			fread(datos + offset, sizeof(char), tamanio_total, bloque); //si no esta lleno entonces es el ultimo bloque -> no necesito cambiar el tamanio ni el offset???
			offset+= tamanio_total;
		}

		fclose(bloque);

	}
	datos[offset] = '\0';

	free(path_blocks);

	char** array_datos = string_split(datos, "\n");

	free(datos);

	return array_datos;
}

int cantidad_bloques(char** blocks){
	int i = 0;
	while(blocks[i]!=NULL){
		i++;
	}
	return i;
}

void abrir_archivo(t_config* config_archivo, char* path_pokemon, char* nombre_pokemon){
	int index_sem_metadata = index_pokemon(nombre_pokemon);

	pthread_mutex_t* semaforo_pokemon = list_get(sem_metadatas, index_sem_metadata);

	//pthread_mutex_lock(semaforo_pokemon);

	config_set_value(config_archivo, OPEN, YES);
	config_save_in_file(config_archivo, path_pokemon);
	//pthread_mutex_unlock(semaforo_pokemon);

}

void cerrar_archivo(t_config* config_archivo, char* path_pokemon, char* nombre_pokemon){

	int index_sem_metadata = index_pokemon(nombre_pokemon);

	pthread_mutex_t* semaforo_pokemon = list_get(sem_metadatas, index_sem_metadata);

	pthread_mutex_lock(semaforo_pokemon);
	config_set_value(config_archivo, OPEN, NO);
	config_save_in_file(config_archivo, path_pokemon);
	pthread_mutex_unlock(semaforo_pokemon);

}

int index_pokemon(char* nombre){
	int index = 0;

	pthread_mutex_lock(&lista_metadatas_mtx);
	pthread_mutex_lock(&pokemones_mtx);
	char* pokemon = list_get(pokemones, index);

	while((!string_equals_ignore_case(pokemon, nombre)) && index < (list_size(pokemones)-1)){
		index++;
		pokemon = list_get(pokemones, index);
	}
	if(!string_equals_ignore_case(pokemon, nombre)){
		pthread_mutex_t* pokemon_mtx;

		pthread_mutex_init(pokemon_mtx, NULL);

		list_add(sem_metadatas, pokemon_mtx);
		list_add(pokemones, nombre);
		index++;
	}
	pthread_mutex_unlock(&pokemones_mtx);
	pthread_mutex_unlock(&lista_metadatas_mtx);

	return index;
}

bool archivo_abierto(t_config* config_archivo){


	char* archivo_open = config_get_string_value(config_archivo, OPEN);
	bool esta_abierto = string_equals_ignore_case(YES, archivo_open);

	//free(archivo_open);

	return esta_abierto;
}

bool existe_pokemon(char* path_pokemon){

	DIR* verificacion = opendir(path_pokemon);

	free(path_pokemon);

	bool existe = !(verificacion == NULL); //si al abrirlo devuelve NULL, el directorio no existe


	return existe;
}

void esperar_cliente(int servidor){

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

void socket_escucha(char*IP, char* Puerto){
	int servidor = iniciar_servidor(IP,Puerto);
	while(1){
		esperar_cliente(servidor);
	}

}

void process_request(int cod_op, int cliente_fd) {
	int size = 0;
	void* buffer = recibir_mensaje(cliente_fd, &size);
	uint32_t id = 0;

	recv(cliente_fd, &id, sizeof(uint32_t),0);

	//t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));

		switch (cod_op) {
		case NEW_POKEMON:
		//	new_pokemon = deserializar_new_pokemon(buffer);
			agregar_pokemon_mapa(buffer);
			break;
		case CATCH_POKEMON:
			puts("catch pokemon");
			break;
		case GET_POKEMON:
			puts("get pokemon");
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}
void crear_conexiones(){
	int tiempoReconexion = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	pthread_t new_pokemon_thread;
	pthread_t catch_pokemon_thread;
	pthread_t get_pokemon_thread;
	while(1){
		pthread_create(&new_pokemon_thread,NULL,(void*)connect_new_pokemon,NULL);
		pthread_detach(new_pokemon_thread);
		pthread_create(&catch_pokemon_thread,NULL,(void*)connect_catch_pokemon,NULL);
		pthread_detach(catch_pokemon_thread);
		pthread_create(&get_pokemon_thread,NULL,(void*)connect_get_pokemon,NULL);
		pthread_detach(get_pokemon_thread);
		sem_wait(&conexiones);
		sem_wait(&conexiones);
		sem_wait(&conexiones);
		sleep(tiempoReconexion);
		log_info(logger_gamecard, "Inicio Reintento de todas las conexiones");

	}
	sem_destroy(&conexiones);
	return;
}

void connect_new_pokemon(){
	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"NEW_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_gamecard(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);

	puts("envia mensaje");

	int size = 0;
	t_new_pokemon* new_pokemon;
	int cod_op;

	while(1){

		if(recv(socket_broker, &cod_op, sizeof(int), MSG_WAITALL) == 0){
			log_info(logger_gamecard,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);
		}
		void* buffer = recibir_mensaje(socket_broker,&size);
		pthread_t solicitud_mensaje;

		if(cod_op == NEW_POKEMON){
//			list_add(mensajes, &solicitud_mensaje);
//			pthread_create(&solicitud_mensaje, NULL, (void*)agregar_pokemon_mapa, buffer);
//			pthread_detach(solicitud_mensaje);

		}
	}

	liberar_conexion(socket_broker);

	//	free(mensaje -> parametros);
	//	free(mensaje);
}

void connect_catch_pokemon(){
	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"CATCH_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_gamecard(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);

	puts("envia mensaje");

	int size = 0;
	t_position_and_name* catch_pokemon;
	int cod_op;

	while(1){

		if(recv(socket_broker, &cod_op, sizeof(int), MSG_WAITALL) == 0){
			log_info(logger_gamecard,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);
		}
		void* buffer = recibir_mensaje(socket_broker,&size);
		pthread_t solicitud_mensaje;

		if(cod_op == CATCH_POKEMON){

			list_add(mensajes, &solicitud_mensaje);
			puts("catch_pokemon");

//			pthread_create(&solicitud_mensaje, NULL, (void*)agregar_pokemon_mapa, buffer);

		}
	}

	liberar_conexion(socket_broker);

	//	free(mensaje -> parametros);
	//	free(mensaje);
}

void connect_get_pokemon(){
	op_code codigo_operacion = SUSCRIPCION;
	t_mensaje* mensaje = malloc(sizeof(t_mensaje));

	char* linea_split[1] = {"GET_POKEMON"};
	mensaje -> tipo_mensaje = codigo_operacion;
	mensaje -> parametros = linea_split;

	int socket_broker = iniciar_cliente_gamecard(config_get_string_value(config, "IP_BROKER"),config_get_string_value(config, "PUERTO_BROKER"));
	enviar_mensaje(mensaje, socket_broker);

	puts("envia mensaje");

	int size = 0;
	t_get_pokemon* get_pokemon;


	while(1){
		int cod_op;
		if(recv(socket_broker, &cod_op, sizeof(op_code), MSG_WAITALL) == 0){
			log_info(logger_gamecard,"Se ha perdido la conexion con el proceso Broker");
			liberar_conexion(socket_broker);
			sem_post(&conexiones);
			pthread_exit(NULL);
		}
		puts(string_itoa(cod_op));
		void* buffer = recibir_mensaje(socket_broker,&size);
		//pthread_t solicitud_mensaje;

		if(cod_op == GET_POKEMON){

			//list_add(mensajes, &solicitud_mensaje);
			puts("get_pokemon");

//			pthread_create(&solicitud_mensaje, NULL, (void*)agregar_pokemon_mapa, buffer);

		}
	}

	liberar_conexion(socket_broker);

	//	free(mensaje -> parametros);
	//	free(mensaje);
}

int iniciar_cliente_gamecard(char* ip, char* puerto){
	struct sockaddr_in direccion_servidor;

	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = inet_addr(ip);
	direccion_servidor.sin_port = htons(atoi(puerto));

	int cliente = socket(AF_INET, SOCK_STREAM, 0);

	if(connect(cliente, (void*) &direccion_servidor, sizeof(direccion_servidor)) !=0){
		log_info(logger_gamecard, "No se pudo realizar la conexion");
		liberar_conexion(cliente);
		sem_post(&conexiones);
		pthread_exit(NULL);
	}

	log_info(logger_gamecard,"Se ha establecido una conexion con el proceso Broker");
	return cliente;
}

void socket_gameboy(){

	socket_escucha(config_get_string_value(config, "IP_GAMECARD"), config_get_string_value(config, "PUERTO_GAMECARD"));

}



