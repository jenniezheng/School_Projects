// EEM16 - Logic Design
// Design Assignment #1 - Problem #1.2
// dassign1_2.v
// Verilog template

// You may define any additional modules as necessary
// Do not change the module or port names of these stubs

// Max/argmax (declarative verilog)
// IMPORTANT: Do not change module or port names
module max2(A,B,E,out);
  input [1:0] A,B;
  input E;
  output out;

  assign out=(!A[1] & B[1]) | (B[0]&!A[0]&!A[1]) | (B[1]&B[0]&!A[0]) |
    (!A[0]&!A[1]&E) | (B[0]&!A[1]&E) | (B[0]&B[1]&E) | (B[1]&!A[0]&E);
endmodule

module max8(A,B,Ein,out);
  input [7:0] A,B;
  input Ein;
  output out;

  wire nE1,nE2,nE3;

  max2  max2_1(A[1:0],B[1:0],Ein,nE1);
  max2  max2_2(A[3:2],B[3:2],nE1,nE2);
  max2  max2_3(A[5:4],B[5:4],nE2,nE3);
  max2  max2_4(A[7:6],B[7:6],nE3,out);
endmodule


module mam (in1_value, in1_label, in2_value, in2_label, out_value, out_label);
    input   [7:0] in1_value, in2_value;
    input   [4:0] in1_label, in2_label;
    output  [7:0] out_value;
    output  [4:0] out_label;

  wire first_smaller;

  max8 max8_1(in1_value,in2_value,0,first_smaller);

  assign out_value = (first_smaller==1) ?   in2_value : in1_value ;
  assign out_label = (first_smaller==1) ?   in2_label : in1_label ;
endmodule


// Maximum index (structural verilog)
// IMPORTANT: Do not change module or port names
module maxindex(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,out);

    input [7:0] a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
    output [4:0] out;

  wire [7:0] r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r13,
    r14,r15,r16,r17,r18,r19,r20,r21,r22,r23,r24,r25;

  wire [4:0] l1,l2,l3,l4,l5,l6,l7,l8,l9,l10,l11,l12,l13,
    l14,l15,l16,l17,l18,l19,l20,l21,l22,l23,l24,l25;

    mam mam_1(a, 5'b00000, b, 5'b00001, r1, l1);
    mam mam_2(c, 5'b00010, d, 5'b00011, r2, l2);
    mam mam_3(e, 5'b00100, f, 5'b00101, r3, l3);
    mam mam_4(g, 5'b00110, h, 5'b00111, r4, l4);
    mam mam_5(i, 5'b01000, j, 5'b01001, r5, l5);
    mam mam_6(k, 5'b01010, l, 5'b01011, r6, l6);
    mam mam_7(m, 5'b01100, n, 5'b01101, r7, l7);
    mam mam_8(o, 5'b01110, p, 5'b01111, r8, l8);
    mam mam_9(q, 5'b10000, r, 5'b10001, r9, l9);
    mam mam_10(s, 5'b10010,t, 5'b10011, r10, l10);

    mam mam_11(u, 5'b10100,v, 5'b10101, r11, l11);
    mam mam_12(w, 5'b10110,x, 5'b10111, r12, l12);
    mam mam_13(y, 5'b11000,z, 5'b11001, r13, l13);
    mam mam_14(r1, l1, r2, l2, r14, l14);
    mam mam_15(r3, l3, r4, l4, r15, l15);
    mam mam_16(r5, l5, r6, l6, r16, l16);
    mam mam_17(r7, l7, r8, l8, r17, l17);
    mam mam_18(r9, l9, r10, l10, r18, l18);

    mam mam_19(r11, l11, r12, l12, r19, l19);
    mam mam_20(r13, l13, r14, l14, r20, l20);
    mam mam_21(r15, l15, r16, l16, r21, l21);
    mam mam_22(r17, l17, r18, l18, r22, l22);

    mam mam_23(r19, l19, r20, l20, r23, l23);
    mam mam_24(r21, l21, r22, l22, r24, l24);

    mam mam_25(r23, l23, r24, l24, r25, out);

endmodule
