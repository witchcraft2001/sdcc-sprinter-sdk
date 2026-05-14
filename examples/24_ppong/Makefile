APP        = ppong
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
PACK_ASSETS ?= 0
SJASMPLUS ?= $(SDK_DIR)tools/bin/sjasmplus
MUSIC_DIR  ?= music
PT3_PLAYER ?= $(MUSIC_DIR)/pt3play.asm
PT3_MODULE ?= $(MUSIC_DIR)/mus1.pt3

include ../common.mk

all: ppong.gfx ppong.pt3

GFX_RAW = $(BUILD)/ppong.gfx.raw
PT3_RAW = $(BUILD)/ppong.pt3.raw

$(BUILD)/main.rel: res.h

res.h $(GFX_RAW): make_assets.py $(SDK_DIR)tools/bmp2gfx.py | $(BUILD)
	$(PYTHON) make_assets.py
	$(PYTHON) $(SDK_DIR)tools/bmp2gfx.py \
		--out $(GFX_RAW) \
		--header res.h \
		--name ppong \
		--image assets/bg0.bmp \
		--image assets/bg1.bmp \
		--sheet assets/sprites.bmp:16x16 \
		--sheet-transparent-index 255

ifeq ($(PACK_ASSETS),1)
ppong.gfx: $(GFX_RAW) $(SDK_DIR)tools/pack_asset.py
	$(PYTHON) $(SDK_DIR)tools/pack_asset.py $< $@
else
ppong.gfx: $(GFX_RAW)
	cp $< $@
endif

$(PT3_RAW): $(PT3_PLAYER) $(PT3_MODULE) | $(BUILD)
	printf '%s\n' '        device zxspectrum128' > $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        org #C000' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'PlayerStart:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        include "$(PT3_PLAYER)"' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'MusicModule:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        incbin "$(PT3_MODULE)"' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'PlayerEnd:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        savebin "$(PT3_RAW)",PlayerStart,PlayerEnd-PlayerStart' >> $(BUILD)/ppong_pt3.asm
	$(SJASMPLUS) $(BUILD)/ppong_pt3.asm

ifeq ($(PACK_ASSETS),1)
ppong.pt3: $(PT3_RAW) $(SDK_DIR)tools/pack_asset.py
	$(PYTHON) $(SDK_DIR)tools/pack_asset.py $< $@
else
ppong.pt3: $(PT3_RAW)
	cp $< $@
endif

clean: clean-ppong-assets

clean-ppong-assets:
	rm -rf assets res.h ppong.gfx ppong.pt3
