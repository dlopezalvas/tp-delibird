#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

//Config
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"

typedef struct
{
	uint32_t ID;
	int coordenadas [2]; // tipo coordenada
	t_pokemon* pokemons; // lista string
	t_pokemon* objetivo; // lista string
	t_estado estado; //para saber si esta en ready o block
} t_entrenador;

typedef struct
{
	char* nombre;
	t_pokemon* siguiente;
} t_pokemon;

typedef enum
{
	NEW = 1,
	READY = 2,
	EXECT = 3,
	BLOCK = 4,
	EXIT = 5,
}t_estado;



void iniciarTeam(void);
t_log* iniciar_logger(t_config*);
t_config* leer_config(void);
void terminarTeam(int, t_log*, t_config*);


#endif /* UTILS_H_ */
