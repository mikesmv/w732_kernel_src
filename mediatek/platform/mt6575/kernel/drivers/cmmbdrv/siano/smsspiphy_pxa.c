/****************************************************************

Siano Mobile Silicon, Inc.
MDTV receiver kernel modules.
Copyright (C) 2006-2008, Uri Shkolnik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

 This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************/
//#define PXA_310_LV

#include <linux/kernel.h>
#include <asm/irq.h>
#include <linux/semaphore.h>
//#include <asm/hardware.h>                 //xingyu add
#ifdef PXA_310_LV
#include <asm/arch/ssp.h>
#include <asm/arch/mfp.h>
#include <asm/arch/gpio.h>
#include <asm/arch/pxa3xx_gpio.h>
#endif
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/dma-mapping.h>
#include <asm/dma.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include "smsdbg_prn.h"
#include <linux/slab.h>
#ifdef PXA_310_LV

#define SSP_PORT 4
#define SSP_CKEN CKEN_SSP4
#define SMS_IRQ_GPIO MFP_PIN_GPIO93

#if (SSP_PORT == 1)
#define SDCMR_RX DRCMRRXSSDR
#define SDCMR_TX DRCMRTXSSDR
#else
#if (SSP_PORT == 2)
#define SDCMR_RX DRCMR15
#define SDCMR_TX DRCMR16
#else
#if (SSP_PORT == 3)
#define SDCMR_RX DRCMR66
#define SDCMR_TX DRCMR67
#else
#if (SSP_PORT == 4)
#define SDCMR_RX DRCMRRXSADR
#define SDCMR_TX DRCMRTXSADR
#endif
#endif
#endif
#endif
#else /*PXA_310_LV */
#define SSP_PORT 1
#define SDCMR_RX DRCMRRXSSDR
#define SDCMR_TX DRCMRTXSSDR

#endif /*PXA_310_LV */

/* Macros defining physical layer behaviour*/
#ifdef PXA_310_LV
#define CLOCK_FACTOR 1
#else /*PXA_310_LV */
#define CLOCK_FACTOR 2
#endif /*PXA_310_LV */

#include "smscoreapi.h"
#ifdef MTK_SPI                                                                      //xingyu add
#include <linux/spi/spi.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>

#include <mach/mt6575_gpio.h>
#include <linux/delay.h>
#include <mach/eint.h>
#include <mach/mt6575_spi.h>
#include <cust_gpio_usage.h>
#include <cust_eint.h>


#include <mach/mt6575_reg_base.h>
#include <mach/irqs.h>

#include <linux/io.h>                              //debug
#include <cust_cmmb.h>
#ifdef mtk_fp_spi
#include "mtk_cmmb_spi.h"
SPI_INTERNAL_HANDLE_T spi_handle;
#endif
volatile int *spi_cfg0 = NULL;                                 //debug
volatile int *spi_cmd = NULL;                                 //debug

struct SMSDev_data {
	dev_t			    devt;
	spinlock_t		    spi_lock;
	struct spi_device	*spi;
	struct list_head	device_entry;

	struct mutex		buf_lock;
	unsigned char   	users;
	u8			       *tx_buf;
	u8                         *rx_buf;
};
extern struct SMSDev_data* SMSDev;
static struct mutex g_sms_mtkspi_mutex;
static struct mt6575_chip_conf spi_conf;
#define TRANSFER_NUM (RX_BUFFER_SIZE/DMA_LEN+1)
#define MTK_SPI_RULE
#define DMA_LEN   (4*1024)
struct spi_transfer transfer[2];                // if len%1024 !=0 ,need transfer twice, due to mtk spi rule
#ifndef  mtk_fp_spi
#define CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES (0x400)
#define CMMB_SPI_INTERFACE_MAX_PKT_COUNT_PER_TIMES  (0x100)
#endif

#if 0                  //xingyu 1201
#define SPI_CMD_CPHA_MASK 0x100
#define SPI_CMD_CPOL_MASK 0x200

#define SPI_CMD_CPHA_OFFSET 8
#define SPI_CMD_CPOL_OFFSET 9

#define SPI_CMD_RX_ENDIAN_OFFSET 14
#define SPI_CMD_TX_ENDIAN_OFFSET 15
#define SPI_CMD_RX_ENDIAN_MASK 0x4000
#define SPI_CMD_TX_ENDIAN_MASK 0x8000

#define SPI_CMD_RX_MSB_OFFSET 13
#define SPI_CMD_TX_MSB_OFFSET 12
#define SPI_CMD_RX_MSB_MASK 0x4000
#define SPI_CMD_TX_MSB_MASK 0x8000


#define CUST_EINT_POLARITY_LOW              0
#define CUST_EINT_POLARITY_HIGH             1
#define CUST_EINT_DEBOUNCE_DISABLE          0
#define CUST_EINT_DEBOUNCE_ENABLE           1
#define CUST_EINT_EDGE_SENSITIVE            0
#define CUST_EINT_LEVEL_SENSITIVE           1
//////////////////////////////////////////////////////////////////////////////


#define CUST_EINT_SIANO_NUM              0
#define CUST_EINT_SIANO_DEBOUNCE_CN      0
#define CUST_EINT_SIANO_POLARITY         CUST_EINT_POLARITY_LOW
#define CUST_EINT_SIANO_SENSITIVE        CUST_EINT_EDGE_SENSITIVE
#define CUST_EINT_SIANO_DEBOUNCE_EN      CUST_EINT_DEBOUNCE_DISABLE

#define GPIO_CMMB_POWER_PIN         GPIO180
#define GPIO_CMMB_POWER_PIN_M_GPIO   GPIO_MODE_00
#define GPIO_CMMB_POWER_PIN_M_CLK   GPIO_MODE_03
#define GPIO_CMMB_POWER_PIN_CLK     CLK_OUT5

#define GPIO_CMMB_EINT_PIN         GPIO187
#define GPIO_CMMB_EINT_PIN_M_GPIO   GPIO_MODE_00
#define GPIO_CMMB_EINT_PIN_M_EINT   GPIO_MODE_01

#define GPIO_CMMB_RESET_PIN         GPIO177
#define GPIO_CMMB_RESET_PIN_M_GPIO   GPIO_MODE_00
#define GPIO_CMMB_RESET_PIN_M_DAIPCMOUT   GPIO_MODE_01
#endif
#endif
/* Macros for coding reuse */

/*! macro to align the divider to the proper offset in the register bits */
#define CLOCK_DIVIDER(i)((i-1)<<8)	/* 1-4096 */

/*! DMA related macros */
#define DMA_INT_MASK (DCSR_ENDINTR | DCSR_STARTINTR | DCSR_BUSERR)
#define RESET_DMA_CHANNEL (DCSR_NODESC | DMA_INT_MASK)

#define SSP_TIMEOUT_SCALE (769)
#define SSP_TIMEOUT(x) ((x*10000)/SSP_TIMEOUT_SCALE)

#define SPI_PACKET_SIZE 256

/* physical layer variables */
/*! global bus data */
struct spiphy_dev_s {
//	struct ssp_dev sspdev;	/*!< ssp port configuration */                              //xingyu add
	struct completion transfer_in_process;
	void (*interruptHandler) (void *);
	void *intr_context;
	struct device *dev;	/*!< device model stuff */
	int rx_dma_channel;
	int tx_dma_channel;
	int rx_buf_len;
	int tx_buf_len;

	int temp;                                      //xingyu add
};

/*!
invert the endianness of a single 32it integer

\param[in]		u: word to invert

\return		the inverted word
*/
static inline u32 invert_bo(u32 u)
{
	return ((u & 0xff) << 24) | ((u & 0xff00) << 8) | ((u & 0xff0000) >> 8)
		| ((u & 0xff000000) >> 24);
}

/*!
invert the endianness of a data buffer

\param[in]		buf: buffer to invert
\param[in]		len: buffer length

\return		the inverted word
*/

static int invert_endianness(char *buf, int len)
{
	int i;
	u32 *ptr = (u32 *) buf;

	len = (len + 3) / 4;
	for (i = 0; i < len; i++, ptr++)
		*ptr = invert_bo(*ptr);

	return 4 * ((len + 3) & (~3));
}

/*! Map DMA buffers when request starts

\return	error status
*/
static unsigned long dma_map_buf(struct spiphy_dev_s *spiphy_dev, char *buf,
		int len, int direction)
{
	unsigned long phyaddr;
	/* map dma buffers */
	if (!buf) {
		PERROR(" NULL buffers to map\n");
		return 0;
	}
	/* map buffer */
#ifdef PXA_310_LV                                    //xingyu add
	phyaddr = dma_map_single(spiphy_dev->dev, buf, len, direction);
	if (dma_mapping_error(phyaddr)) {
		PERROR("exiting  with error\n");
		return 0;
	}
#endif
	return phyaddr;
}

static irqreturn_t spibus_interrupt(int irq, void *context)
{
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;
	//PDEBUG("recieved interrupt from device dev=%p.\n", context);
	PDEBUG("Eint");
	if (spiphy_dev->interruptHandler)
		spiphy_dev->interruptHandler(spiphy_dev->intr_context);
	return IRQ_HANDLED;

}

/*!	DMA controller callback - called only on BUS error condition

\param[in]	channel: DMA channel with error
\param[in]	data: Unused
\param[in]	regs: Unused
\return		void
*/
static void spibus_dma_handler(int channel, void *context)
{
#ifdef PXA_310_LV                                    //xingyu add
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;
	u32 irq_status = DCSR(channel) & DMA_INT_MASK;

	PDEBUG("recieved interrupt from dma channel %d irq status %x.\n",
	       channel, irq_status);
	if (irq_status & DCSR_BUSERR) {
		PERROR("bus error!!! resetting channel %d\n", channel);

		DCSR(spiphy_dev->rx_dma_channel) = RESET_DMA_CHANNEL;
		DCSR(spiphy_dev->tx_dma_channel) = RESET_DMA_CHANNEL;
	}
	DCSR(spiphy_dev->rx_dma_channel) = RESET_DMA_CHANNEL;
	complete(&spiphy_dev->transfer_in_process);
#endif
}

#ifdef MTK_SPI                                                                      //xingyu add
static void SMSDev_complete(void *arg)
{
	complete(arg);
}


static ssize_t SMSDev_sync(struct SMSDev_data *SMSDevS, struct spi_message *message)
{
	int ret;
/*
	if(!flag_spi_ok){
		inno_msg("Warning:spi_sync timeout,should not to use spi");
		return -1;
	}	
*/
        kmutex_lock(&g_sms_mtkspi_mutex); 
	//inno_msg("**s"); 
	if (SMSDevS==NULL || SMSDevS->spi == NULL){
		sms_err("error :spi ==NULL \r\n");
		ret = -ESHUTDOWN;
	}
	else
		ret = spi_sync(SMSDevS->spi, message);
	// inno_msg("**e");
        kmutex_unlock(&g_sms_mtkspi_mutex); 

	if(ret){
		sms_err("spi_sync fail ret=%d,should check",ret);
/*
		if(ret == -ETIME){
			flag_spi_ok = 0;
		}
*/
	}		 
	return ret;
}
#endif

void smsspibus_xfer(void *context, unsigned char *txbuf,
		    unsigned long txbuf_phy_addr, unsigned char *rxbuf,
		    unsigned long rxbuf_phy_addr, int len)
{
#ifdef MTK_SPI                                                                      //xingyu add    
#ifdef MTK_SPI_RULE
#ifdef mtk_fp_spi
	int const pkt_count = len / CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	int const remainder = len % CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	int result;	
//	printk("-->s len=%d\n",len);
	unsigned char * tt_txbuf=NULL,* tt_rxbuf=NULL;
	if(len>CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES){	
		result=mtk_spi_readwrite(&spi_handle,txbuf,rxbuf,CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES,pkt_count,1); 	
		if(0 != remainder)	 { 
			tt_txbuf=(txbuf)?txbuf+ (CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count):NULL;
			tt_rxbuf=(rxbuf)?rxbuf+ (CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count):NULL;
			result=mtk_spi_readwrite(&spi_handle,tt_txbuf,tt_rxbuf,remainder,1,1);
		}
	}
	else{
		result= mtk_spi_readwrite(&spi_handle,txbuf,rxbuf,len,1,1);
	}
//	printk("s<--\n");
	return;
#else   //mtk_fp_spi  using sp spi
	struct spi_message msg;
	spi_message_init(&msg);
	int const pkt_count = len / CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
	int const remainder = len % CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES;
//	printk("-->s len=%d\n",len);
	if(len>CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES){	
		transfer[0].tx_buf =(txbuf==NULL)?NULL: txbuf;
		transfer[0].rx_buf =(rxbuf==NULL)?NULL: rxbuf;
		transfer[0].len = CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES*pkt_count;
		spi_message_add_tail(&transfer[0], &msg);

		if(0 != remainder)	 { 
			transfer[1].tx_buf =(txbuf==NULL)?NULL:txbuf+ (CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count);
			transfer[1].rx_buf =(rxbuf==NULL)?NULL:rxbuf+ (CMMB_SPI_INTERFACE_MAX_PKT_LENGTH_PER_TIMES * pkt_count);
			transfer[1].len = remainder;
			spi_message_add_tail(&transfer[1], &msg);
		}
	}
	else{
		transfer[0].tx_buf =(txbuf==NULL)?NULL: txbuf;
		transfer[0].rx_buf =(rxbuf==NULL)?NULL: rxbuf;
		transfer[0].len = len;
		spi_message_add_tail(&transfer[0], &msg);
	}
	SMSDev_sync(SMSDev,&msg);	
//	printk("s<--\n");
	return;
#endif   //mtk_fp_spi	
#else    // else MTK_SPI_RULE
  	int i;
	struct spi_message msg;
	spi_message_init(&msg);

       int index=0;
	for(i=0;i<len;i=i+DMA_LEN)
       {
              index = i/DMA_LEN;
	       // setting tx_buf		  
                   transfer[index].tx_buf =(txbuf==NULL)?NULL: (unsigned char *)txbuf+i;
              // setting rx_buf
                   transfer[index].rx_buf =(rxbuf==NULL)?NULL: (unsigned char *)rxbuf+i;
		// setting len
		    transfer[index].len = (len<i+DMA_LEN)?(len-i):DMA_LEN;
		//add msg
		    spi_message_add_tail(&transfer[index], &msg);
	}
//	printk("*s\n");
       SMSDev_sync(SMSDev,&msg);	
//	printk("*e\n");
	//printk
/*
	if(txbuf){
		 printk("txbuf:  ");
		 for(i=0;i<16;i++)
		         printk("%x ",txbuf[i]);
		printk("\n");
	}
       if(rxbuf){
		printk("rxbuf:  ");
		for(i=0;i<16;i++)
		         printk("%x ",rxbuf[i]);
		printk("\n");
       }
*/       
	return;
#endif
#endif

#ifdef PXA_310_LV                                    //xingyu add
	/* DMA burst is 8 bytes, therefore we need tmp buffer that size. */
	unsigned long tmp[2];
	unsigned long txdma;
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;

	/* program the controller */
	if (txbuf)
		invert_endianness(txbuf, len);

	tmp[0] = -1;
	tmp[1] = -1;

	/* map RX buffer */

	if (!txbuf)
		txdma =
		    dma_map_buf(spiphy_dev, (char *)tmp, sizeof(tmp),
				DMA_TO_DEVICE);
	else
		txdma = txbuf_phy_addr;

	init_completion(&spiphy_dev->transfer_in_process);
	/* configure DMA Controller */
	DCSR(spiphy_dev->rx_dma_channel) = RESET_DMA_CHANNEL;
	DSADR(spiphy_dev->rx_dma_channel) = __PREG(SSDR_P(SSP_PORT));
	DTADR(spiphy_dev->rx_dma_channel) = rxbuf_phy_addr;
	DCMD(spiphy_dev->rx_dma_channel) = DCMD_INCTRGADDR | DCMD_FLOWSRC
	    | DCMD_WIDTH4 | DCMD_ENDIRQEN | DCMD_BURST8 | len;
	PDEBUG("rx channel=%d, src=0x%x, dst=0x%x, cmd=0x%x\n",
	       spiphy_dev->rx_dma_channel, __PREG(SSDR_P(SSP_PORT)),
	       (unsigned int)rxbuf_phy_addr, DCMD(spiphy_dev->rx_dma_channel));
	spiphy_dev->rx_buf_len = len;

	DCSR(spiphy_dev->tx_dma_channel) = RESET_DMA_CHANNEL;
	DTADR(spiphy_dev->tx_dma_channel) = __PREG(SSDR_P(SSP_PORT));
	DSADR(spiphy_dev->tx_dma_channel) = txdma;
	if (txbuf) {
		DCMD(spiphy_dev->tx_dma_channel) =
		    DCMD_INCSRCADDR | DCMD_FLOWTRG | DCMD_WIDTH4
		    /* | DCMD_ENDIRQEN */  | DCMD_BURST8 | len;
		spiphy_dev->tx_buf_len = len;
	} else {
		DCMD(spiphy_dev->tx_dma_channel) = DCMD_FLOWTRG
		    | DCMD_WIDTH4 /* | DCMD_ENDIRQEN */  | DCMD_BURST8 | len;
		spiphy_dev->tx_buf_len = 4;
	}

	PDEBUG("tx channel=%d, src=0x%x, dst=0x%x, cmd=0x%x\n",
	       spiphy_dev->tx_dma_channel, (unsigned int)txdma,
	       __PREG(SSDR_P(SSP_PORT)), DCMD(spiphy_dev->tx_dma_channel));
	/* DALGN - DMA ALIGNMENT REG. */
	if (rxbuf_phy_addr & 0x7)
		DALGN |= (1 << spiphy_dev->rx_dma_channel);
	else
		DALGN &= ~(1 << spiphy_dev->rx_dma_channel);
	if (txdma & 0x7)
		DALGN |= (1 << spiphy_dev->tx_dma_channel);
	else
		DALGN &= ~(1 << spiphy_dev->tx_dma_channel);

	/* Start DMA controller */
	DCSR(spiphy_dev->rx_dma_channel) |= DCSR_RUN;
	DCSR(spiphy_dev->tx_dma_channel) |= DCSR_RUN;
	PDEBUG("DMA running. wait for completion.\n");
	wait_for_completion(&spiphy_dev->transfer_in_process);
	PDEBUG("DMA complete.\n");
	invert_endianness(rxbuf, len);
#endif
}

void smschipreset()
{
#ifdef MTK_SPI                                                                      //xingyu add
	/*reset*/
	mt_set_gpio_mode(GPIO_CMMB_RST_PIN, GPIO_CMMB_RST_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_CMMB_RST_PIN, GPIO_DIR_OUT);

	mdelay(1);
	mt_set_gpio_out(GPIO_CMMB_RST_PIN, GPIO_OUT_ZERO); 	
	mdelay(100);                                                                         //siano suggestion
	mt_set_gpio_out(GPIO_CMMB_RST_PIN, GPIO_OUT_ONE); 	
	mdelay(100);                                                           //1122
#endif
}
void smschipSpiSetup()
{
#ifdef MTK_SPI          
#ifdef mtk_fp_spi	
               SPI_MODE_T mode_parameter;
		 mtk_spi_open();
	        mtk_spi_config(NULL);	
		 
               mode_parameter.mode=SPI_MODE_DEASSERT;
               mode_parameter.bEnable=false;
	        mtk_spi_mode_setting(mode_parameter);
	        mode_parameter.mode=SPI_MODE_PAUSE;
               mode_parameter.bEnable=false;
               mtk_spi_mode_setting(mode_parameter);
#else
	       struct mt6575_chip_conf* spi_par;
	       SMSDev->spi->controller_data=(void*)&spi_conf;
	       memset(&spi_conf,0,sizeof(struct mt6575_chip_conf));
	       spi_par =&spi_conf;
	       if(!spi_par){
		       PDEBUG("spi config fail");
		       return -1;
	       }
	       spi_par->setuptime =15;
	       spi_par->holdtime = 15;
	       spi_par->high_time = 10;       //10--6m   15--4m   20--3m  30--2m  [ 60--1m 120--0.5m  300--0.2m]
	       spi_par->low_time = 10;
	       spi_par->cs_idletime = 20;

	       spi_par->rx_mlsb = 1; 
	       spi_par->tx_mlsb = 1;		 
	       spi_par->tx_endian = 0;
	       spi_par->rx_endian = 0;

	       spi_par->cpol = 0;
	       spi_par->cpha = 0;
	       spi_par->com_mod = DMA_TRANSFER;
	       //spi_par->com_mod = FIFO_TRANSFER;

	       spi_par->pause = 0;
	       spi_par->finish_intr = 1;
	       spi_par->deassert = 0;

	       printk("setuptime=%d \n",spi_par->setuptime);
	       if(spi_setup(SMSDev->spi)){
		       PDEBUG("spi_setup fail");
		       return -1;
	       }
	       printk("DMA_LEN=%d",DMA_LEN);
#endif   //mtk_fp_spi
#endif
}


typedef void (*func_ptr)(void);
func_ptr intHander=NULL;
void smschipEintEnable()
{
#ifdef MTK_SPI                 
	// spi setup
	sms_info("smschipSpiSetup");
	smschipSpiSetup();

	mt_set_gpio_mode(GPIO_CMMB_EINT_PIN, GPIO_CMMB_EINT_PIN_M_EINT);                 //set to eint MODE for enable eint function
	mt_set_gpio_dir(GPIO_CMMB_EINT_PIN, GPIO_DIR_IN); 
	// register irp 
	mt65xx_eint_set_sens(CUST_EINT_CMMB_NUM, CUST_EINT_EDGE_SENSITIVE);
	mt65xx_eint_registration(CUST_EINT_CMMB_NUM, CUST_EINT_DEBOUNCE_DISABLE, CUST_EINT_POLARITY_HIGH, intHander, 0);         //1202
	mt65xx_eint_mask(CUST_EINT_CMMB_NUM);   
	sms_info("[smsspiphy_init]CMMB GPIO EINT PIN mode: %d, dir:%d\n",mt_get_gpio_mode(GPIO_CMMB_EINT_PIN),
			mt_get_gpio_dir(GPIO_CMMB_EINT_PIN));    

	mt65xx_eint_unmask(CUST_EINT_CMMB_NUM);                        //enable eint

	sms_info("CMMB GPIO EINT PIN mode: %d, dir:%d\n",mt_get_gpio_mode(GPIO_CMMB_EINT_PIN),
			mt_get_gpio_dir(GPIO_CMMB_EINT_PIN));    
#endif
}
void smschipEintDisable()
{
#ifdef MTK_SPI                                                                      
        //xingyu  interrupt setting
	mt65xx_eint_mask(CUST_EINT_CMMB_NUM);   
	mt_set_gpio_mode(GPIO_CMMB_EINT_PIN, GPIO_CMMB_EINT_PIN_M_GPIO);                 //set to eint MODE for enable eint function
	mt_set_gpio_pull_enable(GPIO_CMMB_EINT_PIN, GPIO_PULL_DISABLE);
	sms_info("CMMB GPIO EINT PIN mode:num:%d, %d, dir:%d,pullen:%d,pullup%d",GPIO_CMMB_EINT_PIN,mt_get_gpio_mode(GPIO_CMMB_EINT_PIN),
			mt_get_gpio_dir(GPIO_CMMB_EINT_PIN),mt_get_gpio_pull_enable(GPIO_CMMB_EINT_PIN),mt_get_gpio_pull_select(GPIO_CMMB_EINT_PIN));    
#endif
}

/*
void *smsspiphy_init(void *context, void (*smsspi_interruptHandler) (void *),
		     void *intr_context)
*/		               //xingyu add

void *smsspiphy_init(void *context, void (*smsspi_interruptHandler) (void ),
		     void *intr_context)		     
{
#ifdef MTK_SPI   //xingyu add

	struct mt6575_chip_conf* spi_par;
	kmutex_init(&g_sms_mtkspi_mutex);
	/*          // move to smschipEintEnable function to fix issue that cmmb can't use when resume
	//xingyu  interrupt setting
	mt_set_gpio_mode(GPIO_CMMB_EINT_PIN, GPIO_CMMB_EINT_PIN_M_EINT);                 //set to eint MODE for enable eint function
	mt_set_gpio_dir(GPIO_CMMB_EINT_PIN, GPIO_DIR_IN); 

	// register irp 
	mt65xx_eint_set_sens(CUST_EINT_CMMB_NUM, CUST_EINT_CMMB_SENSITIVE);
	mt65xx_eint_registration(CUST_EINT_CMMB_NUM, 0, CUST_EINT_POLARITY_HIGH, smsspi_interruptHandler, 0);         //1202
	mt65xx_eint_mask(CUST_EINT_CMMB_NUM);   
	sms_info("[smsspiphy_init]CMMB GPIO EINT PIN mode: %d, dir:%d\n",mt_get_gpio_mode(GPIO_CMMB_EINT_PIN),
	mt_get_gpio_dir(GPIO_CMMB_EINT_PIN));    
	 */
	printk("smsspi_interruptHandler =%p \n",smsspi_interruptHandler);
	intHander = (func_ptr)smsspi_interruptHandler;
#ifdef REQUEST_FIRMWARE_SUPPORTED   //xingyu 1202 ko version
	//power on
        cust_cmmb_power_on();
	//reset 
	smschipreset(context);
	//eint enable
	smschipEintEnable();
#endif  //REQUEST_FIRMWARE_SUPPORTED

#if 0
	//setting spi config
	//debug	 cfg0	
	spi_cfg0 = (int*)ioremap(0x700B2000,4);
	//printk("[Siano_CharDriver]SPI CFG0:%8x\n",*spi_cfg0);
	*spi_cfg0 &= 0x00000000;
	*spi_cfg0 |= 10 | (10<<8) |(15<<16)|(15<<24);                                //6m
	//*spi_cfg0 |= 30 | (30<<8) |(15<<16)|(15<<24);                              //1203  2m
	// *spi_cfg0 |= 6 | (6<<8) |(15<<16)|(15<<24);                              //1203  10m
	//*spi_cfg0 |= 10 | (10<<8);
	//printk("[Siano_CharDriver]SPI CFG0:%8x\n",*spi_cfg0);

	//debug	 cmd
	spi_cmd = (int*)ioremap(0x700B2000+0x18,4);
	//printk("[Siano_CharDriver]SPI CMD:%8x\n",*spi_cmd);
	*spi_cmd &= ~(SPI_CMD_CPHA_MASK);	
	*spi_cmd &= ~(SPI_CMD_CPOL_MASK);
	*spi_cmd &= ~(SPI_CMD_TX_ENDIAN_MASK|SPI_CMD_RX_ENDIAN_MASK);
	*spi_cmd |= (0<<SPI_CMD_CPOL_OFFSET|0<<SPI_CMD_CPHA_OFFSET);
	*spi_cmd |= (1<<SPI_CMD_RX_MSB_OFFSET|1<<SPI_CMD_TX_MSB_OFFSET);
#else
#ifdef mtk_fp_spi	
	SPI_MODE_T mode_parameter;
	mtk_spi_open();
	mtk_spi_config(NULL);	

	mode_parameter.mode=SPI_MODE_DEASSERT;
	mode_parameter.bEnable=false;
	mtk_spi_mode_setting(mode_parameter);
	mode_parameter.mode=SPI_MODE_PAUSE;
	mode_parameter.bEnable=false;
	mtk_spi_mode_setting(mode_parameter);
#else
	if(SMSDev && SMSDev->spi)
	{
		SMSDev->spi->controller_data=(void*)&spi_conf;
		spi_par =&spi_conf;
		if(!spi_par){
			PDEBUG("spi config fail");
			return -1;
		}
		spi_par->setuptime =15;
		spi_par->holdtime = 15;
		spi_par->high_time = 10;       //10--6m   15--4m   20--3m  30--2m  [ 60--1m 120--0.5m  300--0.2m]
		spi_par->low_time = 10;
		spi_par->cs_idletime = 20;

		spi_par->rx_mlsb = 1; 
		spi_par->tx_mlsb = 1;		 
		spi_par->tx_endian = 0;
		spi_par->rx_endian = 0;

		spi_par->cpol = 0;
		spi_par->cpha = 0;
		spi_par->com_mod = DMA_TRANSFER;
		//spi_par->com_mod = FIFO_TRANSFER;

		spi_par->pause = 0;
		spi_par->finish_intr = 1;
		spi_par->deassert = 0;

		printk("setuptime=%d \n",spi_par->setuptime);
		if(spi_setup(SMSDev->spi)){
			PDEBUG("spi_setup fail");
			return -1;
		}
	}
	else
		sms_err("SMSDev && SMSDev->spi =NULL");
	printk("DMA_LEN=%d",DMA_LEN);
#endif   //mtk_fp_spi
#endif //MTK_SPI
	PDEBUG("DMA_LEN=%d",DMA_LEN);
	//
	//     PERROR("******* sleep smsspiphy_init *************");
	//     msleep(1000*60*30);                              //xingyu add debug interrupt

	struct spiphy_dev_s *spiphy_dev;
	spiphy_dev = kmalloc(sizeof(struct spiphy_dev_s), GFP_KERNEL);
	spiphy_dev->temp = 1;
	return spiphy_dev;
#endif

#ifdef PXA_310_LV                                    //xingyu add
	int ret;
	struct spiphy_dev_s *spiphy_dev;
	u32 mode = 0, flags = 0, psp_flags = 0, speed = 0;
	PDEBUG("entering\n");

	spiphy_dev = kmalloc(sizeof(struct spiphy_dev_s), GFP_KERNEL);

	ret = ssp_init(&spiphy_dev->sspdev, SSP_PORT, 0);
	if (ret) {
		PERROR("ssp_init failed. error %d", ret);
		goto error_sspinit;
	}
#ifdef PXA_310_LV
	pxa3xx_mfp_set_afds(SMS_IRQ_GPIO, MFP_AF0, MFP_DS03X);              //xingyu ����interrupt
	pxa3xx_gpio_set_rising_edge_detect(SMS_IRQ_GPIO, 1);
	pxa3xx_gpio_set_direction(SMS_IRQ_GPIO, GPIO_DIR_IN);
#else /*PXA_310_LV */
	/* receive input interrupts from the SMS 1000 on J32 pin 11 */
	pxa_gpio_mode(22 | GPIO_IN);
#endif /*PXA_310_LV */
	speed = CLOCK_DIVIDER(CLOCK_FACTOR); /* clock divisor for this mode */
	/* 32bit words in the fifo */
	mode = SSCR0_Motorola | SSCR0_DataSize(16) | SSCR0_EDSS;
	/* SSCR1 = flags */
	flags = SSCR1_RxTresh(1) | SSCR1_TxTresh(1) | SSCR1_TSRE |
	 SSCR1_RSRE | SSCR1_RIE | SSCR1_TRAIL;	/* | SSCR1_TIE */

	ssp_config(&spiphy_dev->sspdev, mode, flags, psp_flags, speed);
	ssp_disable(&(spiphy_dev->sspdev));
#ifdef PXA_310_LV                        //xingyu spi io

	pxa3xx_mfp_set_afds(MFP_PIN_GPIO95, MFP_AF1, MFP_DS03X);
	pxa3xx_mfp_set_afds(MFP_PIN_GPIO96, MFP_AF1, MFP_DS03X);
	pxa3xx_mfp_set_afds(MFP_PIN_GPIO97, MFP_AF1, MFP_DS03X);
	pxa3xx_mfp_set_afds(MFP_PIN_GPIO98, MFP_AF1, MFP_DS03X);
#else /*PXA_310_LV */
	pxa_gpio_mode(GPIO23_SCLK_MD);
	pxa_gpio_mode(GPIO24_SFRM_MD);
	pxa_gpio_mode(GPIO25_STXD_MD);
	pxa_gpio_mode(GPIO26_SRXD_MD);
#endif /*PXA_310_LV */
	/* setup the dma */
	spiphy_dev->rx_dma_channel =
	    pxa_request_dma("spibusdrv_rx", DMA_PRIO_HIGH, spibus_dma_handler,
			    spiphy_dev);
	if (spiphy_dev->rx_dma_channel < 0) {
		ret = -EBUSY;
		PERROR("Could not get RX DMA channel.\n");
		goto error_rxdma;
	}
	spiphy_dev->tx_dma_channel =
	    pxa_request_dma("spibusdrv_tx", DMA_PRIO_HIGH, spibus_dma_handler,
			    spiphy_dev);
	if (spiphy_dev->tx_dma_channel < 0) {
		ret = -EBUSY;
		PERROR("Could not get TX DMA channel.\n");
		goto error_txdma;
	}

	SDCMR_RX = DRCMR_MAPVLD | spiphy_dev->rx_dma_channel;
	SDCMR_TX = DRCMR_MAPVLD | spiphy_dev->tx_dma_channel;

	PDEBUG("dma rx channel: %d, dma tx channel: %d\n",
	       spiphy_dev->rx_dma_channel, spiphy_dev->tx_dma_channel);
	/* enable the clock */

	spiphy_dev->interruptHandler = smsspi_interruptHandler;
	spiphy_dev->intr_context = intr_context;
#ifdef PXA_310_LV
	set_irq_type(IRQ_GPIO(MFP2GPIO(SMS_IRQ_GPIO)), IRQT_FALLING);
	ret =
	    request_irq(IRQ_GPIO(MFP2GPIO(SMS_IRQ_GPIO)), spibus_interrupt,
			SA_INTERRUPT, "SMSSPI", spiphy_dev);
#else /*PXA_310_LV */
	set_irq_type(IRQ_GPIO(22), IRQT_FALLING);
	ret =
	    request_irq(IRQ_GPIO(22), spibus_interrupt, SA_INTERRUPT, "SMSSPI",
			&(g_spidata.sspdev));
#endif /*PXA_310_LV */
	if (ret) {
		PERROR("Could not get interrupt for SMS device. status =%d\n",
		       ret);
		goto error_irq;
	}

	ssp_enable(&(spiphy_dev->sspdev));
	PDEBUG("exiting\n");
	return spiphy_dev;
error_irq:
	if (spiphy_dev->tx_dma_channel >= 0)
		pxa_free_dma(spiphy_dev->tx_dma_channel);

error_txdma:
	if (spiphy_dev->rx_dma_channel >= 0)
		pxa_free_dma(spiphy_dev->rx_dma_channel);

error_rxdma:
	ssp_exit(&spiphy_dev->sspdev);
error_sspinit:
	PDEBUG("exiting on error\n");
#endif
	return 0;
}

int smsspiphy_deinit(void *context)
{
#ifdef mtk_fp_spi
        printk("mtk spi close\n");
	mtk_spi_close();
#endif //mtk_fp_spi
#ifdef PXA_310_LV                                    //xingyu add
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;
	PDEBUG("entering\n");

	/* disable the spi port */
	ssp_flush(&spiphy_dev->sspdev);
	ssp_disable(&spiphy_dev->sspdev);

	/*  release DMA resources */
	if (spiphy_dev->rx_dma_channel >= 0)
		pxa_free_dma(spiphy_dev->rx_dma_channel);

	if (spiphy_dev->tx_dma_channel >= 0)
		pxa_free_dma(spiphy_dev->tx_dma_channel);

	/* release Memory resources */
#ifdef PXA_310_LV
	free_irq(IRQ_GPIO(MFP2GPIO(SMS_IRQ_GPIO)), spiphy_dev);
#else /*PXA_310_LV */
	free_irq(IRQ_GPIO(22), &spiphy_dev->sspdev);
#endif /*PXA_310_LV */
	ssp_exit(&spiphy_dev->sspdev);
	PDEBUG("exiting\n");
#endif
	return 0;
}

/*
       //xingyu 
*/
void smsspiphy_set_config(struct spiphy_dev_s *spiphy_dev, int clock_divider)
{
#ifdef PXA_310_LV                                                                  //xingyu add
	u32 mode, flags, speed, psp_flags = 0;
	ssp_disable(&spiphy_dev->sspdev);
	/* clock divisor for this mode. */
	speed = CLOCK_DIVIDER(clock_divider);
	/* 32bit words in the fifo */
	mode = SSCR0_Motorola | SSCR0_DataSize(16) | SSCR0_EDSS;
	flags = SSCR1_RxTresh(1) | SSCR1_TxTresh(1) | SSCR1_TSRE |
		 SSCR1_RSRE | SSCR1_RIE | SSCR1_TRAIL;	/* | SSCR1_TIE */
	ssp_config(&spiphy_dev->sspdev, mode, flags, psp_flags, speed);
	ssp_enable(&spiphy_dev->sspdev);
#endif
}

void prepareForFWDnl(void *context)
{
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;
	smsspiphy_set_config(spiphy_dev, 2);
	msleep(100);
}

void fwDnlComplete(void *context, int App)
{
	struct spiphy_dev_s *spiphy_dev = (struct spiphy_dev_s *) context;
	smsspiphy_set_config(spiphy_dev, 1);
	msleep(100);
	sms_info("end");
}
