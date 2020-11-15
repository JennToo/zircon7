BITSTREAM     := build/zircon7_ulx3s.bit
ROUTED_CONFIG := build/ulx3s_out.config
SYNTH_JSON    := build/ulx3s.json
YOSYS_SCRIPT  := build/ulx3s.ys

SRCS := $(shell find src -name '*.sv')

COMMON_LIB_SRCS := $(shell find sim/common -name '*.cpp')
COMMON_LIB_OBJS := $(COMMON_LIB_SRCS:sim/common/%.cpp=build/common/%.o)
COMMON_LIB_ARCH := build/common/common.a

SIM_DIRS := $(filter-out common,$(shell ls sim/))
SIM_TARGETS := $(addprefix sim-,$(SIM_DIRS))

export CXXFLAGS := -I $(abspath sim/common) -std=c++20 -flto
export LDFLAGS := -flto $(abspath $(COMMON_LIB_ARCH))
export CXX := g++-10
export CC := gcc-10
export LD := g++-10
export VERILATOR_FLAGS := \
	-I$(abspath src/util) \
	-I$(abspath src/uart) \
	--compiler clang

.PHONY: all
all: $(BITSTREAM) $(SIM_TARGETS)

.PHONY: clean
clean:
	rm -rf build

.PHONY: prog
prog: $(BITSTREAM)
	fujprog $(BITSTREAM)

.PHONY: check
check: $(BITSTREAM)
	./scripts/lint
	./scripts/format --check

.PHONY: format
format:
	./scripts/format

.PHONY: sim
sim: $(SIM_TARGETS)

$(BITSTREAM): $(ROUTED_CONFIG)
	ecppack $(ROUTED_CONFIG) $@

$(ROUTED_CONFIG): $(SYNTH_JSON)
	nextpnr-ecp5 \
	  --85k \
	  --json $(SYNTH_JSON) \
	  --lpf ulx3s_v20.lpf \
	  --textcfg $(ROUTED_CONFIG) \
	  --package CABGA381 \
	  --top top

$(SYNTH_JSON): $(YOSYS_SCRIPT) $(SRCS)
	mkdir -p $(@D)
	yosys $<

$(YOSYS_SCRIPT): $(SRCS)
	./scripts/generate_yosys

$(SIM_TARGETS): sim-%: $(COMMON_LIB_ARCH)
	+./scripts/verilate_module $*

$(COMMON_LIB_OBJS): build/common/%.o: sim/common/%.cpp
	mkdir -p $(@D)
	rm -rf build/sim
	$(CXX) $$(pkg-config --cflags verilator) $(CXXFLAGS) -o $@ -c $<

$(COMMON_LIB_ARCH): $(COMMON_LIB_OBJS)
	ar rcs $@ $^
