#include<LPC21xx.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "lcd.h"
#include "kpm.h"
#include "delay.h"
#include "edit.h"
#include "i2c_defines.h"
#include "i2c_eeprom.h"
extern u32 depass;
extern u8 r_temp,r_humi,read_mobile[10];
u32 curpass;
u32 newpass,key;
u8 a[5];
u32 temp_sp;
u32 hum_sp;

void extract(char *p)
{
	char c;
	if(strstr(p,"8765"))
	{
		if(p[strlen(p)-1]=='#')
		{
			c=*(p+4);
			Write_CMD_LCD(0x01);
			Write_DAT_LCD(c);
			delay_ms(2000);
			switch(c)
			{
				case 'T':i2c_eeprom_bytewrite(I2C_EEPROM_SA,TEMP_ADDR,atoi(p+5));//defalut temp set point
						r_temp=i2c_eeprom_randomread(I2C_EEPROM_SA,TEMP_ADDR); //READING UPDATED TEMP SETPOINT
						Write_CMD_LCD(0x01);
						Write_str_LCD("TEMP UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;
				case 'H':i2c_eeprom_bytewrite(I2C_EEPROM_SA,HUM_ADDR,atoi(p+5));//defalut humi set point
						r_humi=i2c_eeprom_randomread(I2C_EEPROM_SA,HUM_ADDR); //RAEDING UPDATED HUMI SETPOINT		
						Write_CMD_LCD(0x01);
						Write_str_LCD("HUMI UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;
				case 'M':i2c_eeprom_pagewrite(I2C_EEPROM_SA,MOBILE_ADDR,p+5,10);
						i2c_eeprom_seqread(I2C_EEPROM_SA,MOBILE_ADDR,(s8 *)read_mobile,10);//READING UPDATED MOBILE NUMBER		
						Write_CMD_LCD(0x01);
						Write_str_LCD("MOBILE NUM UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;
			}
			
		}
	}
}

void myitoa(u32 num,u8 *p)
{
	int cnt=0,temp;
	temp=num;
	while(temp)
	{
		cnt++;
		temp/=10;
	}
	p[cnt--]='\0';
	while(num)
	{
		p[cnt--]=num%10+48;
		num/=10;
	}
}

void edit_password(void)
{
	Write_CMD_LCD(0x01);
	curpass=0;
	Write_str_LCD("enter old pass");
	delay_ms(500);
	password(&curpass);
	if(curpass==depass)
	{
	    curpass=0;
		Write_CMD_LCD(0x01);
		Write_str_LCD("enter new pass");
		password(&curpass);
		if(curpass!=depass)
		{
			newpass=curpass;
			Write_CMD_LCD(0x01);
			Write_str_LCD("cnfm new pass");
			password(&curpass);
			if(curpass==newpass)
			{
				myitoa(newpass,a);
				i2c_eeprom_pagewrite(I2C_EEPROM_SA,PASS_ADDR,(s8 *)a,4);
				Write_CMD_LCD(0x01);
				Write_str_LCD("password updated");

			}
			else
			{
				Write_CMD_LCD(0x01);
				Write_str_LCD("pass not same");
			}
			delay_s(2);
			Write_CMD_LCD(0x01);
		  
		  }
		  else
		  {
		  	Write_CMD_LCD(0x01);
			Write_str_LCD("   NEW & OLD   ");
			Write_CMD_LCD(0xc0);
			Write_str_LCD("  PASS ARE SAME ");
		  }
		  delay_s(2);
			Write_CMD_LCD(0x01);
		}
		else
		{
			Write_CMD_LCD(0x01);
			 Write_str_LCD("incorrect pass");
		}
}

void edit_menu(void)
{
	/*Write_CMD_LCD(0x01);
	Write_str_LCD("enter password");
      password(&curpass);
				if(curpass==depass)
				{ 
					Write_CMD_LCD(0x01);
					Write_str_LCD("validating");
					for(i=11;i<16;i++)
					{
						CharLCD('.');
						delay_ms(60);
				}
				Write_CMD_LCD(0x01);
				Write_str_LCD("valid password"); */
	Write_CMD_LCD(0x01); 
		Write_str_LCD("MENU");
		delay_ms(2000);
		Write_CMD_LCD(0x01);
	while(1)
	{
	 Write_CMD_LCD(0x80);
		Write_str_LCD("1.Edit SetPoint");
		Write_CMD_LCD(0xc0);
		Write_str_LCD("2.Edit password");
		Write_CMD_LCD(0x94);
		Write_str_LCD("3.Edit RTC");
		Write_CMD_LCD(0xD4);
		Write_str_LCD("4.Exit");
		key=keyScan();
		delay_ms(100);
		while(ColScan()==0);
		switch(key)
		{
			case '1':edit_setpoint();
			        break;
			case '2':edit_password();
			        break;
			case '3':edit_rtc();
			       break;
			case '4':Write_CMD_LCD(0x01);
		         Write_str_LCD("Exit menu");
				 delay_ms(1000);
				 Write_CMD_LCD(0x01);
			      return;
			default:Write_CMD_LCD(0x01);
			Write_str_LCD("invalid option");
			         break;
		}
		//Write_CMD_LCD(0x01);
		//Write_str_LCD("Exit menu");
	}
}

void edit_setpoint(void)
{
	  u32 key;
    u32 pass;
    u8 attempts = 0;
    while(attempts<3)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Enter Password");
        pass=0;
        password(&pass);

        if(pass==depass)
            break;
				attempts++;
        Write_CMD_LCD(0x01);
        Write_str_LCD("Wrong Password");
        delay_s(1);
    }

    if(attempts==3)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("System Locked");
        delay_s(5);
        Write_CMD_LCD(0x01);
        return;
    }
    Write_CMD_LCD(0x01);
    Write_str_LCD("Access Granted");
    delay_ms(800);
		IOPIN0|=(1<<5);
	  while(1)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("1.Temp SetPt");
        Write_CMD_LCD(0xC0);
        Write_str_LCD("2.Hum SetPt");
		Write_CMD_LCD(0x94);
		Write_str_LCD("3.Exit");
  			key = keyScan();
        while(ColScan()==0);
  			switch(key)
        {
            case '1':set_temperature();
                break;
            case '2':set_humidity();
                break;
             case '3':IOPIN0&=~(1<<5);   
			 // exit
                Write_CMD_LCD(0x01);
                Write_str_LCD("Exit");
                delay_ms(800);
                Write_CMD_LCD(0x01);
                return;
   						  default:
                Write_CMD_LCD(0x01);
                Write_str_LCD("Invalid Option");
                delay_ms(800);
        }
    }
}
void set_temperature(void)
{
    u32 temp;
	  Write_CMD_LCD(0x01);
    Write_str_LCD("Temp Set Point");
    Write_CMD_LCD(0xC0);
	  Readnum(&temp);
	  if(temp>80) //limit
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Invalid Temp");
        delay_s(1);
        return;
    }
		temp_sp=temp;
    i2c_eeprom_bytewrite(I2C_EEPROM_SA,TEMP_ADDR, temp_sp);
		Write_CMD_LCD(0x01);
    Write_str_LCD("Temp Updated");
    delay_s(1);
}
void set_humidity(void)
{
    u32 hum;
	  Write_CMD_LCD(0x01);
    Write_str_LCD("Humidity SetPt");
    Write_CMD_LCD(0xC0);
    //Write_str_LCD("Enter Value");
  	//Write_CMD_LCD(0x01);
    Readnum(&hum);
	  if(hum>100)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Invalid Hum");
        delay_s(1);
        return;
    }
    hum_sp = hum;
    i2c_eeprom_bytewrite(I2C_EEPROM_SA,HUM_ADDR,hum_sp);
	Write_CMD_LCD(0x01);
    Write_str_LCD("Hum Updated");
    delay_s(1);
}
	
void edit_rtc()
{
	u32 key;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Edit RTC Time");
	delay_ms(2000);
	Write_CMD_LCD(0x01);
	//Write_str_LCD("HH:MM:SS");
	while(1)
	{
		Write_CMD_LCD(0x80);
		Write_str_LCD("1.HOUR      2.MIN");
		Write_CMD_LCD(0xC0);
		Write_str_LCD("3.SEC       4.DAY");
		Write_CMD_LCD(0x94);
		Write_str_LCD("5.DATE      6.MONTH");
		Write_CMD_LCD(0xD4);
		Write_str_LCD("7.YEAR      8.EXIT");
		key=keyScan();
		delay_ms(100);
		while(ColScan()==0);
		switch(key)
		{
			case '1':edit_hour();
			         break;
			case '2':edit_minute();
			         break;
			case '3':edit_second();
			         break;
			case '4':edit_day();
			         break;
			case '5':edit_date();
			         break;
			case '6':edit_month();
			         break;
			case '7':edit_year();
			        break;
			case '8':Write_CMD_LCD(0x01);
			         Write_str_LCD("Exit Menu");
			         delay_s(1);
					 Write_CMD_LCD(0x01);
			         return;
			default:Write_CMD_LCD(0x01);
			Write_str_LCD("invalid option");
			         break;
		}
	}
}
void edit_hour()
{
	u32 hh;
	input:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter hour(0-23)");
	Write_CMD_LCD(0xc0);
	Readnum(&hh);
	if(hh>23)
	{
	 goto input;
	}
	HOUR=hh;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Hour updated");
	delay_ms(900);
}
void edit_minute()
{
	u32 mm;
	input1:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Minute(0-59)");
	Write_CMD_LCD(0xc0);
	Readnum(&mm);
	if(mm>59)
	{
	goto input1;
	}
	MIN=mm;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Minute updated");
	delay_ms(900);
}
void edit_second()
{
	u32 ss;
	input2:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter second(0-59)");
	Write_CMD_LCD(0xc0);
	Readnum(&ss);
	if(ss>59)
	{
	goto input2;
	}
	SEC=ss;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Second updated");
	delay_ms(900);
}
void edit_day()
{
	u32 day;
	input3:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Day(0-6)");
	Write_CMD_LCD(0xc0);
	Readnum(&day);
	if(day>6)
	{
	goto input3;
	}
	DOW=day;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Day updated");
	delay_ms(900);
}
void edit_date()
{
	u32 date;
	input4:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Date");
	Write_CMD_LCD(0xc0);
	Readnum(&date);
	if(date>30)
	{
	goto input4;
	}
	DOM=date;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Date updated");
	delay_ms(900);
}
void edit_month()
{
	u32 mm;
	input5:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Month(0-12)");
	Write_CMD_LCD(0xc0);
	Readnum(&mm);
	if(mm>11)
	{
	goto input5;
	}
	MONTH=mm;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Month updated");
}
void edit_year()
{
	u32 yy;
	input6:Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Year");
	Write_CMD_LCD(0xc0);
	Readnum(&yy);
	if(yy>4095)//based on DATA SHEET
	{
	goto input6;
	}
	YEAR=yy;
	Write_CMD_LCD(0x01);
	Write_str_LCD("Year updated");
	delay_ms(900);
}
