#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/bitarray.h>
#include<../CommonsMCLDG/utils.h>
#include<../CommonsMCLDG/serializacion.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<pthread.h>
#include<dirent.h>

#define PUNTO_MONTAJE_TALLGRASS "PUNTO_MONTAJE_TALLGRASS"
#define BLOCK_SIZE "BLOCK_SIZE"
#define BLOCKS "BLOCKS"
#define MAGIC_NUMBER "MAGIC_NUMBER"
#define OPEN "OPEN"
#define YES "Y"
#define NO "N"
#define DIRECTORIO "DIRECTORIO"
#define SIZE "SIZE"
#define NUEVO_TAMANIO "NUEVO_TAMANIO"



typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}t_metadata;

t_bitarray* bitarray;
char* pto_montaje;

t_metadata* metadata_fs;

void crear_tall_grass(t_config* config);
//void crear_metadata(char* path_metadata);
void crear_bitmap(char* path);


bool existe_pokemon(char* path_pokemon);
void agregar_pokemon_mapa(t_new_pokemon* pokemon);
void crear_pokemon(t_new_pokemon* pokemon);
bool archivo_abierto(t_config* config_archivo);
void actualizar_nuevo_pokemon(t_new_pokemon* pokemon);

char** abrir_archivo(t_config* config_archivo, char* path_pokemon);
char* leer_archivo(char** blocks, int tamanio_total);
int cantidad_bloques(char** blocks);

t_config* transformar_a_config(char* datos);


#endif /* UTILS_H_ */
