#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

static void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

static uint8_t DHT11_ReadByte(void)
{
	uint8_t i,byte = 0;
	
	for(i=0;i<8;i++)
	{
		byte <<= 1;
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == RESET);
		Delay_us(40);
		
		if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET)
		{
			byte |= 0x01;
			while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET);

		}
	}
	return byte;
}

uint8_t DHT11_Read(uint8_t *temperature, uint8_t *humidity)
{
	uint8_t buf[5] = {0};
	uint8_t i;
	
	DHT11_Mode_Out_PP();
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	Delay_ms(20);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
	Delay_us(30);
	
	DHT11_Mode_IPU();
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == RESET)
	{
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == RESET);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == SET);
		
		for(i=0;i<5;i++)
		{
			buf[i]=DHT11_ReadByte();
		}
		if(buf[0]+buf[1]+buf[2]+buf[3] ==buf[4])
		{
			*humidity = buf[0];
            *temperature = buf[2];
			return 0;
		}
	}
	return 1;
}
