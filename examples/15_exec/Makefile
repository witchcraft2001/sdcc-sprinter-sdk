APP  = exec
SRCS = main.c
include ../common.mk

all: child.exe

$(BUILD)/child.rel: child.c | $(BUILD)
	$(SDCPP) $(APP_CPPFLAGS) $< > $(basename $@).i
	$(SDCC) $(APP_CFLAGS) --c1mode -o $(basename $@).asm < $(basename $@).i
	$(SDASZ80) -plosgff $@ $(basename $@).asm
	cp $@ $(basename $@).o

$(BUILD)/child.ihx: $(CRT0) $(SPRLIB) $(BUILD)/child.rel
	printf '%s\n' '-mjwx' > $(BUILD)/child.lk
	printf '%s\n' '-i $@' >> $(BUILD)/child.lk
	printf '%s\n' '-b _CODE = $(CODE_LOC)' >> $(BUILD)/child.lk
	if [ -n "$(DATA_LOC)" ]; then printf '%s\n' '-b _DATA = $(DATA_LOC)' >> $(BUILD)/child.lk; fi
	printf '%s\n' '-l $(abspath $(SPRLIB))' >> $(BUILD)/child.lk
	printf '%s\n' '$(abspath $(CRT0_LINK))' >> $(BUILD)/child.lk
	printf '%s\n' '$(BUILD)/child.rel' >> $(BUILD)/child.lk
	printf '%s\n' '-e' >> $(BUILD)/child.lk
	$(SDLDZ80) -nf $(BUILD)/child.lk

child.exe: $(BUILD)/child.ihx
	$(PYTHON) $(SDK_DIR)tools/ihx2exe.py $< $@ --load $(CODE_LOC) --entry $(CODE_LOC) --stack $(STACK)
	@echo "Built: $@"

clean: clean-child

clean-child:
	rm -f child.exe
