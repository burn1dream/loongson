`timescale 1ns/1ps




//aclk freq = 50MHz
// calculation time = 1s



module freqcalc (
    input aclk,
    input rstn,
    input [7:0] data_in,
    output reg freq_en,
    output reg [31:0] freq
);

reg data_in_r0;
reg data_in_r1;

reg [31:0] count1;
reg [31:0] count2;        


// change the data to the pulse
always @(posedge aclk or negedge rstn) begin
    if (~rstn) begin
        data_in_r0 <= 'd0;
        data_in_r1 <= 'd0;     
    end
    else begin
        if(data_in >= 8'b1000_0000)begin
            data_in_r0 <= 1'b1;
            data_in_r1 <= data_in_r0;
        end
        else begin
            data_in_r0 <= 1'b0;
            data_in_r1 <= data_in_r0;
        end
    end
end

// count1 for 1s 
always @(posedge aclk or negedge rstn) begin
    if (~rstn) begin
        count1 <= 'd0;
    end
    else begin
        if(count1 == 50_000_000 - 1)
            count1 <= 'd0;
        else
            count1 <= count1 + 1'b1;
    end
        
end

//count2 for freq
always @(posedge aclk or negedge rstn) begin
    if(~rstn)begin
        count2 <= 'd0;
        freq_en <= 1'b1;
        freq <= 'd0;
    end
    else begin
        if((data_in_r0 == 1'b1) && (data_in_r1 == 1'b0))
            count2 <= count2 + 1'b1;
        else begin 
            if(count1 == 50_000_000 - 1)begin
                freq <= count2;
                freq_en <= 1'b1;
                count2 <= 'd0;
            end
            else begin
                count2 <= count2;
                freq_en <= 1'b1;
            end
        end  
    end

end


endmodule //freqcalc