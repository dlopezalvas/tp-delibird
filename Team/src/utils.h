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
#include<stdbool.h>
#include<../CommonsMCLDG/utils.h>
#include<pthread.h>
#include<math.h>


#define PATH "/home/utnso/workspace/tp-2020-1c-MCLDG/Team/TEAM.config"

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


typedef struct t_pokemon
{
	char* especie;
	uint32_t coordx; // coordenada x
	uint32_t coordy; //coordenada y
} t_pokemon;


void iniciarTeam(t_config** config, t_log** logger);
void terminarTeam(int conexion, t_log* logger, t_config* config); //falta conexion
void configurarEntrenadores(t_config* config);
t_entrenador* crearEntrenador(char* posiciones, char* pokemonEntrenadores, char* objetivos);
t_list* configurarPokemons(char** pokemons);
t_entrenador* cambiarEstado (t_entrenador* entrenador,t_estado nuevoEstado);
bool cambioEstadoValido(t_estado estadoViejo,t_estado nuevoEstado);
bool cumpleObjetivoGlobal();
bool esEstadoExit(t_entrenador* entrenador);
bool listasIguales(t_list* lista1, t_list* lista2);
bool criterioOrden(char* elem1, char* elem2);
bool cumpleObjetivoParticular (t_entrenador* entrenador);
void entrenadorDestroy(t_entrenador * entrenador);
bool tieneMenosElementos (t_list* listaChica, t_list* lista );
bool puedeAtraparPokemon(t_entrenador* entrenador);
void capturoPokemon(t_entrenador* entrenador, char* pokemon);
void configurarObjetivoGlobal();
void removerPokemon(char* pokemon, t_list* lista);
void* entrenadorMaster();
uint32_t distancia(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
//void moverEntrenador(t_config* config, t_entrenador* entrenador, uint32_t x, uint32_t y);

#endif /* UTILS_H_ */
