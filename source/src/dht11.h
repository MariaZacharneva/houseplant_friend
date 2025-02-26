/*
 * dht11.h
 *
 *  Created on: Jan 25, 2025
 *      Author: Masha
 */

#ifndef SRC_DHT11_H_
#define SRC_DHT11_H_

void DHT11_Init(void);

int DHT_ReadData(int *temperature, int *humidity);

#endif /* SRC_DHT11_H_ */
