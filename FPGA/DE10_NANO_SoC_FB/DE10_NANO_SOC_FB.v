`default_nettype none
//=======================================================
//  This code is generated by Terasic System Builder
//=======================================================

module DE10_NANO_SOC_FB(

	//////////// CLOCK //////////
	input 		          		FPGA_CLK1_50,
	input 		          		FPGA_CLK2_50,
	input 		          		FPGA_CLK3_50,

	//////////// HDMI //////////
	inout 		          		HDMI_I2C_SCL,
	inout 		          		HDMI_I2C_SDA,
	inout 		          		HDMI_I2S,
	inout 		          		HDMI_LRCLK,
	inout 		          		HDMI_MCLK,
	inout 		          		HDMI_SCLK,
	output		          		HDMI_TX_CLK,
	output		    [23:0]		HDMI_TX_D,
	output		          		HDMI_TX_DE,
	output		          		HDMI_TX_HS,
	input 		          		HDMI_TX_INT,
	output		          		HDMI_TX_VS,

	//////////// HPS //////////
	inout 		          		HPS_CONV_USB_N,
	output		    [14:0]		HPS_DDR3_ADDR,
	output		     [2:0]		HPS_DDR3_BA,
	output		          		HPS_DDR3_CAS_N,
	output		          		HPS_DDR3_CK_N,
	output		          		HPS_DDR3_CK_P,
	output		          		HPS_DDR3_CKE,
	output		          		HPS_DDR3_CS_N,
	output		     [3:0]		HPS_DDR3_DM,
	inout 		    [31:0]		HPS_DDR3_DQ,
	inout 		     [3:0]		HPS_DDR3_DQS_N,
	inout 		     [3:0]		HPS_DDR3_DQS_P,
	output		          		HPS_DDR3_ODT,
	output		          		HPS_DDR3_RAS_N,
	output		          		HPS_DDR3_RESET_N,
	input 		          		HPS_DDR3_RZQ,
	output		          		HPS_DDR3_WE_N,
	output		          		HPS_ENET_GTX_CLK,
	inout 		          		HPS_ENET_INT_N,
	output		          		HPS_ENET_MDC,
	inout 		          		HPS_ENET_MDIO,
	input 		          		HPS_ENET_RX_CLK,
	input 		     [3:0]		HPS_ENET_RX_DATA,
	input 		          		HPS_ENET_RX_DV,
	output		     [3:0]		HPS_ENET_TX_DATA,
	output		          		HPS_ENET_TX_EN,
	inout 		          		HPS_GSENSOR_INT,
	inout 		          		HPS_I2C0_SCLK,
	inout 		          		HPS_I2C0_SDAT,
	inout 		          		HPS_I2C1_SCLK,
	inout 		          		HPS_I2C1_SDAT,
	inout 		          		HPS_KEY,
	inout 		          		HPS_LED,
	inout 		          		HPS_LTC_GPIO,
	output		          		HPS_SD_CLK,
	inout 		          		HPS_SD_CMD,
	inout 		     [3:0]		HPS_SD_DATA,
	output		          		HPS_SPIM_CLK,
	input 		          		HPS_SPIM_MISO,
	output		          		HPS_SPIM_MOSI,
	inout 		          		HPS_SPIM_SS,
	input 		          		HPS_UART_RX,
	output		          		HPS_UART_TX,
	input 		          		HPS_USB_CLKOUT,
	inout 		     [7:0]		HPS_USB_DATA,
	input 		          		HPS_USB_DIR,
	input 		          		HPS_USB_NXT,
	output		          		HPS_USB_STP,

	//////////// KEY //////////
	input 		     [1:0]		KEY,

	//////////// LED //////////
	output		     [7:0]		LED,

	//////////// SW //////////
	input 		     [3:0]		SW,
	
	output	reg	  [1:0]     oNoFF123
	
	
);



//=======================================================
//  REG/WIRE declarations
//=======================================================
 wire  hps_fpga_reset_n;
  wire [1:0] fpga_debounced_buttons;
  wire [6:0]  fpga_led_internal;
  wire [2:0]  hps_reset_req;
  wire        hps_cold_reset;
  wire        hps_warm_reset;
  wire        hps_debug_reset;
  wire [27:0] stm_hw_events;
  wire 		  fpga_clk_50;
  wire               clk_65;
  wire               clk_130;
// connection of internal logics
  assign fpga_clk_50=FPGA_CLK1_50;
  assign stm_hw_events    = {{15{1'b0}}, SW, fpga_led_internal, fpga_debounced_buttons};


  
  //----communication with HPS --------------- (AXI) 
wire  [31:0]  Data_type; 
wire  [31:0]  Input_addr;
wire  [31:0]  Weight_addr;
wire  [31:0]  Output_addr;
reg   [31:0]  Finish_signal;


/////////////////// PING RAM ///////////////////
wire          	PING_RAM_ready;                  //          	 .avl.waitrequest
reg 	[26:0]   PING_RAM_addr;                   //             .address
wire          	PING_RAM_rdata_valid;            //             .readdatavalid
wire 	[31:0]   PING_RAM_rdata;                   //             .readdata
reg 	[31:0]   PING_RAM_wdata;                   //             .writedata
reg          	PING_RAM_read_req;               //             .read
reg          	PING_RAM_write_req;              //             .write
  
  
  

//=======================================================
//  Structural coding
//=======================================================
I2C_HDMI_Config u_I2C_HDMI_Config (
	.iCLK(FPGA_CLK1_50),
	.iRST_N( 1'b1),
	.I2C_SCLK(HDMI_I2C_SCL),
	.I2C_SDAT(HDMI_I2C_SDA),
	.HDMI_TX_INT(HDMI_TX_INT)
	);

vga_pll  vga_pll_inst(
			.refclk(fpga_clk_50),   //  refclk.clk
		   .rst(1'b0),      //   reset.reset
		   .outclk_0(clk_65), // outclk0.clk
		   .outclk_1(clk_130), // outclk1.clk
		   .locked()    //  locked.export
);

assign HDMI_TX_CLK = clk_65;

soc_system u0 (
		//Clock&Reset
	  .clk_clk                                  (FPGA_CLK1_50 ),                               //                            clk.clk
	  .reset_reset_n                            (hps_fpga_reset_n ),                         //                          reset.reset_n
	  .clk_130_clk                               (clk_130 ),                               //                            clk.clk
	  .alt_vip_itc_0_clocked_video_vid_clk       (HDMI_TX_CLK ),       // alt_vip_itc_0_clocked_video.vid_clk
	  .alt_vip_itc_0_clocked_video_vid_data      (HDMI_TX_D ),      //                            .vid_data
	  .alt_vip_itc_0_clocked_video_underflow     ( ),     //                            .underflow
	  .alt_vip_itc_0_clocked_video_vid_datavalid (HDMI_TX_DE), //                            .vid_datavalid
	  .alt_vip_itc_0_clocked_video_vid_v_sync    (HDMI_TX_VS ),    //                            .vid_v_sync
	  .alt_vip_itc_0_clocked_video_vid_h_sync    (HDMI_TX_HS ),    //                            .vid_h_sync
	  .alt_vip_itc_0_clocked_video_vid_f         ( ),         //                            .vid_f
	  .alt_vip_itc_0_clocked_video_vid_h         ( ),         //                            .vid_h
	  .alt_vip_itc_0_clocked_video_vid_v         ( ),          //                            .vid_v

	  //HPS ddr3
	  .memory_mem_a                          ( HPS_DDR3_ADDR),                       //                memory.mem_a
	  .memory_mem_ba                         ( HPS_DDR3_BA),                         //                .mem_ba
	  .memory_mem_ck                         ( HPS_DDR3_CK_P),                       //                .mem_ck
	  .memory_mem_ck_n                       ( HPS_DDR3_CK_N),                       //                .mem_ck_n
	  .memory_mem_cke                        ( HPS_DDR3_CKE),                        //                .mem_cke
	  .memory_mem_cs_n                       ( HPS_DDR3_CS_N),                       //                .mem_cs_n
	  .memory_mem_ras_n                      ( HPS_DDR3_RAS_N),                      //                .mem_ras_n
	  .memory_mem_cas_n                      ( HPS_DDR3_CAS_N),                      //                .mem_cas_n
	  .memory_mem_we_n                       ( HPS_DDR3_WE_N),                       //                .mem_we_n
	  .memory_mem_reset_n                    ( HPS_DDR3_RESET_N),                    //                .mem_reset_n
	  .memory_mem_dq                         ( HPS_DDR3_DQ),                         //                .mem_dq
	  .memory_mem_dqs                        ( HPS_DDR3_DQS_P),                      //                .mem_dqs
	  .memory_mem_dqs_n                      ( HPS_DDR3_DQS_N),                      //                .mem_dqs_n
	  .memory_mem_odt                        ( HPS_DDR3_ODT),                        //                .mem_odt
	  .memory_mem_dm                         ( HPS_DDR3_DM),                         //                .mem_dm
	  .memory_oct_rzqin                      ( HPS_DDR3_RZQ),                        //                .oct_rzqin
	  //HPS ethernet
	  .hps_0_hps_io_hps_io_emac1_inst_TX_CLK ( HPS_ENET_GTX_CLK),       //                             hps_0_hps_io.hps_io_emac1_inst_TX_CLK
	  .hps_0_hps_io_hps_io_emac1_inst_TXD0   ( HPS_ENET_TX_DATA[0] ),   //                             .hps_io_emac1_inst_TXD0
	  .hps_0_hps_io_hps_io_emac1_inst_TXD1   ( HPS_ENET_TX_DATA[1] ),   //                             .hps_io_emac1_inst_TXD1
	  .hps_0_hps_io_hps_io_emac1_inst_TXD2   ( HPS_ENET_TX_DATA[2] ),   //                             .hps_io_emac1_inst_TXD2
	  .hps_0_hps_io_hps_io_emac1_inst_TXD3   ( HPS_ENET_TX_DATA[3] ),   //                             .hps_io_emac1_inst_TXD3
	  .hps_0_hps_io_hps_io_emac1_inst_RXD0   ( HPS_ENET_RX_DATA[0] ),   //                             .hps_io_emac1_inst_RXD0
	  .hps_0_hps_io_hps_io_emac1_inst_MDIO   ( HPS_ENET_MDIO ),         //                             .hps_io_emac1_inst_MDIO
	  .hps_0_hps_io_hps_io_emac1_inst_MDC    ( HPS_ENET_MDC  ),         //                             .hps_io_emac1_inst_MDC
	  .hps_0_hps_io_hps_io_emac1_inst_RX_CTL ( HPS_ENET_RX_DV),         //                             .hps_io_emac1_inst_RX_CTL
	  .hps_0_hps_io_hps_io_emac1_inst_TX_CTL ( HPS_ENET_TX_EN),         //                             .hps_io_emac1_inst_TX_CTL
	  .hps_0_hps_io_hps_io_emac1_inst_RX_CLK ( HPS_ENET_RX_CLK),        //                             .hps_io_emac1_inst_RX_CLK
	  .hps_0_hps_io_hps_io_emac1_inst_RXD1   ( HPS_ENET_RX_DATA[1] ),   //                             .hps_io_emac1_inst_RXD1
	  .hps_0_hps_io_hps_io_emac1_inst_RXD2   ( HPS_ENET_RX_DATA[2] ),   //                             .hps_io_emac1_inst_RXD2
	  .hps_0_hps_io_hps_io_emac1_inst_RXD3   ( HPS_ENET_RX_DATA[3] ),   //                             .hps_io_emac1_inst_RXD3
	  //HPS SD card
	  .hps_0_hps_io_hps_io_sdio_inst_CMD     ( HPS_SD_CMD    ),           //                               .hps_io_sdio_inst_CMD
	  .hps_0_hps_io_hps_io_sdio_inst_D0      ( HPS_SD_DATA[0]     ),      //                               .hps_io_sdio_inst_D0
	  .hps_0_hps_io_hps_io_sdio_inst_D1      ( HPS_SD_DATA[1]     ),      //                               .hps_io_sdio_inst_D1
	  .hps_0_hps_io_hps_io_sdio_inst_CLK     ( HPS_SD_CLK   ),            //                               .hps_io_sdio_inst_CLK
	  .hps_0_hps_io_hps_io_sdio_inst_D2      ( HPS_SD_DATA[2]     ),      //                               .hps_io_sdio_inst_D2
	  .hps_0_hps_io_hps_io_sdio_inst_D3      ( HPS_SD_DATA[3]     ),      //                               .hps_io_sdio_inst_D3
	  //HPS USB
	  .hps_0_hps_io_hps_io_usb1_inst_D0      ( HPS_USB_DATA[0]    ),      //                               .hps_io_usb1_inst_D0
	  .hps_0_hps_io_hps_io_usb1_inst_D1      ( HPS_USB_DATA[1]    ),      //                               .hps_io_usb1_inst_D1
	  .hps_0_hps_io_hps_io_usb1_inst_D2      ( HPS_USB_DATA[2]    ),      //                               .hps_io_usb1_inst_D2
	  .hps_0_hps_io_hps_io_usb1_inst_D3      ( HPS_USB_DATA[3]    ),      //                               .hps_io_usb1_inst_D3
	  .hps_0_hps_io_hps_io_usb1_inst_D4      ( HPS_USB_DATA[4]    ),      //                               .hps_io_usb1_inst_D4
	  .hps_0_hps_io_hps_io_usb1_inst_D5      ( HPS_USB_DATA[5]    ),      //                               .hps_io_usb1_inst_D5
	  .hps_0_hps_io_hps_io_usb1_inst_D6      ( HPS_USB_DATA[6]    ),      //                               .hps_io_usb1_inst_D6
	  .hps_0_hps_io_hps_io_usb1_inst_D7      ( HPS_USB_DATA[7]    ),      //                               .hps_io_usb1_inst_D7
	  .hps_0_hps_io_hps_io_usb1_inst_CLK     ( HPS_USB_CLKOUT    ),       //                               .hps_io_usb1_inst_CLK
	  .hps_0_hps_io_hps_io_usb1_inst_STP     ( HPS_USB_STP    ),          //                               .hps_io_usb1_inst_STP
	  .hps_0_hps_io_hps_io_usb1_inst_DIR     ( HPS_USB_DIR    ),          //                               .hps_io_usb1_inst_DIR
	  .hps_0_hps_io_hps_io_usb1_inst_NXT     ( HPS_USB_NXT    ),          //                               .hps_io_usb1_inst_NXT
		//HPS SPI
	  .hps_0_hps_io_hps_io_spim1_inst_CLK    ( HPS_SPIM_CLK  ),           //                               .hps_io_spim1_inst_CLK
	  .hps_0_hps_io_hps_io_spim1_inst_MOSI   ( HPS_SPIM_MOSI ),           //                               .hps_io_spim1_inst_MOSI
	  .hps_0_hps_io_hps_io_spim1_inst_MISO   ( HPS_SPIM_MISO ),           //                               .hps_io_spim1_inst_MISO
	  .hps_0_hps_io_hps_io_spim1_inst_SS0    ( HPS_SPIM_SS   ),             //                               .hps_io_spim1_inst_SS0
		//HPS UART
	  .hps_0_hps_io_hps_io_uart0_inst_RX     ( HPS_UART_RX   ),          //                               .hps_io_uart0_inst_RX
	  .hps_0_hps_io_hps_io_uart0_inst_TX     ( HPS_UART_TX   ),          //                               .hps_io_uart0_inst_TX
		//HPS I2C1
	  .hps_0_hps_io_hps_io_i2c0_inst_SDA     ( HPS_I2C0_SDAT  ),        //                               .hps_io_i2c0_inst_SDA
	  .hps_0_hps_io_hps_io_i2c0_inst_SCL     ( HPS_I2C0_SCLK  ),        //                               .hps_io_i2c0_inst_SCL
		//HPS I2C2
	  .hps_0_hps_io_hps_io_i2c1_inst_SDA     ( HPS_I2C1_SDAT  ),        //                               .hps_io_i2c1_inst_SDA
	  .hps_0_hps_io_hps_io_i2c1_inst_SCL     ( HPS_I2C1_SCLK  ),        //                               .hps_io_i2c1_inst_SCL
		//GPIO
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO09  ( HPS_CONV_USB_N ),  //                               .hps_io_gpio_inst_GPIO09
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO35  ( HPS_ENET_INT_N ),  //                               .hps_io_gpio_inst_GPIO35
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO40  ( HPS_LTC_GPIO   ),  //                               .hps_io_gpio_inst_GPIO40
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO53  ( HPS_LED   ),  //                               .hps_io_gpio_inst_GPIO53
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO54  ( HPS_KEY   ),  //                               .hps_io_gpio_inst_GPIO54
	  .hps_0_hps_io_hps_io_gpio_inst_GPIO61  ( HPS_GSENSOR_INT ),  //                               .hps_io_gpio_inst_GPIO61
		//FPGA Partion
	  .led_pio_external_connection_export    ( fpga_led_internal 	),    //    led_pio_external_connection.export
	  .dipsw_pio_external_connection_export  ( SW	),  //  dipsw_pio_external_connection.export
	  .button_pio_external_connection_export ( fpga_debounced_buttons	), // button_pio_external_connection.export
	  .hps_0_h2f_reset_reset_n               ( hps_fpga_reset_n ),                //                hps_0_h2f_reset.reset_n
	  .hps_0_f2h_cold_reset_req_reset_n      (~hps_cold_reset ),      //       hps_0_f2h_cold_reset_req.reset_n
     .hps_0_f2h_debug_reset_req_reset_n     (~hps_debug_reset ),     //      hps_0_f2h_debug_reset_req.reset_n
     .hps_0_f2h_stm_hw_events_stm_hwevents  (stm_hw_events ),  //        hps_0_f2h_stm_hw_events.stm_hwevents
     .hps_0_f2h_warm_reset_req_reset_n      (~hps_warm_reset ),      //       hps_0_f2h_warm_reset_req.reset_n

	  
	  //-------------------------Communicate with HPS---------------------------------
	  .data_type_external_connection_export         			 (Data_type),
	  .input_addr_external_connection_export         			 (Input_addr),	 
	  .weight_addr_external_connection_export         			 (Weight_addr),
	  .output_addr_external_connection_export        			 (Output_addr),
	  .finish_signal_external_connection_export         		 (Finish_signal),  
	 
	  //----------------------SDRAM RAM 1G--------------------------
	  .ping_ram_address                  (PING_RAM_addr),                //                  					   .ddr3_sdram0_data.address
	  .ping_ram_burstcount               (9'd1),                			//                                  .burstcount
	  .ping_ram_waitrequest              (PING_RAM_ready),               //                                  .waitrequest
	  .ping_ram_readdata                 (PING_RAM_rdata),               //                                  .readdata
	  .ping_ram_readdatavalid            (PING_RAM_rdata_valid),         //                                  .readdatavalid
	  .ping_ram_read                     (PING_RAM_read_req),            //                                  .read
	  .ping_ram_writedata                (PING_RAM_wdata),               //                                  .writedata
	  .ping_ram_byteenable               (4'hf),                         //                              .byteenable
	  .ping_ram_write                    (PING_RAM_write_req)            //                                  .write
	  //---------------------------------------------------------
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
 );

// Debounce logic to clean out glitches within 1ms
debounce debounce_inst (
  .clk                                  (fpga_clk_50),
  .reset_n                              (hps_fpga_reset_n),
  .data_in                              (KEY),
  .data_out                             (fpga_debounced_buttons)
);
  defparam debounce_inst.WIDTH = 2;
  defparam debounce_inst.POLARITY = "LOW";
  defparam debounce_inst.TIMEOUT = 50000;               // at 50Mhz this is a debounce time of 1ms
  defparam debounce_inst.TIMEOUT_WIDTH = 16;            // ceil(log2(TIMEOUT))

// Source/Probe megawizard instance
hps_reset hps_reset_inst (
  .source_clk (fpga_clk_50),
  .source     (hps_reset_req)
);

altera_edge_detector pulse_cold_reset (
  .clk       (fpga_clk_50),
  .rst_n     (hps_fpga_reset_n),
  .signal_in (hps_reset_req[0]),
  .pulse_out (hps_cold_reset)
);
  defparam pulse_cold_reset.PULSE_EXT = 6;
  defparam pulse_cold_reset.EDGE_TYPE = 1;
  defparam pulse_cold_reset.IGNORE_RST_WHILE_BUSY = 1;

altera_edge_detector pulse_warm_reset (
  .clk       (fpga_clk_50),
  .rst_n     (hps_fpga_reset_n),
  .signal_in (hps_reset_req[1]),
  .pulse_out (hps_warm_reset)
);
  defparam pulse_warm_reset.PULSE_EXT = 2;
  defparam pulse_warm_reset.EDGE_TYPE = 1;
  defparam pulse_warm_reset.IGNORE_RST_WHILE_BUSY = 1;

altera_edge_detector pulse_debug_reset (
  .clk       (fpga_clk_50),
  .rst_n     (hps_fpga_reset_n),
  .signal_in (hps_reset_req[2]),
  .pulse_out (hps_debug_reset)
);
  defparam pulse_debug_reset.PULSE_EXT = 32;
  defparam pulse_debug_reset.EDGE_TYPE = 1;
  defparam pulse_debug_reset.IGNORE_RST_WHILE_BUSY = 1;

reg [25:0] counter;
reg  led_level;
always @	(posedge fpga_clk_50 or negedge hps_fpga_reset_n)
begin
if(~hps_fpga_reset_n)
begin
		counter<=0;
		led_level<=0;
end

else if(counter==24999999)
	begin
		counter<=0;
		led_level<=~led_level;
	end
else
		counter<=counter+1'b1;
end

assign LED[0]=led_level;
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////// Data_type reg //////////////////
reg 	[10:0] 		input_channel;
reg 	[10:0] 		filter_size;
reg	[3:0]      	Type;
reg					Relu;
reg					BN;
reg      			stride;
reg           	   start;

/////////////// Input addr reg //////////////////
reg [7:0] 		width;
reg [7:0] 		height;
reg [23:0]     Input_addr_init;
reg [23:0]     Weight_addr_init;
reg [23:0]     Output_addr_init;

/////////// DEPTHWISE MODULE PARAMETER //////////
wire 				Dep_finish;
wire 				Dep_data_request;
wire [26:0] 	Dep_data_addr;
reg 				Dep_data_valid;		
reg  [31:0] 	Dep_data_input;
wire				Dep_out_en;
wire [31:0] 	Dep_out_data;

/////////// POINTWISE MODULE PARAMETER //////////
wire 				Poi_finish;
wire 				Poi_data_request;
wire [26:0] 	Poi_data_addr;
reg 				Poi_data_valid;		
reg  [31:0] 	Poi_data_input;
wire				Poi_out_en;
wire [31:0] 	Poi_out_data;


reg 			DEP_START;
reg 			POI_START;
reg 			AVG_START;
reg			BN_START;

assign LED[6] = (!PING_RAM_ready)? 1'b1:1'b0;
assign LED[7] = led_level;


always@(posedge fpga_clk_50)begin
	if(~hps_fpga_reset_n)begin
	
			/////////// Data Type ////////////
			input_channel 	<= 3;
			filter_size 	<= 96;
			Relu				<= 0;
			BN					<= 0;
			stride 			<= 1;
			start				<= 0;
			Type				<= 0;
			oNoFF123			<= 0;
			
			
			/////////////// Input addr reg //////////////////
			width					<= 161;
			height				<= 161;
			Input_addr_init	<= 0;
			Weight_addr_init	<= 0;
			Output_addr_init	<= 0;
	
			///////// Control ON/OFF ////////
			DEP_START <= 0;
			POI_START <= 0;
			AVG_START <= 0;
			BN_START  <= 0;
	end
	else begin
	
			start <= Data_type[2];
			Type  <= Data_type[9:6];
			oNoFF123 <= Data_type[1:0];
			
			if(start)begin
				/////////// Data Type ////////////
				input_channel 	<= Data_type[31:21];
				filter_size 	<= Data_type[20:10];
				Relu				<= Data_type[5];
				BN					<= Data_type[4];
				stride 			<= Data_type[3];		
				
				/////////////// Input addr reg //////////////////
				width 				<= Input_addr[31:24];
				height 				<= Input_addr[31:24];
				Input_addr_init 	<= Input_addr[23:0];
				Weight_addr_init	<= Weight_addr[23:0];
				Output_addr_init	<= Output_addr[23:0];
				
				case(Type)
					0:begin
						POI_START <= 1;
					end
					1:begin
						DEP_START <= 1;
					end
					2:begin
						AVG_START <= 1;
					end
					3:begin
						BN_START  <= 1;
					end
				endcase				
			end
			else begin
				DEP_START <= 0;
				POI_START <= 0;
				AVG_START <= 0;
				BN_START  <= 0;
			end
	end
end


always@(*)begin
	case(Type)
		0:begin
			// POI_START //
			PING_RAM_addr 			<= Poi_data_addr;
			PING_RAM_read_req 	<= Poi_data_request;
			PING_RAM_write_req  	<= Poi_out_en;
			PING_RAM_wdata      	<= Poi_out_data;
			Finish_signal       	<= Poi_finish;
	
			Poi_data_valid			<= PING_RAM_rdata_valid;
			Poi_data_input 		<= PING_RAM_rdata;
		end
		1:begin
			// DEP_START //
			
			PING_RAM_addr 			<= Dep_data_addr;
			PING_RAM_read_req 	<= Dep_data_request;
			PING_RAM_write_req  	<= Dep_out_en;
			PING_RAM_wdata      	<= Dep_out_data;
			Finish_signal       	<= Dep_finish;
			
			Dep_data_valid			<= PING_RAM_rdata_valid;
			Dep_data_input			<= PING_RAM_rdata;
			
		end
		default:begin
			PING_RAM_addr 			<= 0;
			PING_RAM_read_req 	<= 0;
			PING_RAM_write_req  	<= 0;
			PING_RAM_wdata      	<= 0;
			Finish_signal       	<= 0;
			Poi_data_valid			<= 0;
			Poi_data_input			<= 0;
			Dep_data_valid			<= 0;
			Dep_data_input			<= 0;
		end
	endcase
end


Depthwise Dep_module(
						// CLOCK_50MHZ , and RESET
						.CLOCK_50MHZ(fpga_clk_50),
						.RESET(hps_fpga_reset_n),
						
						// Define the input image size
						.width(width),
						.height(height),
						.input_channel(input_channel),
						.stride(stride),
						
						.Input_addr_initial(Input_addr_init),
						.Weight_addr_initial(Weight_addr_init),
						.Output_addr_initial(Output_addr_init),
						
		
						// Some parameter of the Depthwise module, Start_en, in_addr, we_addr ..
						.Dep_start(DEP_START),
						
						.PING_RAM_ready(PING_RAM_ready),
						.Dep_data_request(Dep_data_request),
						.Dep_data_addr(Dep_data_addr),
						.Dep_data_valid(Dep_data_valid),					
						.Dep_data_input(Dep_data_input),

						// Some parameter of the Depthwise store part
						.Dep_out_data(Dep_out_data),
						.Dep_out_en(Dep_out_en),
							
						// When the depthwise is finished , trun on the Finish en
						.Dep_finish(Dep_finish)
						
);



Pointwise Poi_module(
						// CLOCK_50MHZ , and RESET
						.CLOCK_50MHZ(fpga_clk_50),
						.RESET(hps_fpga_reset_n),

						// Define the input image size
						.width(width),
						.height(height),
						.input_channel(input_channel),
						.filter_size(filter_size),
						.ReLU(Relu),
						.BN(BN),
						
						.Input_addr_initial(Input_addr_init),
						.Weight_addr_initial(Weight_addr_init),
						.Output_addr_initial(Output_addr_init),
						
						// Some parameter of the Pointwise module, Start_en, in_addr, we_addr ..
						.Poi_start(POI_START),
						
						.PING_RAM_ready(PING_RAM_ready),
						.Poi_data_request(Poi_data_request),
						.Poi_data_addr(Poi_data_addr),
						.Poi_data_valid(Poi_data_valid),		
						.Poi_data_input(Poi_data_input),
						
						// Some parameter of the Pointwise store part
						.Poi_out_en(Poi_out_en),
						.Poi_out_data(Poi_out_data),
							
						// When the pointwise is finished , trun on the Finish en
						.Poi_finish(Poi_finish)
);






























endmodule
