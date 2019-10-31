`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2019/09/08 14:43:09
// Design Name: 
// Module Name: Depthwise
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


module Depthwise(
					// CLOCK_50MHZ , and RESET
					input CLOCK_50MHZ,
					input RESET,
					// Define the input image size
					input [7:0] 	width,
					input [7:0] 	height,
					input [10:0] 	input_channel,
					input 			stride,

					input [23:0]		Input_addr_initial,
					input [23:0]    	Weight_addr_initial,
					input [23:0]    	Output_addr_initial,
					
					// Some parameter of the Depthwise module, Start_en, in_addr, we_addr ..
					input								PING_RAM_ready,
					input 							Dep_start,
					output 							Dep_data_request,
					output	reg	[26:0]		Dep_data_addr,
					input								Dep_data_valid,					
					input  			[31:0] 		Dep_data_input,
					
					

					// Some parameter of the Depthwise store part
					output signed [31:0]	    	Dep_out_data,
					output reg 			 			Dep_out_en,
						
					// When the depthwise is finished , trun on the Finish en
					output reg 	Dep_finish
);
	
integer i;
	
// Set the kernel size in here, we just use the 3x3 kernel size 
parameter kernel_size = 3;  
parameter Total_kernel_size = kernel_size*kernel_size;

// The input image size, is using for the address of the PING, PONG RAM
wire [15:0] imgSize;
assign imgSize = height * width;

// The final Data of input size is depended on the padding signal, strides, and the original input size (Odd number, Double number)
// The size is using for the calculation of output size
//wire [7:0] Dep_H;
//assign Dep_H = 	(stride==0) ? height + 2 : height + 1;
				
//wire [7:0] Dep_W;
//assign Dep_W = 	(stride==0) ? width + 2 : width + 1;

// Caculate the output size, is using for the store addr of the output
wire [7:0] out_H;
assign out_H = (stride==0)?  height-(kernel_size-1) : height>>1;

wire [7:0] out_W;
assign out_W = (stride==0)?  width-(kernel_size-1) : width>>1;

wire [19:0] out_size;
assign out_size = out_H*out_W*input_channel;


// Control the x asix and y asix limitation of the sliding window 
wire [7:0] yr_cnt_limit;
assign yr_cnt_limit = height - (kernel_size-1) - 1;

wire [7:0] x_cnt_limit;
assign x_cnt_limit = width;	
	
	
// Controll input_addr , amd weight addr
reg  [10:0] x_cnt, y_cnt, yr_cnt, z_cnt;
reg  [10:0] wx_cnt, wz_cnt;

// Use to control the multiply_en trun on or trun off
reg [3:0] stride_cnt;

// Read_nine and Read_three is different case for how to read the input data
reg Read_nine;
reg Read_three;

// Because the real value of the input will ready when input_en delay one clock
wire input_en;
wire weight_en;

// Here is about some controller of the AMU : Enable, Multiply, Add
reg Multiply_en;
reg Add1_en;
reg Add2_en;
reg Add3_en;
reg Add4_en;
reg signed [63:0] Multiply [0:8];
reg signed [63:0] Add [0:10];

// Design a moduel about calculate the input_addr, and output_addr module, is using the Combinational logic.
reg [26:0] Dep_addr_mul1;
reg [26:0] Dep_addr_mul2;
reg [26:0] Dep_we_addr_mul1;
reg [26:0] Dep_in_addr;
reg [26:0] Dep_we_addr;
reg [26:0] Dep_out_addr;

// Control the weigth and intput receive ok signal
reg weight_read_ok;
reg input_read_ok;

// Control the FIFO of the output
wire [31:0]din;
reg  wr_en;
wire rd_en;
wire [31:0]dout;
wire full;
wire empty;

// Control the input address and weight address part 
wire max_weight_address;
wire max_nine_address;
wire max_three_address;
assign max_weight_address 	= (wx_cnt==Total_kernel_size-1)?1'b1:1'b0;
assign max_nine_address 	= (x_cnt==kernel_size-1)?1'b1:1'b0;
assign max_three_address 	= (y_cnt==kernel_size+yr_cnt-1)?1'b1:1'b0;

// The stride number will influence the calculation of address
always@(*)begin
	// The input_addr ---
	Dep_addr_mul1 		<= z_cnt * imgSize;
	Dep_addr_mul2 		<= y_cnt * height;
	Dep_in_addr 		<= Dep_addr_mul1 + Dep_addr_mul2 + x_cnt;

	// The weight_addr ---
	Dep_we_addr_mul1 	<= (wz_cnt<<3)+wz_cnt;
	Dep_we_addr 		<=  Dep_we_addr_mul1 + wx_cnt;
end
 
always@(*)begin
	casex({Dep_out_en, input_en, weight_en})
		3'b1xx: Dep_data_addr <= Dep_out_addr+Output_addr_initial;
		3'b010: Dep_data_addr <= Dep_in_addr+Input_addr_initial;
		3'b001: Dep_data_addr <= Dep_we_addr+Weight_addr_initial;
		default : Dep_data_addr <= 0;
	endcase
end	 

// The Final output value
assign  din 			= (Add[10]+32768)>>16;
assign  Dep_out_data = dout;
assign  Dep_data_request   = ((STEP==1||STEP==3||STEP==5) && RESET==1)? 1:0;
assign  weight_en 			= (STEP==1||STEP==2) ? 1:0;
assign  input_en 				= (STEP==3||STEP==4||STEP==5||STEP==6) ? 1:0;


reg [5:0] STEP;
always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
		STEP 			<= 0;
		// Some paramter of the input, weight address
		x_cnt 			<= 0;
		y_cnt 			<= 0;
		yr_cnt 			<= 0;
		z_cnt 			<= 0;
		wx_cnt 			<= 0;
		wz_cnt 			<= 0;
		// Read_nine and Read_three is different case for how to read the input data
		Read_nine 		<= 0;
		Read_three 		<= 0;
		//---Multiply, Add Control
		Multiply_en 	<= 0;
		//----Finish en----
		Dep_finish 		<= 0;
		// Use to control the multiply_en trun on or trun off
		stride_cnt 		<= 0;
		
	end
	else begin
		case(STEP)
			0:begin
				if(Dep_start && !Dep_finish)begin
					STEP <= 1;
				end
				else begin
					STEP <= 0;
					// Some paramter of the input, weight address
					x_cnt 			<= 0;
					y_cnt 			<= 0;
					yr_cnt 			<= 0;
					z_cnt 			<= 0;
					wx_cnt 			<= 0;
					wz_cnt 			<= 0;
					// Read_nine and Read_three is different case for how to read the input data
					Read_nine 		<= 0;
					Read_three 		<= 0;
					//---Multiply, Add Control
					Multiply_en 	<= 0;
					//----Finish en----
					Dep_finish 		<= 0;
					// Use to control the multiply_en trun on or trun off
					stride_cnt 		<= 0;
					// The signal mean the module is already to get the data
				end
			end
			1:begin
				Multiply_en <= 0;
				//Read weight start-------
				if(!PING_RAM_ready & !max_weight_address & !full & !rd_en)begin
					wx_cnt <= wx_cnt+1;
					wz_cnt <= wz_cnt;
					STEP <= 1;	
				end		
				else if(!PING_RAM_ready & max_weight_address)begin
					wx_cnt <= 0;
					wz_cnt <= wz_cnt+1;
					STEP <= 2;
				end
				else begin
					wx_cnt <= wx_cnt;
					wz_cnt <= wz_cnt;
					STEP <= STEP;
				end
			end	
			2:begin
				if(weight_read_ok)begin
					STEP <= 3;
				end
				else begin
					STEP <= 2;
				end
			end	
			3:begin
				stride_cnt 	<= 0;
				Read_nine 	<= 1;
				Read_three 	<= 0;
				Multiply_en <= 0;
				//Read 9 input data--------
				if(!PING_RAM_ready & !max_nine_address & !full & !rd_en)begin
					x_cnt <= x_cnt + 1;
				end
				else if(!PING_RAM_ready & max_nine_address)begin
					if(y_cnt == kernel_size-1+yr_cnt)begin
						STEP <= 4;
						x_cnt <= kernel_size;
						y_cnt <= yr_cnt;
					end
					else begin
						STEP <= 3;
						x_cnt <= 0;
						y_cnt <= y_cnt +1;
					end
				end
				else begin
					x_cnt <= x_cnt;
				end
			end
			4:begin
				if(input_read_ok)begin
					STEP <= 10;
				end
				else begin
					STEP <= 4;
				end
			end
			5:begin
				Read_nine <= 0;
				Read_three <= 1;
				Multiply_en <= 0;
				//Read 3 input data--------
				if(!PING_RAM_ready & !max_three_address & !full)begin
					y_cnt <= y_cnt + 1;
					STEP <= 5;
				end
				else if(!PING_RAM_ready & max_three_address )begin
					y_cnt <= yr_cnt;
					STEP <= 6;
					x_cnt <= x_cnt + 1;
				end
				else begin
					y_cnt <= y_cnt;
					STEP <= STEP;
				end
			end
			6:begin
				if(input_read_ok)begin
					stride_cnt <= stride_cnt+1;
					STEP <= 8;
				end
				else begin
					stride_cnt <= stride_cnt;
					STEP <= 6;
				end
			end
			7:begin
				Read_nine <= 0;
				Read_three <= 0;
				Multiply_en <= 0;
				if(yr_cnt == yr_cnt_limit)begin
					yr_cnt <= 0;
					y_cnt <= 0;
					if(z_cnt < input_channel-1)begin
						STEP <= 11;
						z_cnt <= z_cnt+1;
					end
					else begin
						STEP <= 9;
						z_cnt <= 0;
					end	
				end
				else begin
					STEP <= 11;
					yr_cnt <= yr_cnt + stride +1;
					y_cnt <= yr_cnt + stride +1;
				end
			end
			8:begin		
				if(stride_cnt == stride+1)begin
					Multiply_en <= 1;
					stride_cnt <= 0;
				end
				else begin
					Multiply_en <= 0;
					stride_cnt <= stride_cnt;
				end		
				
				if(x_cnt >= x_cnt_limit)begin
					x_cnt <= 0;
					STEP <= 7;
				end
				else begin
					STEP <= 5;
				end	
				
			end
			9:begin
				Multiply_en <= 0;	
				if(Dep_out_addr==out_size)begin
					Dep_finish <= 1;
				end
				else begin
					Dep_finish <= Dep_finish;
				end
					
				if(!Dep_start)begin
					STEP <= 0;
				end	
				else begin
					STEP <= 9;
				end
			
			end
			10:begin
				STEP <= 5;
				Multiply_en <= 1;
			end
			11:begin
				Read_nine <= 0;
				Read_three <= 0;
				Multiply_en <= 0;
				if(Dep_out_en)begin
					if(yr_cnt==0)begin
						STEP <= 1;
					end
					else begin
						STEP <= 3;
					end
				end
			end
		endcase
	end

end

// Pipeline the signal. 
always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
	

		Add4_en<=0;
		Add3_en<=0;
		Add2_en<=0;
		Add1_en<=0;		
		wr_en  <=0;
		
	end
	else begin
						
		//---------Mul. Add. Control-----
		wr_en <= Add4_en;
		Add4_en <= Add3_en;
		Add3_en <= Add2_en;
		Add2_en <= Add1_en;
		Add1_en <= Multiply_en;
	end
end



//--------Shift Input?eight buffer !!!!!!
reg signed [31:0] Input_buffer[0:8]; 
reg signed [31:0] weight_buffer[0:8];
reg [3:0] wcnt;
reg [3:0] icnt;
reg [3:0] tcnt;


always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
		for(i=0;i<Total_kernel_size;i=i+1)begin
			Input_buffer[i]<=0;
			weight_buffer[i]<=0;
		end
		wcnt <= 0;
		icnt <= 0;
		tcnt <= 2;
		
		weight_read_ok <= 0;
		input_read_ok <= 0;
	end
	else begin
		if(weight_en && Dep_data_valid)begin
			weight_buffer[wcnt] <= Dep_data_input;				
			if(wcnt==Total_kernel_size-1)begin
				weight_read_ok <= 1;
				wcnt <= 0;
			end
			else begin
				weight_read_ok <= 0;
				wcnt <= wcnt+1;
			end		
		end
		else begin
			wcnt <= wcnt;
			weight_read_ok <= 0;
		end
		
		
		case({Read_nine,Read_three})
			2'b01:begin
				icnt <= 0;
				if(input_en && Dep_data_valid)begin
					Input_buffer[tcnt-2]<=Input_buffer[tcnt-1];
					Input_buffer[tcnt-1]<=Input_buffer[tcnt];
					Input_buffer[tcnt] <= Dep_data_input;					
					
					if(tcnt==Total_kernel_size-1)begin
						input_read_ok <= 1;
						tcnt <= 2;
					end
					else begin
						input_read_ok <= 0;
						tcnt <= tcnt + 3;
					end	
				end
				else begin
					input_read_ok <= 0;
					tcnt <= tcnt;
				end
			end
			2'b10:begin
				if(input_en && Dep_data_valid)begin
					Input_buffer[icnt] <= Dep_data_input;
					
					if(icnt==Total_kernel_size-1)begin
						input_read_ok <= 1;
						icnt <= icnt;
					end
					else begin
						input_read_ok <= 0;
						icnt <= icnt + 1;
					end	
				end
				else begin
					icnt <= icnt;
					input_read_ok <= 0;
				end
			end
			default:begin
				icnt <= icnt;
				tcnt <= 2;
				input_read_ok <= 0;
			end
		endcase
			
	end
end


//---------------- Caculate the output value here --------
always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
		for(i=0;i<9;i=i+1)begin
			Multiply[i]<=0;
		end
		for(i=0;i<11;i=i+1)begin
			Add[i]<=0;
		end
	end
	else begin
		//-----Multiply part1-----
		if(Multiply_en)begin
			for(i=0;i<9;i=i+1)begin
				Multiply[i]<=weight_buffer[i]*Input_buffer[i];
			end	
		end
		//-----ADD part1---
		if(Add1_en)begin
			Add[0]<=Multiply[0]+Multiply[1];
			Add[1]<=Multiply[2]+Multiply[3];
			Add[2]<=Multiply[4]+Multiply[5];
			Add[3]<=Multiply[6]+Multiply[7];
			Add[4]<=Multiply[8];
		end
		//-----ADD part2---
		if(Add2_en)begin
			Add[5]<=Add[0]+Add[1];
			Add[6]<=Add[2]+Add[3];
			Add[7]<=Add[4];
		end
		//-----ADD part3---
		if(Add3_en)begin
			Add[8]<=Add[5]+Add[6];
			Add[9]<=Add[7];
		end
		//-----ADD part4---
		if(Add4_en)begin
			Add[10]<=Add[8]+Add[9];
		end	
		
	end
end

always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
		Dep_out_addr<=0;
	end
	else begin
		if(Dep_finish)begin
			Dep_out_addr<=0;
		end
		else if(Dep_out_en)begin
			Dep_out_addr<=Dep_out_addr+1;
		end
		else begin
			Dep_out_addr<=Dep_out_addr;
		end
	end
end
	
always@(posedge CLOCK_50MHZ)begin
	if(~RESET)begin
		Dep_out_en<=0;
	end
	else begin
		Dep_out_en<=rd_en;
	end
end

assign rd_en = (!PING_RAM_ready && !Dep_data_request && !empty) ? 1:0;

output_fifo uu1(
					.clock(CLOCK_50MHZ),
					.sclr(!RESET),
					.data(din),
					.wrreq(wr_en),
					.rdreq(rd_en),
					.q(dout),
					.full(full),
					.empty(empty)
);

	
endmodule
