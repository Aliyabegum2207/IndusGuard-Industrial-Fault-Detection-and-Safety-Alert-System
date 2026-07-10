#include "i2c.h"
#include "i2c_eeprom.h"
#include "i2c_eeprom_defines.h"
#include "delay.h"
#include "lcd.h"
u8 t __attribute__((at(0x40000000)));
s8 rbuff[18] __attribute__((at(0x40000040)))="";
main()
{
    int i;
	s8 wbuff[]="ABCDEFGHIJKLMNOPQ";
	init_i2c();
	Init_LCD();
	StrLCD("I2C test");
	CmdLCD(0x01);
	i2c_eeprom_bytewrite(I2C_EEPROM_SA1,0x00,'a');
	delay_ms(2000);
	t=i2c_eeprom_randomread(I2C_EEPROM_SA1,0x00);
	CharLCD(t);
	delay_ms(2000);
	i2c_eeprom_pagewrite(I2C_EEPROM_SA1,0x1100,wbuff,17);
	delay_ms(1000);
	i2c_eeprom_seqread(I2C_EEPROM_SA1,0x1100,rbuff,17);
	CmdLCD(0xc0);
	rbuff[17] = '\0';
	for(i=0; wbuff[i]; i++)
		CharLCD(rbuff[i]);
		
	while(1);
}
