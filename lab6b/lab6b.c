#include <stdio.h>
#include <string.h>
#include <ctype.h>

// IMPORTANT
//
//  Uncomment one of the two #defines below
//  Define StartOfExceptionVectorTable as 08030000 if running programs from sram or
//  0B000000 for running programs from dram
//
//  In your labs, you will initially start by designing a system with SRam and later move to
//  Dram, so these constants will need to be changed based on the version of the system you have
//  building
//
//  The working 68k system SOF file posted on canvas that you can use for your pre-lab
//  is based around Dram so #define accordingly before building

// #define StartOfExceptionVectorTable 0x08030000
#define StartOfExceptionVectorTable 0x0B000000

/**********************************************************************************************
**	Parallel port addresses
**********************************************************************************************/

#define PortA *(volatile unsigned char *)(0x00400000)
#define PortB *(volatile unsigned char *)(0x00400002)
#define PortC *(volatile unsigned char *)(0x00400004)
#define PortD *(volatile unsigned char *)(0x00400006)
#define PortE *(volatile unsigned char *)(0x00400008)

/*********************************************************************************************
**	Hex 7 seg displays port addresses
*********************************************************************************************/

#define HEX_A *(volatile unsigned char *)(0x00400010)
#define HEX_B *(volatile unsigned char *)(0x00400012)
#define HEX_C *(volatile unsigned char *)(0x00400014) // de2 only
#define HEX_D *(volatile unsigned char *)(0x00400016) // de2 only

/**********************************************************************************************
**	LCD display port addresses
**********************************************************************************************/

#define LCDcommand *(volatile unsigned char *)(0x00400020)
#define LCDdata *(volatile unsigned char *)(0x00400022)

/********************************************************************************************
**	Timer Port addresses
*********************************************************************************************/

#define Timer1Data *(volatile unsigned char *)(0x00400030)
#define Timer1Control *(volatile unsigned char *)(0x00400032)
#define Timer1Status *(volatile unsigned char *)(0x00400032)

#define Timer2Data *(volatile unsigned char *)(0x00400034)
#define Timer2Control *(volatile unsigned char *)(0x00400036)
#define Timer2Status *(volatile unsigned char *)(0x00400036)

#define Timer3Data *(volatile unsigned char *)(0x00400038)
#define Timer3Control *(volatile unsigned char *)(0x0040003A)
#define Timer3Status *(volatile unsigned char *)(0x0040003A)

#define Timer4Data *(volatile unsigned char *)(0x0040003C)
#define Timer4Control *(volatile unsigned char *)(0x0040003E)
#define Timer4Status *(volatile unsigned char *)(0x0040003E)

/*********************************************************************************************
**	RS232 port addresses
*********************************************************************************************/

#define RS232_Control *(volatile unsigned char *)(0x00400040)
#define RS232_Status *(volatile unsigned char *)(0x00400040)
#define RS232_TxData *(volatile unsigned char *)(0x00400042)
#define RS232_RxData *(volatile unsigned char *)(0x00400042)
#define RS232_Baud *(volatile unsigned char *)(0x00400044)

/*********************************************************************************************
**	PIA 1 and 2 port addresses
*********************************************************************************************/

#define PIA1_PortA_Data *(volatile unsigned char *)(0x00400050) // combined data and data direction register share same address
#define PIA1_PortA_Control *(volatile unsigned char *)(0x00400052)
#define PIA1_PortB_Data *(volatile unsigned char *)(0x00400054) // combined data and data direction register share same address
#define PIA1_PortB_Control *(volatile unsigned char *)(0x00400056)

#define PIA2_PortA_Data *(volatile unsigned char *)(0x00400060) // combined data and data direction register share same address
#define PIA2_PortA_Control *(volatile unsigned char *)(0x00400062)
#define PIA2_PortB_data *(volatile unsigned char *)(0x00400064) // combined data and data direction register share same address
#define PIA2_PortB_Control *(volatile unsigned char *)(0x00400066)

/*************************************************************
** I2C Controller registers
**************************************************************/
// I2C Registers
#define I2C_PRERLO (*(volatile unsigned char *)(0x00408000))
#define I2C_PRERHI (*(volatile unsigned char *)(0x00408002))
#define I2C_CTR (*(volatile unsigned char *)(0x00408004))
#define I2C_TXR (*(volatile unsigned char *)(0x00408006))
#define I2C_RXR (*(volatile unsigned char *)(0x00408006))
#define I2C_CR (*(volatile unsigned char *)(0x00408008))
#define I2C_SR (*(volatile unsigned char *)(0x00408008))

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

#define CAN0_CONTROLLER(i) (*(volatile unsigned char *)(0x00500000 + (i << 1)))
#define CAN1_CONTROLLER(i) (*(volatile unsigned char *)(0x00500200 + (i << 1)))

/* Can 0 register definitions */
#define Can0_ModeControlReg CAN0_CONTROLLER(0)
#define Can0_CommandReg CAN0_CONTROLLER(1)
#define Can0_StatusReg CAN0_CONTROLLER(2)
#define Can0_InterruptReg CAN0_CONTROLLER(3)
#define Can0_InterruptEnReg CAN0_CONTROLLER(4) /* PeliCAN mode */
#define Can0_BusTiming0Reg CAN0_CONTROLLER(6)
#define Can0_BusTiming1Reg CAN0_CONTROLLER(7)
#define Can0_OutControlReg CAN0_CONTROLLER(8)

/* address definitions of Other Registers */
#define Can0_ArbLostCapReg CAN0_CONTROLLER(11)
#define Can0_ErrCodeCapReg CAN0_CONTROLLER(12)
#define Can0_ErrWarnLimitReg CAN0_CONTROLLER(13)
#define Can0_RxErrCountReg CAN0_CONTROLLER(14)
#define Can0_TxErrCountReg CAN0_CONTROLLER(15)
#define Can0_RxMsgCountReg CAN0_CONTROLLER(29)
#define Can0_RxBufStartAdr CAN0_CONTROLLER(30)
#define Can0_ClockDivideReg CAN0_CONTROLLER(31)

/* address definitions of Acceptance Code & Mask Registers - RESET MODE */
#define Can0_AcceptCode0Reg CAN0_CONTROLLER(16)
#define Can0_AcceptCode1Reg CAN0_CONTROLLER(17)
#define Can0_AcceptCode2Reg CAN0_CONTROLLER(18)
#define Can0_AcceptCode3Reg CAN0_CONTROLLER(19)
#define Can0_AcceptMask0Reg CAN0_CONTROLLER(20)
#define Can0_AcceptMask1Reg CAN0_CONTROLLER(21)
#define Can0_AcceptMask2Reg CAN0_CONTROLLER(22)
#define Can0_AcceptMask3Reg CAN0_CONTROLLER(23)

/* address definitions Rx Buffer - OPERATING MODE - Read only register*/
#define Can0_RxFrameInfo CAN0_CONTROLLER(16)
#define Can0_RxBuffer1 CAN0_CONTROLLER(17)
#define Can0_RxBuffer2 CAN0_CONTROLLER(18)
#define Can0_RxBuffer3 CAN0_CONTROLLER(19)
#define Can0_RxBuffer4 CAN0_CONTROLLER(20)
#define Can0_RxBuffer5 CAN0_CONTROLLER(21)
#define Can0_RxBuffer6 CAN0_CONTROLLER(22)
#define Can0_RxBuffer7 CAN0_CONTROLLER(23)
#define Can0_RxBuffer8 CAN0_CONTROLLER(24)
#define Can0_RxBuffer9 CAN0_CONTROLLER(25)
#define Can0_RxBuffer10 CAN0_CONTROLLER(26)
#define Can0_RxBuffer11 CAN0_CONTROLLER(27)
#define Can0_RxBuffer12 CAN0_CONTROLLER(28)

/* address definitions of the Tx-Buffer - OPERATING MODE - Write only register */
#define Can0_TxFrameInfo CAN0_CONTROLLER(16)
#define Can0_TxBuffer1 CAN0_CONTROLLER(17)
#define Can0_TxBuffer2 CAN0_CONTROLLER(18)
#define Can0_TxBuffer3 CAN0_CONTROLLER(19)
#define Can0_TxBuffer4 CAN0_CONTROLLER(20)
#define Can0_TxBuffer5 CAN0_CONTROLLER(21)
#define Can0_TxBuffer6 CAN0_CONTROLLER(22)
#define Can0_TxBuffer7 CAN0_CONTROLLER(23)
#define Can0_TxBuffer8 CAN0_CONTROLLER(24)
#define Can0_TxBuffer9 CAN0_CONTROLLER(25)
#define Can0_TxBuffer10 CAN0_CONTROLLER(26)
#define Can0_TxBuffer11 CAN0_CONTROLLER(27)
#define Can0_TxBuffer12 CAN0_CONTROLLER(28)

/* read only addresses */
#define Can0_TxFrameInfoRd CAN0_CONTROLLER(96)
#define Can0_TxBufferRd1 CAN0_CONTROLLER(97)
#define Can0_TxBufferRd2 CAN0_CONTROLLER(98)
#define Can0_TxBufferRd3 CAN0_CONTROLLER(99)
#define Can0_TxBufferRd4 CAN0_CONTROLLER(100)
#define Can0_TxBufferRd5 CAN0_CONTROLLER(101)
#define Can0_TxBufferRd6 CAN0_CONTROLLER(102)
#define Can0_TxBufferRd7 CAN0_CONTROLLER(103)
#define Can0_TxBufferRd8 CAN0_CONTROLLER(104)
#define Can0_TxBufferRd9 CAN0_CONTROLLER(105)
#define Can0_TxBufferRd10 CAN0_CONTROLLER(106)
#define Can0_TxBufferRd11 CAN0_CONTROLLER(107)
#define Can0_TxBufferRd12 CAN0_CONTROLLER(108)

/* CAN1 Controller register definitions */
#define Can1_ModeControlReg CAN1_CONTROLLER(0)
#define Can1_CommandReg CAN1_CONTROLLER(1)
#define Can1_StatusReg CAN1_CONTROLLER(2)
#define Can1_InterruptReg CAN1_CONTROLLER(3)
#define Can1_InterruptEnReg CAN1_CONTROLLER(4) /* PeliCAN mode */
#define Can1_BusTiming0Reg CAN1_CONTROLLER(6)
#define Can1_BusTiming1Reg CAN1_CONTROLLER(7)
#define Can1_OutControlReg CAN1_CONTROLLER(8)

/* address definitions of Other Registers */
#define Can1_ArbLostCapReg CAN1_CONTROLLER(11)
#define Can1_ErrCodeCapReg CAN1_CONTROLLER(12)
#define Can1_ErrWarnLimitReg CAN1_CONTROLLER(13)
#define Can1_RxErrCountReg CAN1_CONTROLLER(14)
#define Can1_TxErrCountReg CAN1_CONTROLLER(15)
#define Can1_RxMsgCountReg CAN1_CONTROLLER(29)
#define Can1_RxBufStartAdr CAN1_CONTROLLER(30)
#define Can1_ClockDivideReg CAN1_CONTROLLER(31)

/* address definitions of Acceptance Code & Mask Registers - RESET MODE */
#define Can1_AcceptCode0Reg CAN1_CONTROLLER(16)
#define Can1_AcceptCode1Reg CAN1_CONTROLLER(17)
#define Can1_AcceptCode2Reg CAN1_CONTROLLER(18)
#define Can1_AcceptCode3Reg CAN1_CONTROLLER(19)
#define Can1_AcceptMask0Reg CAN1_CONTROLLER(20)
#define Can1_AcceptMask1Reg CAN1_CONTROLLER(21)
#define Can1_AcceptMask2Reg CAN1_CONTROLLER(22)
#define Can1_AcceptMask3Reg CAN1_CONTROLLER(23)

/* address definitions Rx Buffer - OPERATING MODE - Read only register*/
#define Can1_RxFrameInfo CAN1_CONTROLLER(16)
#define Can1_RxBuffer1 CAN1_CONTROLLER(17)
#define Can1_RxBuffer2 CAN1_CONTROLLER(18)
#define Can1_RxBuffer3 CAN1_CONTROLLER(19)
#define Can1_RxBuffer4 CAN1_CONTROLLER(20)
#define Can1_RxBuffer5 CAN1_CONTROLLER(21)
#define Can1_RxBuffer6 CAN1_CONTROLLER(22)
#define Can1_RxBuffer7 CAN1_CONTROLLER(23)
#define Can1_RxBuffer8 CAN1_CONTROLLER(24)
#define Can1_RxBuffer9 CAN1_CONTROLLER(25)
#define Can1_RxBuffer10 CAN1_CONTROLLER(26)
#define Can1_RxBuffer11 CAN1_CONTROLLER(27)
#define Can1_RxBuffer12 CAN1_CONTROLLER(28)

/* address definitions of the Tx-Buffer - OPERATING MODE - Write only register */
#define Can1_TxFrameInfo CAN1_CONTROLLER(16)
#define Can1_TxBuffer1 CAN1_CONTROLLER(17)
#define Can1_TxBuffer2 CAN1_CONTROLLER(18)
#define Can1_TxBuffer3 CAN1_CONTROLLER(19)
#define Can1_TxBuffer4 CAN1_CONTROLLER(20)
#define Can1_TxBuffer5 CAN1_CONTROLLER(21)
#define Can1_TxBuffer6 CAN1_CONTROLLER(22)
#define Can1_TxBuffer7 CAN1_CONTROLLER(23)
#define Can1_TxBuffer8 CAN1_CONTROLLER(24)
#define Can1_TxBuffer9 CAN1_CONTROLLER(25)
#define Can1_TxBuffer10 CAN1_CONTROLLER(26)
#define Can1_TxBuffer11 CAN1_CONTROLLER(27)
#define Can1_TxBuffer12 CAN1_CONTROLLER(28)

/* read only addresses */
#define Can1_TxFrameInfoRd CAN1_CONTROLLER(96)
#define Can1_TxBufferRd1 CAN1_CONTROLLER(97)
#define Can1_TxBufferRd2 CAN1_CONTROLLER(98)
#define Can1_TxBufferRd3 CAN1_CONTROLLER(99)
#define Can1_TxBufferRd4 CAN1_CONTROLLER(100)
#define Can1_TxBufferRd5 CAN1_CONTROLLER(101)
#define Can1_TxBufferRd6 CAN1_CONTROLLER(102)
#define Can1_TxBufferRd7 CAN1_CONTROLLER(103)
#define Can1_TxBufferRd8 CAN1_CONTROLLER(104)
#define Can1_TxBufferRd9 CAN1_CONTROLLER(105)
#define Can1_TxBufferRd10 CAN1_CONTROLLER(106)
#define Can1_TxBufferRd11 CAN1_CONTROLLER(107)
#define Can1_TxBufferRd12 CAN1_CONTROLLER(108)

/* bit definitions for the Mode & Control Register */
#define RM_RR_Bit 0x01 /* reset mode (request) bit */
#define LOM_Bit 0x02   /* listen only mode bit */
#define STM_Bit 0x04   /* self test mode bit */
#define AFM_Bit 0x08   /* acceptance filter mode bit */
#define SM_Bit 0x10    /* enter sleep mode bit */

/* bit definitions for the Interrupt Enable & Control Register */
#define RIE_Bit 0x01  /* receive interrupt enable bit */
#define TIE_Bit 0x02  /* transmit interrupt enable bit */
#define EIE_Bit 0x04  /* error warning interrupt enable bit */
#define DOIE_Bit 0x08 /* data overrun interrupt enable bit */
#define WUIE_Bit 0x10 /* wake-up interrupt enable bit */
#define EPIE_Bit 0x20 /* error passive interrupt enable bit */
#define ALIE_Bit 0x40 /* arbitration lost interr. enable bit*/
#define BEIE_Bit 0x80 /* bus error interrupt enable bit */

/* bit definitions for the Command Register */
#define TR_Bit 0x01  /* transmission request bit */
#define AT_Bit 0x02  /* abort transmission bit */
#define RRB_Bit 0x04 /* release receive buffer bit */
#define CDO_Bit 0x08 /* clear data overrun bit */
#define SRR_Bit 0x10 /* self reception request bit */

/* bit definitions for the Status Register */
#define RBS_Bit 0x01 /* receive buffer status bit */
#define DOS_Bit 0x02 /* data overrun status bit */
#define TBS_Bit 0x04 /* transmit buffer status bit */
#define TCS_Bit 0x08 /* transmission complete status bit */
#define RS_Bit 0x10  /* receive status bit */
#define TS_Bit 0x20  /* transmit status bit */
#define ES_Bit 0x40  /* error status bit */
#define BS_Bit 0x80  /* bus status bit */

/* bit definitions for the Interrupt Register */
#define RI_Bit 0x01  /* receive interrupt bit */
#define TI_Bit 0x02  /* transmit interrupt bit */
#define EI_Bit 0x04  /* error warning interrupt bit */
#define DOI_Bit 0x08 /* data overrun interrupt bit */
#define WUI_Bit 0x10 /* wake-up interrupt bit */
#define EPI_Bit 0x20 /* error passive interrupt bit */
#define ALI_Bit 0x40 /* arbitration lost interrupt bit */
#define BEI_Bit 0x80 /* bus error interrupt bit */

/* bit definitions for the Bus Timing Registers */
#define SAM_Bit 0x80 /* sample mode bit 1 == the bus is sampled 3 times, 0 == the bus is sampled once */

/* bit definitions for the Output Control Register OCMODE1, OCMODE0 */
#define BiPhaseMode 0x00 /* bi-phase output mode */
#define NormalMode 0x02  /* normal output mode */
#define ClkOutMode 0x03  /* clock output mode */

/* output pin configuration for TX1 */
#define OCPOL1_Bit 0x20 /* output polarity control bit */
#define Tx1Float 0x00   /* configured as float */
#define Tx1PullDn 0x40  /* configured as pull-down */
#define Tx1PullUp 0x80  /* configured as pull-up */
#define Tx1PshPull 0xC0 /* configured as push/pull */

/* output pin configuration for TX0 */
#define OCPOL0_Bit 0x04 /* output polarity control bit */
#define Tx0Float 0x00   /* configured as float */
#define Tx0PullDn 0x08  /* configured as pull-down */
#define Tx0PullUp 0x10  /* configured as pull-up */
#define Tx0PshPull 0x18 /* configured as push/pull */

/* bit definitions for the Clock Divider Register */
#define DivBy1 0x07      /* CLKOUT = oscillator frequency */
#define DivBy2 0x00      /* CLKOUT = 1/2 oscillator frequency */
#define ClkOff_Bit 0x08  /* clock off bit, control of the CLK OUT pin */
#define RXINTEN_Bit 0x20 /* pin TX1 used for receive interrupt */
#define CBP_Bit 0x40     /* CAN comparator bypass control bit */
#define CANMode_Bit 0x80 /* CAN mode definition bit */

/*- definition of used constants ---------------------------------------*/
#define YES 1
#define NO 0
#define ENABLE 1
#define DISABLE 0
#define ENABLE_N 0
#define DISABLE_N 1
#define INTLEVELACT 0
#define INTEDGEACT 1
#define PRIORITY_LOW 0
#define PRIORITY_HIGH 1

/* default (reset) value for register content, clear register */
#define ClrByte 0x00

/* constant: clear Interrupt Enable Register */
#define ClrIntEnSJA ClrByte

/* definitions for the acceptance code and mask register */
#define DontCare 0xFF

/*********************************************************************************************************************************
(( DO NOT initialise global variables here, do it main even if you want 0
(( it's a limitation of the compiler
(( YOU HAVE BEEN WARNED
*********************************************************************************************************************************/

unsigned int i, x, y, z, PortA_Count;
unsigned char Timer1Count, Timer2Count, Timer3Count, Timer4Count;

/*******************************************************************************************
** Function Prototypes
*******************************************************************************************/
void Wait1ms(void);
void Wait3ms(void);
void Wait250ms(void);
void Wait750ms(void);
void Wait1s(void);
void Init_LCD(void);
void LCDOutchar(int c);
void LCDOutMess(char *theMessage);
void LCDClearln(void);
void LCDline1Message(char *theMessage);
void LCDline2Message(char *theMessage);
int sprintf(char *out, const char *format, ...);

// I2C
void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void WaitForTIPFlagReset(void);
void WaitForRxACK(void);
void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo);
void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo);
void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, int size);
void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, int size);
void GetMemAddr(char *hi, char *lo);
void GetBank(char *bank);
void Wait(void);
char readADC(int);

// CANBUS
void CanBus1_Transmit(unsigned char data, unsigned char channel_num);
void CanBus0_Transmit(unsigned char data, unsigned char channel_num);
void Init_CanBus_Controller1(void);
void Init_CanBus_Controller0(void);
void CanBus1_Receive(int *channel_num, unsigned char *channel_data);
void CanBus0_Receive(int *channel_num, unsigned char *channel_data);


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
        return c - (char)(0x30);      // 0 - 9 = 0x30 - 0x39 so convert to number by sutracting 0x30
    else if ((char)(c) > (char)('F')) // assume lower case
        return c - (char)(0x57);      // a-f = 0x61-66 so needs to be converted to 0x0A - 0x0F so subtract 0x57
    else
        return c - (char)(0x37); // A-F = 0x41-46 so needs to be converted to 0x0A - 0x0F so subtract 0x37
}

int Get2HexDigits(char *CheckSumPtr)
{
    register int i = (xtod(_getch()) << 4) | (xtod(_getch()));

    if (CheckSumPtr)
        *CheckSumPtr += i;

    return i;
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
    Init_CanBus_Controller0();
    Init_CanBus_Controller1();

    // Thermistor Interrupt
    if (Timer4Status == 1)
    {                          // Did Timer 4 produce the Interrupt?
        Timer4Control = 3;     // reset the timer to clear the interrupt, enable interrupts and allow counter to run
        // HEX_B = Timer4Count++; // increment a HEX count on HEX_B with each tick of Timer 4
        // Use portA_count as a counter, since this isr happens every 500ms
        if(PortA_Count == 3){
            CanBus1_Transmit(readADC(3), 0x4);
            PortA_Count = 0;
            z = 1;
            // printf("\r\nInterrupt: Timer 4");
        }
        else{
            PortA_Count += 1;
        }
        // printf("\r\nInterrupt: Timer 4 hex A: %d", PortA_Count);
    }
    // Light sensor interrupt
    if (Timer3Status == 1)
    {                          // Did Timer 3 produce the Interrupt?
        Timer3Control = 3;     // reset the timer to clear the interrupt, enable interrupts and allow counter to run
        // HEX_A = Timer3Count++; // increment a HEX count on Port HEX_A with each tick of Timer 3
        CanBus1_Transmit(readADC(2), 0x3);
        z = 1;
        // printf("\r\nInterrupt: Timer 3");
    }
    // Potentiometer interrupt
    if (Timer2Status == 1)
    {                          // Did Timer 2 produce the Interrupt?
        Timer2Control = 3;     // reset the timer to clear the interrupt, enable interrupts and allow counter to run
        // PortC = Timer2Count++; // increment an LED count on PortC with each tick of Timer 2
        CanBus1_Transmit(readADC(1), 0x2);
        z = 1;
        // printf("\r\nInterrupt: Timer 2");
    }
    // Switch interrupt
    if (Timer1Status == 1)
    {                          // Did Timer 1 produce the Interrupt?
        Timer1Control = 3;     // reset the timer to clear the interrupt, enable interrupts and allow counter to run
        // PortA = Timer1Count++; // increment an LED count on PortA with each tick of Timer 1
        CanBus1_Transmit(PortA, 0x1);
        z = 1;
        // printf("\r\nInterrupt: Timer 1");
    }

}

/*****************************************************************************************
**	Interrupt service routine for ACIA. This device has it's own dedicate IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void ACIA_ISR()
{
}

/***************************************************************************************
**	Interrupt service routine for PIAs 1 and 2. These devices share an IRQ level
**  Add your code here to poll Status register and clear interrupt
*****************************************************************************************/

void PIA_ISR()
{
}

/***********************************************************************************
**	Interrupt service routine for Key 2 on DE1 board. Add your own response here
************************************************************************************/
void Key2PressISR()
{
}

/***********************************************************************************
**	Interrupt service routine for Key 1 on DE1 board. Add your own response here
************************************************************************************/
void Key1PressISR()
{
}

/************************************************************************************
**   Delay Subroutine to give the 68000 something useless to do to waste 1 mSec
************************************************************************************/
void Wait1ms(void)
{
    int i;
    for (i = 0; i < 1000; i++)
        ;
}

/************************************************************************************
**  Subroutine to give the 68000 something useless to do to waste 3 mSec
**************************************************************************************/
void Wait3ms(void)
{
    int i;
    for (i = 0; i < 3; i++)
        Wait1ms();
}

void Wait250ms(void)
{
    int i;
    for (i = 0; i < 250; i++)
        Wait1ms();
}

void Wait500ms(void)
{
    int i;
    for (i = 0; i < 500; i++)
        Wait1ms();
}

void Wait750ms(void)
{
    int i;
    for (i = 0; i < 750; i++)
        Wait1ms();
}

/************************************************************************************
**  Subroutine to give the 68000 something useless to do to waste 1s
**************************************************************************************/
void Wait1s(void)
{
    int i;
    for (i = 0; i < 1000; i++)
        Wait1ms();
}

/*********************************************************************************************
**  Subroutine to initialise the LCD display by writing some commands to the LCD internal registers
**  Sets it for parallel port and 2 line display mode (if I recall correctly)
*********************************************************************************************/
void Init_LCD(void)
{
    LCDcommand = 0x0c;
    Wait3ms();
    LCDcommand = 0x38;
    Wait3ms();
}

/*********************************************************************************************
**  Subroutine to initialise the RS232 Port by writing some commands to the internal registers
*********************************************************************************************/
void Init_RS232(void)
{
    RS232_Control = 0x15; //  %00010101 set up 6850 uses divide by 16 clock, set RTS low, 8 bits no parity, 1 stop bit, transmitter interrupt disabled
    RS232_Baud = 0x1;     // program baud rate generator 001 = 115k, 010 = 57.6k, 011 = 38.4k, 100 = 19.2, all others = 9600
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

int _putch(int c)
{
    while ((RS232_Status & (char)(0x02)) != (char)(0x02)) // wait for Tx bit in status register or 6850 serial comms chip to be '1'
        ;

    RS232_TxData = (c & (char)(0x7f)); // write to the data register to output the character (mask off bit 8 to keep it 7 bit ASCII)
    return c;                          // putchar() expects the character to be returned
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
int _getch(void)
{
    char c;
    while ((RS232_Status & (char)(0x01)) != (char)(0x01)) // wait for Rx bit in 6850 serial comms chip status register to be '1'
        ;

    return (RS232_RxData & (char)(0x7f)); // read received character, mask off top bit and return as 7 bit ASCII character
}

/******************************************************************************
**  Subroutine to output a single character to the 2 row LCD display
**  It is assumed the character is an ASCII code and it will be displayed at the
**  current cursor position
*******************************************************************************/
void LCDOutchar(int c)
{
    LCDdata = (char)(c);
    Wait1ms();
}

/**********************************************************************************
 *subroutine to output a message at the current cursor position of the LCD display
 ************************************************************************************/
void LCDOutMessage(char *theMessage)
{
    char c;
    while ((c = *theMessage++) != 0) // output characters from the string until NULL
        LCDOutchar(c);
}

/******************************************************************************
 *subroutine to clear the line by issuing 24 space characters
 *******************************************************************************/
void LCDClearln(void)
{
    int i;
    for (i = 0; i < 24; i++)
        LCDOutchar(' '); // write a space char to the LCD display
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 1 and clear that line
*******************************************************************************/
void LCDLine1Message(char *theMessage)
{
    LCDcommand = 0x80;
    Wait3ms();
    LCDClearln();
    LCDcommand = 0x80;
    Wait3ms();
    LCDOutMessage(theMessage);
}

/******************************************************************************
**  Subroutine to move the LCD cursor to the start of line 2 and clear that line
*******************************************************************************/
void LCDLine2Message(char *theMessage)
{
    LCDcommand = 0xC0;
    Wait3ms();
    LCDClearln();
    LCDcommand = 0xC0;
    Wait3ms();
    LCDOutMessage(theMessage);
}

void I2C_Start(void)
{
    // Start condition is 8'b1000_0000
    I2C_CR = 0x80;
}

void I2C_Stop(void)
{
    // Stop condition is 8'b0100_0000
    I2C_CR = 0x40;
}

void I2C_Init(void)
{
    // Make sure I2C core is off
    I2C_CTR = (char)0x00;
    // Prescale register clock, 25Mhz / (5 * 100KHz) - 1  -> 0x0031
    I2C_PRERLO = (char)0x31;
    I2C_PRERHI = (char)0x00;
    // Enable I2C Core, no interrupt enabled
    // 8'b10xx_xxxx
    I2C_CTR = (char)0x80;
}

void WaitForRxACK(void)
{
    // Read RxACK bit from Status Register, should be '0'
    // Status Register [7] == 0
    while (((I2C_SR >> 7) & 1) == 1)
    {
    }
}

void WaitForTIPFlagReset(void)
{
    // Status Register [1] == 0
    while ((I2C_SR >> 1) & 1)
    {
    }
}

void Wait(void)
{
    WaitForTIPFlagReset();
    WaitForRxACK();
}

void TransmitI2C(char data, char cr)
{
    I2C_TXR = data;
    I2C_CR = cr;
    Wait();
}

// Assumptions: addr is at most 7 bits wide
void WriteI2CChar(char data, char slave_addr, char memaddr_hi, char memaddr_lo)
{
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
void ReadI2CChar(char *data, char slave_addr, char memaddr_hi, char memaddr_lo)
{
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
    while ((I2C_SR & 1) != 1)
    {
    }
    *data = I2C_RXR;
    I2C_CR = 0x41;
}

// Increment logic, handles bank overflows, handle write addressing boundaries
void incrI2C(char *slave_addr, char *memaddr_hi, char *memaddr_lo, int read, int addr_change)
{
    char zeewo = 0x00;
    char one = 0x01;
    int edge = 0;
    // printf("\r\n %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
    if ((*memaddr_lo & 0xFF) == 0xFF)
    {
        if ((*memaddr_hi & 0xFF) == 0xFF)
        { // Change bank
            *slave_addr = ((*slave_addr & 0x8) == 0x8) ? EEPROM_SLAVE_BANK0 : EEPROM_SLAVE_BANK1;
            *memaddr_lo = zeewo;
            *memaddr_hi = zeewo;

            if (read & 1)
            {
                printf("\r\n Changing slave and restart %x", *slave_addr);
                TransmitI2C(*slave_addr, WSTART);
                TransmitI2C(*memaddr_hi, WRITE);
                TransmitI2C(*memaddr_lo, WRITE);
                TransmitI2C(*slave_addr | 1, WSTART);
            }
            else
            {
                TransmitI2C(*slave_addr, WSTART);
                TransmitI2C(*memaddr_hi, WRITE);
                TransmitI2C(*memaddr_lo, WRITE);
            }
            printf("\r\n HIT EDGE CASE %x %x%x", *slave_addr, *memaddr_hi, *memaddr_lo);
            edge = 1;
        }
        else
        {
            *memaddr_hi += one;
            *memaddr_lo = zeewo;
        }
    }
    else
    {
        *memaddr_lo += one;
    }

    if (read == 0 && addr_change == 1)
    {
        printf("\r\n Write address boundary, restarting without changing slave");
        TransmitI2C(*slave_addr, WSTART);
        TransmitI2C(*memaddr_hi, WRITE);
        TransmitI2C(*memaddr_lo, WRITE);
    }
}

void WriteI2CPage(char slave_addr, char memaddr_hi, char memaddr_lo, int size)
{
    // Data is array of size "size", max size 128
    // WriteI2CChar();
    int i;
    char data = 0;
    int full_addr = (memaddr_hi) << 8 | memaddr_lo;
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    for (i = 0; i <= size; i++)
    {
        // Write without stop unless we're at the end of a write boundary
        if (i >= (size - 1) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF) || ((((full_addr + 1) % 0x80) == 0)) && i != 0)
        {
            TransmitI2C(data, WSTOP);
            printf("\r\n--------------");
            printf("\r\nfulladdr: %x", full_addr);
            printf("\r\ndata: %x", i);
            printf("\r\n--------------");
            if (i != size)
                incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 0, 1);
            else
                return;
        }
        else
        {
            TransmitI2C(data, WRITE);
            // Page crossing, if it happens, is handled inside incrI2C
            incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 0, 0);
        }
        if (i % 1000 == 0 || size == 0)
            printf("\r\nWriting: %2x ", data);
        if (data == 0x7f)
            data = 0;
        else
            data++;
        full_addr++;
        // printf("%x ", full_addr);
    }
}

void ReadI2CPage(char *data, char slave_addr, char memaddr_hi, char memaddr_lo, int size)
{
    // Data is array of size "size", max size 128
    int i;
    int temp = 0;
    // Check before doing anything
    WaitForTIPFlagReset();
    // Set slave to write mode, Generate start command
    TransmitI2C(slave_addr, WSTART);
    // Write Mem Address and set W bit
    TransmitI2C(memaddr_hi, WRITE);
    TransmitI2C(memaddr_lo, WRITE);
    // Set slave to read mode and generate start command for reading
    TransmitI2C(slave_addr | 1, WSTART);
    for (i = 0; i <= size; i++)
    {
        // Read without NACK unless we're at the end
        if (i == (size) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF))
        {
            I2C_CR = READNACK;
        }
        else
            I2C_CR = READACK;
        while ((I2C_SR & 1) != 1)
        {
        }
        printf("%2x ", I2C_RXR);
        // data[i] = I2C_RXR;
        if (i == (size) || ((memaddr_hi & 0xFF) == 0xFF) && ((memaddr_lo & 0xFF) == 0xFF))
            I2C_CR = 0x41;
        else
            I2C_CR = 0x01;
        // Page crossing, if it happens, is handled inside incrI2C
        incrI2C(&slave_addr, &memaddr_hi, &memaddr_lo, 1, 0);
        if (i == size)
            return;
    }
}

void GetBank(char *bank)
{
    char asdf;
    while (1)
    {
        printf("\r\nSelect bank:\r\n0 - Bank 0\r\n1 - Bank 1");
        asdf = getchar();
        putchar(asdf);
        if (asdf == '0')
        {
            *bank = EEPROM_SLAVE_BANK0;
            break;
        }
        else if (asdf == '1')
        {
            *bank = EEPROM_SLAVE_BANK1;
            break;
        }
        else
        {
            printf("\r\nInvalid selection.");
        }
    }
}

void GetMemAddr(char *hi, char *lo)
{
    printf("\r\nEnter mem address hi:");
    *hi = Get2HexDigits(0);
    printf("\r\nEnter mem address lo:");
    *lo = Get2HexDigits(0);
    // TODO: Hex digit validation?
}

void blinky()
{
    // Make sure nothing is going on in the I2C bus
    WaitForTIPFlagReset();
    // Write address
    TransmitI2C(ADCDAC_SLAVE, WSTART);
    // Set control to OUT: 8'b01xx_0xxx
    TransmitI2C(DAC_OUT_ON, WRITE);
    // Vout calculation: 5/256 * 8 bit data
    // Blinky until reset is pressed on DE1
    while (1)
    {
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

char readADC(int input_val)
{
    char Vchannel1, Vchannel2, Vchannel3, Vchannel4;

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
        while ((I2C_SR & 1) != 1)
        {
        }
        Vchannel4 = I2C_RXR; // This is always invalid because the jumper isn't connected
        I2C_CR = READACK;
        while ((I2C_SR & 1) != 1)
        {
        }
        Vchannel1 = I2C_RXR;
        I2C_CR = READACK;
        // Wait for read data to come in
        while ((I2C_SR & 1) != 1)
        {
        }
        Vchannel2 = I2C_RXR;
        I2C_CR = READACK;
        // Wait for read data to come in
        while ((I2C_SR & 1) != 1)
        {
        }
        Vchannel3 = I2C_RXR;
        I2C_CR = 0x41;
        // Vout calculation: 5/256 * 8 bit data, but we can just leave it.
        // printf("\r\nThermistor: %d", Vchannel1);
        // printf("\r\nPotentiometer: %d", Vchannel2);
        // printf("\r\nPhotoresistor: %d", Vchannel3);
        // printf("\r\n--------------");
        // Wait1s();
    // Potentiometer
    if (input_val == 1)
        return Vchannel2;
    // Photoresistor
    else if (input_val == 2)
        return Vchannel3;
    // Thermistor
    else if (input_val == 3)
        return Vchannel1;
    else return 0;

}

/*  bus timing values for
**  bit-rate : 100 kBit/s
**  oscillator frequency : 25 MHz, 1 sample per bit, 0 tolerance %
**  maximum tolerated propagation delay : 4450 ns
**  minimum requested propagation delay : 500 ns
**
**  https://www.kvaser.com/support/calculators/bit-timing-calculator/
**  T1 	T2 	BTQ 	SP% 	SJW 	BIT RATE 	ERR% 	BTR0 	BTR1
**  17	8	25	    68	     1	      100	    0	      04	7f
*/

// initialisation for Can controller 0
void Init_CanBus_Controller0(void)
{
    // TODO - put your Canbus initialisation code for CanController 0 here
    // See section 4.2.1 in the application note for details (PELICAN MODE)

    while ((Can0_ModeControlReg & RM_RR_Bit) == ClrByte)
        Can0_ModeControlReg = Can0_ModeControlReg | RM_RR_Bit;

    // Set clock divide register to use pelican mode
    Can0_ClockDivideReg = CANMode_Bit | CBP_Bit;

    Can0_InterruptEnReg = ClrIntEnSJA;

    // Clr byte, doesn't matter since we set mask to don't care
    Can0_AcceptCode0Reg = ClrByte;
    Can0_AcceptCode1Reg = ClrByte;
    Can0_AcceptCode2Reg = ClrByte;
    Can0_AcceptCode3Reg = ClrByte;
    // Set to don't care for acceptance filtering
    Can0_AcceptMask0Reg = DontCare;
    Can0_AcceptMask1Reg = DontCare;
    Can0_AcceptMask2Reg = DontCare;
    Can0_AcceptMask3Reg = DontCare;

    //8'b0000_0100 = 2 * tclk * (4 * 1 + 1) = 10 *tclk
    Can0_BusTiming0Reg = 0x04;
    //8'b0111_1111
    Can0_BusTiming1Reg = 0x7f;

    Can0_OutControlReg = Tx1Float | Tx0PshPull | NormalMode;

    // Set mode control to clr
    Can0_ModeControlReg = ClrByte;
    while ((Can0_ModeControlReg & RM_RR_Bit) != ClrByte)
        Can0_ModeControlReg = ClrByte;
}

// initialisation for Can controller 1
void Init_CanBus_Controller1(void)
{
    // TODO - put your Canbus initialisation code for CanController 1 here
    // See section 4.2.1 in the application note for details (PELICAN MODE)
    while ((Can1_ModeControlReg & RM_RR_Bit) == ClrByte)
    {
        /* other bits than the reset mode/request bit are unchanged */
        Can1_ModeControlReg = Can1_ModeControlReg | RM_RR_Bit;
    }
    Can1_ClockDivideReg = CANMode_Bit | CBP_Bit;
    Can1_InterruptEnReg = ClrIntEnSJA;

    Can1_AcceptCode0Reg = ClrByte;
    Can1_AcceptCode1Reg = ClrByte;
    Can1_AcceptCode2Reg = ClrByte;
    Can1_AcceptCode3Reg = ClrByte;
    Can1_AcceptMask0Reg = DontCare;
    Can1_AcceptMask1Reg = DontCare;
    Can1_AcceptMask2Reg = DontCare;
    Can1_AcceptMask3Reg = DontCare;

    Can1_BusTiming0Reg = 0x04; 
    Can1_BusTiming1Reg = 0x7f; 

    Can1_OutControlReg = Tx1Float | Tx0PshPull | NormalMode;

    do
    {
        Can1_ModeControlReg = ClrByte;
    } while ((Can1_ModeControlReg & RM_RR_Bit) != ClrByte);
}

void CanBus0_Transmit(unsigned char data, unsigned char channel_num)
{
do
  {
  } while ((Can0_StatusReg & TBS_Bit) != TBS_Bit);

  Can0_TxFrameInfo = 0x08;
  Can0_TxBuffer1 = 0xA5;
  Can0_TxBuffer2 = 0x20;
  Can0_TxBuffer3 = data;
  Can0_TxBuffer4 = channel_num;
  Can0_TxBuffer5 = data;
  Can0_TxBuffer6 = data;

  Can0_CommandReg = TR_Bit;

  do
  {
  } while ((Can0_StatusReg & TCS_Bit) != TCS_Bit);
}

// Transmit for sending a message via Can controller 1
void CanBus1_Transmit(unsigned char data, unsigned char channel_num)
{
    // TODO - put yourunsigned char data, unsigned char channel_numbus transmit code for CanController 1 here
    // See section 4.2.2 in the application note for details (PELICAN MODE)
    do
    {
    } while ((Can1_StatusReg & TBS_Bit) != TBS_Bit);

    Can1_TxFrameInfo = 0x08;
    Can1_TxBuffer1 = 0xA5; // ID 1
    Can1_TxBuffer2 = 0x20; // ID 2
    Can1_TxBuffer3 = data;
    Can1_TxBuffer4 = channel_num;
    Can1_TxBuffer5 = data;
    Can1_TxBuffer6 = data;

    Can1_CommandReg = TR_Bit;
    do
    {
    } while ((Can1_StatusReg & TCS_Bit) != TCS_Bit);
}

// Receive for reading a received message via Can controller 0
void CanBus0_Receive(int *channel_num, unsigned char *channel_data)
{
    // TODO - put your Canbus receive code for CanController 0 here
    // See section 4.2.4 in the application note for details (PELICAN MODE)
 unsigned char c[7];

  do
  {
  } while ((Can0_StatusReg & RBS_Bit) != RBS_Bit);

  c[2] = Can0_RxBuffer3 & 0xFF;
  c[3] = Can0_RxBuffer4 & 0xFF;
  c[4] = Can0_RxBuffer5 & 0xFF;
  c[5] = Can0_RxBuffer6 & 0xFF;
  c[6] = Can0_RxBuffer7 & 0xFF;

  Can0_CommandReg = Can0_CommandReg & RRB_Bit;

  *channel_num = c[3];
  *channel_data = c[2];
}

// Receive for reading a received message via Can controller 1
void CanBus1_Receive(int *channel_num, unsigned char *channel_data)
{
    // TODO - put your Canbus receive code for CanController 1 here
    // See section 4.2.4 in the application note for details (PELICAN MODE)
    unsigned char c[7];

    do
    {
    } while ((Can1_StatusReg & RBS_Bit) != RBS_Bit);

    c[2] = Can1_RxBuffer3 & 0xFF;
    c[3] = Can1_RxBuffer4 & 0xFF;
    c[4] = Can1_RxBuffer5 & 0xFF;
    c[5] = Can1_RxBuffer6 & 0xFF;
    c[6] = Can1_RxBuffer7 & 0xFF;

    Can1_CommandReg = Can1_CommandReg & RRB_Bit;

    *channel_num = c[3];
    *channel_data = c[2];
}

void CanBusTest(void)
{
    // initialise the two Can controllers

    unsigned char data = 0;
    int channel = 1;
    int i= 0;
    
    // simple application to alternately transmit and receive messages from each of two nodes

    while (1)
    {
    
        if (z){
            CanBus0_Receive(&channel, &data);  // receive a message via Controller 1 (and display it)
            // printf("\r\nasdf2222 %d, %d", channel, data);
            if (channel == 0x01)
                printf("\r\nLower 8 Switches: 0x%x", data);
            else if (channel == 0x02)
                printf("\r\nPotentiometer:    0x%x", data);
            else if (channel == 0x03)
                printf("\r\nLight Sensor:     0x%x", data);
            else if (channel == 0x04)
                printf("\r\nThermistor:       0x%x", data);
            z = 0;
        }
    }
}

/*********************************************************************************************************************************
**  IMPORTANT FUNCTION
**  This function install an exception handler so you can capture and deal with any 68000 exception in your program
**  You pass it the name of a function in your code that will get called in response to the exception (as the 1st parameter)
**  and in the 2nd parameter, you pass it the exception number that you want to take over (see 68000 exceptions for details)
**  Calling this function allows you to deal with Interrupts for example
***********************************************************************************************************************************/

void InstallExceptionHandler(void (*function_ptr)(), int level)
{
    volatile long int *RamVectorAddress = (volatile long int *)(StartOfExceptionVectorTable); // pointer to the Ram based interrupt vector table created in Cstart in debug monitor

    RamVectorAddress[level] = (long int *)(function_ptr); // install the address of our function into the exception table
}

/******************************************************************************************************************************
 * Start of user program
 ******************************************************************************************************************************/

void main()
{
    unsigned int row, j = 0, count = 0, counter1 = 1;
    int i = 0;
    char c, text[150];

    unsigned char asdf, asdf1, asdf2, asdf3, bank;
    unsigned char wBuf[256];
    unsigned char rBuf[256];
    int addr = 2048;
    int size;
    i = x = y = z = PortA_Count =0;
    Timer1Count = Timer2Count = Timer3Count = Timer4Count = 0;
    

    printf("\r\nLab 6: CANBUS");

    // Set to level 6, for some reason we need 4 exception handlers
    InstallExceptionHandler(Timer_ISR, 30) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
    InstallExceptionHandler(Timer_ISR, 29) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
    InstallExceptionHandler(Timer_ISR, 28) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ
    InstallExceptionHandler(Timer_ISR, 27) ;		// install interrupt handler for Timers 1-4 on level 3 IRQ

	// program time delay into timers 1-4
    Timer1Data = 0x25;	// 100 ms
    Timer2Data = 0x4B; // 200ms
    Timer3Data = 0xBD; // 500ms
    Timer4Data = 0xBD; // 500ms, use some counter to multiply by 4 to turn on for 2s

    Timer1Control = 3;		// write 3 to control register to Bit0 = 1 (enable interrupt from timers) 1 - 4 and allow them to count Bit 1 = 1
    Timer2Control = 3;
    Timer3Control = 3;
    Timer4Control = 3;

    Init_LCD();   // initialise the LCD display to use a parallel data interface and 2 lines of display
    Init_CanBus_Controller0();
    Init_CanBus_Controller1();
    I2C_Init();
    Init_RS232(); // initialise the RS232 port for use with hyper terminal


    /*************************************************************************************************
    **  I2C Program Lab 5
    *************************************************************************************************/

    printf("\r\n\r\n---- CANBUS Test ----\r\n");

    CanBusTest();

}