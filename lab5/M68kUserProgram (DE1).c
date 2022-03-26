#include <stdio.h>
#include <string.h>
#include <ctype.h>


//IMPORTANT
//
// Uncomment one of the two #defines below
// Define StartOfExceptionVectorTable as 08030000 if running programs from sram or
// 0B000000 for running programs from dram
//
// In your labs, you will initially start by designing a system with SRam and later move to
// Dram, so these constants will need to be changed based on the version of the system you have
// building
//
// The working 68k system SOF file posted on canvas that you can use for your pre-lab
// is based around Dram so #define accordingly before building

// #define StartOfExceptionVectorTable 0x08030000
#define StartOfExceptionVectorTable 0x0B000000

/**********************************************************************************************
**	Parallel port addresses
**********************************************************************************************/

#define PortA   *(volatile unsigned char *)(0x00400000)
#define PortB   *(volatile unsigned char *)(0x00400002)
#define PortC   *(volatile unsigned char *)(0x00400004)
#define PortD   *(volatile unsigned char *)(0x00400006)
#define PortE   *(volatile unsigned char *)(0x00400008)

/*********************************************************************************************
**	Hex 7 seg displays port addresses
*********************************************************************************************/

#define HEX_A        *(volatile unsigned char *)(0x00400010)
#define HEX_B        *(volatile unsigned char *)(0x00400012)
#define HEX_C        *(volatile unsigned char *)(0x00400014)    // de2 only
#define HEX_D        *(volatile unsigned char *)(0x00400016)    // de2 only

/**********************************************************************************************
**	LCD display port addresses
**********************************************************************************************/

#define LCDcommand   *(volatile unsigned char *)(0x00400020)
#define LCDdata      *(volatile unsigned char *)(0x00400022)

/********************************************************************************************
**	Timer Port addresses
*********************************************************************************************/

#define Timer1Data      *(volatile unsigned char *)(0x00400030)
#define Timer1Control   *(volatile unsigned char *)(0x00400032)
#define Timer1Status    *(volatile unsigned char *)(0x00400032)

#define Timer2Data      *(volatile unsigned char *)(0x00400034)
#define Timer2Control   *(volatile unsigned char *)(0x00400036)
#define Timer2Status    *(volatile unsigned char *)(0x00400036)

#define Timer3Data      *(volatile unsigned char *)(0x00400038)
#define Timer3Control   *(volatile unsigned char *)(0x0040003A)
#define Timer3Status    *(volatile unsigned char *)(0x0040003A)

#define Timer4Data      *(volatile unsigned char *)(0x0040003C)
#define Timer4Control   *(volatile unsigned char *)(0x0040003E)
#define Timer4Status    *(volatile unsigned char *)(0x0040003E)

/*********************************************************************************************
**	RS232 port addresses
*********************************************************************************************/

#define RS232_Control     *(volatile unsigned char *)(0x00400040)
#define RS232_Status      *(volatile unsigned char *)(0x00400040)
#define RS232_TxData      *(volatile unsigned char *)(0x00400042)
#define RS232_RxData      *(volatile unsigned char *)(0x00400042)
#define RS232_Baud        *(volatile unsigned char *)(0x00400044)

/*********************************************************************************************
**	PIA 1 and 2 port addresses
*********************************************************************************************/

#define PIA1_PortA_Data     *(volatile unsigned char *)(0x00400050)         // combined data and data direction register share same address
#define PIA1_PortA_Control *(volatile unsigned char *)(0x00400052)
#define PIA1_PortB_Data     *(volatile unsigned char *)(0x00400054)         // combined data and data direction register share same address
#define PIA1_PortB_Control *(volatile unsigned char *)(0x00400056)

#define PIA2_PortA_Data     *(volatile unsigned char *)(0x00400060)         // combined data and data direction register share same address
#define PIA2_PortA_Control *(volatile unsigned char *)(0x00400062)
#define PIA2_PortB_data     *(volatile unsigned char *)(0x00400064)         // combined data and data direction register share same address
#define PIA2_PortB_Control *(volatile unsigned char *)(0x00400066)

/*************************************************************
** I2C Controller registers
**************************************************************/
// I2C Registers
#define I2C_PRERLO     (*(volatile unsigned char *)(0x00408000))
#define I2C_PRERHI     (*(volatile unsigned char *)(0x00408002))
#define I2C_CTR        (*(volatile unsigned char *)(0x00408004))
#define I2C_TXR        (*(volatile unsigned char *)(0x00408006))
#define I2C_RXR        (*(volatile unsigned char *)(0x00408006))
#define I2C_CR         (*(volatile unsigned char *)(0x00408008))
#define I2C_SR         (*(volatile unsigned char *)(0x00408008))

// STA bit == cmd[7], W bit == cmd[4], IACK bit == cmd[0] -> 8'b1001_0001
#define WSTART 0x91
// STO bit == cmd[6], W bit == cmd[4] -> 8'b0101_0000
#define WSTOP 0x50
// W bit = cmd[4] -> 8'b0001_0000
#define WRITE 0x10
// R bit == cmd[5], NACK bit == cmd[3], IACK bit == cmd[0] -> 8'b0010_1001
#define READNACK 0x29
// R bit == cmd[5], NACK bit == cmd[3], IACK bit == cmd[0] -> 8'b0010_0001
#define READACK 0x21

#define EEPROM_SLAVE_BANK0 0xA0
#define EEPROM_SLAVE_BANK1 0xA8
#define ADCDAC_SLAVE 0x90
// 8'b01xx_0xxx
#define DAC_OUT_ON 0x40
// 8'b00xx_0xxx
#define DAC_OUT_OFF 0x00
// 8'b0000_0101
#define ADC_AUTO_INCREMENT_A1 0x05
#define ADC_AUTO_INCREMENT_A0 0x04

/*********************************************************************************************************************************
(( DO NOT initialise global variables here, do it main even if you want 0
(( it's a limitation of the compiler
(( YOU HAVE BEEN WARNED
*********************************************************************************************************************************/

unsigned int i, x, y, z, PortA_Count;
unsigned char Timer1Count, Timer2Count, Timer3Count, Timer4Count ;

/*******************************************************************************************
** Function Prototypes
*******************************************************************************************/
void Wait1ms(void);
void Wait3ms(void);
void Wait250ms(void);
void Wait750ms(void);
void Wait1s(void);
void Init_LCD(void) ;
void LCDOutchar(int c);
void LCDOutMess(char *theMessage);
void LCDClearln(void);
void LCDline1Message(char *theMessage);
void LCDline2Message(char *theMessage);
int sprintf(char *out, const char *format, ...) ;

// I2C
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void WaitForTIPFlagReset(void);
void WaitForRxACK(void);
void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo);
void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo);
void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, char size);
void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, char size);
void GetMemAddr(char *hi, char *lo);
void GetBank(char *bank);
void Wait(void);
void readADC(void);

/*****************************************************************************************
**	Interrupt service routine for Timers
**
**  Timers 1 - 4 share a common IRQ on the CPU  so this function uses polling to figure
**  out which timer is producing the interrupt
**
*****************************************************************************************/
// converts hex char to 4 bit binary equiv in range 0000-1111 (0-F)
// char assumed to be a valid hex char 0-9, a-f, A-F

char xtod(int c)
{
    if ((char)(c) <= (char)('9'))
        return c - (char)(0x30);    // 0 - 9 = 0x30 - 0x39 so convert to number by sutracting 0x30
    else if((char)(c) > (char)('F'))    // assume lower case
        return c - (char)(0x57);    // a-f = 0x61-66 so needs to be converted to 0x0A - 0x0F so subtract 0x57
    else
        return c - (char)(0x37);    // A-F = 0x41-46 so needs to be converted to 0x0A - 0x0F so subtract 0x37
}

int Get2HexDigits(char *CheckSumPtr)
{
    register int i = (xtod(_getch()) << 4) | (xtod(_getch()));

    if(CheckSumPtr)
        *CheckSumPtr += i ;

    return i ;
}

int Get4HexDigits(char *CheckSumPtr)
{
    return (Get2HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
}

int Get6HexDigits(char *CheckSumPtr)
{
    return (Get4HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
}

int Get8HexDigits(char *CheckSumPtr)
{
    return (Get4HexDigits(CheckSumPtr) << 16) | (Get4HexDigits(CheckSumPtr));
}

void Timer_ISR()
{
   	if(Timer1Status == 1) {         // Did Timer 1 produce the Interrupt?
   	    Timer1Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
   	    PortA = Timer1Count++ ;     // increment an LED count on PortA with each tick of Timer 1
   	}

  	if(Timer2Status == 1) {         // Did Timer 2 produce the Interrupt?
   	    Timer2Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
   	    PortC = Timer2Count++ ;     // increment an LED count on PortC with each tick of Timer 2
   	}

   	if(Timer3Status == 1) {         // Did Timer 3 produce the Interrupt?
   	    Timer3Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
        HEX_A = Timer3Count++ ;     // increment a HEX count on Port HEX_A with each tick of Timer 3
   	}

   	if(Timer4Status == 1) {         // Did Timer 4 produce the Interrupt?
   	    Timer4Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
        HEX_B = Timer4Count++ ;     // increment a HEX count on HEX_B with each tick of Timer 4
   	}
}

/*****************************************************************************************
**	Interrupt service routine for ACIA. This device has it's own dedicate IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void ACIA_ISR()
{}

/***************************************************************************************
**	Interrupt service routine for PIAs 1 and 2. These devices share an IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void PIA_ISR()
{}

/***********************************************************************************
**	Interrupt service routine for Key 2 on DE1 board. Add your own response here
************************************************************************************/
void Key2PressISR()
{}

/***********************************************************************************
**	Interrupt service routine for Key 1 on DE1 board. Add your own response here
************************************************************************************/
void Key1PressISR()
{}

/************************************************************************************
**   Delay Subroutine to give the 68000 something useless to do to waste 1 mSec
************************************************************************************/
void Wait1ms(void)
{
    int  i ;
    for(i = 0; i < 1000; i ++)
        ;
}

/************************************************************************************
**  Subroutine to give the 68000 something useless to do to waste 3 mSec
**************************************************************************************/
void Wait3ms(void)
{
    int i ;
    for(i = 0; i < 3; i++)
        Wait1ms() ;
}

void Wait250ms(void)
{
    int i ;
    for(i = 0; i < 250; i++)
        Wait1ms() ;
}

void Wait750ms(void)
{
    int i ;
    for(i = 0; i < 750; i++)
        Wait1ms() ;
}

/************************************************************************************
**  Subroutine to give the 68000 something useless to do to waste 1s
**************************************************************************************/
void Wait1s(void){
    int i;
    for(i = 0; i < 1000; i++)
        Wait1ms();
}

/*********************************************************************************************
**  Subroutine to initialise the LCD display by writing some commands to the LCD internal registers
**  Sets it for parallel port and 2 line display mode (if I recall correctly)
*********************************************************************************************/
void Init_LCD(void)
{
    LCDcommand = 0x0c ;
    Wait3ms() ;
    LCDcommand = 0x38 ;
    Wait3ms() ;
}

/*********************************************************************************************
**  Subroutine to initialise the RS232 Port by writing some commands to the internal registers
*********************************************************************************************/
void Init_RS232(void)
{
    RS232_Control = 0x15 ; //  %00010101 set up 6850 uses divide by 16 clock, set RTS low, 8 bits no parity, 1 stop bit, transmitter interrupt disabled
    RS232_Baud = 0x1 ;      // program baud rate generator 001 = 115k, 010 = 57.6k, 011 = 38.4k, 100 = 19.2, all others = 9600
}

/*********************************************************************************************************
**  Subroutine to provide a low level output function to 6850 ACIA
**  This routine provides the basic functionality to output a single character to the serial Port
**  to allow the board to communicate with HyperTerminal Program
**
**  NOTE you do not call this function directly, instead you call the normal putchar() function
**  which in turn calls _putch() below). Other functions like puts(), printf() call putchar() so will
**  call _putch() also
*********************************************************************************************************/

int _putch( int c)
{
    while((RS232_Status & (char)(0x02)) != (char)(0x02))    // wait for Tx bit in status register or 6850 serial comms chip to be '1'
        ;

    RS232_TxData = (c & (char)(0x7f));                      // write to the data register to output the character (mask off bit 8 to keep it 7 bit ASCII)
    return c ;                                              // putchar() expects the character to be returned
}

/*********************************************************************************************************
**  Subroutine to provide a low level input function to 6850 ACIA
**  This routine provides the basic functionality to input a single character from the serial Port
**  to allow the board to communicate with HyperTerminal Program Keyboard (your PC)
**
**  NOTE you do not call this function directly, instead you call the normal getchar() function
**  which in turn calls _getch() below). Other functions like gets(), scanf() call getchar() so will
**  call _getch() also
*********************************************************************************************************/
int _getch( void )
{
    char c ;
    while((RS232_Status & (char)(0x01)) != (char)(0x01))    // wait for Rx bit in 6850 serial comms chip status register to be '1'
        ;

    return (RS232_RxData & (char)(0x7f));                   // read received character, mask off top bit and return as 7 bit ASCII character
}

/******************************************************************************
**  Subroutine to output a single character to the 2 row LCD display
**  It is assumed the character is an ASCII code and it will be displayed at the
**  current cursor position
*******************************************************************************/
void LCDOutchar(int c)
{
    LCDdata = (char)(c);
    Wait1ms() ;
}

/**********************************************************************************
*subroutine to output a message at the current cursor position of the LCD display
************************************************************************************/
void LCDOutMessage(char *theMessage)
{
    char c ;
    while((c = *theMessage++) != 0)     // output characters from the string until NULL
        LCDOutchar(c) ;
}

/******************************************************************************
*subroutine to clear the line by issuing 24 space characters
*******************************************************************************/
void LCDClearln(void)
{
    int i ;
    for(i = 0; i < 24; i ++)
        LCDOutchar(' ') ;       // write a space char to the LCD display
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 1 and clear that line
*******************************************************************************/
void LCDLine1Message(char *theMessage)
{
    LCDcommand = 0x80 ;
    Wait3ms();
    LCDClearln() ;
    LCDcommand = 0x80 ;
    Wait3ms() ;
    LCDOutMessage(theMessage) ;
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 2 and clear that line
*******************************************************************************/
void LCDLine2Message(char *theMessage)
{
    LCDcommand = 0xC0 ;
    Wait3ms();
    LCDClearln() ;
    LCDcommand = 0xC0 ;
    Wait3ms() ;
    LCDOutMessage(theMessage) ;
}

void I2C_Start(void){
    // Start condition is 8'b1000_0000
    I2C_CR = 0x80;
}

void I2C_Stop(void){
    // Stop condition is 8'b0100_0000
    I2C_CR = 0x40;
}

void I2C_Init(void){
    // Make sure I2C core is off
    I2C_CTR = (char)0x00;
    // Prescale register clock, 25Mhz / (5 * 100KHz) - 1  -> 0x0031
    I2C_PRERLO = (char)0x31;
    I2C_PRERHI = (char)0x00;
    // Enable I2C Core, no interrupt enabled
    // 8'b10xx_xxxx
    I2C_CTR = (char)0x80;
}

void WaitForRxACK(void){
    // Read RxACK bit from Status Register, should be '0'
    // Status Register [7] == 0
    while(((I2C_SR >> 7) & 1) == 1){
    }
}

void WaitForTIPFlagReset(void){
    // Status Register [1] == 0
    while((I2C_SR >> 1) & 1){
    }
}

void Wait(void){
    WaitForTIPFlagReset();
    WaitForRxACK();
}

void TransmitI2C(char data, char cr){
    I2C_TXR = data;
    I2C_CR = cr;
    Wait();
}

// Assumptions: addr is at most 7 bits wide
void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo){
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    // Write data transmit register, set STO bit, set W bit. 
    TransmitI2C(data, WSTOP);
}

// Assumptions: slave_addr is in write mode
void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo){
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    // Set slave to read mode and generate start command for reading
    TransmitI2C(slave_addr | 1, WSTART);
    // Read data transmit register, set R bit, set NACK and IACK
    I2C_CR = READNACK;
    // Wait for read data to come in
    while((I2C_SR & 1) !=1){}
    *data = I2C_RXR;
    I2C_CR = 0x41;
}

// Increment logic, handles bank overflows
void incrI2C(char *slave_addr, char *memaddr_hi, char *memaddr_lo, int read){
    char zeewo = 0x00;
    char one = 0x01;
    // printf("\r\n %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
    if ((*memaddr_lo & 0xFF) == 0xFF){
        if ((*memaddr_hi & 0xFF) == 0xFF){ // Change bank
            *slave_addr = ((*slave_addr & 0x8) == 0x8) ? EEPROM_SLAVE_BANK0 : EEPROM_SLAVE_BANK1;
            *memaddr_lo = zeewo;
            *memaddr_hi = zeewo;
            
            if(read & 1){
                printf("\r\n Changing slave and restart %x", *slave_addr);
                TransmitI2C(*slave_addr, WSTART);
                TransmitI2C(*memaddr_hi, WRITE);
                TransmitI2C(*memaddr_lo, WRITE);
                TransmitI2C(*slave_addr | 1, WSTART);
            }
            else{
                TransmitI2C(*slave_addr, WSTART);
                TransmitI2C(*memaddr_hi, WRITE);
                TransmitI2C(*memaddr_lo, WRITE);
            }
            printf("\r\n HIT EDGE CASE %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
        }
        else{
            *memaddr_hi += one;
            *memaddr_lo = zeewo;
        }
    }
    else{
        *memaddr_lo += one;
    }
}

void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, char size){
    // Data is array of size "size", max size 128
    // WriteI2CChar();
    char i;
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    for (i = 0; i < size; i++){
        // Write without stop unless we're at the end
        if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF)){
            TransmitI2C(i,WSTOP);
        }
        else{
            TransmitI2C(i,WRITE);
        }
        // Page crossing, if it happens, is handled inside incrI2C
        incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 0);
    }

}

void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, char size){
     // Data is array of size "size", max size 128
    char i;
    char temp = 0;
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    // Set slave to read mode and generate start command for reading
    TransmitI2C(slave_addr | 1, WSTART);
    for (i = 0; i < size; i++){
        // Read without NACK unless we're at the end
        if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF)){
            I2C_CR = READNACK;
        }
        else
            I2C_CR = READACK;
        while((I2C_SR & 1) !=1){}
        data[i] = I2C_RXR;
        if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF))
            I2C_CR = 0x41;
        else
            I2C_CR = 0x01;
        // Page crossing, if it happens, is handled inside incrI2C
        incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 1);
    }
}

void GetBank(char *bank){
    char asdf;
    while(1){
        printf("\r\nSelect bank:\r\n0 - Bank 0\r\n1 - Bank 1");
        asdf = getchar();
        putchar(asdf);
        if(asdf == '0'){
            *bank = EEPROM_SLAVE_BANK0;
            break;
        }
        else if (asdf == '1'){
            *bank = EEPROM_SLAVE_BANK1;
            break;
        }
        else{
            printf("\r\nInvalid selection.");
        }
    }       
}

void GetMemAddr(char *hi, char *lo){
    printf("\r\nEnter mem address hi:");
    *hi = Get2HexDigits(0);
    printf("\r\nEnter mem address lo:");
    *lo = Get2HexDigits(0);
    // TODO: Hex digit validation?
}

void blinky(){
    // Make sure nothing is going on in the I2C bus 
    WaitForTIPFlagReset();
    // Write address
    TransmitI2C(ADCDAC_SLAVE, WSTART);
    // Set control to OUT: 8'b01xx_0xxx
    TransmitI2C(DAC_OUT_ON, WRITE);
    // Vout calculation: 5/256 * 8 bit data
    // Blinky until reset is pressed on DE1
    while(1){
        TransmitI2C(0xFF, WRITE);
        Wait250ms();
        TransmitI2C(0x00, WRITE);
        Wait250ms();
        TransmitI2C(0xFF, WRITE);
        Wait250ms();
        TransmitI2C(0x00, WRITE);
        Wait250ms();
        TransmitI2C(0xFF, WRITE);
        Wait750ms();
        TransmitI2C(0x00, WRITE);
        Wait750ms();
    }
}
// AN0: External analog source (remove jumper)
// 2. AN1: On board potentiometer to supply a variable voltage.
// 3. AN2: On board thermistor to measure temperature
// 4. AN3: On board photo resistor to measure light intensity
// 5. OUT: A RED LED which can be driven by the D/A output. 

void readADC(void){
    char Vchannel1, Vchannel2, Vchannel3, Vchannel4;
    
    while(1){
        // Make sure nothing is going on in the I2C bus 
        WaitForTIPFlagReset();
        // Write address
        TransmitI2C(ADCDAC_SLAVE, WSTART);
        // Set control to auto increment starting at 1: 8'b0000_0101
        TransmitI2C(ADC_AUTO_INCREMENT_A0, WRITE);
        // Set slave to Read mode
        TransmitI2C(ADCDAC_SLAVE | 1, WSTART);
        // Read data transmit register, set R bit, set ACK
        I2C_CR = READACK;
        // Wait for read data to come in
        while((I2C_SR & 1) !=1){}
        Vchannel4 = I2C_RXR; // This is always invalid because the jumper isn't connected
        I2C_CR = READACK;
        while((I2C_SR & 1) !=1){}
        Vchannel1 = I2C_RXR;
        I2C_CR = READACK;
        // Wait for read data to come in
        while((I2C_SR & 1) !=1){}
        Vchannel2 = I2C_RXR;
        I2C_CR = READACK;
        // Wait for read data to come in
        while((I2C_SR & 1) !=1){}
        Vchannel3 = I2C_RXR;
        I2C_CR = 0x41;
        // Vout calculation: 5/256 * 8 bit data, but we can just leave it.
        printf("\r\nThermistor: %d", Vchannel1 );
        printf("\r\nPotentiometer: %d", Vchannel2 );
        printf("\r\nPhotoresistor: %d", Vchannel3);
        printf("\r\n--------------");
        Wait1s();
    }
}
/*********************************************************************************************************************************
**  IMPORTANT FUNCTION
**  This function install an exception handler so you can capture and deal with any 68000 exception in your program
**  You pass it the name of a function in your code that will get called in response to the exception (as the 1st parameter)
**  and in the 2nd parameter, you pass it the exception number that you want to take over (see 68000 exceptions for details)
**  Calling this function allows you to deal with Interrupts for example
***********************************************************************************************************************************/

void InstallExceptionHandler( void (*function_ptr)(), int level)
{
    volatile long int *RamVectorAddress = (volatile long int *)(StartOfExceptionVectorTable) ;   // pointer to the Ram based interrupt vector table created in Cstart in debug monitor

    RamVectorAddress[level] = (long int *)(function_ptr);                       // install the address of our function into the exception table
}

/******************************************************************************************************************************
* Start of user program
******************************************************************************************************************************/

void main()
{
    unsigned int row, j=0, count=0, counter1=1;
    char i = 0;
    char c, text[150] ;

    unsigned char asdf, asdf1, asdf2, asdf3, bank;
    unsigned char wBuf[256];
    unsigned char rBuf[256];
    int addr = 2048;
    char size;
    char data[128];

	int PassFailFlag = 1 ;

    i = x = y = z = PortA_Count =0;
    Timer1Count = Timer2Count = Timer3Count = Timer4Count = 0;

    InstallExceptionHandler(PIA_ISR, 25) ;          // install interrupt handler for PIAs 1 and 2 on level 1 IRQ
    InstallExceptionHandler(ACIA_ISR, 26) ;		    // install interrupt handler for ACIA on level 2 IRQ
    InstallExceptionHandler(Timer_ISR, 27) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
    InstallExceptionHandler(Key2PressISR, 28) ;	    // install interrupt handler for Key Press 2 on DE1 board for level 4 IRQ
    InstallExceptionHandler(Key1PressISR, 29) ;	    // install interrupt handler for Key Press 1 on DE1 board for level 5 IRQ

    Timer1Data = 0x10;		// program time delay into timers 1-4
    Timer2Data = 0x20;
    Timer3Data = 0x15;
    Timer4Data = 0x25;

    Timer1Control = 3;		// write 3 to control register to Bit0 = 1 (enable interrupt from timers) 1 - 4 and allow them to count Bit 1 = 1
    Timer2Control = 3;
    Timer3Control = 3;
    Timer4Control = 3;

    Init_LCD();             // initialise the LCD display to use a parallel data interface and 2 lines of display
    Init_RS232() ;          // initialise the RS232 port for use with hyper terminal

/*************************************************************************************************
**  I2C Program Lab 5
*************************************************************************************************/

    I2C_Init();

    // printf("%x\n", I2C_PRERLO);
    // printf("%x\n", I2C_PRERHI);
    // printf("%x\n", I2C_CTR);
    // printf("%x\n", I2C_TXR);
    // printf("%x\n", I2C_RXR);
    // printf("%x\n", I2C_CR);
    // printf("%x\n", I2C_SR);

    printf("\r\nLab 5: I2C");
    while(1){
       
        printf("\r\nChoose the following:\r\n0 - Write Byte\r\n1 - Read Byte\r\n2 - Page Write\r\n3 - Page Read\r\n4 - DAC Blinky LED\r\n5 - ADC Read");
        asdf = getchar();
        putchar(asdf);
        if(asdf == '0'){
            printf("\r\nEnter byte to write: ");
            asdf = Get2HexDigits(0);
            // Bank Selection
            GetBank(&bank);
            // Mem Address Selection
            GetMemAddr(&asdf2, &asdf1);
            WriteI2CChar(asdf, bank, asdf2, asdf1);
            printf("\r\nWritten %x to bank %d at memaddr hi: %x mem addr lo: %x", asdf, bank == EEPROM_SLAVE_BANK0 ? 0:1, asdf2, asdf1);
        }
        else if(asdf == '1'){
            // Bank Selection
            GetBank(&bank);
            // Mem Address Selection
            GetMemAddr(&asdf, &asdf1);
            ReadI2CChar(&asdf3, bank, asdf, asdf1);
            printf("\r\nRead %x from bank %d at memaddr hi: %x mem addr lo: %x", asdf3, bank == EEPROM_SLAVE_BANK0 ? 0:1, asdf, asdf1);
        }
        else if(asdf == '2'){
            printf("\r\nbruh");printf("\r\nEnter size of page to write(max 128 in hex == 0x7F): ");
            size = Get2HexDigits(0);
            // Bank Selection
            GetBank(&bank);
            // Mem Address Selection
            GetMemAddr(&asdf2, &asdf1);
            WriteI2CPage(bank, asdf2, asdf1,size);
            printf("\r\nWrote values starting at memaddr 0x%x%x and bank %d, total size: %x\r\nData: ", asdf2, asdf1, bank == EEPROM_SLAVE_BANK0 ? 0:1,size);
            for(i = 0; i < size; i++){
                printf("%x", i);
            }
        }
        else if(asdf == '3'){
            printf("\r\nEnter size of page to read(max 128 in hex == 0x7F): ");
            size = Get2HexDigits(0);
            // Bank Selection
            GetBank(&bank);
            // Mem Address Selection
            GetMemAddr(&asdf2, &asdf1);
            ReadI2CPage(data, bank, asdf2, asdf1,size);
            printf("\r\nRead values starting at memaddr 0x%x%x and bank %d, total size: %x\r\nData: ", asdf2, asdf1, bank == EEPROM_SLAVE_BANK0 ? 0:1,size);
            for(i = 0; i < size; i++){
                printf("%x", data[i]);
            }
        }
        else if(asdf == '4'){
            printf("\r\nDAC Blinky... frequency of blinky is: \r\nON - 250ms - OFF - 250ms\r\n0N - 250ms - OFF - 250ms\r\nON - 750ms - OFF - 750ms\r\nRepeat forever\r\n");
            blinky();
        }
        else if(asdf == '5'){
            printf("\r\nADC Channel Output:");
            readADC();
        }
        else{
            printf("\r\nInvalid Selection.\r\n");
            
            continue;
        }   
    }
}