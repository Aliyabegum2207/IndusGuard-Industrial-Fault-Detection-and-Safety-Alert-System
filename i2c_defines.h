//define pin functions
#define PIN_FUNC2 1
#define SCL_0_2 0x00000010
#define SDA_0_3 0x00000040

//defines for I2C_SPEED Configurarion
#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define I2C_SPEED 100000
#define I2C_DIVIDER ((PCLK/I2C_SPEED)/2)

//bit defines for I2CONSET sfr
#define AA_BIT 2
#define SI_BIT 3
#define STO_BIT 4
#define STA_BIT 5
#define I2EN_BIT 6

//bit defines for I2CONCLR sfr
#define AAc_BIT 2
#define SIC_BIT 3
#define STAC_BIT 5
#define I2ENC_BIT 6

#define I2C_EEPROM_SA 0x50 //7 bit slave addr
#define TEMP_ADDR 0x0000
#define HUM_ADDR 0x0004
#define PASS_ADDR 0x0006
#define MOBILE_ADDR 0x0010


