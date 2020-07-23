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
#include<commons/collections/list.h>
#include<../CommonsMCLDG/utils.h>
#include<../CommonsMCLDG/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<pthread.h>
#include<dirent.h>
#include<math.h>
#include<semaphore.h>

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
#define TIEMPO_DE_REINTENTO_OPERACION "TIEMPO_DE_REINTENTO_OPERACION"
#define TIEMPO_RETARDO_OPERACION "TIEMPO_RETARDO_OPERACION"

typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}t_metadata;

t_bitarray* bitarray;
pthread_mutex_t bitarray_mtx;

t_list* sem_metadatas;
pthread_mutex_t lista_metadatas_mtx;

t_list* pokemones; //index del pokemon deberia ser el mismo index de su semaforo
pthread_mutex_t pokemones_mtx;

pthread_mutex_t log_mtx;

char* pto_montaje;

t_metadata* metadata_fs;

pthread_t thread;

sem_t conexiones;
t_log* logger_gamecard;
t_config* config;

t_list* mensajes;

pthread_t gameboy;

void crear_tall_grass(t_config* config);
void crear_bitmap(char* punto_montaje);

void new_pokemon(t_buffer* buffer);
void catch_pokemon(void* buffer);
void get_pokemon(void* buffer);

char* obtener_posiciones(t_get_pokemon* pokemon);
void crear_pokemon(t_new_pokemon* pokemon);
void actualizar_nuevo_pokemon(t_new_pokemon* pokemon);
void actualizar_quitar_pokemon(t_position_and_name* pokemon, int* resultado);

void guardar_archivo(t_list* lista_datos, t_config* config_pokemon, char* path_pokemon);
void escribir_bloque(int* offset, char* datos, char* bloque, int* tamanio);
char** leer_archivo(char** blocks, int tamanio_total);

char** buscar_bloques_libres(int cantidad);
int bloque_libre();
int cantidad_bloques(char** blocks);

char* transformar_a_dato(t_list* lista_datos, int tamanio);
t_list* transformar_a_lista(char** lineas);
t_config* transformar_a_config(char** lineas);

void abrir_archivo(t_config* config_archivo, char* path_pokemon, char* nombre_pokemon);
void cerrar_archivo(t_config* config_archivo, char* path_pokemon, char* nombre_pokemon);
int index_pokemon(char* nombre);
bool archivo_abierto(t_config* config_archivo);

void crear_conexiones();
void connect_new_pokemon();
void connect_get_pokemon();
void connect_catch_pokemon();
int iniciar_cliente_gamecard(char* ip, char* puerto);

void socket_gameboy();
void socket_escucha(char*IP, char* Puerto);
void esperar_cliente(int servidor);
void serve_client(int* socket);
void process_request(int cod_op, int cliente_fd);

int minimo_entre (int nro1, int nro2);
int calcular_tamanio(int acc, char* linea);
bool comienza_con(char* posicion, char* linea);
bool existe_pokemon(char* path_pokemon);
#endif /* UTILS_H_ */
