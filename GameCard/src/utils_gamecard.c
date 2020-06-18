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

	actualizar_pokemon(pokemon);
}

void actualizar_pokemon(t_new_pokemon* pokemon){
	char* path_pokemon = string_new();
	string_append(&path_pokemon, pto_montaje);
	string_append(&path_pokemon, "/Files/");
	string_append(&path_pokemon, pokemon->nombre.nombre);

	if(!archivo_abierto(path_pokemon)){

	}else{
		puts("reintentar");
	}
}

void abrir_archivo(char* path_archivo){

	char* metadata_archivo = string_new();

	string_append(&metadata_archivo, path_archivo);
	string_append(&metadata_archivo, "/Metadata.bin");

	puts(path_archivo);

	t_config* config_archivo = config_create(metadata_archivo);

	config_set_value(config_archivo, OPEN, YES);
	config_save_in_file(config_archivo, metadata_archivo);

	config_destroy(config_archivo);
}

bool archivo_abierto(char* path_archivo){

	t_config* config_archivo = config_create(path_archivo);

	char* archivo_open = config_get_string_value(config_archivo, OPEN);
	bool esta_abierto = string_equals_ignore_case(YES, archivo_open);

	config_destroy(config_archivo);

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



