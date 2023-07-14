`timescale 1ns/1ns

module test;
reg clk;
reg rstn;
reg [7:0] AD;
reg EOC_n;

wire PD;
wire CS_RD_n;
wire AD_CLK;
wire S_H;
wire TX;


initial begin
	clk = 0;
	rstn = 0;
	EOC_n = 1;
	#200 rstn = 1;
end

always #25 clk = ~clk;

reg [7:0] data [0:511];
integer i;
initial begin
	$readmemh("C:/Users/DRX/Desktop/workspace/test/sys_test10k.txt",data);
	i = 0;
	AD = 0;
	#500;
	forever begin
		@(negedge clk)begin
			AD = data[i];
			if(i == 511)begin
				i = 0;
			end
			else
				i = i+1;
		end
	end
end






main u_main(
.clk		(clk),
.rstn		(rstn),
			
.AD			(AD),
.EOC_n		(EOC_n),
			
.PD			(PD), 		
.CS_RD_n	(CS_RD_n),
.AD_CLK		(AD_CLK),
.S_H		(S_H),
.TX			(TX)
);

endmodule
