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
#include<../CommonsMCLDG/socket.h>
//#include<../CommonsMCLDG/serializacion.h>
#include <commons/collections/queue.h>
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


typedef struct t_pokemon
{
	char* especie;
	uint32_t coordx; // coordenada x
	uint32_t coordy; //coordenada y
} t_pokemon;


typedef struct t_entrenador
{
	uint32_t ID;
	uint32_t coordx; // coordenada x
	uint32_t coordy; //coordenada y
	t_list* pokemons; // lista string
	t_list* objetivos; // array string
	t_estado estado; //para saber si esta en ready o block
	t_pokemon* pokemonACapturar;
	void* intercambio; //siempre castear como t_intercambio
} t_entrenador;

typedef struct t_intercambio{
	t_entrenador** entrenador;
	char* pokemonARecibir;
	char* pokemonAEntregar;
}t_intercambio;




void iniciarTeam();
void terminarTeam(int conexion, pthread_t* hilo); //falta conexion
void configurarEntrenadores();
t_entrenador* crearEntrenador(char* posiciones, char* pokemonEntrenadores, char* objetivos);
t_list* configurarPokemons(char** pokemons);
void cambiarEstado (t_entrenador** entrenador,t_estado nuevoEstado);
bool cambioEstadoValido(t_estado estadoViejo,t_estado nuevoEstado);
bool cumpleObjetivoGlobal();
bool esEstadoExit(t_entrenador* entrenador);
bool listasIguales(t_list* lista1, t_list* lista2);
bool criterioOrden(char* elem1, char* elem2);
bool cumpleObjetivoParticular (t_entrenador* entrenador);
void entrenadorDestroy(t_entrenador * entrenador);
bool tieneMenosElementos (t_list* listaChica, t_list* lista );
bool puedeAtraparPokemon(t_entrenador* entrenador);
void capturoPokemon(t_entrenador** entrenador);
void configurarObjetivoGlobal();
void removerPokemon(char* pokemon, t_list* lista);
void* entrenadorMaster(void* entrenador);
uint32_t distancia(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
void moverEntrenador(t_entrenador** entrenador, uint32_t x, uint32_t y);
void intercambiarPokemon(t_entrenador** entrenador);
void atraparPokemon(t_entrenador* entrenador);
void planificarFIFO();
void* saludar();
bool menorDistancia(t_entrenador* elem1, t_entrenador* elem2, t_pokemon* pokemon);
void llenarColaReady();
void ejecutaEntrenadores(t_queue* ready);
bool estadoNewOBlock(t_entrenador* entrenador);
void appeared_pokemon(t_pokemon* pokemonNuevo);
void esperar_cliente(int servidor);
void serve_client(int* socket);
void socketEscucha(char*IP, char* Puerto);
void process_request(int cod_op, int cliente_fd);


#endif /* UTILS_H_ */
