// Code your design here
`timescale 1ns / 1ps
module speed_fsm (speed, clk, rst, a, b);
  //Inputs, Outputs, and Regs
  output [1:0] speed;
  input clk, rst, a,b;
  reg [1:0] state, next_state;
  
  //Parameters
  parameter stop=2'b00, slow=2'b01, med=2'b10, high=2'b11;
  
  //State logic
  always @ (state or a or b)
    begin
      case (state)
        stop: begin
          if (b==1'b1) next_state=stop;
          else if (a==1'b1) next_state=slow;
          else next_state=state;          
        end
        slow: begin
          if (b==1'b1) next_state=stop;
          else if (a==1'b1) next_state=med;
          else next_state=state;          
        end
        med: begin
          if (b==1'b1) next_state=slow;
          else if (a==1'b1) next_state=high;
          else next_state=state;          
        end
        high: begin
          if (b==1'b1) next_state=med;
          else if (a==1'b1) next_state=high;
          else next_state=state;          
        end 
      endcase
    end
  
  //Output Logic
  assign speed=state;
  
  
  
  //State registers
  always @ (posedge clk or posedge rst)
    begin
      if(rst) 
        state<=stop;
      else 
        state<=next_state;
    end
endmodule


module light_fsm (clk, rst, car_ew, lights);
  //Inputs, Outputs, and Regs
  output [5:0] lights;
  input clk, rst, car_ew;
  reg [1:0] state, next_state;
  reg [5:0] lights;
  
  //Parameters
  parameter green_ns=2'b00, yellow_ns=2'b01, green_ew=2'b10, yellow_ew=2'b11;
  parameter l_g_ns=6'b100_001, l_y_ns=6'b010_001, l_g_ew=6'b001_100, l_y_ew=6'b000_010;
  
  //State logic
  always @ (state or car_ew)
    begin
      case (state)
        green_ns: begin
          if (car_ew==1'b1) next_state=yellow_ns;
          else next_state=state;          
        end
        yellow_ns: begin
          next_state=green_ew;      
        end
        green_ew: begin
          next_state=yellow_ew;
        end
        yellow_ew: begin
          next_state=green_ns;          
        end 
      endcase
    end
  
  //Output Logic
  always @ (state)
    begin
      case (state)
        green_ns: begin
          lights=l_g_ns;          
        end
        yellow_ns: begin
          lights=l_y_ns;      
        end
        green_ew: begin
          lights=l_g_ew;
        end
        yellow_ew: begin
          lights=l_y_ew;          
        end 
      endcase
    end 
  
  
  
  //State registers
  always @ (posedge clk or posedge rst)
    begin
      if(rst) 
        state<=green_ns;
      else 
        state<=next_state;
    end
endmodule