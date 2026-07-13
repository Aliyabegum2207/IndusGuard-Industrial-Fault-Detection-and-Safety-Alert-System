#include "types.h"
#include "i2c_defines.h"

#include<LPC21xx.h>

/*---------------------------------------------------------------
 Function : init_i2c()
 Purpose  : Initializes the I2C peripheral.
            - Configures SCL and SDA pins
            - Sets I2C clock frequency
            - Enables I2C communication
----------------------------------------------------------------*/
void init_i2c(void)
{
	/* Configure P0.2 and P0.3 as SCL and SDA pins */
	PINSEL0|=SCL_0_2|SDA_0_3;

	/* Configure I2C clock speed */
	I2SCLH=0x75;
	I2SCLL=0x75;

	/* Enable I2C peripheral */
	I2CONSET=1<<I2EN_BIT;
}


/*---------------------------------------------------------------
 Function : i2c_start()
 Purpose  : Generates an I2C START condition to initiate
            communication with the slave device.
----------------------------------------------------------------*/
void i2c_start(void)
{
	/* Generate START condition */
	I2CONSET=1<<STA_BIT;

	/* Wait until START condition is transmitted */
	while(((I2CONSET>>SI_BIT)&1)==0);

	/* Clear START flag */
	I2CONCLR=1<<STAC_BIT;
}


/*---------------------------------------------------------------
 Function : i2c_restart()
 Purpose  : Generates an I2C Repeated START condition.
            Used when switching from write mode to read mode
            without releasing the bus.
----------------------------------------------------------------*/
void i2c_restart(void)
{
	/* Generate Repeated START condition */
	I2CONSET=1<<STA_BIT;

	/* Clear SI flag to continue communication */
	I2CONCLR=1<<SIC_BIT;

	/* Wait until START condition is transmitted */
	while(((I2CONSET>>SI_BIT)&1)==0);

	/* Clear START flag */
	I2CONCLR=1<<STAC_BIT;
}


/*---------------------------------------------------------------
 Function : i2c_stop()
 Purpose  : Generates an I2C STOP condition to terminate
            communication with the slave device.
----------------------------------------------------------------*/
void i2c_stop(void)
{
	/* Generate STOP condition */
	I2CONSET=1<<STO_BIT;

	/* Clear SI flag */
	I2CONCLR=1<<SIC_BIT;
}


/*---------------------------------------------------------------
 Function : i2c_write()
 Purpose  : Sends one byte of data over the I2C bus.
 Parameter :
    dat -> Data byte to be transmitted
----------------------------------------------------------------*/
void i2c_write(u8 dat)
{
	/* Load data into I2C data register */
	I2DAT=dat;

	/* Start data transmission */
	I2CONCLR=1<<SIC_BIT;

	/* Wait until transmission is complete */
	while(((I2CONSET>>SI_BIT)&1)==0);
}


/*---------------------------------------------------------------
 Function : i2c_nack()
 Purpose  : Receives one byte from the I2C slave and sends
            a NACK to indicate end of reception.
 Returns :
    Received data byte
----------------------------------------------------------------*/
u8 i2c_nack(void)
{
	/* Start data reception */
	I2CONCLR=1<<SIC_BIT;

	/* Wait until data is received */
	while(((I2CONSET>>SI_BIT)&1)==0);

	/* Return received data */
	return I2DAT;
}


/*---------------------------------------------------------------
 Function : i2c_mack()
 Purpose  : Receives one byte from the I2C slave and sends
            an ACK to request the next byte.
 Returns :
    Received data byte
----------------------------------------------------------------*/
u8 i2c_mack(void)
{
	/* Enable ACK generation */
	I2CONSET=1<<AA_BIT;

	/* Start data reception */
	I2CONCLR=1<<SIC_BIT;

	/* Wait until data is received */
	while(((I2CONSET>>SI_BIT)&1)==0);

	/* Clear ACK bit */
	I2CONCLR=1<<AA_BIT;

	/* Return received data */
	return I2DAT;
}
