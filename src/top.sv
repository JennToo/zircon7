module top (
    input var  clk_25mhz,
    output var wifi_gpio0
);
  // Prevents reset
  assign wifi_gpio0 = 1'b1;

  logic _unused = {clk_25mhz};
endmodule
