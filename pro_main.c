#include<LPC21xx.h>
#include<string.h>
#include"lcd.h"
#include"types.h"
#include"delay.h"
#include "kpm.h"
#include "UART_INT.h"
#include "i2c_defines.h"
#include "i2c_eeprom.h"
#include "i2c.h"
#include "uart0.h"
#include "edit.h"
#include "dht11.h"
#include "GSM.h"
void Enable_int(void);

extern u32 i_flag;
extern char buff[300];
extern unsigned char i;
u32 depass=1234;
u8 pass[5],rpass[5],r_temp,r_humi,read_mobile[10],rd[50];
extern unsigned char nm_flag;
unsigned char humidity_integer, humidity_decimal, temp_integer, temp_decimal, checksum;
main()
{
	Enable_int();
	LCD_Init();
	RTC_Init();
	IODIR0|=1<<5;
	IODIR0|=1<<22;
	init_i2c();
	InitUART0();
	Write_CMD_LCD(0x01);
	Init_KPM();
	//GSM_SMS_Init();
	//Write_str_LCD("Hello");
	delay_ms(100);
	Write_CMD_LCD(0x01);
	//GSM_INIT();
	GSM_SMS_Init();
	 myitoa(depass,pass);
	//Write_str_LCD("Hello");
	 i2c_eeprom_pagewrite(I2C_EEPROM_SA,PASS_ADDR,(s8 *)pass,4);
	 
	 i2c_eeprom_bytewrite(I2C_EEPROM_SA,TEMP_ADDR,50);//defalut temp set point
	 i2c_eeprom_bytewrite(I2C_EEPROM_SA,HUM_ADDR,50);//defalut humi set point
	i2c_eeprom_pagewrite(I2C_EEPROM_SA,MOBILE_ADDR,"9121040296",10);
	delay_s(3);
	while(1)
	{
		i_flag=0,nm_flag=0;
		i2c_eeprom_seqread(I2C_EEPROM_SA,PASS_ADDR,(s8 *)rpass,4);
		i2c_eeprom_seqread(I2C_EEPROM_SA,MOBILE_ADDR,(s8 *)read_mobile,10);
		r_temp=i2c_eeprom_randomread(I2C_EEPROM_SA,TEMP_ADDR);
		r_humi=i2c_eeprom_randomread(I2C_EEPROM_SA,HUM_ADDR);
	//Write_str_LCD("Hello");
		do
		{
			//DHT11 main
		dht11_request();
		dht11_response();
		humidity_integer = dht11_data();
		humidity_decimal = dht11_data();
		temp_integer = dht11_data();
		temp_decimal = dht11_data();
		checksum = dht11_data();
		if( (humidity_integer + humidity_decimal + temp_integer + temp_decimal) != checksum )
			Write_str_LCD("Checksum Error\r\n");
		else
		{
			Write_CMD_LCD(0x80);
			Write_str_LCD("H : ");
			Write_int_LCD(humidity_integer);
			Write_DAT_LCD('.');
			Write_int_LCD(humidity_decimal);
			Write_str_LCD(" % RH ");
			Write_CMD_LCD(0xC0);
			Write_str_LCD("T : ");
			Write_int_LCD(temp_integer);
			Write_DAT_LCD('.');
			Write_int_LCD(temp_decimal);
			Write_DAT_LCD(223);
			Write_str_LCD("C");
			Write_CMD_LCD(0x94); // LINE 3 POS 0
			Write_str_LCD("TSP: ");	// TEMPERATURE SETPOINT
			Write_int_LCD(r_temp);	// TEMPERATURE SETPOINT
			Write_CMD_LCD(0x94+9); // LINE 3 POS 9
			Write_str_LCD("HSP: "); // HUMIDITY SETPOINT
			Write_int_LCD(r_humi);	// HUMIDITY SETPOINT
			Write_CMD_LCD(0xD4);
			Write_str_LCD("Checksum : ");
			Write_int_LCD(checksum);
			delay_ms(3000);
			Write_CMD_LCD(0x01);
		}
		/*Write_CMD_LCD(0x94);
		Write_str_LCD("TSP: ");
		Write_int_LCD(r_temp);
		Write_CMD_LCD(0x94+9);
		Write_str_LCD("HSP: ");
		Write_int_LCD(r_temp);*/
		if(temp_integer>r_temp)
		{
			//buzzer();
			IOPIN0|=(1<<22);
			//SEND_SMS("WARNING! TEMPERATURE ALERT",read_mobile);
		}
		if(humidity_integer>r_humi)
		{
			//buzzer();
			//SEND_SMS("WARNING! HUMIDITY ALERT",read_mobile);
		}
		}while((i_flag==0)&&(nm_flag==0));
		if(i_flag==1)
		{
			i_flag=0;
			edit_menu();
		}
		else if(nm_flag==1)
		{
			nm_flag=0;
			memset(rd,'\0',50);
			readsms((char*)rd);
			extract((char*)rd);
			UART0_Str("AT+CMGD=1\r\n");
	i=0;memset(buff,'\0',300);
	while(i<4);
	delay_ms(500);
	buff[i] = '\0';
	delay_ms(2000);
	if(strstr(buff,"OK"))
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("OK");
		delay_ms(1000);		
	}
	else
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("ERROR");
		delay_ms(1000);		
	}
	Write_CMD_LCD(0x01);
		}

	}
}
