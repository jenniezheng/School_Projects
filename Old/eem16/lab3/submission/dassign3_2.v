// EEM16 - Logic Design
// Design Assignment #3.2
// dassign3_2.v
// Verilog template

// You may define any additional modules as necessary
// Do not delete or modify any of the modules provided

// ****************
// Blocks to design
// ****************

// 3.2a) Inter-layer module
// four sets of inputs: 8 bit value, 1 bit input ready
// one 1 bit input new (from input layer of neurons)
// four sets of 8 bits output
// one 1 bit output ready

module interlayer(clk, new, in1, ready1, in2, ready2, in3, ready3, in4, ready4,
                  out1, out2, out3, out4, ready_out);
    input clk;
    input new;
    input [7:0] in1, in2, in3, in4;
    input ready1, ready2, ready3, ready4;
    output [7:0] out1, out2, out3, out4;
    output ready_out;

    wire [3:0] onceReady;

    dreg #(8) dreg_1(ready1,in1,out1);
    dreg #(8) dreg_2(ready2,in2,out2);
    dreg #(8) dreg_3(ready3,in3,out3);
    dreg #(8) dreg_4(ready4,in4,out4);

    assign validAll=ready1&ready2&ready3&ready4;

    dreg dreg_5(clk,onceReady[2],onceReady[0]);
    mux2 mux2_4(onceReady[0],1'b0,new,onceReady[1]);
    mux2 mux2_5(onceReady[1],1'b1,validAll,onceReady[2]);

    dreg dreg_6(clk, ready_outPrev,ready_out);
    mux2 mux6(0, 1, validAll & !onceReady[0],ready_outPrev);


endmodule
