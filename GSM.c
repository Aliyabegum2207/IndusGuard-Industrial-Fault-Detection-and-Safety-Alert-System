#include <string.h>
#include "UART_INT.h"
#include "lcd.h"
#include "delay.h"

/* Global variables updated by UART interrupt */
extern char buff[300];          // Buffer to store GSM responses
extern unsigned char i;         // Buffer index
extern unsigned char r_flag;    // UART receive flag

/* GSM initialization AT commands */
const char at_cmds[][20] =
{
    "AT\r\n",                  // Check GSM communication
    "ATE0\r\n",                // Disable command echo
    "AT+CMGF=1\r\n",           // Select SMS Text Mode
    "AT+CNMI=2,1,0,0,0\r\n",   // Enable new message indication
    "AT+CMGD=1\r\n"            // Delete previously stored SMS
};

/*------------------------------------------------------------------
 Function : GSM_SMS_Init()
 Purpose  : Initializes the GSM module by sending required
            AT commands and verifying the "OK" response.
-------------------------------------------------------------------*/
void GSM_SMS_Init(void)
{
    int j;

    for(j = 0; j < 5; j++)
    {
        /* Display current AT command on LCD */
        Write_CMD_LCD(0x01);
        Write_CMD_LCD(0x80);
        Write_str_LCD((char *)at_cmds[j]);

        /* Send AT command to GSM module */
        UART0_Str((char *)at_cmds[j]);

        /* Clear receive buffer */
        i = 0;
        memset(buff, '\0', 300);

        /* Wait until response is received */
        while(i < 4);

        delay_ms(500);

        buff[i] = '\0';

        /* Display GSM response */
        Write_CMD_LCD(0x01);
        Write_CMD_LCD(0x80);
        Write_str_LCD(buff);

        /* Check whether GSM acknowledged the command */
        if(strstr(buff, "OK"))
        {
            Write_CMD_LCD(0xC0);
            Write_str_LCD("OK");
            delay_ms(1000);
        }
        else
        {
            Write_CMD_LCD(0xC0);
            Write_str_LCD("ERROR");
            delay_ms(1000);
            return;
        }
    }
}

/*------------------------------------------------------------------
 Function : send_sms()
 Purpose  : Sends an SMS to the specified mobile number using
            GSM AT commands.
 Parameters:
    number  -> Destination mobile number
    message -> SMS text
-------------------------------------------------------------------*/
void send_sms(char *number, char *message)
{
    /* Clear previous GSM response */
    Write_CMD_LCD(0x01);
    Write_str_LCD("Sending SMS....");
    delay_ms(1000);

    i = 0;
    r_flag = 0;
    memset(buff, 0, 300);

    /* Send AT+CMGS command */
    UART0_Str("AT+CMGS=");
    UART0_Tx('"');
    UART0_Str(number);
    UART0_Tx('"');
    UART0_Str("\r\n");

    delay_ms(500);

    /* Send SMS content */
    i = 0;
    UART0_Str(message);

    /* Ctrl+Z terminates the SMS */
    UART0_Tx(0x1A);

    /* Wait for GSM response */
    while(i < 3);

    buff[i] = '\0';

    /* Verify whether SMS was sent successfully */
    if(strstr(buff, "OK"))
    {
        Write_CMD_LCD(0xC0);
        Write_str_LCD("MSG SENT.....");
        delay_ms(1000);
        Write_CMD_LCD(0x01);
    }
    else
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("MSG FAIL....");
        delay_ms(1000);
        Write_CMD_LCD(0x01);
        return;
    }
}

/*------------------------------------------------------------------
 Function : readsms()
 Purpose  : Reads the latest SMS stored in GSM memory,
            extracts only the message content,
            and stores it in the user buffer.
 Parameters:
    s1 -> Buffer to store received SMS text
-------------------------------------------------------------------*/
void readsms(char *s1)
{
    int j = 0;
    char *p;
    char cnt = 0;

    Write_CMD_LCD(0x01);
    Write_str_LCD("reading sms...");

    i = 0;
    r_flag = 0;
    memset(buff, 0, 300);

    /* Read SMS stored at memory location 1 */
    UART0_Str("AT+CMGR=1\r\n");

    delay_ms(1000);

    /* Wait until GSM response is received */
    while(i < 4);

    buff[i] = '\0';

    if(strstr(buff, "OK"))
    {
        Write_CMD_LCD(0x01);
        Write_str_LCD("msg received...");
        delay_ms(1000);

        /* Locate beginning of SMS content */
        p = buff;
        p = strchr(p, 0x0A);

        while(p)
        {
            cnt++;

            if(cnt == 2)
                break;

            p++;
            p = strchr(p, 0x0A);
        }

        /* Copy only SMS text into user buffer */
        i = p - buff;
        i++;

        for(; buff[i] != 0x0D; i++)
        {
            s1[j++] = buff[i];
        }

        s1[j] = '\0';
    }
}
