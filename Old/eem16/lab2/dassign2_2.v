// EEM16 - Logic Design
// Design Assignment #2 - Problem #2.2
// dassign2_2.v
// Verilog template

// You may define any additional modules as necessary
// Do not delete or modify any of the modules provided
//
// The modules you will have to design are at the end of the file
// Do not change the module or port names of these stubs

// Include constants file defining THRESHOLD, CMDs, STATEs

`include "constants2_2.vh"

// ***************
// Building blocks
// ***************

// D-register (flipflop).  Width set via parameter.
// Includes propagation delay t_PD = 3
module dreg(clk, d, q);
    parameter width = 1;
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
    parameter width = 1;
    input [width-1:0] a, b;
    input sel;
    output [width-1:0] y;

    assign #3 y = sel ? b : a;
endmodule


// 4-1 Mux.  Width set via parameter.
// Includes propagation delay t_PD = 3
module mux4(a, b, c, d, sel, y);
    parameter width = 1;
    input [width-1:0] a, b, c, d;
    input [1:0] sel;
    output [width-1:0] y;
    reg [width-1:0] y;

    always @(*) begin
        case (sel)
            2'b00:    y <= #3 a;
            2'b01:    y <= #3 b;
            2'b10:    y <= #3 c;
            default:  y <= #3 d;
        endcase
    end
endmodule



// ****************
// Blocks to design
// ****************

module counter(prevSum,isNew,sum);
    parameter width = 8;
    input [width-1:0] prevSum;
    input isNew;
    output [width-1:0] sum;
    mux2 #(width) mux_1(prevSum + 1,1,isNew,sum);
endmodule

// Evaluates incoming pulses/gaps
// Use any combination of declarative or structural verilog
// IMPORTANT: Do not change module or port names
module pulse_width(clk, in, pwidth, long, type, new);
    parameter width = 8;
    input clk, in;
    output [width-1:0] pwidth;
    output long, type, new;

    wire [width-1:0] c_pwidth;
    wire long_out, prev_in;

    dreg in_reg(clk, in, prev_in);

    assign new = in ^ prev_in;
    assign type = !in;
    assign long = pwidth > `THRESHOLD? 1 : 0;

    dreg #(width) pwidth_reg(clk,c_pwidth,pwidth);
    mux2 #(width) mux_1(pwidth + 1,1,new,c_pwidth);

endmodule

// Four valued shift-register
// Use any combination of declarative or structural verilog
//    or procedural verilog, provided it obeys rules for combinational logic
// IMPORTANT: Do not change module or port names
module shift4(clk, in, cmd, out3, out2, out1, out0);
    parameter width = 8;
    input clk;
    input [width-1:0] in;
    input [`CMD_WIDTH-1:0] cmd;
    output [width-1:0] out3, out2, out1, out0;

    wire [width*4-1:0] rout3, rout2, rout1;
    dreg #(32) dreg_1(clk, rout3, rout1);
    mux2 #(32) mux2_1(rout1, {rout1[width*3-1:0],in}, (`CMD_LOAD==cmd), rout2);
    mux2 #(32) mux2_2(rout2, 32'b00000000000000000000000000000000, (`CMD_CLEAR==cmd), rout3);

    assign out3 = rout1[width*4-1:width*3];
    assign out2 = rout1[width*3-1:width*2];
    assign out1 = rout1[width*2-1:width];
    assign out0 = rout1[width-1:0];
endmodule

// Control FSM for shift register
// Use any combination of declarative or structural verilog
//    or procedural verilog, provided it obeys rules for combinational logic
// IMPORTANT: Do not change module or port names
module control_fsm(clk, long, type, cmd, done);
    input clk, long, type;
    output [`CMD_WIDTH-1:0] cmd;
    output done;
    assign cmd = (clk&type) ? `CMD_LOAD : ((clk&long) ?  `CMD_CLEAR : `CMD_HOLD);
    assign done = (clk&!type&long) ? 1 : 0;
endmodule

// Input de-serializer
// Use structural verilog only
// IMPORTANT: Do not change module or port names
module deserializer(clk, in, out3, out2, out1, out0, done);
    parameter width = 8;
    input clk, in;
    output [width-1:0] out3, out2, out1, out0;
    output done;
    wire [`CMD_WIDTH-1:0] cmd,cmd_temp;
    wire [width-1:0] pwidth;
    wire new, long, type;

    pulse_width pulse_width_1(clk, in, pwidth, long, type, new);
    control_fsm control_fsm_1(new, long, type, cmd, done);
    shift4 shift4_1(clk, pwidth, cmd, out3, out2, out1, out0);
endmodule


// Use assign statements, instantiating othe modules, procedural Verilog
// FSM -- flip flops, resettable, defines/parameters