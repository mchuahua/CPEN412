; C:\USERS\MARTINCHUA\DESKTOP\CPEN412\CPEN412\LAB5\M68KUSERPROGRAM (DE1).C - Compiled by CC68K  Version 5.00 (c) 1991-2005  Peter J. Fondse
; #include <stdio.h>
; #include <string.h>
; #include <ctype.h>
; //IMPORTANT
; //
; // Uncomment one of the two #defines below
; // Define StartOfExceptionVectorTable as 08030000 if running programs from sram or
; // 0B000000 for running programs from dram
; //
; // In your labs, you will initially start by designing a system with SRam and later move to
; // Dram, so these constants will need to be changed based on the version of the system you have
; // building
; //
; // The working 68k system SOF file posted on canvas that you can use for your pre-lab
; // is based around Dram so #define accordingly before building
; // #define StartOfExceptionVectorTable 0x08030000
; #define StartOfExceptionVectorTable 0x0B000000
; /**********************************************************************************************
; **	Parallel port addresses
; **********************************************************************************************/
; #define PortA   *(volatile unsigned char *)(0x00400000)
; #define PortB   *(volatile unsigned char *)(0x00400002)
; #define PortC   *(volatile unsigned char *)(0x00400004)
; #define PortD   *(volatile unsigned char *)(0x00400006)
; #define PortE   *(volatile unsigned char *)(0x00400008)
; /*********************************************************************************************
; **	Hex 7 seg displays port addresses
; *********************************************************************************************/
; #define HEX_A        *(volatile unsigned char *)(0x00400010)
; #define HEX_B        *(volatile unsigned char *)(0x00400012)
; #define HEX_C        *(volatile unsigned char *)(0x00400014)    // de2 only
; #define HEX_D        *(volatile unsigned char *)(0x00400016)    // de2 only
; /**********************************************************************************************
; **	LCD display port addresses
; **********************************************************************************************/
; #define LCDcommand   *(volatile unsigned char *)(0x00400020)
; #define LCDdata      *(volatile unsigned char *)(0x00400022)
; /********************************************************************************************
; **	Timer Port addresses
; *********************************************************************************************/
; #define Timer1Data      *(volatile unsigned char *)(0x00400030)
; #define Timer1Control   *(volatile unsigned char *)(0x00400032)
; #define Timer1Status    *(volatile unsigned char *)(0x00400032)
; #define Timer2Data      *(volatile unsigned char *)(0x00400034)
; #define Timer2Control   *(volatile unsigned char *)(0x00400036)
; #define Timer2Status    *(volatile unsigned char *)(0x00400036)
; #define Timer3Data      *(volatile unsigned char *)(0x00400038)
; #define Timer3Control   *(volatile unsigned char *)(0x0040003A)
; #define Timer3Status    *(volatile unsigned char *)(0x0040003A)
; #define Timer4Data      *(volatile unsigned char *)(0x0040003C)
; #define Timer4Control   *(volatile unsigned char *)(0x0040003E)
; #define Timer4Status    *(volatile unsigned char *)(0x0040003E)
; /*********************************************************************************************
; **	RS232 port addresses
; *********************************************************************************************/
; #define RS232_Control     *(volatile unsigned char *)(0x00400040)
; #define RS232_Status      *(volatile unsigned char *)(0x00400040)
; #define RS232_TxData      *(volatile unsigned char *)(0x00400042)
; #define RS232_RxData      *(volatile unsigned char *)(0x00400042)
; #define RS232_Baud        *(volatile unsigned char *)(0x00400044)
; /*********************************************************************************************
; **	PIA 1 and 2 port addresses
; *********************************************************************************************/
; #define PIA1_PortA_Data     *(volatile unsigned char *)(0x00400050)         // combined data and data direction register share same address
; #define PIA1_PortA_Control *(volatile unsigned char *)(0x00400052)
; #define PIA1_PortB_Data     *(volatile unsigned char *)(0x00400054)         // combined data and data direction register share same address
; #define PIA1_PortB_Control *(volatile unsigned char *)(0x00400056)
; #define PIA2_PortA_Data     *(volatile unsigned char *)(0x00400060)         // combined data and data direction register share same address
; #define PIA2_PortA_Control *(volatile unsigned char *)(0x00400062)
; #define PIA2_PortB_data     *(volatile unsigned char *)(0x00400064)         // combined data and data direction register share same address
; #define PIA2_PortB_Control *(volatile unsigned char *)(0x00400066)
; /*************************************************************
; ** I2C Controller registers
; **************************************************************/
; // I2C Registers
; #define I2C_PRERLO     (*(volatile unsigned char *)(0x00408000))
; #define I2C_PRERHI     (*(volatile unsigned char *)(0x00408002))
; #define I2C_CTR        (*(volatile unsigned char *)(0x00408004))
; #define I2C_TXR        (*(volatile unsigned char *)(0x00408006))
; #define I2C_RXR        (*(volatile unsigned char *)(0x00408006))
; #define I2C_CR         (*(volatile unsigned char *)(0x00408008))
; #define I2C_SR         (*(volatile unsigned char *)(0x00408008))
; // STA bit == cmd[7], W bit == cmd[4], IACK bit == cmd[0] -> 8'b1001_0001
; #define WSTART 0x91
; // STO bit == cmd[6], W bit == cmd[4] -> 8'b0101_0000
; #define WSTOP 0x50
; // W bit = cmd[4] -> 8'b0001_0000
; #define WRITE 0x10
; // R bit == cmd[5], NACK bit == cmd[3], IACK bit == cmd[0] -> 8'b0010_1001
; #define READNACK 0x29
; // R bit == cmd[5], NACK bit == cmd[3], IACK bit == cmd[0] -> 8'b0010_0001
; #define READACK 0x21
; /*********************************************************************************************************************************
; (( DO NOT initialise global variables here, do it main even if you want 0
; (( it's a limitation of the compiler
; (( YOU HAVE BEEN WARNED
; *********************************************************************************************************************************/
; unsigned int i, x, y, z, PortA_Count;
; unsigned char Timer1Count, Timer2Count, Timer3Count, Timer4Count ;
; /*******************************************************************************************
; ** Function Prototypes
; *******************************************************************************************/
; void Wait1ms(void);
; void Wait3ms(void);
; void Init_LCD(void) ;
; void LCDOutchar(int c);
; void LCDOutMess(char *theMessage);
; void LCDClearln(void);
; void LCDline1Message(char *theMessage);
; void LCDline2Message(char *theMessage);
; int sprintf(char *out, const char *format, ...) ;
; // I2C
; void I2C_Init(void);
; void I2C_Start(void);
; void I2C_Stop(void);
; void WaitForTIPFlagReset(void);
; void WaitForRxACK(void);
; void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo);
; void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo);
; void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, char size);
; void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, char size);
; void GetMemAddr(char *hi, char *lo);
; void GetBank(char *bank);
; void Wait(void);
; /*****************************************************************************************
; **	Interrupt service routine for Timers
; **
; **  Timers 1 - 4 share a common IRQ on the CPU  so this function uses polling to figure
; **  out which timer is producing the interrupt
; **
; *****************************************************************************************/
; // converts hex char to 4 bit binary equiv in range 0000-1111 (0-F)
; // char assumed to be a valid hex char 0-9, a-f, A-F
; char xtod(int c)
; {
       section   code
       xdef      _xtod
_xtod:
       link      A6,#0
       move.l    D2,-(A7)
       move.l    8(A6),D2
; if ((char)(c) <= (char)('9'))
       cmp.b     #57,D2
       bgt.s     xtod_1
; return c - (char)(0x30);    // 0 - 9 = 0x30 - 0x39 so convert to number by sutracting 0x30
       move.b    D2,D0
       sub.b     #48,D0
       bra.s     xtod_3
xtod_1:
; else if((char)(c) > (char)('F'))    // assume lower case
       cmp.b     #70,D2
       ble.s     xtod_4
; return c - (char)(0x57);    // a-f = 0x61-66 so needs to be converted to 0x0A - 0x0F so subtract 0x57
       move.b    D2,D0
       sub.b     #87,D0
       bra.s     xtod_3
xtod_4:
; else
; return c - (char)(0x37);    // A-F = 0x41-46 so needs to be converted to 0x0A - 0x0F so subtract 0x37
       move.b    D2,D0
       sub.b     #55,D0
xtod_3:
       move.l    (A7)+,D2
       unlk      A6
       rts
; }
; int Get2HexDigits(char *CheckSumPtr)
; {
       xdef      _Get2HexDigits
_Get2HexDigits:
       link      A6,#0
       move.l    D2,-(A7)
; register int i = (xtod(_getch()) << 4) | (xtod(_getch()));
       move.l    D0,-(A7)
       jsr       __getch
       move.l    D0,D1
       move.l    (A7)+,D0
       move.l    D1,-(A7)
       jsr       _xtod
       addq.w    #4,A7
       and.l     #255,D0
       asl.l     #4,D0
       move.l    D0,-(A7)
       move.l    D1,-(A7)
       jsr       __getch
       move.l    (A7)+,D1
       move.l    D0,-(A7)
       jsr       _xtod
       addq.w    #4,A7
       move.l    D0,D1
       move.l    (A7)+,D0
       and.l     #255,D1
       or.l      D1,D0
       move.l    D0,D2
; if(CheckSumPtr)
       tst.l     8(A6)
       beq.s     Get2HexDigits_1
; *CheckSumPtr += i ;
       move.l    8(A6),A0
       add.b     D2,(A0)
Get2HexDigits_1:
; return i ;
       move.l    D2,D0
       move.l    (A7)+,D2
       unlk      A6
       rts
; }
; int Get4HexDigits(char *CheckSumPtr)
; {
       xdef      _Get4HexDigits
_Get4HexDigits:
       link      A6,#0
; return (Get2HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
       move.l    8(A6),-(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       asl.l     #8,D0
       move.l    D0,-(A7)
       move.l    8(A6),-(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.l    D0,D1
       move.l    (A7)+,D0
       or.l      D1,D0
       unlk      A6
       rts
; }
; int Get6HexDigits(char *CheckSumPtr)
; {
       xdef      _Get6HexDigits
_Get6HexDigits:
       link      A6,#0
; return (Get4HexDigits(CheckSumPtr) << 8) | (Get2HexDigits(CheckSumPtr));
       move.l    8(A6),-(A7)
       jsr       _Get4HexDigits
       addq.w    #4,A7
       asl.l     #8,D0
       move.l    D0,-(A7)
       move.l    8(A6),-(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.l    D0,D1
       move.l    (A7)+,D0
       or.l      D1,D0
       unlk      A6
       rts
; }
; int Get8HexDigits(char *CheckSumPtr)
; {
       xdef      _Get8HexDigits
_Get8HexDigits:
       link      A6,#0
; return (Get4HexDigits(CheckSumPtr) << 16) | (Get4HexDigits(CheckSumPtr));
       move.l    8(A6),-(A7)
       jsr       _Get4HexDigits
       addq.w    #4,A7
       asl.l     #8,D0
       asl.l     #8,D0
       move.l    D0,-(A7)
       move.l    8(A6),-(A7)
       jsr       _Get4HexDigits
       addq.w    #4,A7
       move.l    D0,D1
       move.l    (A7)+,D0
       or.l      D1,D0
       unlk      A6
       rts
; }
; void Timer_ISR()
; {
       xdef      _Timer_ISR
_Timer_ISR:
; if(Timer1Status == 1) {         // Did Timer 1 produce the Interrupt?
       move.b    4194354,D0
       cmp.b     #1,D0
       bne.s     Timer_ISR_1
; Timer1Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
       move.b    #3,4194354
; PortA = Timer1Count++ ;     // increment an LED count on PortA with each tick of Timer 1
       move.b    _Timer1Count.L,D0
       addq.b    #1,_Timer1Count.L
       move.b    D0,4194304
Timer_ISR_1:
; }
; if(Timer2Status == 1) {         // Did Timer 2 produce the Interrupt?
       move.b    4194358,D0
       cmp.b     #1,D0
       bne.s     Timer_ISR_3
; Timer2Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
       move.b    #3,4194358
; PortC = Timer2Count++ ;     // increment an LED count on PortC with each tick of Timer 2
       move.b    _Timer2Count.L,D0
       addq.b    #1,_Timer2Count.L
       move.b    D0,4194308
Timer_ISR_3:
; }
; if(Timer3Status == 1) {         // Did Timer 3 produce the Interrupt?
       move.b    4194362,D0
       cmp.b     #1,D0
       bne.s     Timer_ISR_5
; Timer3Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
       move.b    #3,4194362
; HEX_A = Timer3Count++ ;     // increment a HEX count on Port HEX_A with each tick of Timer 3
       move.b    _Timer3Count.L,D0
       addq.b    #1,_Timer3Count.L
       move.b    D0,4194320
Timer_ISR_5:
; }
; if(Timer4Status == 1) {         // Did Timer 4 produce the Interrupt?
       move.b    4194366,D0
       cmp.b     #1,D0
       bne.s     Timer_ISR_7
; Timer4Control = 3;      	// reset the timer to clear the interrupt, enable interrupts and allow counter to run
       move.b    #3,4194366
; HEX_B = Timer4Count++ ;     // increment a HEX count on HEX_B with each tick of Timer 4
       move.b    _Timer4Count.L,D0
       addq.b    #1,_Timer4Count.L
       move.b    D0,4194322
Timer_ISR_7:
       rts
; }
; }
; /*****************************************************************************************
; **	Interrupt service routine for ACIA. This device has it's own dedicate IRQ level
; **  Add your code here to poll Status register and clear interrupt
; *****************************************************************************************/
; void ACIA_ISR()
; {}
       xdef      _ACIA_ISR
_ACIA_ISR:
       rts
; /***************************************************************************************
; **	Interrupt service routine for PIAs 1 and 2. These devices share an IRQ level
; **  Add your code here to poll Status register and clear interrupt
; *****************************************************************************************/
; void PIA_ISR()
; {}
       xdef      _PIA_ISR
_PIA_ISR:
       rts
; /***********************************************************************************
; **	Interrupt service routine for Key 2 on DE1 board. Add your own response here
; ************************************************************************************/
; void Key2PressISR()
; {}
       xdef      _Key2PressISR
_Key2PressISR:
       rts
; /***********************************************************************************
; **	Interrupt service routine for Key 1 on DE1 board. Add your own response here
; ************************************************************************************/
; void Key1PressISR()
; {}
       xdef      _Key1PressISR
_Key1PressISR:
       rts
; /************************************************************************************
; **   Delay Subroutine to give the 68000 something useless to do to waste 1 mSec
; ************************************************************************************/
; void Wait1ms(void)
; {
       xdef      _Wait1ms
_Wait1ms:
       move.l    D2,-(A7)
; int  i ;
; for(i = 0; i < 1000; i ++)
       clr.l     D2
Wait1ms_1:
       cmp.l     #1000,D2
       bge.s     Wait1ms_3
       addq.l    #1,D2
       bra       Wait1ms_1
Wait1ms_3:
       move.l    (A7)+,D2
       rts
; ;
; }
; /************************************************************************************
; **  Subroutine to give the 68000 something useless to do to waste 3 mSec
; **************************************************************************************/
; void Wait3ms(void)
; {
       xdef      _Wait3ms
_Wait3ms:
       move.l    D2,-(A7)
; int i ;
; for(i = 0; i < 3; i++)
       clr.l     D2
Wait3ms_1:
       cmp.l     #3,D2
       bge.s     Wait3ms_3
; Wait1ms() ;
       jsr       _Wait1ms
       addq.l    #1,D2
       bra       Wait3ms_1
Wait3ms_3:
       move.l    (A7)+,D2
       rts
; }
; /*********************************************************************************************
; **  Subroutine to initialise the LCD display by writing some commands to the LCD internal registers
; **  Sets it for parallel port and 2 line display mode (if I recall correctly)
; *********************************************************************************************/
; void Init_LCD(void)
; {
       xdef      _Init_LCD
_Init_LCD:
; LCDcommand = 0x0c ;
       move.b    #12,4194336
; Wait3ms() ;
       jsr       _Wait3ms
; LCDcommand = 0x38 ;
       move.b    #56,4194336
; Wait3ms() ;
       jsr       _Wait3ms
       rts
; }
; /*********************************************************************************************
; **  Subroutine to initialise the RS232 Port by writing some commands to the internal registers
; *********************************************************************************************/
; void Init_RS232(void)
; {
       xdef      _Init_RS232
_Init_RS232:
; RS232_Control = 0x15 ; //  %00010101 set up 6850 uses divide by 16 clock, set RTS low, 8 bits no parity, 1 stop bit, transmitter interrupt disabled
       move.b    #21,4194368
; RS232_Baud = 0x1 ;      // program baud rate generator 001 = 115k, 010 = 57.6k, 011 = 38.4k, 100 = 19.2, all others = 9600
       move.b    #1,4194372
       rts
; }
; /*********************************************************************************************************
; **  Subroutine to provide a low level output function to 6850 ACIA
; **  This routine provides the basic functionality to output a single character to the serial Port
; **  to allow the board to communicate with HyperTerminal Program
; **
; **  NOTE you do not call this function directly, instead you call the normal putchar() function
; **  which in turn calls _putch() below). Other functions like puts(), printf() call putchar() so will
; **  call _putch() also
; *********************************************************************************************************/
; int _putch( int c)
; {
       xdef      __putch
__putch:
       link      A6,#0
; while((RS232_Status & (char)(0x02)) != (char)(0x02))    // wait for Tx bit in status register or 6850 serial comms chip to be '1'
_putch_1:
       move.b    4194368,D0
       and.b     #2,D0
       cmp.b     #2,D0
       beq.s     _putch_3
       bra       _putch_1
_putch_3:
; ;
; RS232_TxData = (c & (char)(0x7f));                      // write to the data register to output the character (mask off bit 8 to keep it 7 bit ASCII)
       move.l    8(A6),D0
       and.l     #127,D0
       move.b    D0,4194370
; return c ;                                              // putchar() expects the character to be returned
       move.l    8(A6),D0
       unlk      A6
       rts
; }
; /*********************************************************************************************************
; **  Subroutine to provide a low level input function to 6850 ACIA
; **  This routine provides the basic functionality to input a single character from the serial Port
; **  to allow the board to communicate with HyperTerminal Program Keyboard (your PC)
; **
; **  NOTE you do not call this function directly, instead you call the normal getchar() function
; **  which in turn calls _getch() below). Other functions like gets(), scanf() call getchar() so will
; **  call _getch() also
; *********************************************************************************************************/
; int _getch( void )
; {
       xdef      __getch
__getch:
       link      A6,#-4
; char c ;
; while((RS232_Status & (char)(0x01)) != (char)(0x01))    // wait for Rx bit in 6850 serial comms chip status register to be '1'
_getch_1:
       move.b    4194368,D0
       and.b     #1,D0
       cmp.b     #1,D0
       beq.s     _getch_3
       bra       _getch_1
_getch_3:
; ;
; return (RS232_RxData & (char)(0x7f));                   // read received character, mask off top bit and return as 7 bit ASCII character
       move.b    4194370,D0
       and.l     #255,D0
       and.l     #127,D0
       unlk      A6
       rts
; }
; /******************************************************************************
; **  Subroutine to output a single character to the 2 row LCD display
; **  It is assumed the character is an ASCII code and it will be displayed at the
; **  current cursor position
; *******************************************************************************/
; void LCDOutchar(int c)
; {
       xdef      _LCDOutchar
_LCDOutchar:
       link      A6,#0
; LCDdata = (char)(c);
       move.l    8(A6),D0
       move.b    D0,4194338
; Wait1ms() ;
       jsr       _Wait1ms
       unlk      A6
       rts
; }
; /**********************************************************************************
; *subroutine to output a message at the current cursor position of the LCD display
; ************************************************************************************/
; void LCDOutMessage(char *theMessage)
; {
       xdef      _LCDOutMessage
_LCDOutMessage:
       link      A6,#-4
; char c ;
; while((c = *theMessage++) != 0)     // output characters from the string until NULL
LCDOutMessage_1:
       move.l    8(A6),A0
       addq.l    #1,8(A6)
       move.b    (A0),-1(A6)
       move.b    (A0),D0
       beq.s     LCDOutMessage_3
; LCDOutchar(c) ;
       move.b    -1(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       _LCDOutchar
       addq.w    #4,A7
       bra       LCDOutMessage_1
LCDOutMessage_3:
       unlk      A6
       rts
; }
; /******************************************************************************
; *subroutine to clear the line by issuing 24 space characters
; *******************************************************************************/
; void LCDClearln(void)
; {
       xdef      _LCDClearln
_LCDClearln:
       move.l    D2,-(A7)
; int i ;
; for(i = 0; i < 24; i ++)
       clr.l     D2
LCDClearln_1:
       cmp.l     #24,D2
       bge.s     LCDClearln_3
; LCDOutchar(' ') ;       // write a space char to the LCD display
       pea       32
       jsr       _LCDOutchar
       addq.w    #4,A7
       addq.l    #1,D2
       bra       LCDClearln_1
LCDClearln_3:
       move.l    (A7)+,D2
       rts
; }
; /******************************************************************************
; **  Subroutine to move the LCD cursor to the start of line 1 and clear that line
; *******************************************************************************/
; void LCDLine1Message(char *theMessage)
; {
       xdef      _LCDLine1Message
_LCDLine1Message:
       link      A6,#0
; LCDcommand = 0x80 ;
       move.b    #128,4194336
; Wait3ms();
       jsr       _Wait3ms
; LCDClearln() ;
       jsr       _LCDClearln
; LCDcommand = 0x80 ;
       move.b    #128,4194336
; Wait3ms() ;
       jsr       _Wait3ms
; LCDOutMessage(theMessage) ;
       move.l    8(A6),-(A7)
       jsr       _LCDOutMessage
       addq.w    #4,A7
       unlk      A6
       rts
; }
; /******************************************************************************
; **  Subroutine to move the LCD cursor to the start of line 2 and clear that line
; *******************************************************************************/
; void LCDLine2Message(char *theMessage)
; {
       xdef      _LCDLine2Message
_LCDLine2Message:
       link      A6,#0
; LCDcommand = 0xC0 ;
       move.b    #192,4194336
; Wait3ms();
       jsr       _Wait3ms
; LCDClearln() ;
       jsr       _LCDClearln
; LCDcommand = 0xC0 ;
       move.b    #192,4194336
; Wait3ms() ;
       jsr       _Wait3ms
; LCDOutMessage(theMessage) ;
       move.l    8(A6),-(A7)
       jsr       _LCDOutMessage
       addq.w    #4,A7
       unlk      A6
       rts
; }
; void I2C_Start(void){
       xdef      _I2C_Start
_I2C_Start:
; // Start condition is 8'b1000_0000
; I2C_CR = 0x80;
       move.b    #128,4227080
       rts
; }
; void I2C_Stop(void){
       xdef      _I2C_Stop
_I2C_Stop:
; // Stop condition is 8'b0100_0000
; I2C_CR = 0x40;
       move.b    #64,4227080
       rts
; }
; void I2C_Init(void){
       xdef      _I2C_Init
_I2C_Init:
; // Make sure I2C core is off
; I2C_CTR = (char)0x00;
       clr.b     4227076
; // Prescale register clock, 25Mhz / (5 * 100KHz) - 1  -> 0x0031
; I2C_PRERLO = (char)0x31;
       move.b    #49,4227072
; I2C_PRERHI = (char)0x00;
       clr.b     4227074
; // Enable I2C Core, no interrupt enabled
; // 8'b10xx_xxxx
; I2C_CTR = (char)0x80;
       move.b    #128,4227076
       rts
; }
; void WaitForRxACK(void){
       xdef      _WaitForRxACK
_WaitForRxACK:
; // Read RxACK bit from Status Register, should be '0'
; // Status Register [7] == 0
; while(((I2C_SR >> 7) & 1) == 1){
WaitForRxACK_1:
       move.b    4227080,D0
       lsr.b     #7,D0
       and.b     #1,D0
       cmp.b     #1,D0
       bne.s     WaitForRxACK_3
; }
       bra       WaitForRxACK_1
WaitForRxACK_3:
       rts
; }
; void WaitForTIPFlagReset(void){
       xdef      _WaitForTIPFlagReset
_WaitForTIPFlagReset:
; // Status Register [1] == 0
; while((I2C_SR >> 1) & 1){
WaitForTIPFlagReset_1:
       move.b    4227080,D0
       lsr.b     #1,D0
       and.b     #1,D0
       beq.s     WaitForTIPFlagReset_3
; }
       bra       WaitForTIPFlagReset_1
WaitForTIPFlagReset_3:
       rts
; }
; void Wait(void){
       xdef      _Wait
_Wait:
; WaitForTIPFlagReset();
       jsr       _WaitForTIPFlagReset
; WaitForRxACK();
       jsr       _WaitForRxACK
       rts
; }
; void TransmitI2C(char data, char cr){
       xdef      _TransmitI2C
_TransmitI2C:
       link      A6,#0
; I2C_TXR = data;
       move.b    11(A6),4227078
; I2C_CR = cr;
       move.b    15(A6),4227080
; Wait();
       jsr       _Wait
       unlk      A6
       rts
; }
; // Assumptions: addr is at most 7 bits wide
; void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo){
       xdef      _WriteI2CChar
_WriteI2CChar:
       link      A6,#0
       movem.l   A2/A3,-(A7)
       lea       _TransmitI2C.L,A2
       lea       _printf.L,A3
; printf("\r\n data is %d\r\n", data);
       move.b    11(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_1.L
       jsr       (A3)
       addq.w    #8,A7
; printf("\r\n slaveaddr %x\r\n", slave_addr);
       move.b    15(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_2.L
       jsr       (A3)
       addq.w    #8,A7
; printf("\r\n memaddrhi is %d\r\n", memaddr_hi);
       move.b    19(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_3.L
       jsr       (A3)
       addq.w    #8,A7
; printf("\r\n memaddrlo is %d\r\n", memaddr_lo);
       move.b    23(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_4.L
       jsr       (A3)
       addq.w    #8,A7
; // Check before doing anything
; WaitForTIPFlagReset();
       jsr       _WaitForTIPFlagReset
; // Set slave to write mode, Generate start command
; TransmitI2C(slave_addr, WSTART);
       pea       145
       move.b    15(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Write Mem Address and set W bit
; TransmitI2C(memaddr_hi, WRITE);
       pea       16
       move.b    19(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(memaddr_lo, WRITE);
       pea       16
       move.b    23(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Write data transmit register, set STO bit, set W bit. 
; TransmitI2C(data, WSTOP);
       pea       80
       move.b    11(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
       movem.l   (A7)+,A2/A3
       unlk      A6
       rts
; }
; // Assumptions: slave_addr is in write mode
; void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo){
       xdef      _ReadI2CChar
_ReadI2CChar:
       link      A6,#0
       move.l    A2,-(A7)
       lea       _TransmitI2C.L,A2
; // Check before doing anything
; WaitForTIPFlagReset();
       jsr       _WaitForTIPFlagReset
; // Set slave to write mode, Generate start command
; TransmitI2C(slave_addr, WSTART);
       pea       145
       move.b    15(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Write Mem Address and set W bit
; TransmitI2C(memaddr_hi, WRITE);
       pea       16
       move.b    19(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(memaddr_lo, WRITE);
       pea       16
       move.b    23(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Set slave to read mode and generate start command for reading
; TransmitI2C(slave_addr | 1, WSTART);
       pea       145
       move.b    15(A6),D1
       or.b      #1,D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Read data transmit register, set R bit, set NACK and IACK
; I2C_CR = READNACK;
       move.b    #41,4227080
; // Wait for read data to come in
; while((I2C_SR & 1) !=1){}
ReadI2CChar_1:
       move.b    4227080,D0
       and.b     #1,D0
       cmp.b     #1,D0
       beq.s     ReadI2CChar_3
       bra       ReadI2CChar_1
ReadI2CChar_3:
; *data = I2C_RXR;
       move.l    8(A6),A0
       move.b    4227078,(A0)
; I2C_CR = 0x41;
       move.b    #65,4227080
       move.l    (A7)+,A2
       unlk      A6
       rts
; }
; // Increment logic, handles bank overflows
; void incrI2C(char *slave_addr, char *memaddr_hi, char *memaddr_lo, int read){
       xdef      _incrI2C
_incrI2C:
       link      A6,#0
       movem.l   D2/D3/D4/D5/D6/A2/A3,-(A7)
       move.l    8(A6),D2
       move.l    16(A6),D3
       lea       _TransmitI2C.L,A2
       move.l    12(A6),D4
       lea       _printf.L,A3
; char zeewo = 0x00;
       clr.b     D5
; char one = 0x01;
       moveq     #1,D6
; printf("\r\n %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
       move.l    D3,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.l    D4,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.l    D2,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_5.L
       jsr       (A3)
       add.w     #16,A7
; if ((*memaddr_lo & 0xFF) == 0xFF){
       move.l    D3,A0
       move.b    (A0),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne       incrI2C_1
; if ((*memaddr_hi & 0xFF) == 0xFF){ // Change bank
       move.l    D4,A0
       move.b    (A0),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne       incrI2C_3
; *slave_addr = ((*slave_addr & 0x8) == 0x8) ? 0xA0 : 0xA8;
       move.l    D2,A0
       move.b    (A0),D0
       and.b     #8,D0
       cmp.b     #8,D0
       bne.s     incrI2C_5
       move.w    #160,D0
       bra.s     incrI2C_6
incrI2C_5:
       move.w    #168,D0
incrI2C_6:
       move.l    D2,A0
       move.b    D0,(A0)
; *memaddr_lo = zeewo;
       move.l    D3,A0
       move.b    D5,(A0)
; *memaddr_hi = zeewo;
       move.l    D4,A0
       move.b    D5,(A0)
; if(read & 1){
       move.l    20(A6),D0
       and.l     #1,D0
       beq       incrI2C_7
; printf("\r\n Changing slave and restart %x", *slave_addr);
       move.l    D2,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_6.L
       jsr       (A3)
       addq.w    #8,A7
; TransmitI2C(*slave_addr, WSTART);
       pea       145
       move.l    D2,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(*memaddr_hi, WRITE);
       pea       16
       move.l    D4,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(*memaddr_lo, WRITE);
       pea       16
       move.l    D3,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(*slave_addr | 1, WSTART);
       pea       145
       move.l    D2,A0
       move.b    (A0),D1
       or.b      #1,D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
       bra       incrI2C_8
incrI2C_7:
; }
; else{
; TransmitI2C(*slave_addr, WSTART);
       pea       145
       move.l    D2,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(*memaddr_hi, WRITE);
       pea       16
       move.l    D4,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(*memaddr_lo, WRITE);
       pea       16
       move.l    D3,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
incrI2C_8:
; }
; printf("\r\n HIT EDGE CASE %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
       move.l    D3,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.l    D4,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.l    D2,A0
       move.b    (A0),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_7.L
       jsr       (A3)
       add.w     #16,A7
       bra.s     incrI2C_4
incrI2C_3:
; }
; else{
; *memaddr_hi += one;
       move.l    D4,A0
       add.b     D6,(A0)
; *memaddr_lo = zeewo;
       move.l    D3,A0
       move.b    D5,(A0)
incrI2C_4:
       bra.s     incrI2C_2
incrI2C_1:
; }
; }
; else{
; *memaddr_lo += one;
       move.l    D3,A0
       add.b     D6,(A0)
incrI2C_2:
       movem.l   (A7)+,D2/D3/D4/D5/D6/A2/A3
       unlk      A6
       rts
; }
; }
; void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, char size){
       xdef      _WriteI2CPage
_WriteI2CPage:
       link      A6,#0
       movem.l   D2/A2,-(A7)
       lea       _TransmitI2C.L,A2
; // Data is array of size "size", max size 128
; // WriteI2CChar();
; char i;
; // Check before doing anything
; WaitForTIPFlagReset();
       jsr       _WaitForTIPFlagReset
; // Set slave to write mode, Generate start command
; TransmitI2C(slave_addr, WSTART);
       pea       145
       move.b    11(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Write Mem Address and set W bit
; TransmitI2C(memaddr_hi, WRITE);
       pea       16
       move.b    15(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(memaddr_lo, WRITE);
       pea       16
       move.b    19(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; for (i = 0; i < size; i++){
       clr.b     D2
WriteI2CPage_1:
       cmp.b     23(A6),D2
       bge       WriteI2CPage_3
; // Write without stop unless we're at the end
; if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF)){
       move.b    23(A6),D0
       subq.b    #1,D0
       cmp.b     D0,D2
       beq.s     WriteI2CPage_6
       move.b    15(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     WriteI2CPage_4
       move.b    19(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     WriteI2CPage_4
WriteI2CPage_6:
; TransmitI2C(i,WSTOP);
       pea       80
       ext.w     D2
       ext.l     D2
       move.l    D2,-(A7)
       jsr       (A2)
       addq.w    #8,A7
       bra.s     WriteI2CPage_5
WriteI2CPage_4:
; }
; else{
; TransmitI2C(i,WRITE);
       pea       16
       ext.w     D2
       ext.l     D2
       move.l    D2,-(A7)
       jsr       (A2)
       addq.w    #8,A7
WriteI2CPage_5:
; }
; // Page crossing, if it happens, is handled inside incrI2C
; incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 0);
       clr.l     -(A7)
       pea       19(A6)
       pea       15(A6)
       pea       11(A6)
       jsr       _incrI2C
       add.w     #16,A7
       addq.b    #1,D2
       bra       WriteI2CPage_1
WriteI2CPage_3:
       movem.l   (A7)+,D2/A2
       unlk      A6
       rts
; }
; }
; void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, char size){
       xdef      _ReadI2CPage
_ReadI2CPage:
       link      A6,#-4
       movem.l   D2/D3/A2,-(A7)
       lea       _TransmitI2C.L,A2
       move.b    27(A6),D3
       ext.w     D3
       ext.l     D3
; // Data is array of size "size", max size 128
; char i;
; char temp = 0;
       clr.b     -1(A6)
; // Check before doing anything
; WaitForTIPFlagReset();
       jsr       _WaitForTIPFlagReset
; // Set slave to write mode, Generate start command
; TransmitI2C(slave_addr, WSTART);
       pea       145
       move.b    15(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Write Mem Address and set W bit
; TransmitI2C(memaddr_hi, WRITE);
       pea       16
       move.b    19(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; TransmitI2C(memaddr_lo, WRITE);
       pea       16
       move.b    23(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; // Set slave to read mode and generate start command for reading
; TransmitI2C(slave_addr | 1, WSTART);
       pea       145
       move.b    15(A6),D1
       or.b      #1,D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       (A2)
       addq.w    #8,A7
; for (i = 0; i < size; i++){
       clr.b     D2
ReadI2CPage_1:
       cmp.b     D3,D2
       bge       ReadI2CPage_3
; // Read without NACK unless we're at the end
; if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF)){
       move.b    D3,D0
       subq.b    #1,D0
       cmp.b     D0,D2
       beq.s     ReadI2CPage_6
       move.b    19(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     ReadI2CPage_4
       move.b    23(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     ReadI2CPage_4
ReadI2CPage_6:
; I2C_CR = READNACK;
       move.b    #41,4227080
       bra.s     ReadI2CPage_5
ReadI2CPage_4:
; }
; else
; I2C_CR = READACK;
       move.b    #33,4227080
ReadI2CPage_5:
; while((I2C_SR & 1) !=1){}
ReadI2CPage_7:
       move.b    4227080,D0
       and.b     #1,D0
       cmp.b     #1,D0
       beq.s     ReadI2CPage_9
       bra       ReadI2CPage_7
ReadI2CPage_9:
; data[i] = I2C_RXR;
       move.l    8(A6),A0
       ext.w     D2
       ext.l     D2
       move.b    4227078,0(A0,D2.L)
; if (i == (size-1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF))
       move.b    D3,D0
       subq.b    #1,D0
       cmp.b     D0,D2
       beq.s     ReadI2CPage_12
       move.b    19(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     ReadI2CPage_10
       move.b    23(A6),D0
       ext.w     D0
       and.w     #255,D0
       cmp.w     #255,D0
       bne.s     ReadI2CPage_10
ReadI2CPage_12:
; I2C_CR = 0x41;
       move.b    #65,4227080
       bra.s     ReadI2CPage_11
ReadI2CPage_10:
; else
; I2C_CR = 0x01;
       move.b    #1,4227080
ReadI2CPage_11:
; // Page crossing, if it happens, is handled inside incrI2C
; incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 1);
       pea       1
       pea       23(A6)
       pea       19(A6)
       pea       15(A6)
       jsr       _incrI2C
       add.w     #16,A7
       addq.b    #1,D2
       bra       ReadI2CPage_1
ReadI2CPage_3:
       movem.l   (A7)+,D2/D3/A2
       unlk      A6
       rts
; }
; }
; void GetBank(char *bank){
       xdef      _GetBank
_GetBank:
       link      A6,#0
       move.l    D2,-(A7)
; char asdf;
; while(1){
GetBank_1:
; printf("\r\nSelect bank:\r\n0 - Bank 0\r\n1 - Bank 1");
       pea       @m68kus~1_8.L
       jsr       _printf
       addq.w    #4,A7
; asdf = getchar();
       jsr       _getch
       move.b    D0,D2
; putchar(asdf);
       ext.w     D2
       ext.l     D2
       move.l    D2,-(A7)
       jsr       _putch
       addq.w    #4,A7
; if(asdf == '0'){
       cmp.b     #48,D2
       bne.s     GetBank_4
; *bank = 0xA0;
       move.l    8(A6),A0
       move.b    #160,(A0)
; break;
       bra.s     GetBank_3
GetBank_4:
; }
; else if (asdf == '1'){
       cmp.b     #49,D2
       bne.s     GetBank_6
; *bank = 0xA8;
       move.l    8(A6),A0
       move.b    #168,(A0)
; break;
       bra.s     GetBank_3
GetBank_6:
; }
; else{
; printf("\r\nInvalid selection.");
       pea       @m68kus~1_9.L
       jsr       _printf
       addq.w    #4,A7
       bra       GetBank_1
GetBank_3:
       move.l    (A7)+,D2
       unlk      A6
       rts
; }
; }       
; }
; void GetMemAddr(char *hi, char *lo){
       xdef      _GetMemAddr
_GetMemAddr:
       link      A6,#0
; printf("\r\nEnter mem address hi:");
       pea       @m68kus~1_10.L
       jsr       _printf
       addq.w    #4,A7
; *hi = Get2HexDigits(0);
       clr.l     -(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.l    8(A6),A0
       move.b    D0,(A0)
; printf("\r\nEnter mem address lo:");
       pea       @m68kus~1_11.L
       jsr       _printf
       addq.w    #4,A7
; *lo = Get2HexDigits(0);
       clr.l     -(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.l    12(A6),A0
       move.b    D0,(A0)
       unlk      A6
       rts
; // TODO: Hex digit validation?
; }
; /*********************************************************************************************************************************
; **  IMPORTANT FUNCTION
; **  This function install an exception handler so you can capture and deal with any 68000 exception in your program
; **  You pass it the name of a function in your code that will get called in response to the exception (as the 1st parameter)
; **  and in the 2nd parameter, you pass it the exception number that you want to take over (see 68000 exceptions for details)
; **  Calling this function allows you to deal with Interrupts for example
; ***********************************************************************************************************************************/
; void InstallExceptionHandler( void (*function_ptr)(), int level)
; {
       xdef      _InstallExceptionHandler
_InstallExceptionHandler:
       link      A6,#-4
; volatile long int *RamVectorAddress = (volatile long int *)(StartOfExceptionVectorTable) ;   // pointer to the Ram based interrupt vector table created in Cstart in debug monitor
       move.l    #184549376,-4(A6)
; RamVectorAddress[level] = (long int *)(function_ptr);                       // install the address of our function into the exception table
       move.l    -4(A6),A0
       move.l    12(A6),D0
       lsl.l     #2,D0
       move.l    8(A6),0(A0,D0.L)
       unlk      A6
       rts
; }
; /******************************************************************************************************************************
; * Start of user program
; ******************************************************************************************************************************/
; void main()
; {
       xdef      _main
_main:
       link      A6,#-824
       movem.l   D2/D3/A2/A3/A4/A5,-(A7)
       lea       _printf.L,A2
       lea       _InstallExceptionHandler.L,A3
       lea       -652(A6),A4
       lea       _GetMemAddr.L,A5
; unsigned int row, j=0, count=0, counter1=1;
       clr.l     -818(A6)
       clr.l     -814(A6)
       move.l    #1,-810(A6)
; char i = 0;
       clr.b     D2
; char c, text[150] ;
; unsigned char asdf, asdf1, asdf2, asdf3, bank;
; unsigned char wBuf[256];
; unsigned char rBuf[256];
; int addr = 2048;
       move.l    #2048,-136(A6)
; char size;
; char data[128];
; int PassFailFlag = 1 ;
       move.l    #1,-4(A6)
; i = x = y = z = PortA_Count =0;
       clr.l     _PortA_Count.L
       clr.l     _z.L
       clr.l     _y.L
       clr.l     _x.L
       clr.b     D2
; Timer1Count = Timer2Count = Timer3Count = Timer4Count = 0;
       clr.b     _Timer4Count.L
       clr.b     _Timer3Count.L
       clr.b     _Timer2Count.L
       clr.b     _Timer1Count.L
; InstallExceptionHandler(PIA_ISR, 25) ;          // install interrupt handler for PIAs 1 and 2 on level 1 IRQ
       pea       25
       pea       _PIA_ISR.L
       jsr       (A3)
       addq.w    #8,A7
; InstallExceptionHandler(ACIA_ISR, 26) ;		    // install interrupt handler for ACIA on level 2 IRQ
       pea       26
       pea       _ACIA_ISR.L
       jsr       (A3)
       addq.w    #8,A7
; InstallExceptionHandler(Timer_ISR, 27) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
       pea       27
       pea       _Timer_ISR.L
       jsr       (A3)
       addq.w    #8,A7
; InstallExceptionHandler(Key2PressISR, 28) ;	    // install interrupt handler for Key Press 2 on DE1 board for level 4 IRQ
       pea       28
       pea       _Key2PressISR.L
       jsr       (A3)
       addq.w    #8,A7
; InstallExceptionHandler(Key1PressISR, 29) ;	    // install interrupt handler for Key Press 1 on DE1 board for level 5 IRQ
       pea       29
       pea       _Key1PressISR.L
       jsr       (A3)
       addq.w    #8,A7
; Timer1Data = 0x10;		// program time delay into timers 1-4
       move.b    #16,4194352
; Timer2Data = 0x20;
       move.b    #32,4194356
; Timer3Data = 0x15;
       move.b    #21,4194360
; Timer4Data = 0x25;
       move.b    #37,4194364
; Timer1Control = 3;		// write 3 to control register to Bit0 = 1 (enable interrupt from timers) 1 - 4 and allow them to count Bit 1 = 1
       move.b    #3,4194354
; Timer2Control = 3;
       move.b    #3,4194358
; Timer3Control = 3;
       move.b    #3,4194362
; Timer4Control = 3;
       move.b    #3,4194366
; Init_LCD();             // initialise the LCD display to use a parallel data interface and 2 lines of display
       jsr       _Init_LCD
; Init_RS232() ;          // initialise the RS232 port for use with hyper terminal
       jsr       _Init_RS232
; /*************************************************************************************************
; **  I2C Program Lab 5
; *************************************************************************************************/
; I2C_Init();
       jsr       _I2C_Init
; // printf("%x\n", I2C_PRERLO);
; // printf("%x\n", I2C_PRERHI);
; // printf("%x\n", I2C_CTR);
; // printf("%x\n", I2C_TXR);
; // printf("%x\n", I2C_RXR);
; // printf("%x\n", I2C_CR);
; // printf("%x\n", I2C_SR);
; printf("\r\nLab 5: I2C");
       pea       @m68kus~1_12.L
       jsr       (A2)
       addq.w    #4,A7
; while(1){
main_1:
; printf("\r\nChoose the following:\r\n0 - Write Byte\r\n1 - Read Byte\r\n2 - Page Write\r\n3 - Page Read\r\n4 - ADC stuff todo\r\n");
       pea       @m68kus~1_13.L
       jsr       (A2)
       addq.w    #4,A7
; asdf = getchar();
       jsr       _getch
       move.b    D0,-653(A6)
; putchar(asdf);
       move.b    -653(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       jsr       _putch
       addq.w    #4,A7
; if(asdf == '0'){
       move.b    -653(A6),D0
       cmp.b     #48,D0
       bne       main_4
; printf("\r\nEnter byte to write: ");
       pea       @m68kus~1_14.L
       jsr       (A2)
       addq.w    #4,A7
; asdf = Get2HexDigits(0);
       clr.l     -(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.b    D0,-653(A6)
; // Bank Selection
; GetBank(&bank);
       pea       -649(A6)
       jsr       _GetBank
       addq.w    #4,A7
; // Mem Address Selection
; GetMemAddr(&asdf2, &asdf1);
       move.l    A4,-(A7)
       pea       -651(A6)
       jsr       (A5)
       addq.w    #8,A7
; WriteI2CChar(asdf, bank, asdf2, asdf1);
       move.b    (A4),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -653(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       _WriteI2CChar
       add.w     #16,A7
; printf("\r\nWritten %x to bank %d at memaddr hi: %x mem addr lo: %x", asdf, bank == 0xA0 ? 0:1, asdf2, asdf1);
       move.b    (A4),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       and.w     #255,D1
       cmp.w     #160,D1
       bne.s     main_6
       clr.b     D1
       bra.s     main_7
main_6:
       moveq     #1,D1
main_7:
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -653(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       pea       @m68kus~1_15.L
       jsr       (A2)
       add.w     #20,A7
       bra       main_27
main_4:
; }
; else if(asdf == '1'){
       move.b    -653(A6),D0
       cmp.b     #49,D0
       bne       main_8
; // Bank Selection
; GetBank(&bank);
       pea       -649(A6)
       jsr       _GetBank
       addq.w    #4,A7
; // Mem Address Selection
; GetMemAddr(&asdf, &asdf1);
       move.l    A4,-(A7)
       pea       -653(A6)
       jsr       (A5)
       addq.w    #8,A7
; ReadI2CChar(&asdf3, bank, asdf, asdf1);
       move.b    (A4),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -653(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       -650(A6)
       jsr       _ReadI2CChar
       add.w     #16,A7
; printf("\r\nRead %x from bank %d at memaddr hi: %x mem addr lo: %x", asdf3, bank == 0xA0 ? 0:1, asdf, asdf1);
       move.b    (A4),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -653(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       and.w     #255,D1
       cmp.w     #160,D1
       bne.s     main_10
       clr.b     D1
       bra.s     main_11
main_10:
       moveq     #1,D1
main_11:
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -650(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       pea       @m68kus~1_16.L
       jsr       (A2)
       add.w     #20,A7
       bra       main_27
main_8:
; }
; else if(asdf == '2'){
       move.b    -653(A6),D0
       cmp.b     #50,D0
       bne       main_12
; printf("\r\nbruh");printf("\r\nEnter size of page to write(max 128 in hex == 0x7F): ");
       pea       @m68kus~1_17.L
       jsr       (A2)
       addq.w    #4,A7
       pea       @m68kus~1_18.L
       jsr       (A2)
       addq.w    #4,A7
; size = Get2HexDigits(0);
       clr.l     -(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.b    D0,D3
; // Bank Selection
; GetBank(&bank);
       pea       -649(A6)
       jsr       _GetBank
       addq.w    #4,A7
; // Mem Address Selection
; GetMemAddr(&asdf2, &asdf1);
       move.l    A4,-(A7)
       pea       -651(A6)
       jsr       (A5)
       addq.w    #8,A7
; WriteI2CPage(bank, asdf2, asdf1,size);
       ext.w     D3
       ext.l     D3
       move.l    D3,-(A7)
       move.b    (A4),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       jsr       _WriteI2CPage
       add.w     #16,A7
; printf("\r\nWrote values starting at memaddr 0x%x%x and bank %d, total size: %x\r\nData: ", asdf2, asdf1, bank == 0xA0 ? 0:1,size);
       ext.w     D3
       ext.l     D3
       move.l    D3,-(A7)
       move.b    -649(A6),D1
       and.w     #255,D1
       cmp.w     #160,D1
       bne.s     main_14
       clr.b     D1
       bra.s     main_15
main_14:
       moveq     #1,D1
main_15:
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    (A4),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       pea       @m68kus~1_19.L
       jsr       (A2)
       add.w     #20,A7
; for(i = 0; i < size; i++){
       clr.b     D2
main_16:
       cmp.b     D3,D2
       bge.s     main_18
; printf("%x", i);
       ext.w     D2
       ext.l     D2
       move.l    D2,-(A7)
       pea       @m68kus~1_20.L
       jsr       (A2)
       addq.w    #8,A7
       addq.b    #1,D2
       bra       main_16
main_18:
       bra       main_27
main_12:
; }
; }
; else if(asdf == '3'){
       move.b    -653(A6),D0
       cmp.b     #51,D0
       bne       main_19
; printf("\r\nbruh");printf("\r\nEnter size of page to read(max 128 in hex == 0x7F): ");
       pea       @m68kus~1_17.L
       jsr       (A2)
       addq.w    #4,A7
       pea       @m68kus~1_21.L
       jsr       (A2)
       addq.w    #4,A7
; size = Get2HexDigits(0);
       clr.l     -(A7)
       jsr       _Get2HexDigits
       addq.w    #4,A7
       move.b    D0,D3
; // Bank Selection
; GetBank(&bank);
       pea       -649(A6)
       jsr       _GetBank
       addq.w    #4,A7
; // Mem Address Selection
; GetMemAddr(&asdf2, &asdf1);
       move.l    A4,-(A7)
       pea       -651(A6)
       jsr       (A5)
       addq.w    #8,A7
; ReadI2CPage(data, bank, asdf2, asdf1,size);
       ext.w     D3
       ext.l     D3
       move.l    D3,-(A7)
       move.b    (A4),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    -649(A6),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       -132(A6)
       jsr       _ReadI2CPage
       add.w     #20,A7
; printf("\r\nRead values starting at memaddr 0x%x%x and bank %d, total size: %x\r\nData: ", asdf2, asdf1, bank == 0xA0 ? 0:1,size);
       ext.w     D3
       ext.l     D3
       move.l    D3,-(A7)
       move.b    -649(A6),D1
       and.w     #255,D1
       cmp.w     #160,D1
       bne.s     main_21
       clr.b     D1
       bra.s     main_22
main_21:
       moveq     #1,D1
main_22:
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       move.b    (A4),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       move.b    -651(A6),D1
       and.l     #255,D1
       move.l    D1,-(A7)
       pea       @m68kus~1_22.L
       jsr       (A2)
       add.w     #20,A7
; for(i = 0; i < size; i++){
       clr.b     D2
main_23:
       cmp.b     D3,D2
       bge.s     main_25
; printf("%x", data[i]);
       ext.w     D2
       ext.l     D2
       lea       -132(A6),A0
       move.b    0(A0,D2.L),D1
       ext.w     D1
       ext.l     D1
       move.l    D1,-(A7)
       pea       @m68kus~1_20.L
       jsr       (A2)
       addq.w    #8,A7
       addq.b    #1,D2
       bra       main_23
main_25:
       bra.s     main_27
main_19:
; }
; }
; else if(asdf == '4'){
       move.b    -653(A6),D0
       cmp.b     #52,D0
       bne.s     main_26
; printf("\r\nbruh");
       pea       @m68kus~1_17.L
       jsr       (A2)
       addq.w    #4,A7
       bra.s     main_27
main_26:
; }
; else{
; printf("\r\nInvalid Selection.\r\n");
       pea       @m68kus~1_23.L
       jsr       (A2)
       addq.w    #4,A7
; continue;
       bra       main_27
main_27:
       bra       main_1
; }   
; }
; }
       section   const
@m68kus~1_1:
       dc.b      13,10,32,100,97,116,97,32,105,115,32,37,100
       dc.b      13,10,0
@m68kus~1_2:
       dc.b      13,10,32,115,108,97,118,101,97,100,100,114,32
       dc.b      37,120,13,10,0
@m68kus~1_3:
       dc.b      13,10,32,109,101,109,97,100,100,114,104,105
       dc.b      32,105,115,32,37,100,13,10,0
@m68kus~1_4:
       dc.b      13,10,32,109,101,109,97,100,100,114,108,111
       dc.b      32,105,115,32,37,100,13,10,0
@m68kus~1_5:
       dc.b      13,10,32,37,120,32,37,120,37,120,0
@m68kus~1_6:
       dc.b      13,10,32,67,104,97,110,103,105,110,103,32,115
       dc.b      108,97,118,101,32,97,110,100,32,114,101,115
       dc.b      116,97,114,116,32,37,120,0
@m68kus~1_7:
       dc.b      13,10,32,72,73,84,32,69,68,71,69,32,67,65,83
       dc.b      69,32,37,120,32,37,120,37,120,0
@m68kus~1_8:
       dc.b      13,10,83,101,108,101,99,116,32,98,97,110,107
       dc.b      58,13,10,48,32,45,32,66,97,110,107,32,48,13
       dc.b      10,49,32,45,32,66,97,110,107,32,49,0
@m68kus~1_9:
       dc.b      13,10,73,110,118,97,108,105,100,32,115,101,108
       dc.b      101,99,116,105,111,110,46,0
@m68kus~1_10:
       dc.b      13,10,69,110,116,101,114,32,109,101,109,32,97
       dc.b      100,100,114,101,115,115,32,104,105,58,0
@m68kus~1_11:
       dc.b      13,10,69,110,116,101,114,32,109,101,109,32,97
       dc.b      100,100,114,101,115,115,32,108,111,58,0
@m68kus~1_12:
       dc.b      13,10,76,97,98,32,53,58,32,73,50,67,0
@m68kus~1_13:
       dc.b      13,10,67,104,111,111,115,101,32,116,104,101
       dc.b      32,102,111,108,108,111,119,105,110,103,58,13
       dc.b      10,48,32,45,32,87,114,105,116,101,32,66,121
       dc.b      116,101,13,10,49,32,45,32,82,101,97,100,32,66
       dc.b      121,116,101,13,10,50,32,45,32,80,97,103,101
       dc.b      32,87,114,105,116,101,13,10,51,32,45,32,80,97
       dc.b      103,101,32,82,101,97,100,13,10,52,32,45,32,65
       dc.b      68,67,32,115,116,117,102,102,32,116,111,100
       dc.b      111,13,10,0
@m68kus~1_14:
       dc.b      13,10,69,110,116,101,114,32,98,121,116,101,32
       dc.b      116,111,32,119,114,105,116,101,58,32,0
@m68kus~1_15:
       dc.b      13,10,87,114,105,116,116,101,110,32,37,120,32
       dc.b      116,111,32,98,97,110,107,32,37,100,32,97,116
       dc.b      32,109,101,109,97,100,100,114,32,104,105,58
       dc.b      32,37,120,32,109,101,109,32,97,100,100,114,32
       dc.b      108,111,58,32,37,120,0
@m68kus~1_16:
       dc.b      13,10,82,101,97,100,32,37,120,32,102,114,111
       dc.b      109,32,98,97,110,107,32,37,100,32,97,116,32
       dc.b      109,101,109,97,100,100,114,32,104,105,58,32
       dc.b      37,120,32,109,101,109,32,97,100,100,114,32,108
       dc.b      111,58,32,37,120,0
@m68kus~1_17:
       dc.b      13,10,98,114,117,104,0
@m68kus~1_18:
       dc.b      13,10,69,110,116,101,114,32,115,105,122,101
       dc.b      32,111,102,32,112,97,103,101,32,116,111,32,119
       dc.b      114,105,116,101,40,109,97,120,32,49,50,56,32
       dc.b      105,110,32,104,101,120,32,61,61,32,48,120,55
       dc.b      70,41,58,32,0
@m68kus~1_19:
       dc.b      13,10,87,114,111,116,101,32,118,97,108,117,101
       dc.b      115,32,115,116,97,114,116,105,110,103,32,97
       dc.b      116,32,109,101,109,97,100,100,114,32,48,120
       dc.b      37,120,37,120,32,97,110,100,32,98,97,110,107
       dc.b      32,37,100,44,32,116,111,116,97,108,32,115,105
       dc.b      122,101,58,32,37,120,13,10,68,97,116,97,58,32
       dc.b      0
@m68kus~1_20:
       dc.b      37,120,0
@m68kus~1_21:
       dc.b      13,10,69,110,116,101,114,32,115,105,122,101
       dc.b      32,111,102,32,112,97,103,101,32,116,111,32,114
       dc.b      101,97,100,40,109,97,120,32,49,50,56,32,105
       dc.b      110,32,104,101,120,32,61,61,32,48,120,55,70
       dc.b      41,58,32,0
@m68kus~1_22:
       dc.b      13,10,82,101,97,100,32,118,97,108,117,101,115
       dc.b      32,115,116,97,114,116,105,110,103,32,97,116
       dc.b      32,109,101,109,97,100,100,114,32,48,120,37,120
       dc.b      37,120,32,97,110,100,32,98,97,110,107,32,37
       dc.b      100,44,32,116,111,116,97,108,32,115,105,122
       dc.b      101,58,32,37,120,13,10,68,97,116,97,58,32,0
@m68kus~1_23:
       dc.b      13,10,73,110,118,97,108,105,100,32,83,101,108
       dc.b      101,99,116,105,111,110,46,13,10,0
       section   bss
       xdef      _i
_i:
       ds.b      4
       xdef      _x
_x:
       ds.b      4
       xdef      _y
_y:
       ds.b      4
       xdef      _z
_z:
       ds.b      4
       xdef      _PortA_Count
_PortA_Count:
       ds.b      4
       xdef      _Timer1Count
_Timer1Count:
       ds.b      1
       xdef      _Timer2Count
_Timer2Count:
       ds.b      1
       xdef      _Timer3Count
_Timer3Count:
       ds.b      1
       xdef      _Timer4Count
_Timer4Count:
       ds.b      1
       xref      _putch
       xref      _getch
       xref      _printf
