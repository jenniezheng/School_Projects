// EEM16 - Logic Design
// Design Assignment #3.1
// dassign3_1.v
// Verilog template

// You may define any additional modules as necessary
// Do not delete or modify any of the modules provided

// ****************
// Blocks to design
// ****************

// 3.1a) Rectified linear unit
// out = max(0, in/4)
// 16 bit signed input
// 8 bit unsigned output


module relu(in, out);
    input [15:0] in;
    output [7:0] out;
    wire [15:0] res1,res2,res3;
    mux2 #(16) mux2_1(in, 0, in[15], res1);
    shr #(16) shr_1 (res1,res2);
    shr #(16) shr_2 (res2,res3);
    assign out = res3[7:0];
endmodule

// 3.1b) Pipelined 5 input ripple-carry adder
// 16 bit signed inputs
// 1 bit input: valid (when the other inputs are useful numbers)
// 16 bit signed output (truncated)
// 1 bit output: ready (when the output is the sum of valid inputs)
module piped_adder(clk, a, b, c, d, e, valid, sum, ready);
    input clk, valid;
    input [15:0] a, b, c, d, e;
    output [15:0] sum;
    output ready;
    // your code here
    wire [2:0] c1a,c1b,c2a,c2b,c3a,c3b,c4a,c4b,c5a;
    wire [3:0] o1;
    wire [7:0] o2;
    wire [11:0] o3;
    wire [15:0] o4;
    wire [19:0] o5;
    wire [79:0] i1;
    wire [59:0] i2;
    wire [39:0] i3;
    wire [19:0] i4;
    wire [3:0] readyAlt;


    dreg #(80) dreg_1(clk,{a[15:12],b[15:12],c[15:12],d[15:12],e[15:12],
    a[11:8],b[11:8],c[11:8],d[11:8],e[11:8],
    a[7:4],b[7:4],c[7:4],d[7:4],e[7:4],
    a[3:0],b[3:0],c[3:0],d[3:0],e[3:0]}, i1);
    dreg #(60) dreg_2(clk,i1[79:20],i2);
    dreg #(40) dreg_3(clk,i2[59:20],i3);
    dreg #(20) dreg_4(clk,i3[39:20],i4);

    dreg #(4) dreg_5(clk,o1,o2[3:0]);
    dreg #(8) dreg_6(clk,o2,o3[7:0]);
    dreg #(12) dreg_7(clk,o3,o4[11:0]);
    dreg #(16) dreg_8(clk,o4,sum);

    dreg #(3) dreg_10(clk, c1a,c1b);
    dreg #(3) dreg_11(clk, c2a,c2b);
    dreg #(3) dreg_12(clk, c3a,c3b);

    dreg dreg_14(clk, valid,readyAlt[0]);
    dreg dreg_15(clk, readyAlt[0],readyAlt[1]);
    dreg dreg_16(clk, readyAlt[1],readyAlt[2]);
    dreg dreg_17(clk, readyAlt[2],readyAlt[3]);
    dreg dreg_18(clk, readyAlt[3],ready);

    //module adder5carry (a,b,c,d,e, ci1, ci0a, ci0b, co1, co0a, co0b, sum);
    adder5carry adder5carry_1(i1[19:16],i1[15:12],i1[11:8],i1[7:4],i1[3:0],     0,     0,     0,c1a[0],c1a[1],c1a[2],o1     );
    adder5carry adder5carry_2(i2[19:16],i2[15:12],i2[11:8],i2[7:4],i2[3:0],c1b[0],c1b[1],c1b[2],c2a[0],c2a[1],c2a[2],o2[7:4]);
    adder5carry adder5carry_3(i3[19:16],i3[15:12],i3[11:8],i3[7:4],i3[3:0],c2b[0],c2b[1],c2b[2],c3a[0],c3a[1],c3a[2],o3[11:8]);
    adder5carry adder5carry_4(i4[19:16],i4[15:12],i4[11:8],i4[7:4],i4[3:0],c3b[0],c3b[1],c3b[2],c4a[0],c4a[1],c4a[2],o4[15:12]);

    /*initial begin
        $monitor("readyAlt=%b,sum=%b, ready=%b",readyAlt,sum,ready);
        $monitor("i1=%b\ni2=%b\ni3=%b\ni4=%b",o1,o2,o3,o4);
    end*/
endmodule

// 3.1c) Pipelined neuron
// 8 bit signed weights, bias (constant)
// 8 bit unsigned inputs
// 1 bit input: new (when a set of inputs are available)
// 8 bit unsigned output
// 1 bit output: ready (when the output is valid)
module neuron(clk, w1, w2, w3, w4, b, x1, x2, x3, x4, new, y, ready);
    input clk;
    input [7:0] w1, w2, w3, w4, b;  // signed 2s complement
    input [7:0] x1, x2, x3, x4;     // unsigned
    input new;
    output [7:0] y;                 // unsigned
    output ready;
    wire [5:0] readier;
    output [15:0] prod1,prod2,prod3,prod4,sum;
    wire ready1,ready2,ready3,ready4,valid,valid2;
    wire [2:0] onceReady;
    //multiply (clk, ain, bin, reset, prod, ready);
    multiply  multiply_1(clk, w1,x1,new,prod1,ready1);
    multiply  multiply_2(clk, w2,x2,new,prod2,ready2);
    multiply  multiply_3(clk, w3,x3,new,prod3,ready3);
    multiply  multiply_4(clk, w4,x4,new,prod4,ready4);
    //module piped_adder(clk, a, b, c, d, e, valid, sum, ready);

    mux2 mux2_1(0,ready1,ready2,r1);
    mux2 mux2_2(0,ready3,ready4,r2);
    mux2 mux2_3(0,r1,r2,valid);
    piped_adder piped_adder_1(clk, prod1, prod2, prod3, prod4, {{8{b[7]}},b}, valid, sum, valid2);

    //relu
    dreg dreg_5(clk,onceReady[2],onceReady[0]);
    mux2 mux2_4(onceReady[0],1'b0,new,onceReady[1]);
    mux2 mux2_5(onceReady[1],1'b1,valid2,onceReady[2]);

    assign ready=valid && !onceReady[0];
    relu relu_1(sum, y);

    initial begin
        $monitor("onceReady=%b, ready=%b",onceReady,ready);
    end
endmodule
