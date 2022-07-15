
#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
#include "cobra_system.h"
#if (COBRA_APP_SPI_TEST)
#define SPI_FLASH_CMD_25X_WRITE_ENABLE 		0x06
#define SPI_FLASH_CMD_25X_WRITE_DISABLE		0x04
#define SPI_FLASH_CMD_25X_READ_STATUS 		0x05
#define SPI_FLASH_CMD_25X_READ_STATUS2 		0x35
#define SPI_FLASH_CMD_25X_WRITE_STATUS		0x01
#define SPI_FLASH_CMD_25X_PAGE_PROGRAM		0x02
#define SPI_FLASH_CMD_25X_SECTOR_ERASE		0x20
#define SPI_FLASH_CMD_CHIP_ERASE			0xc7
#define SPI_FLASH_CMD_POWER_DOWN			0xb9
#define SPI_FLASH_CMD_READ_DATA				0x03
#define SPI_FLASH_CMD_JEDEC_ID				0x9f


/* SPI FLASH manufacturer identifiers.  */
#define SPI_FLASH_MANUFACTURER_ID_ATMEL		0x1f
#define SPI_FLASH_MANUFACTURER_ID_ST		0x20
#define SPI_FLASH_MANUFACTURER_ID_WINBOND	0xef
#define SPI_FLASH_MANUFACTURER_ID_SPANSION	0x01

/* SPI FLASH status register bits.  */
#define SPI_FLASH_STATUS_25X_BUSY			0x01
#define SPI_FLASH_STATUS_25X_WEL			0x02

/* SPI FLASH page size.  */
#define SPI_FLASH_PAGE_SIZE					256

#define CS_PIN                              PIN15_INDEX

void rdw_spi_write_read_data(uint8_t isMaster,PINMUX_INDEX csPin,unsigned char *txdata,int txnum,unsigned char *rxdata,int rxnum)
{
   int tx = 0,rx = 0;
    volatile unsigned char rxdumy;
    int txrxsum = txnum + rxnum,txrxsum2 = txrxsum;
    if(isMaster)
    {
        cobra_map_pinmodeset(csPin,PIN_MODE_14); 
    }
    while(txrxsum!=0||txrxsum2!=0)
    {
        if((!cobra_spi_get_tx_fifo_full_state(SPI0))&&txrxsum)
        {
            if(txrxsum>rxnum)
            {
                cobra_spi_send_byte(SPI0,txdata[tx++]);
            }
            else
            {
                cobra_spi_send_byte(SPI0,0xFF);
            }
            txrxsum--;
        }
        if((!cobra_spi_get_rx_fifo_full_state(SPI0))&&txrxsum2)
        {
            if(txrxsum2>rxnum)
            {
                rxdumy = cobra_spi_read_word(SPI0) & 0xFF;
            }
            else
            {
                rxdata[rx] = cobra_spi_read_word(SPI0) & 0xFF;
                rx++;
            }
            txrxsum2--;
        }
    }
    if(isMaster)
    {
        while(!cobra_spi_get_tx_fifo_empty_state(SPI0));
        cobra_map_pinmodeset(csPin,PIN_MODE_15); 
    }
}

unsigned char rdw_spi_get_manufacturer_id (void)
{
    unsigned char txdata,rxdata;
    txdata = SPI_FLASH_CMD_JEDEC_ID;
    rdw_spi_write_read_data(1,CS_PIN,&txdata,1,&rxdata,1);
    return rxdata;
}

/* Write the specified value to the status register in the specified SPI FLASH.  */
void rdw_spi_25x_write_status (unsigned char status)
{
    unsigned char txdata[2];
    txdata[0] = SPI_FLASH_CMD_25X_WRITE_STATUS;
    txdata[1] = status;
    rdw_spi_write_read_data(1,CS_PIN,txdata,2,0,0);
}

/* Read the status register from the specified SPI FLASH.  */
unsigned char rdw_spi_25x_read_status (void)
{
    unsigned char status,txdata;
    txdata=SPI_FLASH_CMD_25X_READ_STATUS;
    rdw_spi_write_read_data(1,CS_PIN,&txdata,1,&status,1);
    return status;
}

//static unsigned char rdw_spi_flash_25x_read_status2 (RDW_SPI_T *spi)
//{
//	unsigned char status,txdata;
//	txdata=SPI_FLASH_CMD_25X_READ_STATUS2;
//	rdw_spi_write_read_data(spi,&txdata,1,&status,1);
//	return status;
//}

/* Wait until the SPI FLASH is ready to accept a new command.  */
void rdw_spi_25x_wait_until_ready (void)
{
    while(rdw_spi_25x_read_status () & SPI_FLASH_STATUS_25X_BUSY)
        ;
}

/* Send the write enable command to the SPI FLASH.  */
void rdw_spi_25x_write_enable (void)
{
    unsigned char txdata = SPI_FLASH_CMD_25X_WRITE_ENABLE;
    rdw_spi_write_read_data(1,CS_PIN,&txdata,1,0,0);
}

/* Erase a SPI FLASH.  */
void rdw_spi_25x_chip_erase (void)
{
    /* Clear the write protect flags in the status register.  */
    rdw_spi_25x_wait_until_ready ();
    rdw_spi_25x_write_enable ();
    rdw_spi_25x_write_status (0x00);

    /* Erase the entire SPI FLASH device.  */
    rdw_spi_25x_wait_until_ready ();
    rdw_spi_25x_write_enable ();

    unsigned char txdata = SPI_FLASH_CMD_CHIP_ERASE;
    rdw_spi_write_read_data(1,CS_PIN,&txdata,1,0,0);
}


/* Erase a SPI FLASH.  */
void spi_flash_erasesector (uint32_t offset)
{
    unsigned char txdata[4];
    /* Clear the write protect flags in the status register.  */
    rdw_spi_25x_wait_until_ready ();
    rdw_spi_25x_write_enable ();
    rdw_spi_25x_write_status (0x00);

    txdata[0] = SPI_FLASH_CMD_25X_SECTOR_ERASE;
    txdata[1] = (offset>>16)&0xff;
    txdata[2] = (offset>>8)&0xff;
    txdata[3] = (offset>>0)&0xff;
    /* Erase the entire SPI FLASH device.  */
    rdw_spi_25x_wait_until_ready ();
    rdw_spi_25x_write_enable ();

    rdw_spi_write_read_data(1,CS_PIN,txdata,4,0,0);
}

/* Write a page of data in the SPI FLASH.  */
void rdw_spi_25x_write_page (long address, int length, const unsigned char *data)
{
    int i;
    unsigned char txdata[length+4];
    txdata[0] = SPI_FLASH_CMD_25X_PAGE_PROGRAM;
    txdata[1] = address>>16 & 0xff;
    txdata[2] = address>>8 & 0xff;
    txdata[3] = address &0xff;
    for(i=4; i<length+4; i++)
    {
        txdata[i] = data[i-4];
    }
    rdw_spi_25x_wait_until_ready ();
    rdw_spi_25x_write_enable ();
    rdw_spi_write_read_data(1,CS_PIN,txdata,length+4,0,0);

}

/* Read data from the specified SPI FLASH.  */
void rdw_spi_read (long address, int length, unsigned char *data)
{
    unsigned char txdata[4];
    txdata[0] = SPI_FLASH_CMD_READ_DATA;
    txdata[1] = address>>16 & 0xff;
    txdata[2] = address>>8 & 0xff;
    txdata[3] = address &0xff;
    rdw_spi_write_read_data(1,CS_PIN,txdata,4,data,length);
}

int spi0_master_test () {

    unsigned char id;
    unsigned char buffer[] = "58961234567";
    uint8_t buffersize = sizeof(buffer);
    unsigned char read_buffer[buffersize];
    int bytes;
    long address;
    bool done;
    cobra_pcrm_spi0_active_mode_clock_enable(1);
    cobra_map_pinmodeset(PIN5_INDEX,PIN_MODE_11); //clk
    cobra_map_pinmodeset(PIN4_INDEX,PIN_MODE_11);  //rx
    cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_21);  //tx
    cobra_spi_control_set(SPI0,SPI_MASTER,SAMPLE_ON_TRAILING_EDGE,IDLE_HIGH,MSB_FIRST);
    cobra_spi_set_bps(SPI0,SPI_2MHZ);
    cobra_spi_set_bits_per_word(SPI0,SPI_8bit);
    cobra_spi_enable(SPI0);
    cobra_map_pinmodeset(CS_PIN,PIN_MODE_15); 
    id = rdw_spi_get_manufacturer_id();
    printf("%x",id);
    address = 0x00;
    done = false;
    while (!done) {
        for (bytes = 0; bytes < sizeof (buffer); bytes++) {
            if (buffer[bytes] == 0x00) {
                done = true;
                break;
            }
        }
        if (bytes > 0) {
            rdw_spi_25x_write_page (address, bytes, buffer);
            address += bytes;
        }
    }
    rdw_spi_25x_wait_until_ready ();

    /* Verify the data was written correctly.  */
//	fputs ("Verifying...\n", stdout);
    address = 0x00;
    done = false;
    while (!done) {
        for (bytes = 0; bytes < sizeof (buffer); bytes++) {
            if (buffer[bytes] == 0x00) {
                done = true;
                break;
            }
        }
        if (bytes > 0) {
            rdw_spi_read (address, bytes, read_buffer);
            if (memcmp (buffer, read_buffer, bytes)) {
                printf ("ERROR: Verification failed.\n");
                return 1;
            }
            address += bytes;
        }
    }
    printf ("Verification OK\n");
//	while(1)
//	{
//		rdw_spi_flash_get_manufacturer_id(gpio,spi);
//	}
    return 0;
}
#endif