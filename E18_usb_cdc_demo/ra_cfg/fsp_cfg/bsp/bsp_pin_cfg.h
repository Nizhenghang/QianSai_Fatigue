/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_
#include "r_ioport.h"

/* Common macro for FSP header files. There is also a corresponding FSP_FOOTER macro at the end of this file. */
FSP_HEADER

#define DAC_EN (BSP_IO_PORT_00_PIN_15)
#define SCREEN_SPI_INT (BSP_IO_PORT_05_PIN_08)
#define LED3 (BSP_IO_PORT_07_PIN_04)
#define LED4 (BSP_IO_PORT_07_PIN_05)
#define CAMERA_SCL (BSP_IO_PORT_07_PIN_06)
#define CAMERA_SDA (BSP_IO_PORT_07_PIN_07)
#define SCREEN_SPI_CS (BSP_IO_PORT_10_PIN_05)
#define SCREEN_SPI_RST (BSP_IO_PORT_10_PIN_06)
#define LED1 (BSP_IO_PORT_11_PIN_00)
#define LED2 (BSP_IO_PORT_11_PIN_01)
#define KEY1 (BSP_IO_PORT_11_PIN_02)
#define KEY2 (BSP_IO_PORT_11_PIN_03)
#define KEY3 (BSP_IO_PORT_11_PIN_04)
#define KEY4 (BSP_IO_PORT_11_PIN_05)

extern const ioport_cfg_t g_bsp_pin_cfg; /* R7FA8D1BHDCBD.pincfg */

void BSP_PinConfigSecurityInit();

/* Common macro for FSP header files. There is also a corresponding FSP_HEADER macro at the top of this file. */
FSP_FOOTER
#endif /* BSP_PIN_CFG_H_ */
