#include <LPC21xx.h>
#include "types.h"

/* External Interrupt (EINT0) Configuration */
#define EINTO_VIC_CHNO 14
#define EINTO_STATUS_LED 21
#define EINTO_PIN_0_1 1

/* External Interrupt Service Routine Prototype */
void einto_isr(void) __irq;

/* Flag used to indicate that external interrupt has occurred */
u32 i_flag=0;

/*---------------------------------------------------------------
 Function : Enable_int()
 Purpose  : Configures External Interrupt 0 (EINT0).
            - Configures status LED as output
            - Configures P0.1 as EINT0 input
            - Enables EINT0 interrupt through VIC
            - Configures interrupt as Edge Triggered
----------------------------------------------------------------*/
void Enable_int(void)
{
	/* Configure status LED pin as GPIO output */
	IODIR0|=1<<EINTO_STATUS_LED;

	/* Configure P0.1 pin for EINT0 function */
	PINSEL1|=0x15400001;

	/* Enable EINT0 interrupt in VIC */
	VICIntEnable=1<<EINTO_VIC_CHNO;

	/* Configure EINT0 as Vectored IRQ with high priority */
	VICVectCntl1=(1<<5)|EINTO_VIC_CHNO;

	/* Load ISR address into VIC vector table */
	VICVectAddr1=(u32)einto_isr;

	/* Configure EINT0 as Edge Triggered */
	EXTMODE=1<<0;

	/* Falling edge triggering is default for EINT0 */
}


/*---------------------------------------------------------------
 Function : einto_isr()
 Purpose  : Interrupt Service Routine for External Interrupt 0.
            - Toggles status LED
            - Sets interrupt flag
            - Clears interrupt status
----------------------------------------------------------------*/
void einto_isr(void) __irq
{
	/* Toggle status LED whenever interrupt occurs */
	IOPIN0^=1<<EINTO_STATUS_LED;

	/* Set interrupt flag for main application */
	i_flag=1;

	/* Clear EINT0 interrupt flag */
	EXTINT=1<<0;

	/* Notify VIC that interrupt servicing is complete */
	VICVectAddr=0;
}
