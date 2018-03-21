// EEM16 - Logic Design
// Design Assignment #1 - Problem #1.1
// dassign1_1.v
// Verilog template

// You may define any additional modules as necessary
// Do not delete or modify any of the modules provided
//
// The modules you will have to design are at the end of the file
// Do not change the module or port names of these stubs

// CMOS gates (declarative Verilog)
// Includes propagation delay t_PD = 1
module inverter(a,y);
    input a;
    output y;

    assign #1 y = ~a;
endmodule

module fa_gate_1(a,b,c,y);
    input a,b,c;
    output y;

    assign #1 y = ~((a&b) | (c&(b|a)));
endmodule

module fa_gate_2(a,b,c,m,y);
    input a,b,c,m;
    output y;

    assign #1 y = ~((a&b&c) | ((a|b|c)&m));
endmodule

// Full adder (structural Verilog)
module fa(a,b,ci,co,s);
    input a,b,ci;
    output s,co;

    wire nco, ns;

    fa_gate_1   fa_gate_1_1(a,b,ci, nco);
    fa_gate_2   fa_gate_2_1(a,b,ci,nco, ns);
    inverter    inverter_1(nco, co);
    inverter    inverter_2(ns, s);
endmodule

// 5+2 input full adder (structural Verilog)
// IMPORTANT: Do not change module or port names
module fa5 (a,b,c,d,e,ci0,ci1,
            co1,co0,s);

    input a,b,c,d,e,ci0,ci1;
    output co1, co0, s;
  wire nc1,nc2,nc3,ns1,ns2;

  fa fa_1(a,b,c,nc1,ns1);
  fa fa_2(d,e,ci0,nc2,ns2);
  fa fa_3(ci1,ns1,ns2,nc3,s);
  fa fa_4(nc1,nc2,nc3,co1,co0);
  //assign {co1,co0,s} =  a+b+c+d+e+ci0+ci1;
endmodule

// 5-input 4-bit ripple-carry adder (structural Verilog)
// IMPORTANT: Do not change module or port names
module adder5 (a,b,c,d,e, sum);
    input [3:0] a,b,c,d,e;
  output [6:0] sum;

  wire nc1,nc2,nc3,nc4,nc5,ns1,ns2,ns3,ns4;
  fa5 fa5_1(a[0],b[0],c[0],d[0],e[0],0,0,nc1,ns1,sum[0]);
  fa5 fa5_2(a[1],b[1],c[1],d[1],e[1],ns1,0,nc2,ns2,sum[1]);
  fa5 fa5_3(a[2],b[2],c[2],d[2],e[2],ns2,nc1,nc3,ns3,sum[2]);
  fa5 fa5_4(a[3],b[3],c[3],d[3],e[3],ns3,nc2,nc4,ns4,sum[3]);

  fa fa_5(nc3,ns4,0,nc5, sum[4]);
  fa fa_6(nc4,nc5,0,sum[6],sum[5]);

endmodule
