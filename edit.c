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

/*---------------------------------------------------------------
 Function : extract()
 Purpose  : Extracts and processes the received SMS command.
            After validating the password and message format,
            updates Temperature, Humidity or Mobile Number
            in EEPROM.
 Parameter :
    p -> Pointer to received SMS string
----------------------------------------------------------------*/
void extract(char *p)
{
	char c;

	/* Check whether the SMS contains the valid password */
	if(strstr(p,"8765"))
	{
		/* Verify that the SMS ends with '#' */
		if(p[strlen(p)-1]=='#')
		{
			/* Read command character (T/H/M) */
			c=*(p+4);

			Write_CMD_LCD(0x01);
			Write_DAT_LCD(c);
			delay_ms(2000);

			switch(c)
			{
				/* Update Temperature Set Point */
				case 'T':
						i2c_eeprom_bytewrite(I2C_EEPROM_SA,TEMP_ADDR,atoi(p+5));
						r_temp=i2c_eeprom_randomread(I2C_EEPROM_SA,TEMP_ADDR);

						Write_CMD_LCD(0x01);
						Write_str_LCD("TEMP UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;

				/* Update Humidity Set Point */
				case 'H':
						i2c_eeprom_bytewrite(I2C_EEPROM_SA,HUM_ADDR,atoi(p+5));
						r_humi=i2c_eeprom_randomread(I2C_EEPROM_SA,HUM_ADDR);

						Write_CMD_LCD(0x01);
						Write_str_LCD("HUMI UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;

				/* Update Mobile Number */
				case 'M':
						i2c_eeprom_pagewrite(I2C_EEPROM_SA,MOBILE_ADDR,p+5,10);

						i2c_eeprom_seqread(I2C_EEPROM_SA,
										   MOBILE_ADDR,
										   (s8 *)read_mobile,
										   10);

						Write_CMD_LCD(0x01);
						Write_str_LCD("MOBILE NUM UPDATED");
						delay_ms(1000);
						Write_CMD_LCD(0X01);
						break;
			}
		}
	}
}


/*---------------------------------------------------------------
 Function : myitoa()
 Purpose  : Converts an unsigned integer into a string.
            Used before storing numeric password into EEPROM.
 Parameters :
    num -> Integer value
    p   -> Output string buffer
----------------------------------------------------------------*/
void myitoa(u32 num,u8 *p)
{
	int cnt=0,temp;

	temp=num;

	/* Count number of digits */
	while(temp)
	{
		cnt++;
		temp/=10;
	}

	p[cnt--]='\0';

	/* Convert integer into ASCII characters */
	while(num)
	{
		p[cnt--]=num%10+48;
		num/=10;
	}
}


/*---------------------------------------------------------------
 Function : edit_password()
 Purpose  : Allows the user to change the system password
            after validating the current password.
            The updated password is stored in EEPROM.
----------------------------------------------------------------*/
void edit_password(void)
{
	Write_CMD_LCD(0x01);

	curpass=0;

	/* Ask user to enter existing password */
	Write_str_LCD("enter old pass");
	delay_ms(500);

	password(&curpass);

	/* Verify current password */
	if(curpass==depass)
	{
	    curpass=0;

		/* Enter new password */
		Write_CMD_LCD(0x01);
		Write_str_LCD("enter new pass");

		password(&curpass);

		/* New password should be different */
		if(curpass!=depass)
		{
			newpass=curpass;

			/* Confirm new password */
			Write_CMD_LCD(0x01);
			Write_str_LCD("cnfm new pass");

			password(&curpass);

			/* Check whether both passwords match */
			if(curpass==newpass)
			{
				myitoa(newpass,a);

				/* Store updated password in EEPROM */
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


/*---------------------------------------------------------------
 Function : edit_menu()
 Purpose  : Displays the main configuration menu.
            User can:
            1. Edit Set Point
            2. Edit Password
            3. Edit RTC
            4. Exit
----------------------------------------------------------------*/
void edit_menu(void)
{
	Write_CMD_LCD(0x01);
	Write_str_LCD("MENU");
	delay_ms(2000);
	Write_CMD_LCD(0x01);

	while(1)
	{
		/* Display menu options */
		Write_CMD_LCD(0x80);
		Write_str_LCD("1.Edit SetPoint");

		Write_CMD_LCD(0xc0);
		Write_str_LCD("2.Edit password");

		Write_CMD_LCD(0x94);
		Write_str_LCD("3.Edit RTC");

		Write_CMD_LCD(0xD4);
		Write_str_LCD("4.Exit");

		/* Read keypad input */
		key=keyScan();
		delay_ms(100);

		while(ColScan()==0);

		switch(key)
		{
			case '1':
				edit_setpoint();
				break;

			case '2':
				edit_password();
				break;

			case '3':
				edit_rtc();
				break;

			case '4':
				Write_CMD_LCD(0x01);
				Write_str_LCD("Exit menu");
				delay_ms(1000);
				Write_CMD_LCD(0x01);
				return;

			default:
				Write_CMD_LCD(0x01);
				Write_str_LCD("invalid option");
				break;
		}
	}
}
/*---------------------------------------------------------------
 Function : edit_setpoint()
 Purpose  : Allows the user to modify temperature and humidity
            set points after successful password verification.
            The system is locked temporarily after three
            consecutive incorrect password attempts.
----------------------------------------------------------------*/
void edit_setpoint(void)
{
	  u32 key;
    u32 pass;
    u8 attempts = 0;

    /* Allow maximum three password attempts */
    while(attempts<3)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Enter Password");

        pass=0;
        password(&pass);

        /* Verify entered password */
        if(pass==depass)
            break;

				attempts++;

        Write_CMD_LCD(0x01);
        Write_str_LCD("Wrong Password");
        delay_s(1);
    }

    /* Lock the system after three failed attempts */
    if(attempts==3)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("System Locked");
        delay_s(5);
        Write_CMD_LCD(0x01);
        return;
    }

    /* Password verified successfully */
    Write_CMD_LCD(0x01);
    Write_str_LCD("Access Granted");
    delay_ms(800);

		/* Turn ON status LED */
		IOPIN0|=(1<<5);

	  while(1)
    {
        /* Display Set Point menu */
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
            /* Modify Temperature Set Point */
            case '1':
                set_temperature();
                break;

            /* Modify Humidity Set Point */
            case '2':
                set_humidity();
                break;

            /* Exit configuration menu */
            case '3':
                IOPIN0&=~(1<<5);

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


/*---------------------------------------------------------------
 Function : set_temperature()
 Purpose  : Reads a new temperature threshold from keypad,
            validates the value and stores it in EEPROM.
----------------------------------------------------------------*/
void set_temperature(void)
{
    u32 temp;

	  Write_CMD_LCD(0x01);
    Write_str_LCD("Temp Set Point");

    Write_CMD_LCD(0xC0);

	  /* Read temperature value from keypad */
	  Readnum(&temp);

	  /* Validate temperature limit */
	  if(temp>80)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Invalid Temp");
        delay_s(1);
        return;
    }

		/* Update temperature set point */
		temp_sp=temp;

    /* Store updated value in EEPROM */
    i2c_eeprom_bytewrite(I2C_EEPROM_SA,TEMP_ADDR,temp_sp);

		Write_CMD_LCD(0x01);
    Write_str_LCD("Temp Updated");
    delay_s(1);
}


/*---------------------------------------------------------------
 Function : set_humidity()
 Purpose  : Reads a new humidity threshold from keypad,
            validates the value and stores it in EEPROM.
----------------------------------------------------------------*/
void set_humidity(void)
{
    u32 hum;

	  Write_CMD_LCD(0x01);
    Write_str_LCD("Humidity SetPt");

    Write_CMD_LCD(0xC0);

    /* Read humidity value from keypad */
    Readnum(&hum);

	  /* Validate humidity limit */
	  if(hum>100)
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("Invalid Hum");
        delay_s(1);
        return;
    }

    /* Update humidity set point */
    hum_sp = hum;

    /* Store updated value in EEPROM */
    i2c_eeprom_bytewrite(I2C_EEPROM_SA,HUM_ADDR,hum_sp);

	Write_CMD_LCD(0x01);
    Write_str_LCD("Hum Updated");
    delay_s(1);
}
/*---------------------------------------------------------------
 Function : edit_rtc()
 Purpose  : Displays the RTC configuration menu and allows the
            user to edit Hour, Minute, Second, Day, Date,
            Month and Year.
----------------------------------------------------------------*/
void edit_rtc()
{
	u32 key;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Edit RTC Time");
	delay_ms(2000);
	Write_CMD_LCD(0x01);

	while(1)
	{
		/* Display RTC editing menu */
		Write_CMD_LCD(0x80);
		Write_str_LCD("1.HOUR      2.MIN");

		Write_CMD_LCD(0xC0);
		Write_str_LCD("3.SEC       4.DAY");

		Write_CMD_LCD(0x94);
		Write_str_LCD("5.DATE      6.MONTH");

		Write_CMD_LCD(0xD4);
		Write_str_LCD("7.YEAR      8.EXIT");

		/* Read keypad input */
		key=keyScan();
		delay_ms(100);

		while(ColScan()==0);

		switch(key)
		{
			case '1':
				edit_hour();
				break;

			case '2':
				edit_minute();
				break;

			case '3':
				edit_second();
				break;

			case '4':
				edit_day();
				break;

			case '5':
				edit_date();
				break;

			case '6':
				edit_month();
				break;

			case '7':
				edit_year();
				break;

			/* Exit RTC menu */
			case '8':
				Write_CMD_LCD(0x01);
				Write_str_LCD("Exit Menu");
				delay_s(1);
				Write_CMD_LCD(0x01);
				return;

			default:
				Write_CMD_LCD(0x01);
				Write_str_LCD("invalid option");
				break;
		}
	}
}


/*---------------------------------------------------------------
 Function : edit_hour()
 Purpose  : Reads and updates RTC Hour register.
            Accepts values from 0 to 23.
----------------------------------------------------------------*/
void edit_hour()
{
	u32 hh;

input:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter hour(0-23)");
	Write_CMD_LCD(0xc0);

	/* Read hour value */
	Readnum(&hh);

	/* Validate input */
	if(hh>23)
	{
		goto input;
	}

	/* Update RTC hour register */
	HOUR=hh;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Hour updated");
	delay_ms(900);
}


/*---------------------------------------------------------------
 Function : edit_minute()
 Purpose  : Reads and updates RTC Minute register.
            Accepts values from 0 to 59.
----------------------------------------------------------------*/
void edit_minute()
{
	u32 mm;

input1:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Minute(0-59)");
	Write_CMD_LCD(0xc0);

	Readnum(&mm);

	if(mm>59)
	{
		goto input1;
	}

	/* Update RTC minute register */
	MIN=mm;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Minute updated");
	delay_ms(900);
}


/*---------------------------------------------------------------
 Function : edit_second()
 Purpose  : Reads and updates RTC Second register.
            Accepts values from 0 to 59.
----------------------------------------------------------------*/
void edit_second()
{
	u32 ss;

input2:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter second(0-59)");
	Write_CMD_LCD(0xc0);

	Readnum(&ss);

	if(ss>59)
	{
		goto input2;
	}

	/* Update RTC second register */
	SEC=ss;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Second updated");
	delay_ms(900);
}


/*---------------------------------------------------------------
 Function : edit_day()
 Purpose  : Reads and updates RTC Day register.
            Accepts values from 0 to 6.
----------------------------------------------------------------*/
void edit_day()
{
	u32 day;

input3:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Day(0-6)");
	Write_CMD_LCD(0xc0);

	Readnum(&day);

	if(day>6)
	{
		goto input3;
	}

	/* Update RTC day register */
	DOW=day;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Day updated");
	delay_ms(900);
}


/*---------------------------------------------------------------
 Function : edit_date()
 Purpose  : Reads and updates RTC Date register.
            Accepts valid date values.
----------------------------------------------------------------*/
void edit_date()
{
	u32 date;

input4:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Date");
	Write_CMD_LCD(0xc0);

	Readnum(&date);

	if(date>30)
	{
		goto input4;
	}

	/* Update RTC date register */
	DOM=date;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Date updated");
	delay_ms(900);
}


/*---------------------------------------------------------------
 Function : edit_month()
 Purpose  : Reads and updates RTC Month register.
            Accepts month values from 1 to 12.
----------------------------------------------------------------*/
void edit_month()
{
	u32 mm;

input5:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Month(0-12)");
	Write_CMD_LCD(0xc0);

	Readnum(&mm);

	if(mm>11)
	{
		goto input5;
	}

	/* Update RTC month register */
	MONTH=mm;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Month updated");
}


/*---------------------------------------------------------------
 Function : edit_year()
 Purpose  : Reads and updates RTC Year register.
            Maximum value is limited according to the LPC2148
            RTC register size.
----------------------------------------------------------------*/
void edit_year()
{
	u32 yy;

input6:
	Write_CMD_LCD(0x01);
	Write_str_LCD("Enter Year");
	Write_CMD_LCD(0xc0);

	Readnum(&yy);

	/* Validate year value */
	if(yy>4095)      // Based on LPC2148 datasheet
	{
		goto input6;
	}

	/* Update RTC year register */
	YEAR=yy;

	Write_CMD_LCD(0x01);
	Write_str_LCD("Year updated");
	delay_ms(900);
}
