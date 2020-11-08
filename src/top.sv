module top(
    input var [6:0] btn,
    output var [7:0] led,
    output var wifi_gpio0
);
    // Prevents reset
    assign wifi_gpio0 = 1'b1;

    always_comb begin
        led[6:0] = btn[6:0];
        led[7] = 1;
    end
endmodule
