// EEM16 - Logic Design
// Design Assignment #2 - Problem #2.1
// dassign2_1.v
// Verilog template

// You may define any additional modules as necessary
// Do not delete or modify any of the modules provided
//
// The modules you will have to design are at the end of the file
// Do not change the module or port names of these stubs

// ***************
// Building blocks
// ***************

// D-register (flipflop).  Width set via parameter.
// Includes propagation delay t_PD = 3
module dreg(clk, d, q);
    parameter width = 16;
    input clk;
    input [width-1:0] d;
    output [width-1:0] q;
    reg [width-1:0] q;

    always @(posedge clk) begin
        q <= #3 d;
    end
endmodule

// 2-1 Mux.  Width set via parameter.
// Includes propagation delay t_PD = 3
module mux2(a, b, sel, y);
    parameter width = 16;
    input [width-1:0] a, b;
    input sel;
    output [width-1:0] y;

    assign #3 y = sel ? b : a;
endmodule


// Left-shifter
// No propagation delay, it's just wires really
module shl(a, y);
    parameter width = 16;
    input [width-1:0] a;
    output [width-1:0] y;

    assign y = {a[width-2:0], 1'b0};
endmodule

// Right-shifter
// more wires
module shr(a, y);
    parameter width = 8;
    input [width-1:0] a;
    output [width-1:0] y;

    assign y = {1'b0, a[width-1:1]};
endmodule

// Lowest order partial product of two inputs
// Use declarative verilog only
// IMPORTANT: Do not change module or port names
module partial_product (a, b, pp);
    input [15:0] a;
    input [15:0] b;
    output [15:0] pp;

    assign #1 pp = (b[0]==1) ? a : 0;
endmodule

// 16-bit adder (declarative Verilog)
// Includes propagation delay t_PD = 3n = 48
module adder16(a, b, sum);
    input [15:0] a, b;
    output [15:0] sum;

    assign #48 sum = a+b;
endmodule

// Determine if multiplication is complete
// Use declarative verilog only
// IMPORTANT: Do not change module or port names
module is_done (a, b, done);
  input [7:0] a, b;
  output done;
  assign #4 done = (b==0) ? 1 : 0;

endmodule

// 8 bit unsigned multiply
// Use structural verilog only
// IMPORTANT: Do not change module or port names
module multiply (clk, ain, bin, reset, prod, ready);
    input clk;
    input [7:0] ain, bin;
    input reset;
    output [15:0] prod;
    output ready;
  wire [7:0] b_reg, b_reg_temp, b_reg_temp2;
  wire [15:0] a_reg, a_reg_temp, a_reg_temp2;
  wire [15:0] accumulator_reg_temp, accumulator_reg_temp2;
    wire [15:0] pp;

  mux2 mux2_1(accumulator_reg_temp, 0, reset, accumulator_reg_temp2);
  mux2 mux2_2( a_reg_temp, {8'b00000000, ain[7:0] }, reset, a_reg_temp2);
    mux2 #(8) mux2_3(b_reg_temp, bin[7:0], reset, b_reg_temp2);

    dreg #(8) b_dreg(clk, b_reg_temp2, b_reg);
    dreg a_dreg(clk, a_reg_temp2, a_reg);
    dreg acc_dreg(clk, accumulator_reg_temp2, prod);

    partial_product partial_product_1(a_reg, {8'b00000000,b_reg}, pp);
    adder16 add_1(pp, prod, accumulator_reg_temp);

  is_done is_done_1(a_reg[7:0], b_reg, ready);
  shl shl_1(a_reg, a_reg_temp);
  shr shr_1(b_reg, b_reg_temp);

endmodule

// Clock generation.  Period set via parameter:
//   clock changes every half_period ticks
//   full clock period = 2*half_period
// Replace half_period parameter with desired value
module clock_gen(clk);
    parameter half_period = 31; // REPLACE half_period HERE
    output clk;
    reg    clk;

    initial clk = 0;
    always #half_period clk = ~clk;
endmodule