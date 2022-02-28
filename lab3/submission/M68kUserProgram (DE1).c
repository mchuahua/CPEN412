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
** SPI Controller registers
**************************************************************/
// SPI Registers
#define SPI_Control         (*(volatile unsigned char *)(0x00408020))
#define SPI_Status          (*(volatile unsigned char *)(0x00408022))
#define SPI_Data            (*(volatile unsigned char *)(0x00408024))
#define SPI_Ext             (*(volatile unsigned char *)(0x00408026))
#define SPI_CS              (*(volatile unsigned char *)(0x00408028))

// these two macros enable or disable the flash memory chip enable off SSN_O[7..0]
// in this case we assume there is only 1 device connected to SSN_O[0] so we can
// write hex FE to the SPI_CS to enable it (the enable on the flash chip is active low)
// and write FF to disable it

#define   Enable_SPI_CS()             SPI_CS = 0xFE
#define   Disable_SPI_CS()            SPI_CS = 0xFF 

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
void Init_LCD(void) ;
void LCDOutchar(int c);
void LCDOutMess(char *theMessage);
void LCDClearln(void);
void LCDline1Message(char *theMessage);
void LCDline2Message(char *theMessage);
int sprintf(char *out, const char *format, ...) ;

// SPI
int TestForSPITransmitData(void);
void SPI_Init(void);
void WaitForSPITransmitComplete(void);
int  WriteSPIChar(int c);
int  ReadSPIChar(void);
void WriteSPICmdAndAddr(int cmd, int FlashAddr);
void ReadSPIData(int FlashAddr, unsigned char *MemAddr, int size);
void WriteSPIData(int FlashAddr, unsigned char *MemAddr, int size);
void WriteSPIDataLarge(int FlashAddr, unsigned char *MemAddr, int size);
void WriteSPICmdOnly(int cmd);
void CheckSPIBusy(void);
void EraseSPIChip(void);





/*****************************************************************************************
**	Interrupt service routine for Timers
**
**  Timers 1 - 4 share a common IRQ on the CPU  so this function uses polling to figure
**  out which timer is producing the interrupt
**
*****************************************************************************************/

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


/******************************************************************************************
** The following code is for the SPI controller
*******************************************************************************************/
// return true if the SPI has finished transmitting a byte (to say the Flash chip) return false otherwise
// this can be used in a polling algorithm to know when the controller is busy or idle.

int TestForSPITransmitDataComplete(void)    {
    /* TODO replace 0 below with a test for status register SPIF bit and if set, return true */
    // Hex 80 is 0b1000_0000, SPIF bit
    return (SPI_Status & 0x80);
}

/************************************************************************************
** initialises the SPI controller chip to set speed, interrupt capability etc.
************************************************************************************/
void SPI_Init(void)
{
    //TODO
    //
    // Program the SPI Control, EXT, CS and Status registers to initialise the SPI controller
    // Don't forget to call this routine from main() before you do anything else with SPI
    //
    // Here are some settings we want to create
    //
    // Control Reg     - interrupts disabled, core enabled, Master mode, Polarity and Phase of clock = [0,0], speed =  divide by 32 = approx 700Khz
    // 0x53 == 0b01x1_0011
    SPI_Control = 0x53;
    // Ext Reg         - in conjunction with control reg, sets speed above and also sets interrupt flag after every completed transfer (each byte)
    // 0x00 == 0b00xx_xx00
    SPI_Ext = 0x00;
    // SPI_CS Reg      - control selection of slave SPI chips via their CS# signals
    Disable_SPI_CS();
    // Status Reg      - status of SPI controller chip and used to clear any write collision and interrupt on transmit complete flag
    // 0xC0 == 0b11xx_xxxx
    // SPI_Status |= 0xC0;
}

/************************************************************************************
** return ONLY when the SPI controller has finished transmitting a byte
************************************************************************************/
void WaitForSPITransmitComplete(void)
{
    // TODO : poll the status register SPIF bit looking for completion of transmission
    // once transmission is complete, clear the write collision and interrupt on transmit complete flags in the status register (read documentation)
    // just in case they were set
    while(!TestForSPITransmitDataComplete()){
    }
    // 11xx_xxxx, x's are read only so doesn't matter tbh
    SPI_Status |= 0xC0;
}


/************************************************************************************
** Write a byte to the SPI flash chip via the controller and returns (reads) whatever was
** given back by SPI device at the same time (removes the read byte from the FIFO)
************************************************************************************/
int WriteSPIChar(int c)
{
    // todo - write the byte in parameter 'c' to the SPI data register, this will start it transmitting to the flash device
    // wait for completion of transmission
    // return the received data from Flash chip (which may not be relevent depending upon what we are doing)
    // by reading fom the SPI controller Data Register.
    // note however that in order to get data from an SPI slave device (e.g. flash) chip we have to write a dummy byte to it
    //
    // modify '0' below to return back read byte from data register
    //
    int data;
    // Write Data
    SPI_Data = c;
    WaitForSPITransmitComplete();
    // Read Data by writing dummy data first into the write buffer, waiting for finish transmission, 
    // then reading from read buffer
    data = SPI_Data;    

    return data;                   
}

int ReadSPIChar(void)
{
    // Write garbagio
    return WriteSPIChar(0x00);
}

void WriteSPICmdAndAddr(int cmd, int FlashAddr)
{
    WriteSPIChar(cmd);
    WriteSPIChar(FlashAddr >> 16);
    WriteSPIChar(FlashAddr >> 8);
    WriteSPIChar(FlashAddr);
}

void ReadSPIData(int FlashAddr, unsigned char *MemAddr, int size)
{
    int i = 0;
    Enable_SPI_CS();
    WriteSPICmdAndAddr(0x03, FlashAddr);
    // Read and put into MemAddr
    for(i = 0; i < size; i++){
        MemAddr[i] = (unsigned char) ReadSPIChar();
        // MemAddr[i] = (unsigned char) WriteSPIChar(0x00);
    }
    Disable_SPI_CS();
}

void WriteSPICmdOnly(int cmd)
{
    Enable_SPI_CS();
    WriteSPIChar(cmd);
    Disable_SPI_CS();
}

void CheckSPIBusy(void)
{
    // Check internal completion by polling flash chip status register (0x05), bit one for BUSY status
    Enable_SPI_CS();
    WriteSPIChar(0x05);
    while(WriteSPIChar(0x00) & 0x01){
    }
    Disable_SPI_CS();
}

// Max 256 bytes
void WriteSPIData(int FlashAddr, unsigned char *MemAddr, int size)
{
    int i = 0;
    // Write enable
    WriteSPICmdOnly(0x06);
    Enable_SPI_CS();
    // Page program command is 0x02
    WriteSPICmdAndAddr(0x02, FlashAddr);
    // Write 
    for(i = 0; i < size; i++){
        WriteSPIChar(MemAddr[i]);
    }
    // Done write
    Disable_SPI_CS();
    // Wait for completion
    CheckSPIBusy();
}

// For data > 256
void WriteSPIDataLarge(int FlashAddr, unsigned char *MemAddr, int size)
{
    while(size > 256){
        WriteSPIData(FlashAddr, MemAddr, 256);
        size -= 256;
    }
    // Write the last remainder portion
    if (size != 0){
        WriteSPIData(FlashAddr, MemAddr, size);
    }
}

void EraseSPIChip(void)
{
    // Write enable
    WriteSPICmdOnly(0x06);
    // Chip Erase, c7 or 60 both work
    WriteSPICmdOnly(0xC7);
    // Wait for completion
    CheckSPIBusy();

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
    unsigned int row, i=0, j=0, count=0, counter1=1;
    char c, text[150] ;

    unsigned char asdf;
    unsigned char wBuf[256];
    unsigned char rBuf[256];
    int addr = 2048;

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
**  Test of scanf function
*************************************************************************************************/

    scanflush() ;                       // flush any text that may have been typed ahead
    // printf("\r\nEnter Integer: ") ;
    // scanf("%d", &i) ;
    // printf("You entered %d", i) ;

    // sprintf(text, "Hello CPEN 412 Student") ;
    // LCDLine1Message(text) ;

    // printf("\r\nHello CPEN 412 Student\r\nYour LEDs should be Flashing") ;
    // printf("\r\nYour LCD should be displaying") ;

/*************************************************************************************************
**  SPI Program Lab 3
*************************************************************************************************/
    // 16384 pages of 256 bytes each
    // Pages can be erased in groups of:
    // - 16 (4KB sector erase)
    // - 128 (32KB block erase)
    // - 256 (64KB block erase)
    // - Entire chip (chip erase)
    SPI_Init();

    // init buffers
    memset(rBuf, 0x00, sizeof(rBuf));
    // memset(wBuf, 0x00, sizeof(wBuf));

    // Init wBuf to have 1, 2, 3, 4, ... 255
    for(i = 0; i < sizeof(wBuf); ++i){
        wBuf[i] = i;
    }
    //     printf("\r\nClearing read buffer.");
    
    //     printf("\r\nSize of rBuf %d\r\n", sizeof(rBuf));
    // for (j = 0; j < sizeof(rBuf); ++j){
    //     printf("%x", rBuf[j]);
    // }
    // printf("\r\nSize of wBuf %d\r\n", sizeof(wBuf));
    // for (j = 0; j < sizeof(wBuf); ++j){
    //     printf("%x", wBuf[j]);
    // }
    

    printf("\r\nErasing SPI Flash Chip on startup...");
    EraseSPIChip();
    printf("\r\nSPI Flash Chip erased.");
    printf("\r\nStarting writing process...");
    printf("\r\nwBuf initialized to [0 1 2 ... 255]");
    printf("\r\nWriting wBuf at pages 0-16383: ");
    for (i = 0; i < 2048; ++i){
    // for (i = 0; i < 1; i++){
        WriteSPIData(addr, wBuf, sizeof(wBuf));
        // Each write is 256 bytes to page i
        addr += 256;
        // Print page # at every 64KB block
        if ((i + 1) % 256 == 0){
            printf(".%d.", i);
        }
    }
    printf("\r\nDone writing.");
    addr = 2048;
    printf("\r\nStarting reading process...");


    printf("\r\nReading and checking entire all pages 0-16383: ");
    for (i = 0; i < 2048; i++){
        ReadSPIData(addr, rBuf, sizeof(rBuf));
        // Each read is 256 bytes to page i
        addr += 256;
        // Print page# every 64KB block
        if ((i + 1) % 256 == 0){
            printf(".%d.", i);
        }
        // Validate read == write
        for (j = 0; j < sizeof(rBuf); j++){
            if (wBuf[j] != rBuf[j]){
                printf("\r\nERR: Mismatch at block %d. Wrote: 0x%02x Received: 0x%02x\r\n", j, wBuf[j], rBuf[j]);
                PassFailFlag = 0;
            }
        }
    }
    printf("\r\nDone reading.\r\n");
    // printf("\r\nSize of rBuf %d\r\n", sizeof(rBuf));
    // for (j = 0; j < sizeof(rBuf); ++j){
    //     printf("%x", rBuf[j]);
    // }
    // printf("\r\nSize of wBuf %d\r\n", sizeof(wBuf));
    // for (j = 0; j < sizeof(wBuf); ++j){
    //     printf("%x", wBuf[j]);
    // }
    
    if (PassFailFlag)
        printf("\r\nPASS: Program finished with no errors!");
    else
        printf("\r\nFAIL: Program finished with errors!");




/*************************************************************************************************
**  While loop
*************************************************************************************************/
    printf("\r\nEntering infinite loop of char sending and receiving!");
    while(1){
        printf("\r\nWrite char to SPI: ");
        asdf = getchar();
        putchar(asdf);
        WriteSPIData(0, asdf, 1);
        ReadSPIData(0, asdf, 1);
        printf("\r\nRead char from SPI: %c", asdf);
    }
        

   // programs should NOT exit as there is nothing to Exit TO !!!!!!
   // There is no OS - just press the reset button to end program and call debug
}