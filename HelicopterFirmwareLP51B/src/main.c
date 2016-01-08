#include <stdio.h>
#include <at89lp51rd2.h>

#define CLK 22118400L
#define BAUD 115200L
#define TIMER_2_RELOAD (0x10000L-(CLK/(32L*BAUD)))

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0=0;	P2M1=0;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
    setbaud_timer2(TIMER_2_RELOAD); // Initialize serial port using timer 2
    
    return 0;
}

void main (void)
{
    printf( "Hello, world you suck!\r\n" );
}
