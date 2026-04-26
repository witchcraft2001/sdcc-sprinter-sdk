APP        = ppong
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
SJASMPLUS ?= $(SDK_DIR)tools/bin/sjasmplus
MUSIC_DIR  ?= music
PT3_PLAYER ?= $(MUSIC_DIR)/pt3play.asm
PT3_MODULE ?= $(MUSIC_DIR)/mus1.pt3

include ../common.mk

all: ppong.gfx ppong.pt3

$(BUILD)/main.rel: res.h

res.h ppong.gfx: make_assets.py $(SDK_DIR)tools/bmp2gfx.py
	$(PYTHON) make_assets.py
	$(PYTHON) $(SDK_DIR)tools/bmp2gfx.py \
		--out ppong.gfx \
		--header res.h \
		--name ppong \
		--image assets/bg0.bmp \
		--image assets/bg1.bmp \
		--sheet assets/sprites.bmp:16x16 \
		--sheet-transparent-index 255

ppong.pt3: $(PT3_PLAYER) $(PT3_MODULE) | $(BUILD)
	printf '%s\n' '        device zxspectrum128' > $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        org #C000' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'PlayerStart:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        include "$(PT3_PLAYER)"' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'MusicModule:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        incbin "$(PT3_MODULE)"' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' 'PlayerEnd:' >> $(BUILD)/ppong_pt3.asm
	printf '%s\n' '        savebin "ppong.pt3",PlayerStart,PlayerEnd-PlayerStart' >> $(BUILD)/ppong_pt3.asm
	$(SJASMPLUS) $(BUILD)/ppong_pt3.asm

clean: clean-ppong-assets

clean-ppong-assets:
	rm -rf assets res.h ppong.gfx ppong.pt3
