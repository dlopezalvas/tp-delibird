/*
 * Broker.h
 *
 *  Created on: 18 abr. 2020
 *      Author: MCLDG
 */

#ifndef BROKER_H_
#define BROKER_H_

#include "utils_broker.h"

#define PATH "/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/BROKER.config"

void iniciar_broker();
void terminar_broker(t_log*, t_config*);
void dump_cache (int n);
void ver_estado_cache_particiones();
void ver_estado_cache_buddy();
char* cola_segun_cod(op_code cod_op);
char* transformar_a_fecha(uint32_t nro_fecha);
void socket_mensajes();

#endif /* BROKER_H_ */
