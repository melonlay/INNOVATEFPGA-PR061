`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/09/10 11:53:56
// Design Name: 
// Module Name: Pointwise
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Pointwise(
					// CLOCK_50MHZ , and RESET
					input CLOCK_50MHZ,
					input RESET,
					
					// Define the input image size
					input [7:0] 	width,
					input [7:0] 	height,
					input [10:0] 	input_channel,
					input [10:0] 	filter_size,
					
					// ReLU IO
					input ReLU,
					input BN,
					
					input					PING_RAM_ready,
					input [23:0]		Input_addr_initial,
					input [23:0]   	Weight_addr_initial,
					input [23:0]    	Output_addr_initial,
					
					// Some parameter of the Pointwise module, Start_en, in_addr, we_addr ..
					input 							Poi_start,		
					output							Poi_data_request,
					output	reg		[26:0]	Poi_data_addr,
					input								Poi_data_valid,			
					input  	signed 	[31:0] 	Poi_data_input,
					

					// Some parameter of the Pointwise store part
					output  						Poi_out_en,
					output  signed  [31:0] 	Poi_out_data,
					
					// When the pointwise is finished , trun on the Finish en
					output reg Poi_finish,
					output reg [5:0] STEP
    );
integer i;

// Define the total image_size limitation
//wire [15:0] imgSize;
//assign imgSize = height * width;

//	Denfine the final input size
wire [12:0] Poi_size;
assign Poi_size = height*width;
wire [19:0] Poi_out_size;
assign Poi_out_size = Poi_size*filter_size;

// Define the biiger size of the module 
parameter imgSize_max = (10*10)-1;

wire [12:0] receive_size;
assign receive_size = (Poi_size<100)? (Poi_size-1):Poi_size; 

// Controll input address, and weight address
reg [12:0] x_cnt;   // max = 6400
reg [10:0] z_cnt;   // max = 1024
reg [10:0] wx_cnt;  // max = 1024
reg [10:0] wz_cnt;  // max = 1024

reg [10:0] x_cnt_n; // max = 1600
reg [10:0] p_cnt_n;  // max = 4

//reg [5:0] STEP;	

// Receive the real value from the PING SDRAM and PONG SDRAM
wire input_en;
wire weight_en;
wire gamma_input_en;
wire beta_input_en;

// Control the weigth and intput receive ok signal
reg input_read_ok;
reg weight_read_ok;
reg gamma_read_ok;
reg beta_read_ok;

// Control the weigth and intput receive number signal
wire max_weight_address;
wire max_input_address;
wire max_write_address;

// Control the caculation signals 
reg Add_en;			// control the add enable
reg Put_en;			// control the enable which is puting the result to the Register
reg BN_en;
reg P_en;
reg ReLU_en;

reg [10:0] add_cnt;
reg [10:0] put_cnt;
reg [10:0] bn_cnt;
reg [10:0] P_cnt;
reg [10:0] relu_cnt;
reg [10:0] out_cnt;
reg [10:0] input_cnt;
reg [26:0] Poi_out_addr;

reg signed 	[31:0] gamma;
reg signed 	[31:0] beta;
wire signed [63:0] Multiply_result;
wire signed [63:0] Add_result;

reg signed [31:0] weight; // This is a latch for the input weight
reg signed [63:0] add_image [0:imgSize_max];


// Control the input address and weight address part 
					
reg [26:0] Poi_in_addr;
reg [26:0] Poi_we_addr;
reg [26:0] gamma_addr_initial;
reg [26:0] beta_addr_initial;

assign max_write_address   =((STEP==11 || STEP==12) & (out_cnt==x_cnt_n-1))?1'b1:1'b0;
assign max_input_address 	= (x_cnt==receive_size)?1'b1:1'b0;
assign max_weight_address 	= (wx_cnt==input_channel)?1'b1:1'b0;
assign Poi_data_request = (!Poi_out_en & (STEP==1||STEP==3||STEP==6||STEP==8) & RESET==1)? 1:0;
assign gamma_input_en = ((STEP==6||STEP==7) & RESET==1)? 1:0;
assign beta_input_en = ((STEP==8||STEP==9) & RESET==1)? 1:0;
assign weight_en = ((STEP==1||STEP==2) & RESET==1)? 1:0;
assign input_en = ((STEP==3||STEP==4||STEP==10) & RESET==1)? 1:0;
assign Poi_out_en  = ((STEP==11 || STEP==12) & RESET==1)? 1:0;
assign Poi_out_data = add_image[out_cnt][31:0];


always@(*)begin
	Poi_in_addr		   <= z_cnt * Poi_size + x_cnt_n + p_cnt_n * (imgSize_max+1);
	Poi_we_addr		   <= wz_cnt * input_channel + wx_cnt;
	gamma_addr_initial <= Weight_addr_initial + filter_size * input_channel;
	beta_addr_initial  <= Weight_addr_initial + filter_size * (input_channel+1) ;	
end

always@(*)begin
	casex({Poi_out_en, input_en, weight_en, gamma_input_en, beta_input_en})
		5'b00001: Poi_data_addr <= wz_cnt+beta_addr_initial;
		5'b00010: Poi_data_addr <= wz_cnt+gamma_addr_initial;
		5'b00100: Poi_data_addr <= Poi_we_addr+Weight_addr_initial;
		5'b01000: Poi_data_addr <= Poi_in_addr+Input_addr_initial;
		5'b1xxxx: Poi_data_addr <= Poi_out_addr+Output_addr_initial;
		default : Poi_data_addr <= 0;
	endcase
end

always@(posedge CLOCK_50MHZ)begin
	if(!RESET)begin
		STEP <= 0;
		// Some paramter of the input, weight address
		x_cnt <= 0;
		z_cnt <= 0;
		wx_cnt <= 0;
		wz_cnt <= 0;
		//----Finish en----
		Poi_finish <= 0;
		//--------------------------------------------------------
		x_cnt_n <= 0;
		p_cnt_n <= 0;
		out_cnt <= 0;
		
	end
	else begin
		case(STEP)
			0:begin
				if(Poi_start & !Poi_finish)begin
					STEP <= 6;
				end
				else begin
					STEP <= 0;
					// Some paramter of the input, weight address
					x_cnt <= 0;
					z_cnt <= 0;
					wx_cnt <= 0;
					wz_cnt <= 0;		
					//----Finish en----
					x_cnt_n <= 0;
					p_cnt_n <= 0;
					
					Poi_finish <= 0;
					//----------------------------------------
				end
			end
			6:begin	
				if(!PING_RAM_ready & BN & Poi_data_request)begin		
					STEP <= 7;
				end
				else begin	
					STEP <= 6;
				end
			end
			7:begin
				if(gamma_read_ok)begin
					STEP <= 8;
				end
				else begin
					STEP <= 7;
				end
			end
			8:begin
				if(!PING_RAM_ready & Poi_data_request)begin
					STEP <= 9;	
				end	
				else begin
					STEP <= 8;	
				end
			end
			9:begin
				if(beta_read_ok)begin
					STEP <= 1;
				end
				else begin
					STEP <= 9;
				end
			end	
			1:begin
				// Read weight start----
				if(!PING_RAM_ready & !max_weight_address & Poi_data_request)begin	
					wx_cnt <= wx_cnt + 1;
					STEP <= 2;
				end
				else begin
					wx_cnt <= wx_cnt;
					STEP <= STEP;
				end
			end
			2:begin
				if(weight_read_ok)begin
					if(wz_cnt == filter_size)begin
						STEP <= 5;
					end
					else begin
						x_cnt_n <= 0;
						x_cnt  <= p_cnt_n * (imgSize_max+1);
						STEP <= 3;
					end
				end
				else begin
					STEP <= STEP;
				end
			end
			3:begin
				// Read input start ---
				if(!PING_RAM_ready & !max_input_address & Poi_data_request)begin		
					x_cnt <= x_cnt + 1;
					x_cnt_n <= x_cnt_n+1;
					if(x_cnt_n==imgSize_max)begin
						STEP <= 10;	
					end
					else begin
						STEP <= 3;		
					end
				end
				else if(!PING_RAM_ready & max_input_address)begin	
					x_cnt <= 0;
					x_cnt_n <= x_cnt_n+1;
					STEP <= 4;
				end
				else begin		
					x_cnt <= x_cnt;
					STEP <= STEP;
				end
			end
			4:begin
				if(input_read_ok)begin
					if(z_cnt < input_channel-1)begin	
						z_cnt <= z_cnt+1;
						STEP <= 1;
					end
					else begin
						z_cnt <= 0;
						STEP <= 13;
					end
				end
				else begin
					STEP <= STEP;
				end
			end
			5:begin
				if(Poi_out_addr==Poi_out_size)begin
					Poi_finish <= 1;
				end
				else begin
					Poi_finish <= Poi_finish;
				end
				
				if(!Poi_start)begin
					STEP <= 0;
				end
				else begin
					STEP <= 5;
				end
				
			end
			10:begin
				if(input_read_ok)begin
					x_cnt <= 0;
					if(z_cnt < input_channel-1)begin
						z_cnt <= z_cnt+1;
						STEP <= 1;
					end
					else begin
						z_cnt <= 0;
						if(max_input_address)begin
							STEP <= 12;
						end
						else begin
							STEP <= 11;
						end	
					end			
				end
				else begin
					STEP <= STEP;
				end
			end
			11:begin
				if(!PING_RAM_ready & !max_write_address)begin
					STEP <= 11;
					out_cnt <= out_cnt + 1;
				end
				else if(!PING_RAM_ready & max_write_address)begin
					STEP <= 1;
					wz_cnt <= wz_cnt;
					wx_cnt <= 0;
					p_cnt_n <= p_cnt_n+1;
					x_cnt_n <= 0;
					out_cnt <= 0;
				end
				else begin
					STEP <= STEP;
					out_cnt <= out_cnt;
				end
			end
			12:begin
				if(!PING_RAM_ready & !max_write_address)begin
					STEP <= 12;
					out_cnt <= out_cnt + 1;
				end
				else if(!PING_RAM_ready & max_write_address)begin
					wz_cnt <= wz_cnt + 1;
					wx_cnt <= 0;
					STEP <= 6;
					p_cnt_n <= 0;
					x_cnt_n <= 0;
					out_cnt <= 0;
				end
				else begin
					STEP <= STEP;
					out_cnt <= out_cnt;
				end
			end
			13:begin
				if(ReLU_en)begin
				    STEP <=12;
				end
				else begin
					STEP <=STEP;
				end
			end
		endcase
	end
end



always@(posedge CLOCK_50MHZ)begin
	if(!RESET)begin
			
		/////////// weight gamma beta register ////////////
		weight <= 0;
		gamma <= 0;
		beta <= 0;
		
		/////////// SDRAM read data ok signal ////////////
		weight_read_ok <= 0;
		gamma_read_ok <= 0;
		beta_read_ok <= 0;
		
		//////////  input SDRAM read data ok signal //////////// 
		input_read_ok <=0;	
		input_cnt <= 0;
		
		
	end
	else begin
		
		//--------------------------------------------
		if(Put_en)begin
			if(input_cnt==x_cnt_n-1)begin
				input_cnt <= 0;
				input_read_ok <= 1;
			end
			else begin
				input_cnt <= input_cnt+1;
				input_read_ok <= 0;
			end
		end
		else begin
			input_cnt <= input_cnt;
			input_read_ok <= 0;	
		end
		
				
		//////////////// receive SDRAM ready signal ///////////////
		if(gamma_input_en && Poi_data_valid)begin
			gamma <= Poi_data_input;
			gamma_read_ok <= 1;
		end
		else begin
			gamma <= gamma;
			gamma_read_ok <= 0;
		end
		
		//////////////// receive SDRAM ready signal ///////////////
		if(beta_input_en && Poi_data_valid)begin
			beta <= Poi_data_input;
			beta_read_ok <= 1;
		end
		else begin
			beta <= beta;
			beta_read_ok <= 0;
		end
		
		//////////////// receive SDRAM ready signal ///////////////
		if(weight_en && Poi_data_valid)begin
			weight <= Poi_data_input;
			weight_read_ok <= 1;
		end
		else begin
			weight <= weight;
			weight_read_ok <= 0;
		end
		
	end
end


wire signed [31:0] BN_add; 
wire signed [31:0] P_add; 
assign BN_add = (add_image[bn_cnt]+32768)>>16;
assign P_add = (add_image[P_cnt]+32768)>>16;

reg Add_en_d;
reg Add_en_2d;
reg Add_en_3d;
reg Add_en_4d;
reg Add_en_5d;
reg Mul_d;
reg Mul_2d;
reg Mul_3d;
reg Mul_4d;

always@(posedge CLOCK_50MHZ)begin
	if(!RESET)begin
		
		for(i=0;i<=imgSize_max;i=i+1)begin
			add_image[i] <= 0;
		end
		
		/////////// some caculate signal ///////////////
		Add_en <= 0;
		Put_en <= 0;
		BN_en	<= 0;
		P_en	<= 0;
		ReLU_en <= 0;
		
		Mul_d <= 0;
		Mul_2d <= 0;
		Mul_3d <= 0;
		Mul_4d <= 0;
		
		Add_en_d<=0;
		Add_en_2d<=0;
		Add_en_3d<=0;
		Add_en_4d<=0;
		Add_en_5d<=0;
		
		/////////// some cnt of the caculate ////////////
		add_cnt <= 0;
		put_cnt <= 0;
		bn_cnt  <= 0;
		P_cnt 	<= 0;
		relu_cnt <= 0;

	end
	else begin

		Put_en 	<= Add_en_5d;
		Add_en_5d <= Add_en_3d;
		Add_en_4d <= Add_en_3d;
		Add_en_3d <= Add_en_2d;
		Add_en_2d <= Add_en_d;
		Add_en_d <= Add_en;
		Add_en 	<= Mul_4d;
		Mul_4d <= Mul_3d;	
		Mul_3d <= Mul_2d;
		Mul_2d <= Mul_d;
		Mul_d <= (input_en & Poi_data_valid);
				
		/////////////// ADD /////////////////
		if(Add_en)begin
			if(add_cnt < x_cnt_n-1)begin
				add_cnt <= add_cnt + 1;
			end
			else begin
				add_cnt <= 0;
			end
		end
		//--------------------------------------
		
		/////////////// PUT /////////////////
		if(Put_en)begin
			add_image[put_cnt] <= Add_result;
			if( put_cnt < x_cnt_n-1)begin
				put_cnt <= put_cnt + 1;
			end
			else begin
				put_cnt <= 0;
			end	
		end	
		//--------------------------------------------
			
		
		ReLU_en <= P_en;
		P_en    <= BN_en;
		
		// When the wx_cnt_d is equal to input_channel size go batchnormalize
		if(wx_cnt == input_channel && Put_en)begin
			BN_en <= Put_en;
		end
		else begin
			BN_en <= 0;
		end
		//--------------------------------------------
		
		
		/////////////// GAMMA /////////////////
		if(BN_en)begin
			add_image[bn_cnt] <= BN_add*gamma;
			if( bn_cnt < x_cnt_n-1)begin
				bn_cnt <= bn_cnt + 1;
			end
			else begin
				bn_cnt <= 0;
			end	
		end
		//--------------------------------------------
		
		/////////////// BETA /////////////////
		if(P_en)begin
			add_image[P_cnt] <= P_add + beta;
			if( P_cnt < x_cnt_n-1)begin
				P_cnt <= P_cnt + 1;
			end
			else begin
				P_cnt <= 0;
			end	
		end
		//--------------------------------------------
		
		/////////////// Relu /////////////////
		if(ReLU_en)begin
			if( relu_cnt < x_cnt_n-1)begin
				relu_cnt <= relu_cnt + 1;
			end
			else begin
				relu_cnt <= 0;
			end	
			if(ReLU==1)begin
				add_image[relu_cnt] <= (add_image[relu_cnt][63]) ? 0 :
											  (add_image[relu_cnt]);
			end
			else begin
				add_image[relu_cnt] <= (add_image[relu_cnt]);
			end	
		end
		//--------------------------------------------
			
	end
end

always@(posedge CLOCK_50MHZ)begin
	if(!RESET)begin
		Poi_out_addr <= 0;
	end
	else begin
		if(!Poi_finish)begin
			if(!PING_RAM_ready & Poi_out_en)begin
				Poi_out_addr <= Poi_out_addr + 1;
			end
		end
		else begin
			Poi_out_addr <= 0;
		end		
	end
end

wire signed [63:0] add_0;
assign add_0 = (wx_cnt == 1) ? 0 : add_image[add_cnt];

add_LPM ADD1(
					.dataa(add_0), 
					.datab(Multiply_result),
					.clock(CLOCK_50MHZ),
					.result(Add_result)
);


mul_LPM MUL1(
					.dataa(weight), 
					.datab(Poi_data_input),
					.clock(CLOCK_50MHZ),
					.result(Multiply_result)
);
	
endmodule