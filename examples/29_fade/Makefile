APP        = fade
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
PACK_ASSETS ?= 0
SJASMPLUS  ?= $(SDK_DIR)tools/bin/sjasmplus
MUSIC_DIR  ?= ../24_ppong/music
PT3_PLAYER ?= $(MUSIC_DIR)/pt3play.asm
PT3_MODULE ?= $(MUSIC_DIR)/mus1.pt3

include ../common.mk

all: fade.gfx fade.pt3

GFX_RAW = $(BUILD)/fade.gfx.raw
PT3_RAW = $(BUILD)/fade.pt3.raw

$(BUILD)/main.rel: res.h

res.h fade.gfx: make_assets.py | $(BUILD)
	$(PYTHON) make_assets.py

$(PT3_RAW): $(PT3_PLAYER) $(PT3_MODULE) | $(BUILD)
	printf '%s\n' '        device zxspectrum128' > $(BUILD)/fade_pt3.asm
	printf '%s\n' '        org #C000' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' 'PlayerStart:' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' '        include "$(PT3_PLAYER)"' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' 'MusicModule:' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' '        incbin "$(PT3_MODULE)"' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' 'PlayerEnd:' >> $(BUILD)/fade_pt3.asm
	printf '%s\n' '        savebin "$(PT3_RAW)",PlayerStart,PlayerEnd-PlayerStart' >> $(BUILD)/fade_pt3.asm
	$(SJASMPLUS) $(BUILD)/fade_pt3.asm

ifeq ($(PACK_ASSETS),1)
fade.pt3: $(PT3_RAW) $(SDK_DIR)tools/pack_asset.py
	$(PYTHON) $(SDK_DIR)tools/pack_asset.py $< $@
else
fade.pt3: $(PT3_RAW)
	cp $< $@
endif

clean: clean-fade-assets

clean-fade-assets:
	rm -f res.h fade.gfx fade.pt3
