/*---------------------------------------------------------------
 Function : delay_us()
 Purpose  : Generates an approximate delay in microseconds
            using a software busy-wait loop.
 Parameter:
    dlyUS - Delay duration in microseconds
----------------------------------------------------------------*/
void delay_us(unsigned int dlyUS)
{
	dlyUS*=12;          // Convert microseconds into loop count
	while(dlyUS--);     // Wait until counter reaches zero
}

/*---------------------------------------------------------------
 Function : delay_ms()
 Purpose  : Generates an approximate delay in milliseconds
            using a software busy-wait loop.
 Parameter:
    dlyMS - Delay duration in milliseconds
----------------------------------------------------------------*/
void delay_ms(unsigned int dlyMS)
{
	dlyMS*=12000;       // Convert milliseconds into loop count
	while(dlyMS--);     // Wait until counter reaches zero
}

/*---------------------------------------------------------------
 Function : delay_s()
 Purpose  : Generates an approximate delay in seconds
            using a software busy-wait loop.
 Parameter:
    dlyS - Delay duration in seconds
----------------------------------------------------------------*/
void delay_s(unsigned int dlyS)
{
	dlyS*=12000000;     // Convert seconds into loop count
	while(dlyS--);      // Wait until counter reaches zero
}
