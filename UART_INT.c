#include <LPC21xx.H>  /* LPC2148 Register Definitions */
#include <string.h>

#define UART_INT_ENABLE 1

/*---------------------------------------------------------------
 Global Variables
 buff     : Stores received UART data
 dummy    : Used to clear interrupt
 i        : Buffer index
 ch       : Received character
 r_flag   : Receive flag
 nm_flag  : Indicates arrival of a new SMS
----------------------------------------------------------------*/
char buff[300],dummy;
unsigned char i=0,ch,r_flag,nm_flag;


/*---------------------------------------------------------------
 Function : UART0_isr()
 Purpose  : UART0 Interrupt Service Routine.
            Handles UART Receive and Transmit interrupts.
            Received characters are stored in the buffer.
            Detects incoming SMS notification (+CMTI).
----------------------------------------------------------------*/
void UART0_isr(void) __irq
{
	/* Check whether Receive Interrupt occurred */
	if((U0IIR & 0x04))
	{
		/* Read received character */
		ch = U0RBR;

		/* Store character in receive buffer */
		if(i<300)
		{
			buff[i++] = ch;

			/* Check for New SMS indication */
			if((strstr(buff,"+CMTI")))
			{
				nm_flag=1;
			}
		}
	}
	else
	{
		/* Clear Transmit Interrupt */
		dummy=U0IIR;
	}

	/* Inform VIC that interrupt servicing is complete */
	VICVectAddr = 0;
}


/*---------------------------------------------------------------
 Function : InitUART0()
 Purpose  : Initializes UART0 for serial communication.
            Configures:
            - Baud Rate : 9600
            - Data Bits : 8
            - Stop Bits : 1
            - No Parity
            - UART Interrupts
----------------------------------------------------------------*/
void InitUART0 (void)
{
	/* Configure UART0 Tx and Rx pins */
	PINSEL0|= 0x00000005;

	/* 8-bit data, No parity, 1 Stop bit */
	U0LCR = 0x83;

	/* Set baud rate to 9600 */
	U0DLL = 97;

	/* Disable DLAB after baud rate configuration */
	U0LCR = 0x03;

#if UART_INT_ENABLE > 0

	/* Configure UART0 interrupt */
	VICIntSelect = 0x00000000;

	VICVectAddr0 = (unsigned)UART0_isr;

	VICVectCntl0 = 0x20 | 6;

	VICIntEnable = 1 << 6;

	/* Enable UART Receive and THRE interrupts */
	U0IER = 0x03;

#endif
}


/*---------------------------------------------------------------
 Function : UART0_Tx()
 Purpose  : Transmits a single character through UART0.
 Parameter:
    ch -> Character to be transmitted
----------------------------------------------------------------*/
void UART0_Tx(char ch)
{
	/* Wait until transmitter becomes empty */
	while (!(U0LSR & 0x20));

	/* Send character */
	U0THR = ch;
}


/*---------------------------------------------------------------
 Function : UART0_Rx()
 Purpose  : Receives a single character from UART0.
 Returns :
    Received character
----------------------------------------------------------------*/
char UART0_Rx(void)
{
	/* Wait until data is received */
	while (!(U0LSR & 0x01));

	return (U0RBR);
}


/*---------------------------------------------------------------
 Function : UART0_Str()
 Purpose  : Sends a complete string through UART0.
 Parameter:
    s -> Pointer to string
----------------------------------------------------------------*/
void UART0_Str(char *s)
{
	while(*s)
		UART0_Tx(*s++);
}


/*---------------------------------------------------------------
 Function : UART0_Int()
 Purpose  : Converts an unsigned integer into ASCII format
            and transmits it through UART0.
 Parameter:
    n -> Integer value to transmit
----------------------------------------------------------------*/
void UART0_Int(unsigned int n)
{
	unsigned char a[10]={0,0,0,0,0,0,0,0,0,0};
	int i=0;

	/* Handle zero separately */
	if(n==0)
	{
		UART0_Tx('0');
		return;
	}
	else
	{
		/* Convert integer into ASCII digits */
		while(n>0)
		{
			a[i++]=(n%10)+48;
			n=n/10;
		}

		/* Transmit digits in correct order */
		--i;

		for(;i>=0;i--)
		{
			UART0_Tx(a[i]);
		}
	}
}


/*---------------------------------------------------------------
 Function : UART0_Float()
 Purpose  : Transmits a floating-point number through UART0.
            Sends integer part followed by two decimal digits.
 Parameter:
    f -> Floating-point value
----------------------------------------------------------------*/
void UART0_Float(float f)
{
	int x;
	float temp;

	/* Extract integer part */
	x=f;

	UART0_Int(x);

	/* Send decimal point */
	UART0_Tx('.');

	/* Extract fractional part */
	temp=(f-x)*100;

	x=temp;

	/* Send fractional part */
	UART0_Int(x);
}
