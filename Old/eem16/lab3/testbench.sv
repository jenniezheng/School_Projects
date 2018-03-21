// Code your testbench here
// or browse Examples
`timescale 1ns / 1ps

module light_tb();
  //registers and wires
  
  reg clk, rst, car_ew;
  
  wire [5:0] lights;
  
  //modules
  light_fsm l1(clk, rst, car_ew, lights);
  
  //clock
  initial clk = 1'b0;
  always #5 clk = ~clk;
  
  //stimulus
  initial begin
    $monitor("rst=%d, car_ew=%d, lights=%b, state=%d, next_state=%d",rst,car_ew,lights,l1.state, l1.next_state);
    
    rst=1'b0;
    car_ew=1'b0;
    #2
    rst=1'b1;

    #20    
    rst=1'b0;
    car_ew=1'b1;
    
    #50
    car_ew=1'b0;


   
    #100 
    $finish;


    

  end
endmodule
/*
module speed_tb();
  //registers and wires
  
  reg clk, rst;
  reg a;
  reg b;
  
  wire [1:0] speed;
  
  //modules
  speed_fsm f1(.speed(speed), .clk (clk), .rst(rst), .a(a), .b(b));
  
  //clock
  initial clk = 1'b0;
  always #5 clk = ~clk;
  
  //stimulus
  initial begin
	$monitor("rst=%d, a=%d, b=%d, current_speed=%d, next_speed=%d",rst,a,b,speed, f1.next_state);
    rst=1'b0;
    a=1'b0;
    b=1'b0;
    #2
    rst=1;

    #20    
    rst=1'b0;
    a=1'b1;
    b=1'b0;
    
    
    
    #10
    a=1'b0;
    b=1'b0;

    
    #10
    a=1'b0;
    b=1'b1;

   
    #100 
    $finish;


    

  end
endmodule
*/