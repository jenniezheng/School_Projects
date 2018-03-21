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
5'h0 : assign a = 1'b1;
5'h1 : assign a = 1'b1;
5'h2 : assign a = 1'b1;
5'h3 : assign a = 1'b1;
5'h4 : assign a = 1'b1;
5'h5 : assign a = 1'b1;
5'h6 : assign a = 1'b1;
5'h7 : assign a = 1'b1;
5'h8 : assign a = 1'b0;
5'h9 : assign a = 1'b0;
5'ha : assign a = 1'b1;
5'hb : assign a = 1'b0;
5'hc : assign a = 1'b0;
5'hd : assign a = 1'b1;
5'he : assign a = 1'b1;
5'hf : assign a = 1'b1;
5'h10 : assign a = 1'b1;
5'h11 : assign a = 1'b1;
5'h12 : assign a = 1'b1;
5'h13 : assign a = 1'b1;
5'h14 : assign a = 1'b0;
5'h15 : assign a = 1'b0;
5'h16 : assign a = 1'b0;
5'h17 : assign a = 1'b1;
5'h18 : assign a = 1'b1;
5'h19 : assign a = 1'b1;
default : assign a = 1'b1;
endcase

case (letter)
5'h0 : assign b = 1'b1;
5'h1 : assign b = 1'b1;
5'h2 : assign b = 1'b0;
5'h3 : assign b = 1'b0;
5'h4 : assign b = 1'b1;
5'h5 : assign b = 1'b1;
5'h6 : assign b = 1'b1;
5'h7 : assign b = 1'b1;
5'h8 : assign b = 1'b0;
5'h9 : assign b = 1'b0;
5'ha : assign b = 1'b1;
5'hb : assign b = 1'b1;
5'hc : assign b = 1'b0;
5'hd : assign b = 1'b0;
5'he : assign b = 1'b0;
5'hf : assign b = 1'b1;
5'h10 : assign b = 1'b1;
5'h11 : assign b = 1'b0;
5'h12 : assign b = 1'b1;
5'h13 : assign b = 1'b0;
5'h14 : assign b = 1'b1;
5'h15 : assign b = 1'b0;
5'h16 : assign b = 1'b1;
5'h17 : assign b = 1'b0;
5'h18 : assign b = 1'b1;
5'h19 : assign b = 1'b0;
default : assign b = 1'b0;
endcase

case (letter)
5'h0 : assign c = 1'b1;
5'h1 : assign c = 1'b1;
5'h2 : assign c = 1'b1;
5'h3 : assign c = 1'b1;
5'h4 : assign c = 1'b1;
5'h5 : assign c = 1'b1;
5'h6 : assign c = 1'b0;
5'h7 : assign c = 1'b1;
5'h8 : assign c = 1'b0;
5'h9 : assign c = 1'b1;
5'ha : assign c = 1'b1;
5'hb : assign c = 1'b1;
5'hc : assign c = 1'b1;
5'hd : assign c = 1'b1;
5'he : assign c = 1'b1;
5'hf : assign c = 1'b1;
5'h10 : assign c = 1'b0;
5'h11 : assign c = 1'b1;
5'h12 : assign c = 1'b0;
5'h13 : assign c = 1'b0;
5'h14 : assign c = 1'b1;
5'h15 : assign c = 1'b1;
5'h16 : assign c = 1'b0;
5'h17 : assign c = 1'b0;
5'h18 : assign c = 1'b0;
5'h19 : assign c = 1'b1;
default : assign c = 1'b0;
endcase

case (letter)
5'h0 : assign d = 1'b0;
5'h1 : assign d = 1'b1;
5'h2 : assign d = 1'b1;
5'h3 : assign d = 1'b1;
5'h4 : assign d = 1'b1;
5'h5 : assign d = 1'b0;
5'h6 : assign d = 1'b1;
5'h7 : assign d = 1'b0;
5'h8 : assign d = 1'b0;
5'h9 : assign d = 1'b1;
5'ha : assign d = 1'b1;
5'hb : assign d = 1'b1;
5'hc : assign d = 1'b0;
5'hd : assign d = 1'b0;
5'he : assign d = 1'b1;
5'hf : assign d = 1'b0;
5'h10 : assign d = 1'b0;
5'h11 : assign d = 1'b0;
5'h12 : assign d = 1'b1;
5'h13 : assign d = 1'b0;
5'h14 : assign d = 1'b1;
5'h15 : assign d = 1'b1;
5'h16 : assign d = 1'b1;
5'h17 : assign d = 1'b1;
5'h18 : assign d = 1'b1;
5'h19 : assign d = 1'b1;
default : assign d = 1'b0;
endcase

case (letter)
5'h0 : assign e = 1'b1;
5'h1 : assign e = 1'b1;
5'h2 : assign e = 1'b0;
5'h3 : assign e = 1'b1;
5'h4 : assign e = 1'b0;
5'h5 : assign e = 1'b0;
5'h6 : assign e = 1'b1;
5'h7 : assign e = 1'b1;
5'h8 : assign e = 1'b1;
5'h9 : assign e = 1'b1;
5'ha : assign e = 1'b0;
5'hb : assign e = 1'b0;
5'hc : assign e = 1'b1;
5'hd : assign e = 1'b1;
5'he : assign e = 1'b1;
5'hf : assign e = 1'b0;
5'h10 : assign e = 1'b1;
5'h11 : assign e = 1'b0;
5'h12 : assign e = 1'b1;
5'h13 : assign e = 1'b1;
5'h14 : assign e = 1'b1;
5'h15 : assign e = 1'b1;
5'h16 : assign e = 1'b0;
5'h17 : assign e = 1'b0;
5'h18 : assign e = 1'b1;
5'h19 : assign e = 1'b0;
default : assign e = 1'b0;
endcase

case (letter)
5'h0 : assign f = 1'b1;
5'h1 : assign f = 1'b0;
5'h2 : assign f = 1'b0;
5'h3 : assign f = 1'b1;
5'h4 : assign f = 1'b0;
5'h5 : assign f = 1'b0;
5'h6 : assign f = 1'b1;
5'h7 : assign f = 1'b1;
5'h8 : assign f = 1'b1;
5'h9 : assign f = 1'b1;
5'ha : assign f = 1'b0;
5'hb : assign f = 1'b0;
5'hc : assign f = 1'b0;
5'hd : assign f = 1'b0;
5'he : assign f = 1'b0;
5'hf : assign f = 1'b1;
5'h10 : assign f = 1'b1;
5'h11 : assign f = 1'b0;
5'h12 : assign f = 1'b0;
5'h13 : assign f = 1'b1;
5'h14 : assign f = 1'b1;
5'h15 : assign f = 1'b0;
5'h16 : assign f = 1'b1;
5'h17 : assign f = 1'b0;
5'h18 : assign f = 1'b1;
5'h19 : assign f = 1'b1;
default : assign f = 1'b0;
endcase

case (letter)
5'h0 : assign g = 1'b1;
5'h1 : assign g = 1'b0;
5'h2 : assign g = 1'b0;
5'h3 : assign g = 1'b0;
5'h4 : assign g = 1'b1;
5'h5 : assign g = 1'b1;
5'h6 : assign g = 1'b1;
5'h7 : assign g = 1'b0;
5'h8 : assign g = 1'b0;
5'h9 : assign g = 1'b0;
5'ha : assign g = 1'b0;
5'hb : assign g = 1'b0;
5'hc : assign g = 1'b1;
5'hd : assign g = 1'b0;
5'he : assign g = 1'b0;
5'hf : assign g = 1'b1;
5'h10 : assign g = 1'b1;
5'h11 : assign g = 1'b0;
5'h12 : assign g = 1'b1;
5'h13 : assign g = 1'b0;
5'h14 : assign g = 1'b0;
5'h15 : assign g = 1'b0;
5'h16 : assign g = 1'b0;
5'h17 : assign g = 1'b1;
5'h18 : assign g = 1'b0;
5'h19 : assign g = 1'b1;
default : assign g = 1'b0;
endcase

  end

endmodule