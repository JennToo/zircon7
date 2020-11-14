module top
(
    input var clk_25mhz,
    input var [6:0] btn,
    output var [7:0] led,
    output var wifi_gpio0,

    input var ftdi_txd,
    output var ftdi_rxd,

    output var [27:0] gp,
    output var [27:0] gn
);
    // Prevents reset
    assign wifi_gpio0 = 1'b1;

    always_comb begin
        led[6:0] = btn[6:0];
        led[7] = !ftdi_txd;
    end

    wire [7:0] foo [0:11];
    assign foo[0]  = "H";
    assign foo[1]  = "e";
    assign foo[2]  = "l";
    assign foo[3]  = "l";
    assign foo[4]  = "o";
    assign foo[5]  = " ";
    assign foo[6]  = "T";
    assign foo[7]  = "h";
    assign foo[8]  = "e";
    assign foo[9]  = "r";
    assign foo[10] = "e";
    assign foo[11] = "!";
    logic been_triggered = 0;
    logic [3:0] index = 0;
    wire wants_data;
    logic [7:0] new_data;
    always_ff @(posedge clk_25mhz) begin
        if (btn[6]) begin
            been_triggered <= 1;
            index <= 0;
        end else if (wants_data && been_triggered) begin
            index <= index + 1;
            new_data <= foo[index];
        end
    end


    uart_transmitter uart_tx(
        .clk(clk_25mhz),
        .have_new_data(been_triggered),
        .new_data(new_data),
        .uart_tx(gn[21]),
        .ready_for_new_data(wants_data)
    );
endmodule
