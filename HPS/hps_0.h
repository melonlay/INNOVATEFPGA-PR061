#ifndef _ALTERA_HPS_0_H_
#define _ALTERA_HPS_0_H_

/*
 * This file was automatically generated by the swinfo2header utility.
 * 
 * Created from SOPC Builder system 'soc_system' in
 * file './soc_system.sopcinfo'.
 */

/*
 * This file contains macros for module 'hps_0' and devices
 * connected to the following masters:
 *   h2f_axi_master
 *   h2f_lw_axi_master
 * 
 * Do not include this header file and another header file created for a
 * different module or master group at the same time.
 * Doing so may result in duplicate macro names.
 * Instead, use the system header file which has macros with unique names.
 */

/*
 * Macros for device 'Data_type', class 'altera_avalon_pio'
 * The macros are prefixed with 'DATA_TYPE_'.
 * The prefix is the slave descriptor.
 */
#define DATA_TYPE_COMPONENT_TYPE altera_avalon_pio
#define DATA_TYPE_COMPONENT_NAME Data_type
#define DATA_TYPE_BASE 0x0
#define DATA_TYPE_SPAN 16
#define DATA_TYPE_END 0xf
#define DATA_TYPE_BIT_CLEARING_EDGE_REGISTER 0
#define DATA_TYPE_BIT_MODIFYING_OUTPUT_REGISTER 0
#define DATA_TYPE_CAPTURE 0
#define DATA_TYPE_DATA_WIDTH 32
#define DATA_TYPE_DO_TEST_BENCH_WIRING 0
#define DATA_TYPE_DRIVEN_SIM_VALUE 0
#define DATA_TYPE_EDGE_TYPE NONE
#define DATA_TYPE_FREQ 50000000
#define DATA_TYPE_HAS_IN 0
#define DATA_TYPE_HAS_OUT 1
#define DATA_TYPE_HAS_TRI 0
#define DATA_TYPE_IRQ_TYPE NONE
#define DATA_TYPE_RESET_VALUE 0

/*
 * Macros for device 'Input_addr', class 'altera_avalon_pio'
 * The macros are prefixed with 'INPUT_ADDR_'.
 * The prefix is the slave descriptor.
 */
#define INPUT_ADDR_COMPONENT_TYPE altera_avalon_pio
#define INPUT_ADDR_COMPONENT_NAME Input_addr
#define INPUT_ADDR_BASE 0x10
#define INPUT_ADDR_SPAN 16
#define INPUT_ADDR_END 0x1f
#define INPUT_ADDR_BIT_CLEARING_EDGE_REGISTER 0
#define INPUT_ADDR_BIT_MODIFYING_OUTPUT_REGISTER 0
#define INPUT_ADDR_CAPTURE 0
#define INPUT_ADDR_DATA_WIDTH 32
#define INPUT_ADDR_DO_TEST_BENCH_WIRING 0
#define INPUT_ADDR_DRIVEN_SIM_VALUE 0
#define INPUT_ADDR_EDGE_TYPE NONE
#define INPUT_ADDR_FREQ 50000000
#define INPUT_ADDR_HAS_IN 0
#define INPUT_ADDR_HAS_OUT 1
#define INPUT_ADDR_HAS_TRI 0
#define INPUT_ADDR_IRQ_TYPE NONE
#define INPUT_ADDR_RESET_VALUE 0

/*
 * Macros for device 'Weight_addr', class 'altera_avalon_pio'
 * The macros are prefixed with 'WEIGHT_ADDR_'.
 * The prefix is the slave descriptor.
 */
#define WEIGHT_ADDR_COMPONENT_TYPE altera_avalon_pio
#define WEIGHT_ADDR_COMPONENT_NAME Weight_addr
#define WEIGHT_ADDR_BASE 0x20
#define WEIGHT_ADDR_SPAN 16
#define WEIGHT_ADDR_END 0x2f
#define WEIGHT_ADDR_BIT_CLEARING_EDGE_REGISTER 0
#define WEIGHT_ADDR_BIT_MODIFYING_OUTPUT_REGISTER 0
#define WEIGHT_ADDR_CAPTURE 0
#define WEIGHT_ADDR_DATA_WIDTH 32
#define WEIGHT_ADDR_DO_TEST_BENCH_WIRING 0
#define WEIGHT_ADDR_DRIVEN_SIM_VALUE 0
#define WEIGHT_ADDR_EDGE_TYPE NONE
#define WEIGHT_ADDR_FREQ 50000000
#define WEIGHT_ADDR_HAS_IN 0
#define WEIGHT_ADDR_HAS_OUT 1
#define WEIGHT_ADDR_HAS_TRI 0
#define WEIGHT_ADDR_IRQ_TYPE NONE
#define WEIGHT_ADDR_RESET_VALUE 0

/*
 * Macros for device 'Output_addr', class 'altera_avalon_pio'
 * The macros are prefixed with 'OUTPUT_ADDR_'.
 * The prefix is the slave descriptor.
 */
#define OUTPUT_ADDR_COMPONENT_TYPE altera_avalon_pio
#define OUTPUT_ADDR_COMPONENT_NAME Output_addr
#define OUTPUT_ADDR_BASE 0x30
#define OUTPUT_ADDR_SPAN 16
#define OUTPUT_ADDR_END 0x3f
#define OUTPUT_ADDR_BIT_CLEARING_EDGE_REGISTER 0
#define OUTPUT_ADDR_BIT_MODIFYING_OUTPUT_REGISTER 0
#define OUTPUT_ADDR_CAPTURE 0
#define OUTPUT_ADDR_DATA_WIDTH 32
#define OUTPUT_ADDR_DO_TEST_BENCH_WIRING 0
#define OUTPUT_ADDR_DRIVEN_SIM_VALUE 0
#define OUTPUT_ADDR_EDGE_TYPE NONE
#define OUTPUT_ADDR_FREQ 50000000
#define OUTPUT_ADDR_HAS_IN 0
#define OUTPUT_ADDR_HAS_OUT 1
#define OUTPUT_ADDR_HAS_TRI 0
#define OUTPUT_ADDR_IRQ_TYPE NONE
#define OUTPUT_ADDR_RESET_VALUE 0

/*
 * Macros for device 'Finish_signal', class 'altera_avalon_pio'
 * The macros are prefixed with 'FINISH_SIGNAL_'.
 * The prefix is the slave descriptor.
 */
#define FINISH_SIGNAL_COMPONENT_TYPE altera_avalon_pio
#define FINISH_SIGNAL_COMPONENT_NAME Finish_signal
#define FINISH_SIGNAL_BASE 0x40
#define FINISH_SIGNAL_SPAN 16
#define FINISH_SIGNAL_END 0x4f
#define FINISH_SIGNAL_BIT_CLEARING_EDGE_REGISTER 0
#define FINISH_SIGNAL_BIT_MODIFYING_OUTPUT_REGISTER 0
#define FINISH_SIGNAL_CAPTURE 0
#define FINISH_SIGNAL_DATA_WIDTH 1
#define FINISH_SIGNAL_DO_TEST_BENCH_WIRING 0
#define FINISH_SIGNAL_DRIVEN_SIM_VALUE 0
#define FINISH_SIGNAL_EDGE_TYPE NONE
#define FINISH_SIGNAL_FREQ 50000000
#define FINISH_SIGNAL_HAS_IN 1
#define FINISH_SIGNAL_HAS_OUT 0
#define FINISH_SIGNAL_HAS_TRI 0
#define FINISH_SIGNAL_IRQ_TYPE NONE
#define FINISH_SIGNAL_RESET_VALUE 0

/*
 * Macros for device 'sysid_qsys', class 'altera_avalon_sysid_qsys'
 * The macros are prefixed with 'SYSID_QSYS_'.
 * The prefix is the slave descriptor.
 */
#define SYSID_QSYS_COMPONENT_TYPE altera_avalon_sysid_qsys
#define SYSID_QSYS_COMPONENT_NAME sysid_qsys
#define SYSID_QSYS_BASE 0x1000
#define SYSID_QSYS_SPAN 8
#define SYSID_QSYS_END 0x1007
#define SYSID_QSYS_ID 2899645186
#define SYSID_QSYS_TIMESTAMP 1569478709

/*
 * Macros for device 'jtag_uart', class 'altera_avalon_jtag_uart'
 * The macros are prefixed with 'JTAG_UART_'.
 * The prefix is the slave descriptor.
 */
#define JTAG_UART_COMPONENT_TYPE altera_avalon_jtag_uart
#define JTAG_UART_COMPONENT_NAME jtag_uart
#define JTAG_UART_BASE 0x2000
#define JTAG_UART_SPAN 8
#define JTAG_UART_END 0x2007
#define JTAG_UART_IRQ 2
#define JTAG_UART_READ_DEPTH 64
#define JTAG_UART_READ_THRESHOLD 8
#define JTAG_UART_WRITE_DEPTH 64
#define JTAG_UART_WRITE_THRESHOLD 8

/*
 * Macros for device 'led_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'LED_PIO_'.
 * The prefix is the slave descriptor.
 */
#define LED_PIO_COMPONENT_TYPE altera_avalon_pio
#define LED_PIO_COMPONENT_NAME led_pio
#define LED_PIO_BASE 0x3000
#define LED_PIO_SPAN 16
#define LED_PIO_END 0x300f
#define LED_PIO_BIT_CLEARING_EDGE_REGISTER 0
#define LED_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LED_PIO_CAPTURE 0
#define LED_PIO_DATA_WIDTH 7
#define LED_PIO_DO_TEST_BENCH_WIRING 0
#define LED_PIO_DRIVEN_SIM_VALUE 0
#define LED_PIO_EDGE_TYPE NONE
#define LED_PIO_FREQ 50000000
#define LED_PIO_HAS_IN 0
#define LED_PIO_HAS_OUT 1
#define LED_PIO_HAS_TRI 0
#define LED_PIO_IRQ_TYPE NONE
#define LED_PIO_RESET_VALUE 127

/*
 * Macros for device 'dipsw_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'DIPSW_PIO_'.
 * The prefix is the slave descriptor.
 */
#define DIPSW_PIO_COMPONENT_TYPE altera_avalon_pio
#define DIPSW_PIO_COMPONENT_NAME dipsw_pio
#define DIPSW_PIO_BASE 0x4000
#define DIPSW_PIO_SPAN 16
#define DIPSW_PIO_END 0x400f
#define DIPSW_PIO_IRQ 0
#define DIPSW_PIO_BIT_CLEARING_EDGE_REGISTER 1
#define DIPSW_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define DIPSW_PIO_CAPTURE 1
#define DIPSW_PIO_DATA_WIDTH 4
#define DIPSW_PIO_DO_TEST_BENCH_WIRING 0
#define DIPSW_PIO_DRIVEN_SIM_VALUE 0
#define DIPSW_PIO_EDGE_TYPE ANY
#define DIPSW_PIO_FREQ 50000000
#define DIPSW_PIO_HAS_IN 1
#define DIPSW_PIO_HAS_OUT 0
#define DIPSW_PIO_HAS_TRI 0
#define DIPSW_PIO_IRQ_TYPE EDGE
#define DIPSW_PIO_RESET_VALUE 0

/*
 * Macros for device 'button_pio', class 'altera_avalon_pio'
 * The macros are prefixed with 'BUTTON_PIO_'.
 * The prefix is the slave descriptor.
 */
#define BUTTON_PIO_COMPONENT_TYPE altera_avalon_pio
#define BUTTON_PIO_COMPONENT_NAME button_pio
#define BUTTON_PIO_BASE 0x5000
#define BUTTON_PIO_SPAN 16
#define BUTTON_PIO_END 0x500f
#define BUTTON_PIO_IRQ 1
#define BUTTON_PIO_BIT_CLEARING_EDGE_REGISTER 1
#define BUTTON_PIO_BIT_MODIFYING_OUTPUT_REGISTER 0
#define BUTTON_PIO_CAPTURE 1
#define BUTTON_PIO_DATA_WIDTH 2
#define BUTTON_PIO_DO_TEST_BENCH_WIRING 0
#define BUTTON_PIO_DRIVEN_SIM_VALUE 0
#define BUTTON_PIO_EDGE_TYPE FALLING
#define BUTTON_PIO_FREQ 50000000
#define BUTTON_PIO_HAS_IN 1
#define BUTTON_PIO_HAS_OUT 0
#define BUTTON_PIO_HAS_TRI 0
#define BUTTON_PIO_IRQ_TYPE EDGE
#define BUTTON_PIO_RESET_VALUE 0

/*
 * Macros for device 'ILC', class 'interrupt_latency_counter'
 * The macros are prefixed with 'ILC_'.
 * The prefix is the slave descriptor.
 */
#define ILC_COMPONENT_TYPE interrupt_latency_counter
#define ILC_COMPONENT_NAME ILC
#define ILC_BASE 0x30000
#define ILC_SPAN 256
#define ILC_END 0x300ff

/*
 * Macros for device 'alt_vip_vfr_hdmi', class 'alt_vip_vfr'
 * The macros are prefixed with 'ALT_VIP_VFR_HDMI_'.
 * The prefix is the slave descriptor.
 */
#define ALT_VIP_VFR_HDMI_COMPONENT_TYPE alt_vip_vfr
#define ALT_VIP_VFR_HDMI_COMPONENT_NAME alt_vip_vfr_hdmi
#define ALT_VIP_VFR_HDMI_BASE 0x31000
#define ALT_VIP_VFR_HDMI_SPAN 128
#define ALT_VIP_VFR_HDMI_END 0x3107f


#endif /* _ALTERA_HPS_0_H_ */
