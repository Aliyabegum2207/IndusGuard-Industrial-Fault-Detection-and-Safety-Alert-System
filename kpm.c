#include "types.h"
#include "kpm_defines.h"
#include "lcd.h"
#include "delay.h"
#include<LPC21xx.h>

/*---------------------------------------------------------------
 Keypad Lookup Table
 Maps Row and Column combinations to corresponding keypad keys.
----------------------------------------------------------------*/
//u32 kpmLUT[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
u32 kpmLUT[4][4]={{'1','2','3','A'},
                  {'4','5','6','B'},
				  {'7','8','9','C'},
				  {'*','0','#','D'}};

/*---------------------------------------------------------------
 Function : Init_KPM()
 Purpose  : Initializes the 4x4 Matrix Keypad.
            Configures row pins as outputs while column pins
            remain as inputs.
----------------------------------------------------------------*/
void Init_KPM(void)
{
	/* Configure row pins as GPIO outputs */
	IODIR1|=15<<ROW0;

	/* Column pins remain configured as inputs */
}


/*---------------------------------------------------------------
 Function : ColScan()
 Purpose  : Detects whether any key is pressed by scanning
            the keypad columns.
 Returns :
    0 -> Key Pressed
    1 -> No Key Pressed
----------------------------------------------------------------*/
u32 ColScan(void)
{
	u32 status;

	/* Check column status */
	status=(((IOPIN1>>COL0)&15)<15)?0:1;

	return status;
}


/*---------------------------------------------------------------
 Function : RowCheck()
 Purpose  : Identifies the row in which the pressed key is
            located by grounding one row at a time.
 Returns :
    Row number (0 to 3)
----------------------------------------------------------------*/
u32 RowCheck(void)
{
	u32 r;

	for(r=0;r<=3;r++)
	{
		/* Drive one row LOW and keep others HIGH */
		IOPIN1=((IOPIN1&~(15<<ROW0))|(~(1<<r)<<ROW0));

		/* Check whether key is pressed in this row */
		if(ColScan()==0)
		{
			break;
		}
	}

	/* Restore all rows */
	IOCLR1=15<<ROW0;

	return r;
}


/*---------------------------------------------------------------
 Function : ColCheck()
 Purpose  : Determines the column number of the pressed key.
 Returns :
    Column number (0 to 3)
----------------------------------------------------------------*/
u32 ColCheck(void)
{
	u32 c;

	for(c=0;c<=3;c++)
	{
		/* Check each column for LOW state */
		if(((IOPIN1>>(COL0+c))&1)==0)
		{
			break;
		}
	}

	return c;
}


/*---------------------------------------------------------------
 Function : keyScan()
 Purpose  : Detects a key press and returns the corresponding
            keypad value using the lookup table.
 Returns :
    Pressed key value
----------------------------------------------------------------*/
u32 keyScan(void)
{
	u32 r,c,keyV;

	/* Wait until any key is pressed */
	while(ColScan());

	/* Identify row and column of pressed key */
	r=RowCheck();
	c=ColCheck();

	/* Retrieve corresponding key value */
	keyV=kpmLUT[r][c];

	return keyV;
}


/*---------------------------------------------------------------
 Function : Readnum()
 Purpose  : Reads a numeric value entered through the keypad.
            Displays entered digits on LCD and stores the
            resulting number.
 Parameter :
    num -> Pointer to store entered number
----------------------------------------------------------------*/
void Readnum(u32 *num)
{
	u32 key=0,i=0;

	*num=0;

	while(1)
	{
		/* Read key from keypad */
		key=keyScan();

		delay_ms(100);

		/* Accept only numeric keys */
		if(key>='0'&&key<='9')
		{
			/* Convert ASCII digit to integer */
			*num=(*num*10)+(key-48);

			/* Display digit on LCD */
			Write_CMD_LCD(0xc0+i);
			Write_DAT_LCD(key);

			i++;

			/* Wait until key is released */
			while(ColScan()==0);
		}

		/* 'D' key acts as Enter key */
		else if(key=='D')
		{
			while(ColScan()==0);

			break;
		}
	}
}


/*---------------------------------------------------------------
 Function : password()
 Purpose  : Reads a password from the keypad.
            Displays '*' instead of actual digits for security.
            Supports Backspace using 'B' key and Enter using
            'D' key.
 Parameter :
    num -> Pointer to store entered password
----------------------------------------------------------------*/
void password(u32 *num)
{
	u32 key=0,i=0;

	*num=0;

	Write_CMD_LCD(0xc0);

	delay_s(1);

	while(1)
	{
		/* Read key from keypad */
		key=keyScan();

		delay_ms(100);

		/* Accept only numeric keys */
		if(key>='0'&&key<='9')
		{
			/* Store entered digit */
			*num=(*num*10)+(key-48);

			/* Display entered digit */
			Write_CMD_LCD(0xc0+i);
			Write_DAT_LCD(key);

			/* Mask password character */
			delay_ms(50);

			Write_CMD_LCD(0xc0+i);
			Write_DAT_LCD('*');

			i++;

			while(ColScan()==0);
		}

		/* 'D' key confirms password entry */
		else if(key=='D')
		{
			while(ColScan()==0);

			break;
		}

		/* 'B' key performs Backspace operation */
		else if(key=='B')
		{
			while (ColScan() == 0);

			if (i > 0)
			{
				/* Remove last entered digit */
				i--;

				*num = *num / 10;

				/* Erase character from LCD */
				Write_CMD_LCD(0xC0 + i);
				Write_DAT_LCD(' ');

				Write_CMD_LCD(0xC0 + i);
			}
		}
	}
}
