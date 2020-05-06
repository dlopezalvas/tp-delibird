
#include "utils_broker.h"

void recibir_mensaje_broker(void){
	//recibe un mensaje, tiene que deserializarlo y llamar a recibir_mensaje_queue
	//generar el id unico (Ver el tipo, quiza tenga que ser un uuid)
	//pasandole el tipo t_mensaje*
};

void recibir_mensaje_queue(t_mensaje* mensaje){
	if(proceso_valido(mensaje -> proceso))
		printf("%s","El proceso no esta autorizado para suscribirse o mandar mensajes");

	if(queue_valida(mensaje -> queue))
		printf("%s","La queue no existe");

	//suscribir_mensaje_queue
}

int proceso_valido(char* proceso){

	//tener en el config un array de procesos
	//fijarse si proceso se encuentra en ese array y devolver true

	return 0;
}

int queue_valida(char* queue){

	//tener en el config un array de procesos
	//fijarse si proceso se encuentra en ese array y devolver true

	return 0;
}

void suscribir_mensaje_queue(t_mensaje* mensaje){
	//loggear que el proceso <mensaje -> proceso> se suscribia a la cola <mensaje -> queue>

	//pushear mensaje a la cola correspondiente
}

void crear_queues(void){
	//Crea las colas de las queues traidas del config
}

void terminar_queues(void){
	//libera de la memoria todas las queues
}
