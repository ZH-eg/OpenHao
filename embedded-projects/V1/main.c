#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "DHT11.h"

int main(void)
{
	uint8_t temp,humi;
	
	OLED_Init();
	DHT11_Init();
	Serial_Init();	
	OLED_ShowString(1,1,"T:");
	OLED_ShowString(2,1,"H:");
	Serial_Printf("DHT11 Systeam Ready\r\n");
	
	while (1)
	{
		if(DHT11_Read(&temp,&humi) == 0)
		{
			Serial_Printf("T:%d,H:%d\r\n",temp,humi);
			OLED_ShowNum(1,3,temp,3);
			OLED_ShowNum(2,3,humi,3);
		}
		else
		{
			OLED_ShowString(3,1,"Error");
			Serial_Printf("DHT11 Read Error\r\n");
		}
		Delay_ms(2000);
	}
}
