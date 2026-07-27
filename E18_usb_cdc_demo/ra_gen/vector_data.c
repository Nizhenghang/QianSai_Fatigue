/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_MAX_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_b_uart_rxi_isr, /* SCI9 RXI (Receive data full) */
            [1] = sci_b_uart_txi_isr, /* SCI9 TXI (Transmit data empty) */
            [2] = sci_b_uart_tei_isr, /* SCI9 TEI (Transmit end) */
            [3] = sci_b_uart_eri_isr, /* SCI9 ERI (Receive error) */
            [4] = spi_b_rxi_isr, /* SPI1 RXI (Receive buffer full) */
            [5] = spi_b_txi_isr, /* SPI1 TXI (Transmit buffer empty) */
            [6] = spi_b_tei_isr, /* SPI1 TEI (Transmission complete event) */
            [7] = spi_b_eri_isr, /* SPI1 ERI (Error) */
            [8] = adc_scan_end_isr, /* ADC0 SCAN END (End of A/D scanning operation) */
            [9] = sci_b_uart_rxi_isr, /* SCI4 RXI (Receive data full) */
            [10] = sci_b_uart_txi_isr, /* SCI4 TXI (Transmit data empty) */
            [11] = sci_b_uart_tei_isr, /* SCI4 TEI (Transmit end) */
            [12] = sci_b_uart_eri_isr, /* SCI4 ERI (Receive error) */
            [13] = ceu_isr, /* CEU CEUI (CEU interrupt) */
            [14] = sci_b_spi_rxi_isr, /* SCI2 RXI (Receive data full) */
            [15] = sci_b_spi_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [16] = sci_b_spi_tei_isr, /* SCI2 TEI (Transmit end) */
            [17] = sci_b_spi_eri_isr, /* SCI2 ERI (Receive error) */
            [18] = agt_int_isr, /* AGT0 INT (AGT interrupt) */
            [19] = sci_b_uart_rxi_isr, /* SCI3 RXI (Receive data full) */
            [20] = sci_b_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [21] = sci_b_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [22] = sci_b_uart_eri_isr, /* SCI3 ERI (Receive error) */
            [23] = sdhimmc_accs_isr, /* SDHIMMC1 ACCS (Card access) */
            [24] = sdhimmc_card_isr, /* SDHIMMC1 CARD (Card detect) */
            [25] = sdhimmc_dma_req_isr, /* SDHIMMC1 DMA REQ (DMA transfer request) */
            [26] = agt_int_isr, /* AGT1 INT (AGT interrupt) */
            [27] = usbfs_interrupt_handler, /* USBFS INT (USBFS interrupt) */
            [28] = usbfs_resume_handler, /* USBFS RESUME (USBFS resume interrupt) */
            [29] = usbfs_d0fifo_handler, /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
            [30] = usbfs_d1fifo_handler, /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
            [31] = usbhs_interrupt_handler, /* USBHS USB INT RESUME (USBHS interrupt) */
            [32] = usbhs_d0fifo_handler, /* USBHS FIFO 0 (DMA transfer request 0) */
            [33] = usbhs_d1fifo_handler, /* USBHS FIFO 1 (DMA transfer request 1) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_MAX_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI9_RXI,GROUP0), /* SCI9 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TXI,GROUP1), /* SCI9 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI9_TEI,GROUP2), /* SCI9 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI9_ERI,GROUP3), /* SCI9 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_SPI1_RXI,GROUP4), /* SPI1 RXI (Receive buffer full) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TXI,GROUP5), /* SPI1 TXI (Transmit buffer empty) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SPI1_TEI,GROUP6), /* SPI1 TEI (Transmission complete event) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SPI1_ERI,GROUP7), /* SPI1 ERI (Error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_ADC0_SCAN_END,GROUP0), /* ADC0 SCAN END (End of A/D scanning operation) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI4_RXI,GROUP1), /* SCI4 RXI (Receive data full) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TXI,GROUP2), /* SCI4 TXI (Transmit data empty) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI4_TEI,GROUP3), /* SCI4 TEI (Transmit end) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI4_ERI,GROUP4), /* SCI4 ERI (Receive error) */
            [13] = BSP_PRV_VECT_ENUM(EVENT_CEU_CEUI,GROUP5), /* CEU CEUI (CEU interrupt) */
            [14] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP6), /* SCI2 RXI (Receive data full) */
            [15] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP7), /* SCI2 TXI (Transmit data empty) */
            [16] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP0), /* SCI2 TEI (Transmit end) */
            [17] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP1), /* SCI2 ERI (Receive error) */
            [18] = BSP_PRV_VECT_ENUM(EVENT_AGT0_INT,GROUP2), /* AGT0 INT (AGT interrupt) */
            [19] = BSP_PRV_VECT_ENUM(EVENT_SCI3_RXI,GROUP3), /* SCI3 RXI (Receive data full) */
            [20] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TXI,GROUP4), /* SCI3 TXI (Transmit data empty) */
            [21] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TEI,GROUP5), /* SCI3 TEI (Transmit end) */
            [22] = BSP_PRV_VECT_ENUM(EVENT_SCI3_ERI,GROUP6), /* SCI3 ERI (Receive error) */
            [23] = BSP_PRV_VECT_ENUM(EVENT_SDHIMMC1_ACCS,GROUP7), /* SDHIMMC1 ACCS (Card access) */
            [24] = BSP_PRV_VECT_ENUM(EVENT_SDHIMMC1_CARD,GROUP0), /* SDHIMMC1 CARD (Card detect) */
            [25] = BSP_PRV_VECT_ENUM(EVENT_SDHIMMC1_DMA_REQ,GROUP1), /* SDHIMMC1 DMA REQ (DMA transfer request) */
            [26] = BSP_PRV_VECT_ENUM(EVENT_AGT1_INT,GROUP2), /* AGT1 INT (AGT interrupt) */
            [27] = BSP_PRV_VECT_ENUM(EVENT_USBFS_INT,GROUP3), /* USBFS INT (USBFS interrupt) */
            [28] = BSP_PRV_VECT_ENUM(EVENT_USBFS_RESUME,GROUP4), /* USBFS RESUME (USBFS resume interrupt) */
            [29] = BSP_PRV_VECT_ENUM(EVENT_USBFS_FIFO_0,GROUP5), /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
            [30] = BSP_PRV_VECT_ENUM(EVENT_USBFS_FIFO_1,GROUP6), /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
            [31] = BSP_PRV_VECT_ENUM(EVENT_USBHS_USB_INT_RESUME,GROUP7), /* USBHS USB INT RESUME (USBHS interrupt) */
            [32] = BSP_PRV_VECT_ENUM(EVENT_USBHS_FIFO_0,FIXED), /* USBHS FIFO 0 (DMA transfer request 0) */
            [33] = BSP_PRV_VECT_ENUM(EVENT_USBHS_FIFO_1,FIXED), /* USBHS FIFO 1 (DMA transfer request 1) */
        };
        #endif
        #endif