APP  = waitemm
SRCS = main.c

include ../common.mk

all: wchild.exe

$(BUILD)/child.rel: child.c | $(BUILD)
	$(SDCC) $(SDCC_FLAGS) $(INC) -c -o $@ $<

$(BUILD)/wchild.ihx: $(CRT0) $(SPRLIB) $(BUILD)/child.rel
	$(SDCC) $(SDCC_TARGET) --no-std-crt0 --code-loc $(CODE_LOC) \
		--max-allocs-per-node 5000 --opt-code-speed \
		$(INC) \
		$(CRT0) $(BUILD)/child.rel \
		-l$(SPRLIB) \
		-o $@

wchild.exe: $(BUILD)/wchild.ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean: clean-wchild

clean-wchild:
	rm -f wchild.exe
