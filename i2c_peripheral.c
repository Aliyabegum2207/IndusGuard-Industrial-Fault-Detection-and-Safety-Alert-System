#include "types.h"
#include "i2c_defines.h"

#include<LPC21xx.h>
void init_i2c(void)
{
	//cfg p0.2,p0.3 for SCL,SDA functions using PINSEL0
	PINSEL0|=SCL_0_2|SDA_0_3;
	//cfg speed for i2c serial communication
	//I2SCLH=I2C_DIVIDER;
	//I2SCLL=I2C_DIVIDER;
	I2SCLH=0x75;
	I2SCLL=0x75;
	//I2C peripheral Enable for Communication
	I2CONSET=1<<I2EN_BIT;
}
void i2c_start(void)
{
	//start condition
	I2CONSET=1<<STA_BIT;
	//wait for start bit status
	while(((I2CONSET>>SI_BIT)&1)==0);
	//clear start condition
	I2CONCLR=1<<STAC_BIT;
}
void i2c_restart(void)
{
	//start condition
	I2CONSET=1<<STA_BIT;
	//clear sic bit initiate repeated start
	I2CONCLR=1<<SIC_BIT;
	//wait for start bit status
	while(((I2CONSET>>SI_BIT)&1)==0);
	//clear start condition
	I2CONCLR=1<<STAC_BIT;
}
void i2c_stop(void)
{
	//stop condition
	I2CONSET=1<<STO_BIT;
	//clear sic bit to initiate stop
	I2CONCLR=1<<SIC_BIT;
}
void i2c_write(u8 dat)
{
	//put data into I2DAT
	I2DAT=dat;
	//clear SIC_BIT initiates serialization out
	I2CONCLR=1<<SIC_BIT;
	//wait for SI bit status,end of serialization
	while(((I2CONSET>>SI_BIT)&1)==0);
}
u8 i2c_nack(void)
{
	//clear SIC_BIT initiates serialization in
	I2CONCLR=1<<SIC_BIT;
	//wait for SI bit status,end of serialization
	while(((I2CONSET>>SI_BIT)&1)==0);
	return I2DAT;
}
u8 i2c_mack(void)
{
	//cfg AA_BIT,to issue mack
	I2CONSET=1<<AA_BIT;
	//clr SIC_BIT initiates serialization in
	I2CONCLR=1<<SIC_BIT;
	//wait for SI bit status,end of serialization
	while(((I2CONSET>>SI_BIT)&1)==0);
	//clr AA_BIT
	I2CONCLR=1<<AA_BIT;
	return I2DAT;
}
