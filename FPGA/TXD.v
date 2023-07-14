`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/03/31 00:52:55
// Design Name: 
// Module Name: TXD
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


module TXD#(
    parameter bps = 115200,
    parameter freq =  50_000_000
    )(
    input clk,
    input valid,
    input [7:0] TXD_data,
    input rstn,
    output ready,
    output TXD
    );
    
   reg [4:0] txd_state = 0;
   reg [7:0] txd_shift = 0;
   wire clk_sampling;
   
       clk_sampling_module#(
        .bps(bps),
        .freq(freq)
        )sampling_u2(
        .clk(clk),
        .rstn(rstn),
        .clk_sampling(clk_sampling)
        );
   
    always@(posedge clk or negedge rstn)
        begin
        if(~rstn)begin
            txd_state <= 5'b00000;
            txd_shift <= 8'd0;
        end
        else if(txd_state == 5'b01000)
            txd_shift <= TXD_data;
        else if(txd_state[4] && clk_sampling)
            txd_shift <= txd_shift >> 1;
        case(txd_state)
        5'b00000:if(valid)txd_state <= 5'b01000;       //if valid and txd_state = IDLE, then change the state to start mode
        5'b01000:if(clk_sampling)txd_state <= 5'b01001;
        5'b01001:if(clk_sampling)txd_state <= 5'b10000;                               //start state    pull down the txd
        5'b10000:if(clk_sampling)txd_state <= 5'b10001;                               //bit 0
        5'b10001:if(clk_sampling)txd_state <= 5'b10010;                               //bit 1
        5'b10010:if(clk_sampling)txd_state <= 5'b10011;                               //bit 2
        5'b10011:if(clk_sampling)txd_state <= 5'b10100;                               //bit 3
        5'b10100:if(clk_sampling)txd_state <= 5'b10101;                               //bit 4
        5'b10101:if(clk_sampling)txd_state <= 5'b10110;                               //bit 5
        5'b10110:if(clk_sampling)txd_state <= 5'b10111;                               //bit 6     
        5'b10111:if(clk_sampling)txd_state <= 5'b00000;                               //bit 7
        //5'b11000:if(clk_sampling)txd_state <= 5'b00000;                               //bit 8   pull up the txd in the next stage
        default:txd_state <= 5'b0000;        
        endcase
        end
        
    assign TXD = (txd_state < 9)| (txd_state[4] & txd_shift[0]); 
    assign ready = ~(|txd_state);   
    
    
    
    
    
    
    
    
endmodule
