/* generated vector header file - do not edit */
        #ifndef VECTOR_DATA_H
        #define VECTOR_DATA_H
        #ifdef __cplusplus
        extern "C" {
        #endif
                /* Number of interrupts allocated */
        #ifndef VECTOR_DATA_IRQ_COUNT
        #define VECTOR_DATA_IRQ_COUNT    (34)
        #endif
        /* ISR prototypes */
        void sci_b_uart_rxi_isr(void);
        void sci_b_uart_txi_isr(void);
        void sci_b_uart_tei_isr(void);
        void sci_b_uart_eri_isr(void);
        void spi_b_rxi_isr(void);
        void spi_b_txi_isr(void);
        void spi_b_tei_isr(void);
        void spi_b_eri_isr(void);
        void adc_scan_end_isr(void);
        void ceu_isr(void);
        void sci_b_spi_rxi_isr(void);
        void sci_b_spi_txi_isr(void);
        void sci_b_spi_tei_isr(void);
        void sci_b_spi_eri_isr(void);
        void agt_int_isr(void);
        void sdhimmc_accs_isr(void);
        void sdhimmc_card_isr(void);
        void sdhimmc_dma_req_isr(void);
        void usbfs_interrupt_handler(void);
        void usbfs_resume_handler(void);
        void usbfs_d0fifo_handler(void);
        void usbfs_d1fifo_handler(void);
        void usbhs_interrupt_handler(void);
        void usbhs_d0fifo_handler(void);
        void usbhs_d1fifo_handler(void);

        /* Vector table allocations */
        #define VECTOR_NUMBER_SCI9_RXI ((IRQn_Type) 0) /* SCI9 RXI (Receive data full) */
        #define SCI9_RXI_IRQn          ((IRQn_Type) 0) /* SCI9 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI9_TXI ((IRQn_Type) 1) /* SCI9 TXI (Transmit data empty) */
        #define SCI9_TXI_IRQn          ((IRQn_Type) 1) /* SCI9 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI9_TEI ((IRQn_Type) 2) /* SCI9 TEI (Transmit end) */
        #define SCI9_TEI_IRQn          ((IRQn_Type) 2) /* SCI9 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI9_ERI ((IRQn_Type) 3) /* SCI9 ERI (Receive error) */
        #define SCI9_ERI_IRQn          ((IRQn_Type) 3) /* SCI9 ERI (Receive error) */
        #define VECTOR_NUMBER_SPI1_RXI ((IRQn_Type) 4) /* SPI1 RXI (Receive buffer full) */
        #define SPI1_RXI_IRQn          ((IRQn_Type) 4) /* SPI1 RXI (Receive buffer full) */
        #define VECTOR_NUMBER_SPI1_TXI ((IRQn_Type) 5) /* SPI1 TXI (Transmit buffer empty) */
        #define SPI1_TXI_IRQn          ((IRQn_Type) 5) /* SPI1 TXI (Transmit buffer empty) */
        #define VECTOR_NUMBER_SPI1_TEI ((IRQn_Type) 6) /* SPI1 TEI (Transmission complete event) */
        #define SPI1_TEI_IRQn          ((IRQn_Type) 6) /* SPI1 TEI (Transmission complete event) */
        #define VECTOR_NUMBER_SPI1_ERI ((IRQn_Type) 7) /* SPI1 ERI (Error) */
        #define SPI1_ERI_IRQn          ((IRQn_Type) 7) /* SPI1 ERI (Error) */
        #define VECTOR_NUMBER_ADC0_SCAN_END ((IRQn_Type) 8) /* ADC0 SCAN END (End of A/D scanning operation) */
        #define ADC0_SCAN_END_IRQn          ((IRQn_Type) 8) /* ADC0 SCAN END (End of A/D scanning operation) */
        #define VECTOR_NUMBER_SCI4_RXI ((IRQn_Type) 9) /* SCI4 RXI (Receive data full) */
        #define SCI4_RXI_IRQn          ((IRQn_Type) 9) /* SCI4 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI4_TXI ((IRQn_Type) 10) /* SCI4 TXI (Transmit data empty) */
        #define SCI4_TXI_IRQn          ((IRQn_Type) 10) /* SCI4 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI4_TEI ((IRQn_Type) 11) /* SCI4 TEI (Transmit end) */
        #define SCI4_TEI_IRQn          ((IRQn_Type) 11) /* SCI4 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI4_ERI ((IRQn_Type) 12) /* SCI4 ERI (Receive error) */
        #define SCI4_ERI_IRQn          ((IRQn_Type) 12) /* SCI4 ERI (Receive error) */
        #define VECTOR_NUMBER_CEU_CEUI ((IRQn_Type) 13) /* CEU CEUI (CEU interrupt) */
        #define CEU_CEUI_IRQn          ((IRQn_Type) 13) /* CEU CEUI (CEU interrupt) */
        #define VECTOR_NUMBER_SCI2_RXI ((IRQn_Type) 14) /* SCI2 RXI (Receive data full) */
        #define SCI2_RXI_IRQn          ((IRQn_Type) 14) /* SCI2 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI2_TXI ((IRQn_Type) 15) /* SCI2 TXI (Transmit data empty) */
        #define SCI2_TXI_IRQn          ((IRQn_Type) 15) /* SCI2 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI2_TEI ((IRQn_Type) 16) /* SCI2 TEI (Transmit end) */
        #define SCI2_TEI_IRQn          ((IRQn_Type) 16) /* SCI2 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI2_ERI ((IRQn_Type) 17) /* SCI2 ERI (Receive error) */
        #define SCI2_ERI_IRQn          ((IRQn_Type) 17) /* SCI2 ERI (Receive error) */
        #define VECTOR_NUMBER_AGT0_INT ((IRQn_Type) 18) /* AGT0 INT (AGT interrupt) */
        #define AGT0_INT_IRQn          ((IRQn_Type) 18) /* AGT0 INT (AGT interrupt) */
        #define VECTOR_NUMBER_SCI3_RXI ((IRQn_Type) 19) /* SCI3 RXI (Receive data full) */
        #define SCI3_RXI_IRQn          ((IRQn_Type) 19) /* SCI3 RXI (Receive data full) */
        #define VECTOR_NUMBER_SCI3_TXI ((IRQn_Type) 20) /* SCI3 TXI (Transmit data empty) */
        #define SCI3_TXI_IRQn          ((IRQn_Type) 20) /* SCI3 TXI (Transmit data empty) */
        #define VECTOR_NUMBER_SCI3_TEI ((IRQn_Type) 21) /* SCI3 TEI (Transmit end) */
        #define SCI3_TEI_IRQn          ((IRQn_Type) 21) /* SCI3 TEI (Transmit end) */
        #define VECTOR_NUMBER_SCI3_ERI ((IRQn_Type) 22) /* SCI3 ERI (Receive error) */
        #define SCI3_ERI_IRQn          ((IRQn_Type) 22) /* SCI3 ERI (Receive error) */
        #define VECTOR_NUMBER_SDHIMMC1_ACCS ((IRQn_Type) 23) /* SDHIMMC1 ACCS (Card access) */
        #define SDHIMMC1_ACCS_IRQn          ((IRQn_Type) 23) /* SDHIMMC1 ACCS (Card access) */
        #define VECTOR_NUMBER_SDHIMMC1_CARD ((IRQn_Type) 24) /* SDHIMMC1 CARD (Card detect) */
        #define SDHIMMC1_CARD_IRQn          ((IRQn_Type) 24) /* SDHIMMC1 CARD (Card detect) */
        #define VECTOR_NUMBER_SDHIMMC1_DMA_REQ ((IRQn_Type) 25) /* SDHIMMC1 DMA REQ (DMA transfer request) */
        #define SDHIMMC1_DMA_REQ_IRQn          ((IRQn_Type) 25) /* SDHIMMC1 DMA REQ (DMA transfer request) */
        #define VECTOR_NUMBER_AGT1_INT ((IRQn_Type) 26) /* AGT1 INT (AGT interrupt) */
        #define AGT1_INT_IRQn          ((IRQn_Type) 26) /* AGT1 INT (AGT interrupt) */
        #define VECTOR_NUMBER_USBFS_INT ((IRQn_Type) 27) /* USBFS INT (USBFS interrupt) */
        #define USBFS_INT_IRQn          ((IRQn_Type) 27) /* USBFS INT (USBFS interrupt) */
        #define VECTOR_NUMBER_USBFS_RESUME ((IRQn_Type) 28) /* USBFS RESUME (USBFS resume interrupt) */
        #define USBFS_RESUME_IRQn          ((IRQn_Type) 28) /* USBFS RESUME (USBFS resume interrupt) */
        #define VECTOR_NUMBER_USBFS_FIFO_0 ((IRQn_Type) 29) /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
        #define USBFS_FIFO_0_IRQn          ((IRQn_Type) 29) /* USBFS FIFO 0 (DMA/DTC transfer request 0) */
        #define VECTOR_NUMBER_USBFS_FIFO_1 ((IRQn_Type) 30) /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
        #define USBFS_FIFO_1_IRQn          ((IRQn_Type) 30) /* USBFS FIFO 1 (DMA/DTC transfer request 1) */
        #define VECTOR_NUMBER_USBHS_USB_INT_RESUME ((IRQn_Type) 31) /* USBHS USB INT RESUME (USBHS interrupt) */
        #define USBHS_USB_INT_RESUME_IRQn          ((IRQn_Type) 31) /* USBHS USB INT RESUME (USBHS interrupt) */
        #define VECTOR_NUMBER_USBHS_FIFO_0 ((IRQn_Type) 32) /* USBHS FIFO 0 (DMA transfer request 0) */
        #define USBHS_FIFO_0_IRQn          ((IRQn_Type) 32) /* USBHS FIFO 0 (DMA transfer request 0) */
        #define VECTOR_NUMBER_USBHS_FIFO_1 ((IRQn_Type) 33) /* USBHS FIFO 1 (DMA transfer request 1) */
        #define USBHS_FIFO_1_IRQn          ((IRQn_Type) 33) /* USBHS FIFO 1 (DMA transfer request 1) */
        #ifdef __cplusplus
        }
        #endif
        #endif /* VECTOR_DATA_H */