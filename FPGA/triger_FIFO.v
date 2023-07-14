`timescale 1ns/1ps



module triger_FIFO(
	input aclk,
	input rstn,
	input [7:0] data_in,
	input [9:0] depth,
	input [7:0] triger,
	input ready,
	output empty,
	output full,
	output [7:0] data_out,
	output read_out
	//output [9:0] usedw   //depth of the data in FIFO
	
	);


	reg [7:0] data_in_r0;
	reg [7:0] data_in_r1;
	reg [9:0] depth_r = 'd200;
	reg [7:0] triger_r = 8'b1000_0000;
	reg [2:0] state;
	reg write;
	reg read;
	reg [9:0] cnt = 10'd0;
	
always@(posedge aclk or negedge rstn)begin
	if(~rstn)begin
		data_in_r0 <= 'd0;
		data_in_r1 <= 'd0;
		depth_r <= 'd0;
		triger_r <= 'd0;
	end
	else begin
		data_in_r0 <= data_in;
		data_in_r1 <= data_in_r0;
		depth_r <= depth;
		triger_r <= triger;
	end
	
end	





always@(posedge aclk or negedge rstn)begin
	if(~rstn)
		state <= 'd0;
	else
		case(state)
		3'b000:begin
			if((empty) && (data_in_r0 >= triger_r) && (data_in_r0 > data_in_r1))			//triger on in posedge data
				state <= 3'b010;
		end
		3'b010:begin
			if(usedw == depth_r - 1)
				state <= 3'b100;
		end
		3'b100:begin
			if((empty) && (data_in_r0 >= triger_r) && (data_in_r0 > data_in_r1) && (~ready))
				state <= 3'b010;
			else if(ready && (~((empty) && (data_in_r0 >= triger_r) && (data_in_r0 > data_in_r1))))
				state <= 3'b101;
		end
		3'b101:begin
				state <= 3'b110;
		end
		3'b110:begin
				if(cnt == 10'd499)
				begin
					cnt <= 'd0;
					state <= 3'b100;
				end
				else begin
					cnt <= cnt + 1'b1;
				end
		end
		default:
			state <= 3'b000;
	endcase
end






always@(*)begin
	if(~rstn)
		write = 'd0;
	else if((data_in_r0 != data_in_r1) && (state == 2'b10))
		write = 1'b1;
	else
		write	= 'd0;
end

always@(*)begin
	if(~rstn)
		read = 'd0;
	else if(state == 3'b101)
		read = 1'b1;
	else
		read = 'd0;
end	

assign read_out = read;	

FIFO1 FIFO_u1(
	.clock(aclk),
	.data(data_in_r1),
	.rdreq(read),
	.sclr(rstn),
	.wrreq(write),
	.empty(empty),
	.full(full),
	.q(data_out),
	.usedw(usedw)  				//depth of the data in FIFO
	);






	
	
endmodule