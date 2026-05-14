APP        = balls
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
PACK_ASSETS ?= 0

PIC1_BMP   = /Users/dmitry/dev/zx/sprinter/evo_sdk/example_slideshow/gfx/pic1.bmp
BALLS_BMP  = /Users/dmitry/dev/zx/sprinter/evo_sdk/example_sprites/balls.bmp

include ../common.mk

all: balls.gfx

GFX_RAW = $(BUILD)/balls.raw

$(BUILD)/main.rel: res.h

res.h $(GFX_RAW): $(SDK_DIR)tools/bmp2gfx.py $(PIC1_BMP) $(BALLS_BMP) | $(BUILD)
	$(PYTHON) $(SDK_DIR)tools/bmp2gfx.py \
		--out $(GFX_RAW) \
		--header res.h \
		--name balls_demo \
		--palette $(PIC1_BMP) \
		--image $(PIC1_BMP) \
		--sheet $(BALLS_BMP):16x16 \
		--sheet-transparent-index auto

ifeq ($(PACK_ASSETS),1)
balls.gfx: $(GFX_RAW) $(SDK_DIR)tools/pack_asset.py
	$(PYTHON) $(SDK_DIR)tools/pack_asset.py $< $@
else
balls.gfx: $(GFX_RAW)
	cp $< $@
endif

clean: clean-balls-assets

clean-balls-assets:
	rm -f res.h balls.gfx
