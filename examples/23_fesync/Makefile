APP      = fesync
SDK_DIR ?= ../../

include $(SDK_DIR)toolchain.mk

BUILD    = _build
CODE_LOC ?= 0x4100
STACK    ?= 0xBFFF
CRT0     = $(SDK_DIR)build/crt0.rel

.PHONY: all clean

all: $(APP).exe

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/main.rel: main.s | $(BUILD)
	$(SDASZ80) $(SDASZ_FLAGS) $@ $<
	cp $@ $(basename $@).o

$(BUILD)/$(APP).ihx: $(CRT0) $(BUILD)/main.rel
	printf '%s\n' '-mjx' > $(BUILD)/$(APP).lk
	printf '%s\n' '-i $@' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/$(APP).lk
	printf '%s\n' '$(abspath $(CRT0))' >> $(BUILD)/$(APP).lk
	printf '%s\n' '$(BUILD)/main.rel' >> $(BUILD)/$(APP).lk
	printf '%s\n' '-e' >> $(BUILD)/$(APP).lk
	$(SDLDZ80) -n -f $(BUILD)/$(APP).lk

$(APP).exe: $(BUILD)/$(APP).ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean:
	rm -rf $(BUILD) $(APP).exe
