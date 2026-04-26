APP        = ppong
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib

include ../common.mk

all: ppong.gfx

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

clean: clean-ppong-assets

clean-ppong-assets:
	rm -rf assets res.h ppong.gfx
