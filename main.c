#include <msp430.h>
#include "font.h"

#define TAxCCR_05Hz 0xffff /* timer upper bound count value */
#define BUTTON_DELAY 0x0300

void writeCommand(unsigned char *sCmd, unsigned char i);
void writeData(unsigned char *sData, unsigned char i);
void setPosition(unsigned char page, unsigned char col);
void printNumber(int num);
void printSymbol(int index, unsigned char page, unsigned int col);

void writeCommand(unsigned char *sCmd, unsigned char i) {
    // Store current GIE state
    unsigned int gie = __get_SR_register() & GIE;
    // Make this operation atomic
    __disable_interrupt();
    // CS Low
    P7OUT &= ~BIT4;
    // CD Low
    P5OUT &= ~BIT6;
    while (i){
        // USCI_B1 TX buffer ready?
        while (!(UCB1IFG & UCTXIFG)) ;
        // Transmit data
        UCB1TXBUF = *sCmd;
        // Increment the pointer on the array
        sCmd++;
        // Decrement the Byte counter
        i--;
    }

    // Wait for all TX/RX to finish
    while (UCB1STAT & UCBUSY) ;
    // Dummy read to empty RX buffer and clear any overrun conditions
    UCB1RXBUF;
    // CS High
    P7OUT |= BIT4;
    // Restore original GIE state
    __bis_SR_register(gie);
}

void writeData(unsigned char *sData, unsigned char i) {
    // Store current GIE state
    unsigned int gie = __get_SR_register() & GIE;
    // Make this operation atomic
    __disable_interrupt();
      // CS Low
      P7OUT &= ~BIT4;
      //CD High
      P5OUT |= BIT6;

      while (i){
          // USCI_B1 TX buffer ready?
          while (!(UCB1IFG & UCTXIFG)) ;
          // Transmit data and increment pointer
          UCB1TXBUF = *sData++;
          // Decrement the Byte counter
          i--;
      }

      // Wait for all TX/RX to finish
      while (UCB1STAT & UCBUSY) ;
      // Dummy read to empty RX buffer and clear any overrun conditions
      UCB1RXBUF;
      // CS High
      P7OUT |= BIT4;

    // Restore original GIE state
    __bis_SR_register(gie);
}

#define SET_COLUMN_ADDRESS_MSB		0x10
#define SET_COLUMN_ADDRESS_LSB		0x00
#define SET_PAGE_ADDRESS			0xB0
void setPosition(unsigned char page, unsigned char col){
	unsigned char cmd[3] = {SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB, SET_PAGE_ADDRESS};
	cmd[0] = (cmd[0] & (~0x0f)) | ((col >> 4) & (0x0f));
	cmd[1] = (cmd[1] & (~0x0f)) | (col & 0x0f);
	cmd[2] = (cmd[2] & (~0x0f)) | (page & 0x0f);
	writeCommand(cmd, 3);
}

void printNumber(int num){
	unsigned char current_page = 0;
	unsigned char current_col = 0;

	if (num < 0) {
		num = -num;
		printSymbol(11, current_page, current_col);
	} else {
		printSymbol(10, current_page, current_col);
	}
	current_col += 8;

	unsigned char indices[6] = {13, 12, 12, 12, 12, 12};
	int current_pos = sizeof(indices);
	do {
		indices[--current_pos] = num % 10;
		num /= 10;
	} while (num > 0);

	int i;
	for (i = current_pos; i < current_pos + sizeof(indices); ++i){
		printSymbol(indices[i % sizeof(indices)], current_page, current_col);
		current_col += 8;
	}
}

void printSymbol(int index, unsigned char page, unsigned int col){
	setPosition(page+1, col);
	writeData(_font[index], 6);
	setPosition(page, col);
	writeData(_font[index] + 6, 6);
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    __bis_SR_register(GIE);

	UCSCTL3 = (UCSCTL3 & (~0x070)) | SELREF__XT1CLK;
	UCSCTL3 = (UCSCTL3 & (~0x07)) | FLLREFDIV__2;
	UCSCTL2 = (UCSCTL2 & (~0x0cff)) | ((8 - 1) & (0x0cff)); // FLLN multiplier
	UCSCTL2 = (UCSCTL2 & (~0x07000)) | FLLD__8; // FLLD divider
	UCSCTL1 = (UCSCTL1 & (~0x070)) | DCORSEL_1;

	UCSCTL4 = (UCSCTL4 & (~0x070)) | SELS__DCOCLKDIV;
	UCSCTL5 = (UCSCTL5 & (~0x070)) | DIVS__1;

	TA2CTL = (TA2CTL & (~0x0300)) | TASSEL__SMCLK;
	TA2CTL = (TA2CTL & (~0x030)) | MC__CONTINOUS;
	TA2CTL = (TA2CTL & (~0x0c0)) | ID__4;
	TA2CTL |= TACLR;
	TA2CCTL1 = (TA2CCTL1 & (~0x0100)) & ~CAP;
	TA2CCTL2 = (TA2CCTL2 & (~0x0100)) & ~CAP;
	TA2CCTL1 = (TA2CCTL1 & (~0x010)) & ~CCIE;
	TA2CCTL2 = (TA2CCTL2 & (~0x010)) & ~CCIE;

	UCB1IFG = (UCB1IFG & (~0x03)) | ((~UCTXIFG & (0x02)) | (~UCRXIFG & (0x01)));
	UCB1IE = (UCB1IE & (~0x03)) | UCTXIE | UCRXIE;


	// LCD and UART initialization
	// Chip select and screen backlight
	P7SEL &= ~(BIT4 | BIT6);
	P7DIR |= (BIT4 | BIT6);

	// Reset and Command/Data
	P5SEL &= ~(BIT6 | BIT7);
	P5DIR |= (BIT6 | BIT7);

	// Option select SIMO and select CLK
	P4SEL |= (BIT1 | BIT3);
	P4DIR |= (BIT1 | BIT3);

	// Port initialization for LCD operation
	// CS is active low
	P7OUT &= ~BIT4;
	// Reset is active low
	P5OUT &= BIT7;
	// Reset is active low
	P5OUT |= BIT7;
	// Disable screen backlight
	P7OUT &= ~BIT6;
//	P7OUT |= BIT6;

	P7OUT |= BIT4;

	// Initialize USCI_B1 for SPI Master operation
	// Put state machine in reset
	UCB1CTL1 |= UCSWRST;
	//3-pin, 8-bit SPI master
	UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
	// Clock phase - data captured first edge, change second edge
	// MSB
	// Use SMCLK, keep RESET
	UCB1CTL1 = UCSSEL_2 + UCSWRST;
	UCB1BR0 = 0x02;
	UCB1BR1 = 0;
	// Release USCI state machine
	UCB1CTL1 &= ~UCSWRST;
	UCB1IFG &= ~UCRXIFG;


#define SET_SCROLL_LINE		0x40
#define SET_MX				0xA0
#define SET_MY				0xC0
#define SET_ALL_PIXEL_ON	0xA4
#define SET_INVERSE_DISPLAY		0xA6
#define SET_PM_MSB			0x81
#define SET_PM_LSB			0x00
#define SET_POWER_CONTROL	0x28
#define SET_PC				0x20
#define SET_BIAS_RATIO		0xA2
#define SET_ADV_CONTROL_MSB	0xFA
#define SET_ADV_CONTROL_LSB	0x10
#define SET_DISPLAY_ENABLE	0xAE
#define SYSTEM_RESET		0xE2

	unsigned char cmd[] = {
	    SET_SCROLL_LINE,		// 0
	    SET_MX,					// 1
	    SET_MY,					// 2
	    SET_ALL_PIXEL_ON,		// 3
	    SET_INVERSE_DISPLAY,	// 4
	    SET_PM_MSB,				// 5
	    SET_PM_LSB,				// 6
	    SET_POWER_CONTROL,		// 7
	    SET_PC,					// 8
	    SET_BIAS_RATIO,			// 9
	    SET_ADV_CONTROL_MSB,	// 10
	    SET_ADV_CONTROL_LSB,	// 11
	    SET_DISPLAY_ENABLE		// 12
	};

	cmd[1] = (cmd[1] & (~0x01)) | (BIT0 & 0x01);
	cmd[2] = (cmd[2] & (~0x08)) | (BIT3 & 0x08);
	cmd[3] = (cmd[3] & (~0x01)) | BIT0;
	cmd[6] = (cmd[6] & (~0x03f)) | (0x030 & (0x03f));
	cmd[7] = (cmd[7] & (~0x07)) | ((BIT0 | BIT1 | BIT2) & (0x07));
	cmd[8] = (cmd[8] & (~0x07)) | (0x4 & (0x07));
	cmd[11] = (cmd[11] & (~0x083)) | ((BIT7) & (0x83));
	cmd[12] = (cmd[12] & (~0x01)) | BIT0;

	writeCommand(cmd, sizeof(cmd));

	int i, j = 0;
	for (i = 0; i < 132; i+=sizeof(_font[12])){
		for (j = 0; j < 8; ++j){
			setPosition(j, i);
			writeData(_font[12], sizeof(_font[12]));
		}
	}

	cmd[3] = (cmd[3] & (~0x01)) | (~BIT0 & (0x01));
	writeCommand(cmd + 3, 1);
	return 0;
}
