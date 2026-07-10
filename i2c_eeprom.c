#include "i2c.h"
#include "delay.h"
void i2c_eeprom_bytewrite(u8 slaveAddr,u16 wBufAddr,u8 dat)
{
	//isue i2c start
	i2c_start();
	//slaveAddr+w,&tack ack
	i2c_write(slaveAddr<<1);
	//wBuffAddr,ack
	i2c_write(wBufAddr>>8);
	i2c_write(wBufAddr);
	//data,ack
	i2c_write(dat);
	//isuue i2c stop
	i2c_stop();
	//write cycle delay
	delay_ms(10);
}
u8 i2c_eeprom_randomread(u8 slaveAddr,u16 rBufAddr)
{
	u8 dat;
	//start condition
	i2c_start();
	//slaveAddr+w,&take ack
	i2c_write(slaveAddr<<1);
	//eBuffAddr,&take ack
	i2c_write(rBufAddr>>8);
	i2c_write(rBufAddr);
	//restart
	i2c_restart();
	//slaveAddr+r,&take ack
	i2c_write(slaveAddr<<1|1);
	//read byte&provide nack
	dat=i2c_nack();
	//stop condition
	i2c_stop();
	return dat;
}
void i2c_eeprom_pagewrite(u8 slaveAddr,u8 wBufStartAddr,s8 *p,u8 nBytes)
{
	u8 i;
	//issue start condition
	i2c_start();
	//slaveAddr+w,&take ack
	i2c_write(slaveAddr<<1);
	//wBufStartAddr,&take ack
	i2c_write(wBufStartAddr>>8);
	i2c_write(wBufStartAddr);
	for(i=0;i<nBytes;i++)
	{
		//wDATA,&take ack
	         i2c_write(p[i]);
	}
	//issue stop condtion
	i2c_stop();
	//delay for internal write cycle in eeprom
	delay_ms(10);
}
void i2c_eeprom_seqread(u8 slaveAddr,u8 rBufStartAddr,s8 *p,u8 nBytes)
{
	u8 i;
	//issue start condition
	i2c_start();
	//slaveaddr+w,&take 
	i2c_write(slaveAddr<<1);
	//rBufStartAddr,&take ack
	i2c_write(rBufStartAddr>>8);
	i2c_write(rBufStartAddr);
	//issue repeated start condition
	i2c_restart();
	//slaveAddr+r,&take ack
	i2c_write(slaveAddr<<1|1);
	for(i=0;i<nBytes-1;i++)
	{
		//read (n-1)bytes,for each,provide mack
		p[i]=i2c_mack();
	}
	//read nth (last)byte,&provide nack
	p[i]=i2c_nack();
	//issue stop condition
	i2c_stop();
}
