
#include "utils_broker.h"


void recibir_mensaje_queue(t_mensaje* mensaje){
	if(proceso_valido(mensaje -> proceso))
		printf("%s","El proceso no esta autorizado para suscribirse o mandar mensajes");

	if(queue_valido(mensaje -> queue))
		printf("%s","La queue no existe");

}

int proceso_valido(char* proceso){

	//tener en el config un array de procesos
	//fijarse si proceso se encuentra en ese array y devolver true

	return 0;
}

int queue_valido(char* queue){

	//tener en el config un array de procesos
	//fijarse si proceso se encuentra en ese array y devolver true

	return 0;
}

void suscribir_mensaje_queue(t_mensaje* mensaje){
	//loggear que el proceso <mensaje -> proceso> se suscribio a la cola <mensaje -> queue>

	//pushear mensaje a la cola correspondiente
}
