/******************************************************************************/
/*                                                                            */
/*  Projekt z predmetu IMP - Aplikace modulu Watchdog Timer (WDOG)            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*  Reseni vytvoril(a) a odevzdava: (Tomáš Smädo, xsmado00)                   */
/*                                                                            */
/******************************************************************************/

/** vstavana aplikacia blikania LEDky prevzana z democvicenia **/
/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/* Constants specifying delay loop duration */
#define	DELAY_T 200000

/* Mapping of LEDs and buttons to specific port pins: */
// Note: only D9, SW3 and SW5 are used in this sample app
#define LED_D9  0x20 // Port B, bit 5
#define LED_D10 0x10 // Port B, bit 4
#define LED_D11 0x8 // Port B, bit 3
#define LED_D12 0x4 // Port B, bit 2

#define BTN_SW2 0x400 // Port E, bit 10
#define BTN_SW3 0x1000 // Port E, bit 12
#define BTN_SW4 0x8000000 // Port E, bit 27
#define BTN_SW5 0x4000000 // Port E, bit 26
#define BTN_SW6 0x800 // Port E, bit 11

int pressed_up = 0, pressed_down = 0;
unsigned int compare = 0x100;

/* A delay function */
void delay(long long bound) {

  long long i;
  for(i=0;i<bound;i++);
}

/* Initialize the MCU - basic clock settings, turning the watchdog off */
void MCUInit(void)  {
    MCG_C4 |= ( MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(0x01) );
    SIM_CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(0x00);
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

void PortsInit(void)
{
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK;

    /* Set corresponding PTB pins (connected to LED's) for GPIO functionality */
    PORTB->PCR[5] = PORT_PCR_MUX(0x01); // D9
    PORTB->PCR[4] = PORT_PCR_MUX(0x01); // D10
    PORTB->PCR[3] = PORT_PCR_MUX(0x01); // D11
    PORTB->PCR[2] = PORT_PCR_MUX(0x01); // D12

    PORTE->PCR[10] = PORT_PCR_MUX(0x01); // SW2
    PORTE->PCR[12] = PORT_PCR_MUX(0x01); // SW3
    PORTE->PCR[27] = PORT_PCR_MUX(0x01); // SW4
    PORTE->PCR[26] = PORT_PCR_MUX(0x01); // SW5
    PORTE->PCR[11] = PORT_PCR_MUX(0x01); // SW6

    /* Change corresponding PTB port pins as outputs */
    PTB->PDDR = GPIO_PDDR_PDD( 0x3C );
    PTB->PDOR |= GPIO_PDOR_PDO( 0x3C); // turn all LEDs OFF
}

void LPTMR0_IRQHandler(void)
{
    // Set new compare value set by up/down buttons
    LPTMR0_CMR = compare; // !! the CMR reg. may only be changed while TCF == 1
    LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK; // writing 1 to TCF tclear the flag
    GPIOB_PDOR ^= LED_D9; // invert D9 state
}

void LPTMR0Init(int count)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;     // Turn OFF LPTMR to perform setup

    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(1)) ; // use the choice of clock

    LPTMR0_CMR = count;  // Set compare value

    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt (now)
                 | LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                );

    NVIC_EnableIRQ(WDOG_EWM_IRQn);
    NVIC_EnableIRQ(LPTMR0_IRQn);


    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   // Turn ON LPTMR0 and start counting
}

//vlastna aplikacia, demonstracia modulu WDOG
int main(void)
{

	//inicializacia mikrokontroleru
    MCUInit();
    PortsInit();
    LPTMR0Init(compare);

    //odomknutie WDOG
    WDOG_UNLOCK = 0xC520;
    WDOG_UNLOCK = 0xD928;

    //nastavenie modu WDOG
    //periodicky WDOG
    //WDOG_STCTRLH = 0x5; //0101
    //windowed WDOG
    WDOG_STCTRLH = 0xD; //1101

    //nastavenie velkosti periody, v ktorej je vyzadovany reset
    WDOG_TOVALH = 0;
    WDOG_TOVALL = 3000;

    //nastavenie velkosti okna, kedy nesmie nastat reset
    if (WDOG_STCTRLH == 0xD) {
    	WDOG_WINH = 0;
    	WDOG_WINL = 1000;
    }

    while (1) {
    	//stlacenie tlacitka sposobi refresh watchdogu - dioda neprestane blikat
    	if (!pressed_down && !(GPIOE_PDIR & BTN_SW3))
    	        {
    	            pressed_down = 1;
    	            //refresh WDOG
    	            WDOG_REFRESH = 0xA602;
    	            WDOG_REFRESH = 0xB480;
    	        }
    	        else
    	        {
    	        	if (GPIOE_PDIR & BTN_SW3)
    	        	{
    	        		pressed_down = 0;
    	        	}
    	        }
    	    }


    return 0;
}
