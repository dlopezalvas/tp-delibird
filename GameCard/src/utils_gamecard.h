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



typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}t_metadata;

typedef struct{
	bool directorio;
	uint32_t size;
	uint32_t** blocks;
	bool open;
}t_archivo;

t_bitarray* bitarray;
char* pto_montaje;

t_metadata* metadata_fs;

void crear_tall_grass(t_config* config);
//void crear_metadata(char* path_metadata);
void crear_bitmap(char* path);
t_metadata* cargar_metadata(char* punto_montaje);

bool existe_pokemon(char* path_pokemon);
void agregar_pokemon_mapa(t_new_pokemon* pokemon);
void crear_pokemon(t_new_pokemon* pokemon);
bool archivo_abierto(char* path_archivo);
void actualizar_pokemon(t_new_pokemon* pokemon);

void abrir_archivo(char* path_archivo);


#endif /* UTILS_H_ */
