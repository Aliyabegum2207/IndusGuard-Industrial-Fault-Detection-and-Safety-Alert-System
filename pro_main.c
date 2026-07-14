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
#include "rtc.h"

/* External interrupt initialization function */
void Enable_int(void);

/* Global variables shared with other modules */
extern u32 i_flag;
extern char buff[300];
extern unsigned char i;

/* Default password */
u32 depass=1234;

/* SMS alert messages */
u8 msg1[50]="ALERT! TEMPERATURE EXCEED ";
u8 msg2[50]="ALERT! HUMIDITY EXCEED ";

/* EEPROM stored parameters */
u8 pass[5],rpass[5],r_temp,r_humi,read_mobile[10],rd[50];

/* Flag indicating new SMS received */
extern unsigned char nm_flag;

/* Variables for DHT11 sensor data */
unsigned char humidity_integer, humidity_decimal;
unsigned char temp_integer, temp_decimal, checksum;

/* RTC variables */
extern u32 hr,min,sec,date,year,day,month;


/*---------------------------------------------------------------
 Function : addtime_msg()
 Purpose  : Appends current RTC time to the SMS alert message.
 Example  : ALERT! TEMPERATURE EXCEED @10:30:45
----------------------------------------------------------------*/
void addtime_msg(char *p)
{
	int k;

	/* Read current RTC time */
	GetRTCTimeInfo(&hr,&min,&sec);

	k=strlen(p);

	/* Append time in HH:MM:SS format */
	p[k++]='@';
	p[k++]=hr/10+48;
	p[k++]=hr%10+48;
	p[k++]=':';
	p[k++]=min/10+48;
	p[k++]=min%10+48;
	p[k++]=':';
	p[k++]=sec/10+48;
	p[k++]=sec%10+48;
	p[k]='\0';
}


/*---------------------------------------------------------------
 Function : main()
 Purpose  : Main application for Industrial Fault Detection
            and Safety Alert System.
            - Initializes peripherals
            - Reads DHT11 sensor values
            - Displays data on LCD
            - Monitors threshold values
            - Sends SMS alerts
            - Handles menu through interrupt
            - Processes SMS commands
----------------------------------------------------------------*/
main()
{
	/* Initialize LCD */
	LCD_Init();

	delay_ms(2000);

	Write_str_LCD("Hello");

	/* Enable External Interrupt */
	Enable_int();

	/* Initialize RTC */
	RTC_Init();

	/* Configure output pins */
	IODIR0|=1<<5;
	IODIR0|=1<<22;

	/* Initialize I2C peripheral */
	init_i2c();

	/* Initialize UART */
	InitUART0();

	Write_CMD_LCD(0x01);

	/* Initialize Keypad */
	Init_KPM();

	Write_CMD_LCD(0x01);

	/* Initialize GSM module */
	GSM_SMS_Init();

	/* Convert default password into string */
	myitoa(depass,pass);

	/* Clear interrupt flags */
	i_flag=0;
	nm_flag=0;

	while(1)
	{
		/* Read password from EEPROM */
		i2c_eeprom_seqread(I2C_EEPROM_SA,PASS_ADDR,(s8 *)rpass,4);

		/* Read stored mobile number */
		i2c_eeprom_seqread(I2C_EEPROM_SA,MOBILE_ADDR,(s8 *)read_mobile,10);

		/* Read stored temperature threshold */
		r_temp=i2c_eeprom_randomread(I2C_EEPROM_SA,TEMP_ADDR);

		/* Read stored humidity threshold */
		r_humi=i2c_eeprom_randomread(I2C_EEPROM_SA,HUM_ADDR);

		Write_str_LCD("Hello");

		delay_s(5);

		Write_CMD_LCD(0x01);

		/* Continue monitoring until interrupt or SMS arrives */
		do
		{
			/* Read DHT11 sensor */
			dht11_request();
			dht11_response();

			humidity_integer = dht11_data();
			humidity_decimal = dht11_data();
			temp_integer = dht11_data();
			temp_decimal = dht11_data();
			checksum = dht11_data();

			/* Validate sensor data using checksum */
			if((humidity_integer + humidity_decimal + temp_integer + temp_decimal) != checksum)
			{
				Write_str_LCD("Checksum Error\r\n");
			}
			else
			{
				/* Display RTC time */
				time_disp();

				/* Display humidity */
				Write_CMD_LCD(0x94);
				Write_str_LCD("H : ");
				Write_int_LCD(humidity_integer);
				Write_DAT_LCD('.');
				Write_int_LCD(humidity_decimal);
				Write_str_LCD(" % RH ");

				/* Display temperature */
				Write_CMD_LCD(0xD4);
				Write_str_LCD("T : ");
				Write_int_LCD(temp_integer);
				Write_DAT_LCD('.');
				Write_int_LCD(temp_decimal);
				Write_DAT_LCD(223);
				Write_str_LCD("C");

				/* Display humidity setpoint */
				Write_CMD_LCD(0x94+14);
				Write_str_LCD("HSP:");
				Write_int_LCD(r_humi);

				/* Display temperature setpoint */
				Write_CMD_LCD(0xD4+14);
				Write_str_LCD("TSP:");
				Write_int_LCD(r_temp);

				delay_ms(1000);
			}

			/* Check temperature threshold */
			if(temp_integer>r_temp)
			{
				/* Turn ON alarm */
				IOPIN0|=(1<<22);

				/* Append timestamp */
				addtime_msg((char *)msg1);

				/* Send SMS alert */
				send_sms((char *)read_mobile,(char *)msg1);
			}

			/* Check humidity threshold */
			if(humidity_integer>r_humi)
			{
				/* Turn ON alarm */
				IOPIN0|=(1<<22);

				/* Append timestamp */
				addtime_msg((char *)msg2);

				/* Send SMS alert */
				send_sms((char *)read_mobile,(char *)msg2);
			}

		}while((i_flag==0)&&(nm_flag==0));

		/* External interrupt occurred */
		if(i_flag==1)
		{
			i_flag=0;

			/* Open configuration menu */
			edit_menu();
		}

		/* New SMS received */
		else if(nm_flag==1)
		{
			nm_flag=0;

			/* Clear receive buffer */
			memset(rd,'\0',50);

			/* Read received SMS */
			readsms((char*)rd);

			/* Display received SMS */
			Write_CMD_LCD(0x01);
			Write_str_LCD(rd);

			delay_ms(2000);

			Write_CMD_LCD(0x01);

			/* Process SMS command */
			extract((char*)rd);

			/* Delete processed SMS from GSM memory */
			UART0_Str("AT+CMGD=1\r\n");

			i=0;
			memset(buff,'\0',300);

			while(i<4);

			delay_ms(500);

			buff[i]='\0';

			delay_ms(2000);

			/* Verify SMS deletion */
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
