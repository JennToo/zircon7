BITSTREAM     := build/zircon7_ulx3s.bit
ROUTED_CONFIG := build/ulx3s_out.config
SYNTH_JSON    := build/ulx3s.json

SRCS := $(shell find src -name '*.sv')

.PHONY: all
all: $(BITSTREAM)

.PHONY: clean
clean:
	rm -rf build

.PHONY: prog
prog: $(BITSTREAM)
	fujprog $(BITSTREAM)

.PHONY: check
check:
	svlint $(SRCS)
	verilator --lint-only -Wall $(SRCS)

$(BITSTREAM): $(ROUTED_CONFIG)
	ecppack $(ROUTED_CONFIG) $@

$(ROUTED_CONFIG): $(SYNTH_JSON)
	nextpnr-ecp5 \
	  --85k \
	  --json $(SYNTH_JSON) \
	  --lpf ulx3s_v20.lpf \
	  --textcfg $(ROUTED_CONFIG) \
	  --package CABGA381

$(SYNTH_JSON): zircon7.ys $(SRCS)
	mkdir -p $(@D)
	yosys $<
