// EEM16 - Logic Design
// Design Assignment #1 - Problem #1.3
// dassign1_3.v
// Verilog template

// You may define any additional modules as necessary
// Do not change the module or port names of the given stubs

/* Arrays for convenience

  | abcdefghijklmnopqrstuvwxyz  <-- letter
__|___________________________
G | 11111111001001111111000111
F | 11001111001100011010101010
E | 11111101011111110100110001
D | 01111010011100100010111111
C | 11010011110011101011110010
B | 10010011110000011001101011
A | 10001110000010011010000101
^-- display segment
~~~

  | GFEDCBA <-- display segment
__|________
a | 1110111
b | 1111100
c | 1011000
d | 1011110
e | 1111001
f | 1110001
g | 1101111
h | 1110110
i | 0000110
j | 0011110
k | 1111000
l | 0111000
m | 0010101
n | 1010100
o | 1011100
p | 1110011
q | 1100111
r | 1010000
s | 1101101
t | 1000110
u | 0111110
v | 0011100
w | 0101010
x | 1001001
y | 1101110
z | 1011011
^-- letter
*/

// Display driver (procedural verilog)
// IMPORTANT: Do not change module or port names
module display_rom (letter, display);
    input   [4:0] letter;
    output  [6:0] display;

  reg [6:0] display;
  always @(letter) begin
    case (letter)
      5'h0 : assign display = 7'b1110111;
  5'h1 : assign display = 7'b1111100;
  5'h2 : assign display = 7'b1011000;
  5'h3 : assign display = 7'b1011110;
  5'h4 : assign display = 7'b1111001;
  5'h5 : assign display = 7'b1110001;
  5'h6 : assign display = 7'b1101111;
  5'h7 : assign display = 7'b1110110;
  5'h8 : assign display = 7'b0000110;
  5'h9 : assign display = 7'b0011110;
  5'ha : assign display = 7'b1111000;
  5'hb : assign display = 7'b0111000;
  5'hc : assign display = 7'b0010101;
  5'hd : assign display = 7'b1010100;
  5'he : assign display = 7'b1011100;
  5'hf : assign display = 7'b1110011;
  5'h10 : assign display = 7'b1100111;
  5'h11 : assign display = 7'b1010000;
  5'h12 : assign display = 7'b1101101;
  5'h13 : assign display = 7'b1000110;
  5'h14 : assign display = 7'b0111110;
  5'h15 : assign display = 7'b0011100;
  5'h16 : assign display = 7'b0101010;
  5'h17 : assign display = 7'b1001001;
  5'h18 : assign display = 7'b1101110;
  5'h19 : assign display = 7'b1011011;
      default : assign display = 7'h40;
    endcase
  end

endmodule

// Display driver (declarative verilog)
// IMPORTANT: Do not change module or port names
module display_mux (letter, g,f,e,d,c,b,a);
    input   [4:0] letter;
    output  g,f,e,d,c,b,a;

  reg g,f,e,d,c,b,a;
    reg [6:0] display;
  always @(letter) begin
    case (letter)
      5'h0 : assign display = 7'b1110111;
  5'h1 : assign display = 7'b1111100;
  5'h2 : assign display = 7'b1011000;
  5'h3 : assign display = 7'b1011110;
  5'h4 : assign display = 7'b1111001;
  5'h5 : assign display = 7'b1110001;
  5'h6 : assign display = 7'b1101111;
  5'h7 : assign display = 7'b1110110;
  5'h8 : assign display = 7'b0000110;
  5'h9 : assign display = 7'b0011110;
  5'ha : assign display = 7'b1111000;
  5'hb : assign display = 7'b0111000;
  5'hc : assign display = 7'b0010101;
  5'hd : assign display = 7'b1010100;
  5'he : assign display = 7'b1011100;
  5'hf : assign display = 7'b1110011;
  5'h10 : assign display = 7'b1100111;
  5'h11 : assign display = 7'b1010000;
  5'h12 : assign display = 7'b1101101;
  5'h13 : assign display = 7'b1000110;
  5'h14 : assign display = 7'b0111110;
  5'h15 : assign display = 7'b0011100;
  5'h16 : assign display = 7'b0101010;
  5'h17 : assign display = 7'b1001001;
  5'h18 : assign display = 7'b1101110;
  5'h19 : assign display = 7'b1011011;
      default : assign display = 7'h40;
    endcase

    assign g= display[6];
    assign f= display[5];
    assign e= display[4];
    assign d= display[3];
    assign c= display[2];
    assign b= display[1];
    assign a= display[0];
  end

endmodule