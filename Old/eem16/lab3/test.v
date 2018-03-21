module tb();
    integer file;
    integer scan_faults;
    reg [198:0] data; // needs to be an "integral number of bytes in length"\
    reg [100:0] bits;

    initial begin
      file = $fopen("data_file.txt", "r");
      if (file == 0) begin
        $display("file handle was NULL");
        $finish;
      end
      while (!$feof(file)) begin
        scan_faults = $fgets(data, file); // arg order was wrong in original


        $display("line: %s", data);
      end
      $fclose(file); // Close file before finish
      $finish;
    end
endmodule