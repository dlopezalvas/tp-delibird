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
	free(path_metadata);

	//crear_bitmap(pto_montaje);

	//free(metadata_fs);

}

//metadata es el inodo

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
//	fprintf(metadata, "BLOCKS=5192\n");
//	fprintf(metadata, "MAGIC_NUMBER=TALL_GRASS\n");
//
//	fclose(metadata);
//
//	free(path_metadata);
//
//}

void crear_bitmap(char* punto_montaje){

	char* path_bitarray = string_new();

	string_append(&path_bitarray, punto_montaje);
	string_append(&path_bitarray,"/Metadata/Bitmap.bin");

	int blocks = metadata_fs->blocks/8; //esto esta para probar, depues hay tirar datos del metadata a alguna estructura

	int bitarray_file = open(path_bitarray, O_RDWR | O_CREAT);  //uso open porque necesito el int para el mmap

	char* mapeo_bitarray = mmap(0, blocks, PROT_WRITE | PROT_READ, MAP_SHARED, bitarray_file, 0);

	bitarray = bitarray_create(mapeo_bitarray, blocks); //MODE DEFAULT

	free(path_bitarray);
}

//journaling, checkeo con el log, si falla a la mitad que se pueda deshacer los pasos

void agregar_pokemon_mapa(t_new_pokemon* pokemon){

	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);

	if(existe_pokemon(path_pokemon)){
		actualizar_nuevo_pokemon(pokemon);
	}else{
		crear_pokemon(pokemon);
	}
}

void crear_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);

	mkdir(path_pokemon, 0777); //creo el directorio

	string_append(&path_pokemon, "/Metadata.bin");

	FILE* metadata = fopen(path_pokemon, "w+"); //creo su metadata

	fprintf(metadata, "DIRECTORY=N\n");
	fprintf(metadata, "SIZE=0\n");
	fprintf(metadata, "BLOCKS=[]\n");
	fprintf(metadata, "OPEN=N\n");
	fclose(metadata);

	//actualizar_pokemon(pokemon);
}

void actualizar_nuevo_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);
	string_append(&path_pokemon, "/Metadata.bin");

	t_config* config_pokemon = config_create(path_pokemon);

	if(!archivo_abierto(config_pokemon)){

		char** blocks = abrir_archivo(config_pokemon, path_pokemon);

		int tamanio_total = config_get_int_value(config_pokemon, SIZE);

//		char* datos = leer_archivo(blocks, tamanio_total);

		t_config* config_datos = transformar_a_config(leer_archivo(blocks, tamanio_total));

		char* posicion = string_new();

		string_append(&posicion, string_itoa(pokemon->coordenadas.pos_x));
		string_append(&posicion, "-");
		string_append(&posicion, string_itoa(pokemon->coordenadas.pos_y));

		if(config_has_property(config_datos, posicion)){
			char* nueva_cantidad_posicion = string_itoa(config_get_int_value(config_datos, posicion) + 1); //a la cantidad que ya hay, le sumo el nuevo pokemon
			config_set_value(config_datos, posicion, nueva_cantidad_posicion);

		}else{
			config_set_value(config_datos, posicion, "1"); //si no tiene pokemones en esa posicion, cargo solamente 1 (el pokemon nuevo)
		}

		//guardar_archivo(config_datos);

	}else{
		puts("reintentar");
	}
}

t_config* transformar_a_config(char* datos){
	t_config* config_datos = config_create(pto_montaje);

	char** lineas = string_split(datos, "\n");

	int i = 0;

	while(lineas[i]!=NULL){ //por cada "posicion", hay una linea del vector "lineas" (separado por \n)
		char** key_valor = string_split(lineas[i], "=");
		config_set_value(config_datos, key_valor[0], key_valor[1]); //separo la posicion de la cantidad (a traves del =)y seteo como key la posicion con su valor cantidad
		liberar_vector(key_valor);
		i++;
	}

	liberar_vector(lineas);

	return config_datos;
}


//void guardar_archivo(t_config* config_datos, t_config* config_pokemon){
//
//	//cantidad de bloques = tamanio real en bytes / tamanio de cada bloque redondeado hacia arriba
//
//	int cantidad_bloques_antes = ceil(config_get_int_value(config_pokemon, SIZE) / metadata_fs->block_size);
//	int cantidad_bloques_actuales = ceil(config_get_int_value(config_datos, NUEVO_TAMANIO) / metadata_fs->block_size);
//
//
//
//}

char* leer_archivo(char** blocks, int tamanio_total){ //para leer el archivo, si su tamanio es mayor a block_size del metadata tall grass, leo esa cantidad, si no leo el tamanio que tiene

	char* path_blocks = string_new();

	string_append(&path_blocks, pto_montaje);
	string_append(&path_blocks, "/Blocks/");

	int i;
	int cantidad = cantidad_bloques(blocks);

	char* datos = malloc(tamanio_total);

	int offset = 0;

	for(i = 0; i < cantidad; i++){

		//armo el path de cada bloque que voy a leer
		char* bloque_especifico = string_new();
		string_append(&bloque_especifico, path_blocks);
		string_append(&bloque_especifico, blocks[i]);
		string_append(&bloque_especifico, ".bin");

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

	free(path_blocks);
	return datos;
}

int cantidad_bloques(char** blocks){
	int i = 0;
	while(blocks[i]!=NULL){
		i++;
	}
	return i;
}

char** abrir_archivo(t_config* config_archivo, char* path_pokemon){

	char** bloques = config_get_array_value(config_archivo, BLOCKS);

	config_set_value(config_archivo, OPEN, YES);
	config_save_in_file(config_archivo, path_pokemon);

	return bloques;
}

bool archivo_abierto(t_config* config_archivo){

	char* archivo_open = config_get_string_value(config_archivo, OPEN);
	bool esta_abierto = string_equals_ignore_case(YES, archivo_open);

	return esta_abierto;
}

bool existe_pokemon(char* path_pokemon){

	DIR* verificacion = opendir(path_pokemon);

	free(path_pokemon);

	bool existe = !(verificacion == NULL); //si al abrirlo devuelve NULL, el directorio no existe

	return existe;
}
//
//void esperar_cliente(int servidor){
//	pthread_t thread;
//	struct sockaddr_in direccion_cliente;
//
//	unsigned int tam_direccion = sizeof(struct sockaddr_in);
//
//	int cliente = accept (servidor, (void*) &direccion_cliente, &tam_direccion);
//
//	pthread_create(&thread,NULL,(void*)serve_client,&cliente);
//	pthread_detach(thread);
//}
//
//void serve_client(int* socket)
//{
//	int cod_op;
//	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
//		cod_op = -1;
//	process_request(cod_op, *socket);
//}
//
//void socketEscucha(char*IP, char* Puerto){
//	int servidor = iniciar_servidor(IP,Puerto);
//	while(1){
//		esperar_cliente(servidor);
//	}
//
//}
//void process_request(int cod_op, int cliente_fd) {
//	int size = 0;
//	void* buffer = recibir_mensaje(cliente_fd, &size);
//	uint32_t id = recv(cliente_fd, &id,sizeof(uint32_t),0);
//
//	t_new_pokemon* new_pokemon = malloc(sizeof(t_new_pokemon));
//
//		switch (cod_op) {
//		case NEW_POKEMON:
//			new_pokemon = deserializar_position_and_name(buffer);
//			agregar_pokemon_mapa(new_pokemon);
//			break;
//		case 0:
//			pthread_exit(NULL);
//		case -1:
//			pthread_exit(NULL);
//		}
//}



