#include "types.h"
#include "kpm_defines.h"
#include "lcd.h"
//#include "LCD_defines.h"
#include "delay.h"
#include<LPC21xx.h>
//u32 kpmLUT[4][4]={{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
u32 kpmLUT[4][4]={{'1','2','3','A'},
                  {'4','5','6','B'},
				  {'7','8','9','C'},
				  {'*','0','#','D'}};
void Init_KPM(void)
{
	//rows output&ground
	IODIR1|=15<<ROW0;
	//cols input&high, any by deafault
}
u32 ColScan(void)
{
	u32 status;
	status=(((IOPIN1>>COL0)&15)<15)?0:1;
	return status;
}
u32 RowCheck(void)
{
	u32 r;
	for(r=0;r<=3;r++)
	{
		//startingg with roww 0,upto row 3
		//ground only one row,keep   other rows high
		IOPIN1=((IOPIN1&~(15<<ROW0))|(~(1<<r)<<ROW0));
		//check if key was pressedd in that row
		if(ColScan()==0)
		{
			break;
		}
	}
	//re-initialize rows to their state
	IOCLR1=15<<ROW0;
	return r;
}
u32 ColCheck(void)
{
	u32 c;
	for(c=0;c<=3;c++)
	{
		//starting with col 0 upto col3
		//check one col at a time for low
		if(((IOPIN1>>(COL0+c))&1)==0)
		{
				break;
			}
	}
	return c;
}
u32 keyScan(void)
{
	u32 r,c,keyV;
	//initialize kpm
	//Init_KPM();
	//wait for any key press detection
	while(ColScan());
	//upto key press detection &key is in pressed state
	//identify row in which key was pressed
	r=RowCheck();
	//identify cd in which key pressed
	c=ColCheck();
	//extract key vaaluee from LUT
	keyV=kpmLUT[r][c];
	return keyV;
}
void Readnum(u32 *num)
{
	u32 key=0,i=0;
	*num=0;
	while(1)
	{
	  key=keyScan();
	  delay_ms(100);
	  if(key>='0'&&key<='9')
       {
	    *num=(*num*10)+(key-48);
       Write_CMD_LCD(0xc0+i);
       Write_DAT_LCD(key);
       /*delay_ms(50);
       Write_CMD_LCD(0xc0+i);
       Write_DAT_LCD('*');*/
        i++;
        while(ColScan()==0);
       }
       else if(key=='D')
       {
         while(ColScan()==0);
        break;
        }
	}
}
void password(u32 *num)
{
	u32 key=0,i=0;
	*num=0;
	Write_CMD_LCD(0xc0);
//	StrLCD("enter password");
	delay_s(1);
	while(1)
	{
	  key=keyScan();
	  delay_ms(100);
	  if(key>='0'&&key<='9')
      {
	   *num=(*num*10)+(key-48);
       Write_CMD_LCD(0xc0+i);
       Write_DAT_LCD(key);
       delay_ms(50);
       Write_CMD_LCD(0xc0+i);
       Write_DAT_LCD('*');
       i++;
       while(ColScan()==0);
       }
       else if(key=='D')
       {
         while(ColScan()==0);
        break;
        }
        else if(key=='B')
        {
            while (ColScan() == 0);

            if (i > 0)         
            {
                i--;            
                *num = *num / 10;  

                Write_CMD_LCD(0xC0 + i);
                Write_DAT_LCD(' ');      
                Write_CMD_LCD(0xC0 + i);
            }
        }
    }
} 

				 
