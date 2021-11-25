#include <msp430.h>
#include "font.h"

#define TAxCCR_05Hz 0xffff /* timer upper bound count value */
#define BUTTON_DELAY 0x0380
#define DISPLAY_RATE 5

unsigned int display_available = 1;

unsigned int glitch_counters[] = { 0, 0 };

void readData(unsigned char *sCmd, unsigned char *rData, unsigned int i);
void writeCommand(unsigned char *sCmd, unsigned char i);
void writeData(unsigned char *sData, unsigned char i);
void setPosition(unsigned char page, unsigned char col);
void printNumber(int num);
void printSymbol(int index, unsigned char page, unsigned int col);



#pragma vector = PORT1_VECTOR
__interrupt void S1_handler(void){
	if(P1IFG & BIT7){
		if (glitch_counters[0] == 0){
			if(P1IES & BIT7){
				unsigned char cmd_3[2] = { 0, 0 };
				cmd_3[0] = (cmd_3[0] & (~0x0fe)) | ((0x07 << 2) & (0x0fc));

				unsigned char out[2] = { 0 };

				readData(cmd_3, out, sizeof(cmd_3));

				printNumber((char) out[1]);
			}
			P1IES ^= BIT7;
		}

		++glitch_counters[0];
		TA2CCR1 = TA2R + BUTTON_DELAY;
		TA2CCTL1 = (TA2CCTL1 & (~0x010)) | CCIE;

		P1IFG &= ~BIT7;
	}
}

#pragma vector = PORT2_VECTOR
__interrupt void Accelerometer_handler(void){
	if(P2IFG & BIT5){
		unsigned char cmd_4[2] = { 0, 0 };
		cmd_4[0] = (cmd_4[0] & (~0x0fe)) | ((0x07 << 2) & (0x0fc));

		unsigned char out[2] = { 0 };

		readData(cmd_4, out, sizeof(cmd_4));

		printNumber((char) out[1]);
		// P2IFG &= ~BIT5;
	}
}

#pragma vector = TIMER2_A1_VECTOR
__interrupt void TA2_handler(void){
	switch(TA2IV){
		case TA2IV_TACCR1:
			// S1
			glitch_counters[0] = 0;
			TA2CCTL1 = (TA2CCTL1 & (~0x010)) | (~CCIE & (0x010)); // & ~CCIE;
			break;
		case TA2IV_TACCR2:
			// Display
			display_available = 1;
			TA2CCTL2 = (TA2CCTL2 & (~0x010)) | (~CCIE & (0x010)); // & ~CCIE;
			break;
		default:
			break;
	}
}

void readData(unsigned char *sCmd, unsigned char *rData, unsigned int i){
	unsigned int gie = __get_SR_register() & GIE;
	// Make this operation atomic
	__disable_interrupt();
    // Select acceleration sensor
	P3OUT &= ~BIT5;

	while (i){
		// USCI_B1 TX buffer ready?
		while (!(UCA0IFG & UCTXIFG)) ;
		UCA0TXBUF = *sCmd++;
		if(rData != 0){
			while (!(UCA0IFG & UCRXIFG)) ;
			*rData = UCA0RXBUF;
			++rData;
		}
		--i;
	}

	// Wait for all TX/RX to finish
	while (UCB1STAT & UCBUSY) ;
	// Dummy read to empty RX buffer and clear any overrun conditions
	UCA0RXBUF;
	// CS High
	P3OUT |= BIT5;
	// Restore original GIE state
	__bis_SR_register(gie);
}

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
        UCB1TXBUF = *sCmd++;
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
	unsigned char cmd_6[3] = {SET_COLUMN_ADDRESS_MSB, SET_COLUMN_ADDRESS_LSB, SET_PAGE_ADDRESS};
	cmd_6[0] = (cmd_6[0] & (~0x0f)) | ((col >> 4) & (0x0f));
	cmd_6[1] = (cmd_6[1] & (~0x0f)) | (col & 0x0f);
	cmd_6[2] = (cmd_6[2] & (~0x0f)) | (page & 0x0f);
	writeCommand(cmd_6, 3);
}

void printNumber(int num){
	if (display_available == 0) {
		return;
	}
	display_available = 0;

	unsigned char current_page = 0;
	unsigned char current_col = 0;

	if (num < 0) {
		num = -num;
		printSymbol(11, current_page, current_col);
	} else {
		printSymbol(10, current_page, current_col);
	}
	current_col += 8;

	unsigned char indices[4] = {13, 12, 12, 12};
	unsigned int current_pos = sizeof(indices);
	do {
		indices[--current_pos] = num % 10;
		num /= 10;
	} while (num > 0);

	int i;
	for (i = current_pos; i < current_pos + sizeof(indices); ++i){
		printSymbol(indices[i % sizeof(indices)], current_page, current_col);
		current_col += 8;
	}
	TA2CCR2 = TA2R + (TAxCCR_05Hz / DISPLAY_RATE);
	TA2CCTL2 = (TA2CCTL2 & (~0x010)) | CCIE;
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

	P1SEL &= ~BIT7;
	P1DIR &= ~BIT7;
	P1OUT |= BIT7;
	P1REN |= BIT7;
	P1IES |= BIT7;
	P1IFG &= ~BIT7;
	P1IE |= BIT7;

//	P2SEL &= ~BIT2;
//	P2DIR &= ~BIT2;
//	P2OUT |= BIT2;
//	P2REN |= BIT2;
//	P2IES |= BIT2;
//	P2IFG &= ~BIT2;
//	P2IE |= BIT2;

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

	// UART initialization
	// Initialize USCI_B1 for SPI Master operation
	UCB1IFG = (UCB1IFG & (~0x03)) | ((~UCTXIFG & (0x02)) | (~UCRXIFG & (0x01)));
	UCB1IE = (UCB1IE & (~0x03)) | ((~UCTXIE & 0x02)| (~UCRXIE & 0x01));
	// UCB1IE = (UCB1IE & (~0x03)) | UCTXIE | UCRXIE;
	// Put state machine in reset
	UCB1CTL1 |= UCSWRST;
	//3-pin, 8-bit SPI master; Clock phase - data captured first edge, change second edge; MSB
	UCB1CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
	// Use SMCLK, keep RESET
	UCB1CTL1 = UCSSEL_2 + UCSWRST;
	UCB1BR1 = 0;
	UCB1BR0 = 0x02;
	// Release USCI state machine
	UCB1CTL1 &= ~UCSWRST;


	// Initialize USCI_A0 for SPI Master operation
	UCA0IFG = (UCA0IFG & (~0x03)) | ((~UCTXIFG & (0x02)) | (~UCRXIFG & (0x01)));
	UCA0IE = (UCA0IE & (~0x03)) | ((~UCTXIE & 0x02)| (~UCRXIE & 0x01));
	// UCA0IE = (UCA0IE & (~0x03)) | (UCTXIE | UCRXIE);
	// Put state machine in reset
	UCA0CTL1 |= UCSWRST;
	// 3-pin, 8-bit SPI master; Clock polarity high, MSB
	UCA0CTL0 = UCCKPH + UCMSB + UCMST + UCMODE_0 + UCSYNC;
	// Use SMCLK, keep RESET
	UCA0CTL1 = UCSSEL_2 + UCSWRST;
	UCA0BR1 = 0;
	UCA0BR0 = 0x30;
	// No modulation
	// UCA0MCTL = 0;
	// Release USCI state machine
	UCA0CTL1 &= ~UCSWRST;


	// LCD initialization
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
	P7OUT |= BIT4;
	// Reset is active low
	P5OUT &= BIT7;
	// Reset is active low
	P5OUT |= BIT7;
	// Disable screen backlight
//	P7OUT &= ~BIT6;
	P7OUT |= BIT6;



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

	{
		unsigned char cmd_1[] = {
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

		cmd_1[1] = (cmd_1[1] & (~0x01)) | (BIT0 & 0x01);
		cmd_1[2] = (cmd_1[2] & (~0x08)) | (BIT3 & 0x08);
		cmd_1[3] = (cmd_1[3] & (~0x01)) | BIT0;
		cmd_1[6] = (cmd_1[6] & (~0x03f)) | (0x030 & (0x03f));
		cmd_1[7] = (cmd_1[7] & (~0x07)) | ((BIT0 | BIT1 | BIT2) & (0x07));
		cmd_1[8] = (cmd_1[8] & (~0x07)) | (0x4 & (0x07));
		cmd_1[11] = (cmd_1[11] & (~0x083)) | ((BIT7) & (0x83));
		cmd_1[12] = (cmd_1[12] & (~0x01)) | BIT0;

		writeCommand(cmd_1, sizeof(cmd_1));

		int i, j = 0;
		for (i = 0; i < 132; i+=sizeof(_font[12])){
			for (j = 0; j < 8; ++j){
				setPosition(j, i);
				writeData(_font[12], sizeof(_font[12]));
			}
		}

		cmd_1[3] = (cmd_1[3] & (~0x01)) | (~BIT0 & (0x01));
		writeCommand(cmd_1 + 3, 1);

		printNumber(3123);
	}

	// Accelerometer initialization
	// Chip select and power on
	P3SEL &= ~(BIT5 | BIT6);
	P3DIR |= (BIT5 | BIT6);

	// Option select SIMO and SOMI
	P3SEL |= (BIT3 | BIT4);
	P3DIR |= (BIT3 | BIT4);
	// Select CLK
	P2SEL |= BIT7;
	P2DIR |= BIT7;

	// Chip select is active low
	P3OUT |= BIT5;
	P3OUT |= BIT6;

	// Accelerometer interrupt initialization
	P2DIR &= ~BIT5;
	P2SEL &= ~BIT5;
	P2OUT |= BIT5;
	P2REN |= BIT5;
//	P2OUT &= ~BIT5;
//	P2REN |= BIT5;
	P2IES &= ~BIT5;
	P2IFG &= ~BIT5;
	P2IE |= BIT5;
//	P2IE &= ~BIT5;

	{
		unsigned char cmd_2[2] = { 0, 0 };
		cmd_2[0] = (cmd_2[0] & (~0x0fe)) | (((0x02 << 2) & (0x0fc)) | BIT1) ;
		cmd_2[1] = (cmd_2[1] & (~0x0df)) | ((BIT6 | BIT4 | ((0x01 << 1) & (0x0e))) & (0x0df));

		unsigned char out_2[2] = { 0, 0 };

		readData(cmd_2, out_2, sizeof(cmd_2));

		if(out_2[0] != 0){
			int temp = 0;
		}
	}
	return 0;
}
