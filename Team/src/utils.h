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
#include<commons/collections/node.h>
#include<commons/collections/list.h>
#include<commons/string.h>
<<<<<<< HEAD
#include<stdbool.h>
//Config
#define IP_BROKER "IP_BROKER"
#define PUERTO_BROKER "PUERTO_BROKER"



=======
#include<../CommonsMCLDG/utils.h>
>>>>>>> shared-library

typedef enum
{
	NEW = 1,
	READY = 2,
	EXEC = 3,
	BLOCK = 4,
	EXIT = 5,
}t_estado;

typedef struct
{
	uint32_t ID;
	uint32_t coordx; // coordenada x
	uint32_t coordy; //coordenada y
	t_list* pokemons; // lista string
	t_list* objetivos; // array string
	t_estado estado; //para saber si esta en ready o block
} t_entrenador;


typedef struct t_nodo
{
	struct t_nodo* sig;
	t_entrenador* data;
} t_nodo;

typedef struct t_nodo* t_lista;


<<<<<<< HEAD
void iniciarTeam(t_config** config, t_log** logger, t_list** entrenadores, t_list** objetivoGlobal);
t_log* iniciar_logger(t_config*);
t_config* leer_config(void);
void terminarTeam(int conexion, t_log* logger, t_config* config, t_list* entrenadores, t_list* objetivoGlobal); //falta conexion
void configurarEntrenadores(t_config* config, t_list* entrenadores);
=======
void iniciarTeam(void);
void configurarEntrenadores(t_config* config, t_lista* entrenadores);
>>>>>>> shared-library
t_entrenador* crearEntrenador(char* posiciones, char* pokemonEntrenadores, char* objetivos);
t_list* configurarPokemons(char** pokemons);
void agregarEntrenador(t_lista* entrenadores,t_nodo *nodo);
t_entrenador* cambiarEstado (t_entrenador* entrenador,t_estado nuevoEstado);
bool cambioEstadoValido(t_estado estadoViejo,t_estado nuevoEstado);
bool cumpleObjetivoGlobal(t_list* entrenadores);
bool esEstadoExit(t_entrenador* entrenador);
bool listasIguales(t_list* lista1, t_list* lista2);
bool criterioOrden(char* elem1, char* elem2);
bool cumpleObjetivoParticular (t_entrenador* entrenador);
void entrenadorDestroy(t_entrenador * entrenador);
bool tieneMenosElementos (t_list* listaChica, t_list* lista );
bool puedeAtraparPokemon(t_entrenador* entrenador);
void capturoPokemon(t_entrenador* entrenador, char* pokemon, t_list* objetivoGlobal);
void configurarObjetivoGlobal(t_list* entrenadores, t_list* objetivoGlobal);
void removerPokemon(char* pokemon, t_list* lista);

#endif /* UTILS_H_ */
