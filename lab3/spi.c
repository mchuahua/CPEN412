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

/******************************************************************************************
** The following code is for the SPI controller
*******************************************************************************************/
// return true if the SPI has finished transmitting a byte (to say the Flash chip) return false otherwise
// this can be used in a polling algorithm to know when the controller is busy or idle.

int TestForSPITransmitDataComplete(void)    {
    /* TODO replace 0 below with a test for status register SPIF bit and if set, return true */
    // Hex 80 is 0b1000_0000, SPIF bit
    return (SPI_Status & 0x80) >> 7;
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
    SPI_Status |= 0xC0;
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
    int data = 0;
    // Write Data
    SPI_Data = c;
    WaitForSPITransmitComplete();
    // Read Data by writing dummy data first into the write buffer, waiting for finish transmission, 
    // then reading from read buffer
    data = SPI_Data;    

    return data;                   
}
