APP        = balls
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib

PIC1_BMP   = /Users/dmitry/dev/zx/sprinter/evo_sdk/example_slideshow/gfx/pic1.bmp
BALLS_BMP  = /Users/dmitry/dev/zx/sprinter/evo_sdk/example_sprites/balls.bmp

include ../common.mk

$(BUILD)/main.rel: res.h

res.h balls.gfx: $(SDK_DIR)tools/bmp2gfx.py $(PIC1_BMP) $(BALLS_BMP)
	$(PYTHON) $(SDK_DIR)tools/bmp2gfx.py \
		--out balls.gfx \
		--header res.h \
		--name balls_demo \
		--palette $(PIC1_BMP) \
		--image $(PIC1_BMP) \
		--sheet $(BALLS_BMP):16x16 \
		--sheet-transparent-index auto

clean: clean-balls-assets

clean-balls-assets:
	rm -f res.h balls.gfx
