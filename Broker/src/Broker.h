/*
 * Broker.h
 *
 *  Created on: 18 abr. 2020
 *      Author: MCLDG
 */

#ifndef BROKER_H_
#define BROKER_H_

#include<stdio.h>
#include<stdlib.h>
#include<../CommonsMCLDG/utils.h>
#include "utils_broker.h"
#include<commons/log.h>
#include<commons/config.h>

#define PATH "/home/utnso/workspace/tp-2020-1c-MCLDG/Broker/BROKER.config"

void iniciar_broker(t_config**, t_log**);
void terminar_broker(t_log*, t_config*);

#endif /* BROKER_H_ */
