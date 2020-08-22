#include <msp430g2553.h>
#include <stdint.h>
#include "images.h"
#include "gamma.h"

#define redLED BIT0     // P1.0
#define greenLED BIT6   // P1.6
#define BUT1 BIT3       // P1.3

#define COL_CLK BIT5    // P2.5
#define COL_OE BIT4     // P1.4
#define COL_LE BIT5     // P1.5
#define COL_COPI BIT7   // P1.7

#define ROW_CLK BIT0    // P2.0
#define ROW_OE BIT1     // P2.1
#define ROW_LE BIT2     // P2.2
#define ROW_COPI BIT3   // P2.3

void col_OE_Disable();
void col_OE_Enable();
void col_CLK_Pulse();
void col_LE_Latch();

void row_OE_Disable();
void row_OE_Enable();
void newFrame();
void enableDisplay();
void row_CLK_Pulse();
void row_LE_Latch();

void goToRow(unsigned int);

int main(void)
{
    int brightnessStep = 65;
    unsigned int col, row, frame, brightness;
    volatile unsigned long j, k = 0;
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    BCSCTL1= CALBC1_16MHZ;
    DCOCTL= CALDCO_16MHZ;

    P1DIR |= (redLED | greenLED | COL_OE | COL_LE | COL_COPI);
    P1OUT &= ~ (redLED | greenLED | COL_LE | COL_COPI);
    P1OUT |= COL_OE;

    P2DIR |= (COL_CLK | ROW_CLK | ROW_OE | ROW_LE | ROW_COPI );
    P2OUT &= ~ (COL_CLK | ROW_CLK | ROW_LE | ROW_COPI );
    P2OUT |= ROW_OE;

    P1DIR &= ~BUT1;
    P1REN |= BUT1;
    P1OUT |= BUT1;


    enableDisplay();
    col_OE_Enable();
    for(;;){
        for(row = 0; row < 8; row++)
        {
            goToRow(row);
            for(brightness = 0; brightness < 255; brightness += brightnessStep)
            {
                for(col = 0; col < 8; col++)
                {
                    int temp = 0;
                    temp = gamma8[images4[(row-(k>>2))%16][col]];
                    if(k % 512 < 256)
                        temp = gamma8[anim[(row+(k>>2))%32][col]];
                    if(temp <= brightness)
                    {
                        P1OUT &= ~COL_COPI;
                    }
                    else
                    {
                        P1OUT |= COL_COPI;
                    }
                    col_CLK_Pulse();
                    for(j = 0; j < 20; j++);
                } // column

                col_LE_Latch();

            } // brightness

            for(j = 0; j < 300; j++);
        } // row
        k++;
    } // superloop
}

void enableDisplay()
{
    P2OUT &= ~ROW_OE;
}
void newFrame()
{
    int i;
    for(i = 0; i < 8; i++)
    {
        if(i == 5)
            P2OUT |= ROW_COPI;
        else
            P2OUT &= ~ ROW_COPI;
        row_CLK_Pulse();
    }
}

void goToRow(unsigned int  r)
{
    row_OE_Disable();
    int i;
    for(i = 0; i < 8; i++)
    {
        if(i == r)
            P2OUT |= ROW_COPI;
        else
            P2OUT &= ~ROW_COPI;
        row_CLK_Pulse();
    }
    row_LE_Latch();
    row_OE_Enable();
}

void row_OE_Disable() { P2OUT |= ROW_OE; }
void row_OE_Enable() { P2OUT &= ~ROW_OE; }

void row_CLK_Pulse()
{
    P2OUT |= ROW_CLK;
    P2OUT &= ~ROW_CLK;
}
void row_LE_Latch()
{
    P2OUT |= ROW_LE;
    P2OUT &= ~ROW_LE;
}

void col_OE_Disable() { P1OUT |= COL_OE; }
void col_OE_Enable() { P1OUT &= ~COL_OE; }
void col_CLK_Pulse()
{
    P2OUT |= COL_CLK;
    P2OUT &= ~COL_CLK;
}

void col_LE_Latch()
{
    P1OUT |= COL_LE;
    P1OUT &= ~COL_LE;
}
