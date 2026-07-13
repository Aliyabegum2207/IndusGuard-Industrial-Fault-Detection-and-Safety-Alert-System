#include "i2c.h"
#include "delay.h"

/*---------------------------------------------------------------
 Function : i2c_eeprom_bytewrite()
 Purpose  : Writes a single byte into the specified EEPROM
            memory location using I2C protocol.
 Parameters:
    slaveAddr - EEPROM slave address
    wBufAddr  - EEPROM memory address
    dat       - Data byte to be written
----------------------------------------------------------------*/
void i2c_eeprom_bytewrite(u8 slaveAddr,u16 wBufAddr,u8 dat)
{
	/* Generate START condition */
	i2c_start();

	/* Send slave address with Write bit */
	i2c_write(slaveAddr<<1);

	/* Send EEPROM memory address (High byte) */
	i2c_write(wBufAddr>>8);

	/* Send EEPROM memory address (Low byte) */
	i2c_write(wBufAddr);

	/* Write data byte */
	i2c_write(dat);

	/* Generate STOP condition */
	i2c_stop();

	/* Wait for EEPROM internal write cycle */
	delay_ms(10);
}


/*---------------------------------------------------------------
 Function : i2c_eeprom_randomread()
 Purpose  : Reads a single byte from the specified EEPROM
            memory location.
 Parameters:
    slaveAddr - EEPROM slave address
    rBufAddr  - EEPROM memory address
 Returns :
    Data byte read from EEPROM
----------------------------------------------------------------*/
u8 i2c_eeprom_randomread(u8 slaveAddr,u16 rBufAddr)
{
	u8 dat;

	/* Generate START condition */
	i2c_start();

	/* Send slave address with Write bit */
	i2c_write(slaveAddr<<1);

	/* Send EEPROM memory address */
	i2c_write(rBufAddr>>8);
	i2c_write(rBufAddr);

	/* Generate Repeated START */
	i2c_restart();

	/* Send slave address with Read bit */
	i2c_write(slaveAddr<<1|1);

	/* Read one byte and send NACK */
	dat=i2c_nack();

	/* Generate STOP condition */
	i2c_stop();

	return dat;
}


/*---------------------------------------------------------------
 Function : i2c_eeprom_pagewrite()
 Purpose  : Writes multiple bytes into EEPROM starting from
            the specified memory address.
 Parameters:
    slaveAddr     - EEPROM slave address
    wBufStartAddr - Starting EEPROM address
    p             - Pointer to data buffer
    nBytes        - Number of bytes to write
----------------------------------------------------------------*/
void i2c_eeprom_pagewrite(u8 slaveAddr,u8 wBufStartAddr,s8 *p,u8 nBytes)
{
	u8 i;

	/* Generate START condition */
	i2c_start();

	/* Send slave address with Write bit */
	i2c_write(slaveAddr<<1);

	/* Send starting EEPROM address */
	i2c_write(wBufStartAddr>>8);
	i2c_write(wBufStartAddr);

	/* Write all bytes sequentially */
	for(i=0;i<nBytes;i++)
	{
	    i2c_write(p[i]);
	}

	/* Generate STOP condition */
	i2c_stop();

	/* Wait for EEPROM internal write cycle */
	delay_ms(10);
}


/*---------------------------------------------------------------
 Function : i2c_eeprom_seqread()
 Purpose  : Reads multiple bytes sequentially from EEPROM.
 Parameters:
    slaveAddr     - EEPROM slave address
    rBufStartAddr - Starting EEPROM address
    p             - Buffer to store received data
    nBytes        - Number of bytes to read
----------------------------------------------------------------*/
void i2c_eeprom_seqread(u8 slaveAddr,u8 rBufStartAddr,s8 *p,u8 nBytes)
{
	u8 i;

	/* Generate START condition */
	i2c_start();

	/* Send slave address with Write bit */
	i2c_write(slaveAddr<<1);

	/* Send starting EEPROM address */
	i2c_write(rBufStartAddr>>8);
	i2c_write(rBufStartAddr);

	/* Generate Repeated START */
	i2c_restart();

	/* Send slave address with Read bit */
	i2c_write(slaveAddr<<1|1);

	/* Read all bytes except the last one with ACK */
	for(i=0;i<nBytes-1;i++)
	{
		p[i]=i2c_mack();
	}

	/* Read last byte and send NACK */
	p[i]=i2c_nack();

	/* Generate STOP condition */
	i2c_stop();
}
