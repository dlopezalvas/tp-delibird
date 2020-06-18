#include "utils_gamecard.h"

//creacion de archivos y directorios (checkpoint 3)

void crear_tall_grass(t_config* config){
	pto_montaje = config_get_string_value(config,PUNTO_MONTAJE_TALLGRASS);

	mkdir(pto_montaje, 0777);

	metadata_fs = cargar_metadata(pto_montaje);
	//crear_metadata(pto_montaje);
	crear_bitmap(pto_montaje);

	free(metadata_fs);

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

t_metadata* cargar_metadata(char* punto_montaje){

	char* path_metadata = string_new();

	string_append(&path_metadata, punto_montaje);
	string_append(&path_metadata,"/Metadata/Metadata.bin");

	t_config* config_metadata = config_create(path_metadata);

	t_metadata* aux = malloc(sizeof(t_metadata));

	aux->block_size = atoi(config_get_string_value(config_metadata, BLOCK_SIZE));
	aux->blocks = atoi(config_get_string_value(config_metadata, BLOCKS));
	aux->magic_number=config_get_string_value(config_metadata, MAGIC_NUMBER);

	config_destroy(config_metadata);
	free(path_metadata);

	return aux;
}
//journaling, checkeo con el log, si falla a la mitad que se pueda deshacer los pasos

void agregar_pokemon_mapa(t_new_pokemon* pokemon){

	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);

	if(existe_pokemon(path_pokemon)){
		actualizar_pokemon(pokemon);
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

void actualizar_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);
	string_append(&path_pokemon, "/Metadata.bin");

	t_config* config_pokemon = config_create(path_pokemon);

	if(!archivo_abierto(config_pokemon)){

		//char** blocks = abrir_archivo(config_pokemon, path_pokemon);

		//leer_archivo(blocks);

//		char** posicion = string_new();
//
//		string_append(&posicion, string_itoa(pokemon->coordenadas.pos_x));
//		string_append(&posicion, "-");
//		string_append(&posicion, string_itoa(pokemon->coordenadas.pos_y));
//
//		if(config_has_property(config_pokemon, posicion)){
//
	}else{
		puts("reintentar");
	}
}

t_config* leer_archivo(char** blocks){ //para leer el archivo, bloque por bloque leo linea por linea y la meto en un archivo config

	char* path_blocks = string_new();

	string_append(&path_blocks, pto_montaje);
	string_append(&path_blocks, "/Blocks/");


	int i;
	int cantidad = cantidad_bloques(blocks);

	t_config* config_datos = config_create(path_blocks);

	for(i = 0; i < cantidad; i++){

		//armo el path de cada bloque que voy a leer
		char* bloque_especifico = string_new();
		string_append(&bloque_especifico, path_blocks);
		string_append(&bloque_especifico, blocks[i]);
		string_append(&bloque_especifico, ".bin");


		FILE* bloque = fopen(bloque_especifico, "r");

		char* datos = malloc(sizeof(char));

		while(fgets(datos, metadata_fs->block_size, bloque)){
			char** aux = string_split(datos, "=");
			config_set_value(config_datos, aux[0], aux[1]);
			free(aux[0]);
			free(aux[1]);
			free(aux);

		}

		free(datos);
		fclose(bloque);
		free(bloque_especifico);
	}
	free(path_blocks);
	return config_datos;
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



