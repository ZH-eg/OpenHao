#ifndef __DHT11_H
#define __DHT11_H

void DHT11_Init(void);
static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);
uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity);

#endif
