`timescale 1ns/1ps






module main (
input 		clk		,
input 		rstn	,

input [7:0] AD			,
input 		EOC_n		,

output		PD			, 		
output		CS_RD_n	,
output 		AD_CLK	,
output		S_H		,
output 		TX			
);

reg RD_reg;



pll pll_1M(
.areset	(~rstn)	,
.inclk0	(clk)	,
.c0		(pll_clk)	,
.c1(S_H),
.c2(aclk),
.locked	()	);


wire freq_en;
wire [31:0] freq;

freqcalc freqcalc_u0(
    .aclk(aclk),
    .rstn(rstn),
    .data_in(AD),
    .freq_en(freq_en),
    .freq(freq)
);



wire ready;
wire en;
wire [7:0] data_tri;

triger_FIFO tirger_FIFO_u1(
	.aclk(aclk),
	.rstn(rstn),
	.data_in(AD),
	.ready(ready),
	.data_out(data_tri),
	.read_out(en)
	);



TX TX_u2(
	.clk(aclk),
	.rstn(rstn),
	.en(en),
	.uart_din(data_tri),
	.uart_txd(TX),
	.ready(ready)
);




always@(posedge aclk or negedge rstn)begin
	if(~rstn)
		RD_reg <= 1'b1;
   else
		if(~EOC_n)
			RD_reg <= 1'b0;
		else
			RD_reg <= 1'b1;
	
end



assign AD_CLK = pll_clk;
//assign S_H = pll_clk;
assign CS_RD_n = RD_reg;
assign PD = 1'b1;






endmodule //main