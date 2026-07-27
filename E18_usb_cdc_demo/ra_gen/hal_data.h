/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_usb_basic.h"
#include "r_usb_basic_api.h"
#include "r_usb_pcdc_api.h"
#include "r_capture_api.h"
            #include "r_ceu.h"
#include "r_agt.h"
#include "r_timer_api.h"
#include "r_dtc.h"
#include "r_transfer_api.h"
#include "r_sdhi.h"
#include "r_sdmmc_api.h"
#include "r_sci_b_uart.h"
            #include "r_uart_api.h"
#include "r_sci_b_spi.h"
#include "r_spi_api.h"
#include "r_dac.h"
#include "r_dac_api.h"
#include "r_adc.h"
#include "r_adc_api.h"
#include "r_gpt.h"
#include "r_timer_api.h"
#include "r_spi_b.h"
FSP_HEADER
/* Basic on USB Instance. */
extern const usb_instance_t g_basic0;

/** Access the USB instance using these structures when calling API functions directly (::p_api is not used). */
extern usb_instance_ctrl_t g_basic0_ctrl;
extern const usb_cfg_t g_basic0_cfg;

#ifndef NULL
void NULL(void *);
#endif

#if 0 == BSP_CFG_RTOS
#ifndef usb_callback
void usb_callback(usb_callback_args_t *);
#endif
#endif

#if 2 == BSP_CFG_RTOS
#ifndef usb_callback
void usb_callback(usb_event_info_t *, usb_hdl_t, usb_onoff_t);
#endif
#endif
/** CDC Driver on USB Instance. */
/* CEU on CAPTURE instance */
            extern const capture_instance_t g_ceu_mt9v03x;
            /* Access the CEU instance using these structures when calling API functions directly (::p_api is not used). */
            extern ceu_instance_ctrl_t g_ceu_mt9v03x_ctrl;
            extern const capture_cfg_t g_ceu_mt9v03x_cfg;
            #ifndef g_ceu_mt9v03x_callback
            void g_ceu_mt9v03x_callback(capture_callback_args_t * p_args);
            #endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer1;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer1_ctrl;
extern const timer_cfg_t g_timer1_cfg;

#ifndef agt1_callback
void agt1_callback(timer_callback_args_t * p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer4;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer4_ctrl;
extern const transfer_cfg_t g_transfer4_cfg;
/** SDMMC on SDMMC Instance. */
extern const sdmmc_instance_t g_sdmmc1;


/** Access the SDMMC instance using these structures when calling API functions directly (::p_api is not used). */
extern sdhi_instance_ctrl_t g_sdmmc1_ctrl;
extern sdmmc_cfg_t g_sdmmc1_cfg;

#ifndef g_sdmmc1_callback
void g_sdmmc1_callback(sdmmc_callback_args_t * p_args);
#endif
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart3;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart3_ctrl;
            extern const uart_cfg_t g_uart3_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart3_cfg_extend;

            #ifndef uart3_callback
            void uart3_callback(uart_callback_args_t * p_args);
            #endif
/** AGT Timer Instance */
extern const timer_instance_t g_timer0;

/** Access the AGT instance using these structures when calling API functions directly (::p_api is not used). */
extern agt_instance_ctrl_t g_timer0_ctrl;
extern const timer_cfg_t g_timer0_cfg;

#ifndef agt0_callback
void agt0_callback(timer_callback_args_t * p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer1;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer1_ctrl;
extern const transfer_cfg_t g_transfer1_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer0;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer0_ctrl;
extern const transfer_cfg_t g_transfer0_cfg;
/** SPI on SCI Instance. */
extern const spi_instance_t g_sci_spi2;

/** Access the SCI_B_SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern sci_b_spi_instance_ctrl_t g_sci_spi2_ctrl;
extern const spi_cfg_t g_sci_spi2_cfg;

/** Called by the driver when a transfer has completed or an error has occurred (Must be implemented by the user). */
#ifndef spi2_callback
void spi2_callback(spi_callback_args_t * p_args);
#endif
/* CEU on CAPTURE instance */
            extern const capture_instance_t g_ceu_scc8660;
            /* Access the CEU instance using these structures when calling API functions directly (::p_api is not used). */
            extern ceu_instance_ctrl_t g_ceu_scc8660_ctrl;
            extern const capture_cfg_t g_ceu_scc8660_cfg;
            #ifndef g_ceu_scc8660_callback
            void g_ceu_scc8660_callback(capture_callback_args_t * p_args);
            #endif
/** DAC on DAC Instance. */
extern const dac_instance_t g_dac0;

/** Access the DAC instance using these structures when calling API functions directly (::p_api is not used). */
extern dac_instance_ctrl_t g_dac0_ctrl;
extern const dac_cfg_t g_dac0_cfg;
/** UART on SCI Instance. */
            extern const uart_instance_t      wifi_uart4;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     wifi_uart4_ctrl;
            extern const uart_cfg_t wifi_uart4_cfg;
            extern const sci_b_uart_extended_cfg_t wifi_uart4_cfg_extend;

            #ifndef uart4_callback
            void uart4_callback(uart_callback_args_t * p_args);
            #endif
/** ADC on ADC Instance. */
extern const adc_instance_t g_adc0;

/** Access the ADC instance using these structures when calling API functions directly (::p_api is not used). */
extern adc_instance_ctrl_t g_adc0_ctrl;
extern const adc_cfg_t g_adc0_cfg;
extern const adc_channel_cfg_t g_adc0_channel_cfg;

#ifndef adc_callback
void adc_callback(adc_callback_args_t * p_args);
#endif

#ifndef NULL
#define ADC_DMAC_CHANNELS_PER_BLOCK_NULL  2
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t servo_timer9;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t servo_timer9_ctrl;
extern const timer_cfg_t servo_timer9_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t servo_timer6;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t servo_timer6_ctrl;
extern const timer_cfg_t servo_timer6_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t encoder_timer13;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t encoder_timer13_ctrl;
extern const timer_cfg_t encoder_timer13_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t encoder_timer11;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t encoder_timer11_ctrl;
extern const timer_cfg_t encoder_timer11_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t encoder_timer10;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t encoder_timer10_ctrl;
extern const timer_cfg_t encoder_timer10_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t encoder_timer8;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t encoder_timer8_ctrl;
extern const timer_cfg_t encoder_timer8_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t motor_timer5;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t motor_timer5_ctrl;
extern const timer_cfg_t motor_timer5_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t motor_timer3;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t motor_timer3_ctrl;
extern const timer_cfg_t motor_timer3_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t motor_timer2;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t motor_timer2_ctrl;
extern const timer_cfg_t motor_timer2_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/** Timer on GPT Instance. */
extern const timer_instance_t motor_timer0;

/** Access the GPT instance using these structures when calling API functions directly (::p_api is not used). */
extern gpt_instance_ctrl_t motor_timer0_ctrl;
extern const timer_cfg_t motor_timer0_cfg;

#ifndef NULL
void NULL(timer_callback_args_t * p_args);
#endif
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer3;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer3_ctrl;
extern const transfer_cfg_t g_transfer3_cfg;
/* Transfer on DTC Instance. */
extern const transfer_instance_t g_transfer2;

/** Access the DTC instance using these structures when calling API functions directly (::p_api is not used). */
extern dtc_instance_ctrl_t g_transfer2_ctrl;
extern const transfer_cfg_t g_transfer2_cfg;
/** SPI on SPI Instance. */
extern const spi_instance_t g_spi1;

/** Access the SPI instance using these structures when calling API functions directly (::p_api is not used). */
extern spi_b_instance_ctrl_t g_spi1_ctrl;
extern const spi_cfg_t g_spi1_cfg;

/** Callback used by SPI Instance. */
#ifndef spi1_callback
void spi1_callback(spi_callback_args_t * p_args);
#endif


#define RA_NOT_DEFINED (1)
#if (RA_NOT_DEFINED == g_transfer2)
    #define g_spi1_P_TRANSFER_TX (NULL)
#else
    #define g_spi1_P_TRANSFER_TX (&g_transfer2)
#endif
#if (RA_NOT_DEFINED == g_transfer3)
    #define g_spi1_P_TRANSFER_RX (NULL)
#else
    #define g_spi1_P_TRANSFER_RX (&g_transfer3)
#endif
#undef RA_NOT_DEFINED
/** UART on SCI Instance. */
            extern const uart_instance_t      g_uart9;

            /** Access the UART instance using these structures when calling API functions directly (::p_api is not used). */
            extern sci_b_uart_instance_ctrl_t     g_uart9_ctrl;
            extern const uart_cfg_t g_uart9_cfg;
            extern const sci_b_uart_extended_cfg_t g_uart9_cfg_extend;

            #ifndef uart9_callback
            void uart9_callback(uart_callback_args_t * p_args);
            #endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
