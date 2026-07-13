#include<LPC21xx.h>

#include "delay.h"
#include "lcd.h"
#include "defines.h"

/* LCD Data Bus and Control Pin Definitions */
#define LCD_DAT 8
#define RS 17
#define RW 19
#define EN 18

/*---------------------------------------------------------------
 Function : LCD_Init()
 Purpose  : Initializes the 16x2 LCD in 8-bit mode.
            Configures LCD data and control pins, clears the
            display, and prepares it for displaying data.
----------------------------------------------------------------*/
void LCD_Init(void)
{
		/* Configure LCD data pins as output */
		WRITEBYTE(IODIR0,LCD_DAT,0xFF);

		/* Configure LCD control pins as output */
		WRITEBIT(IODIR0,RS,1);
		WRITEBIT(IODIR0,RW,1);
		WRITEBIT(IODIR0,EN,1);

		/* Wait for LCD power-up */
		delay_ms(16);

		/* LCD initialization sequence */
    Write_CMD_LCD(0x30);
    delay_us(4100);

		Write_CMD_LCD(0x30);
    delay_us(100);

    Write_CMD_LCD(0x30);

    /* Configure LCD: 8-bit, 2-line, 5x7 font */
    Write_CMD_LCD(0x38);

		/* Display ON, Cursor OFF */
		Write_CMD_LCD(0x0C);

		/* Clear LCD display */
		Write_CMD_LCD(0x01);

    /* Auto increment cursor position */
    Write_CMD_LCD(0x06);
}


/*---------------------------------------------------------------
 Function : Write_CMD_LCD()
 Purpose  : Sends a command to the LCD.
 Parameter:
    cmd -> LCD command byte
----------------------------------------------------------------*/
void Write_CMD_LCD(char cmd)
{
	/* RS = 0 indicates Command Register */
	WRITEBIT(IOCLR0,RS,1);

	Write_LCD(cmd);
}


/*---------------------------------------------------------------
 Function : Write_DAT_LCD()
 Purpose  : Sends character data to the LCD.
 Parameter:
    dat -> Character to be displayed
----------------------------------------------------------------*/
void Write_DAT_LCD(char dat)
{
	/* RS = 1 indicates Data Register */
	WRITEBIT(IOSET0,RS,1);

	Write_LCD(dat);
}


/*---------------------------------------------------------------
 Function : Write_LCD()
 Purpose  : Writes one byte to the LCD by generating the
            Enable pulse.
 Parameter:
    ch -> Command or Data byte
----------------------------------------------------------------*/
void Write_LCD(char ch)
{
	/* Select Write operation */
	WRITEBIT(IOCLR0,RW,1);

	/* Place data on LCD data bus */
	WRITEBYTE(IOPIN0,LCD_DAT,ch);

	/* Generate Enable pulse */
	WRITEBIT(IOSET0,EN,1);
	WRITEBIT(IOCLR0,EN,1);

	/* Wait for LCD processing */
	delay_ms(2);
}


/*---------------------------------------------------------------
 Function : Write_str_LCD()
 Purpose  : Displays a null-terminated string on the LCD.
 Parameter:
    p -> Pointer to the string
----------------------------------------------------------------*/
void Write_str_LCD(char *p)
{
	while(*p)
		Write_DAT_LCD(*p++);
}


/*---------------------------------------------------------------
 Function : Write_int_LCD()
 Purpose  : Displays a signed integer on the LCD.
            Supports both positive and negative numbers.
 Parameter:
    n -> Integer value to display
----------------------------------------------------------------*/
void Write_int_LCD(signed int n)
{
	char a[10],i=0;

	/* Check for negative number */
    if(n<0)
	{
		Write_DAT_LCD('-');
		n=-n;
	}

	/* Convert integer into ASCII digits */
	do
	{
		a[i++]=n%10+48;
		n=n/10;
	}while(n);

	/* Display digits in correct order */
	for(;i>0;i--)
		Write_DAT_LCD(a[i-1]);
}
