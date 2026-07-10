#include<string.h>
#include "UART_INT.h"
#include "lcd.h"
#include "delay.h"

extern char buff[300];
extern unsigned char i;
extern unsigned char r_flag;

const char at_cmds[][20]={"AT\r\n","ATE0\r\n","AT+CMGF=1\r\n","AT+CNMI=2,1,0,0,0\r\n","AT+CMGD=1\r\n"};

void GSM_SMS_Init(void)
{
    int j;
	for(j=0;j<5;j++)
	{
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD((char *)at_cmds[j]);
	//delay_ms(1000);
	UART0_Str((char *)at_cmds[j]);
	i=0;memset(buff,'\0',300);
	while(i<4);
	delay_ms(500);
	buff[i] = '\0';
	Write_CMD_LCD(0x01);
	Write_CMD_LCD(0x80);
	Write_str_LCD(buff);
	//delay_ms(2000);
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
		return;
	}
	
	}
}
void send_sms(char *number,char *message)
{
/*removes old GSM response and start fresh reception*/
	
	Write_CMD_LCD(0x01);
	Write_str_LCD("Sending SMS....");
	delay_ms(1000);
	i=0;
	r_flag=0;
	memset(buff,0,300);
	/*Send CMGS command with mobile number */
    UART0_Str("AT+CMGS=");
	UART0_Tx('"');
    UART0_Str(number);
	UART0_Tx('"');
    UART0_Str("\r\n");
	delay_ms(500);
	i=0;
	UART0_Str(message);
	UART0_Tx(0x1A); //Ctrl+Z
	while(i<3);
	buff[i]='\0';
	if(strstr(buff,"OK"))
	{
		Write_CMD_LCD(0xC0);
		Write_str_LCD("MSG SENT.....");
		delay_ms(1000);		
		Write_CMD_LCD(0x01);
	}
	else
	{
		Write_CMD_LCD(0x01);
		Write_str_LCD("MSG FAIL....");
		delay_ms(1000);	
	    Write_CMD_LCD(0x01);		
		return;
	}
}

void readsms(char *s1)
{
	int j=0;
	char *p,cnt=0;
	Write_CMD_LCD(0x01);
	Write_str_LCD("reading sms...");
	i=0;
	r_flag=0;
	memset(buff,0,300);
	/*Send CMGS command with mobile number */
   UART0_Str("AT+CMGR=1\r\n");
	delay_ms(1000);
	while(i<4);
	buff[i]='\0';
	if(strstr(buff,"OK"))
	{
		Write_CMD_LCD(0x01);
	Write_str_LCD("msg received...");
		delay_ms(1000);
		p=buff;
		p=(strchr(p,0x0A));
		while(p)
		{
			cnt++;
			if(cnt==2)
				break;
			p++;
			p=(strchr(p,0x0A));
		}
		i=p-buff;
		i++;
		for(;buff[i]!=0x0D;i++)
		{
			s1[j++]=buff[i];
		}
		s1[j]='\0';
		/*Write_CMD_LCD(0x01);
			Write_str_LCD(s1);
			delay_ms(2000);*/
	}
}
	

