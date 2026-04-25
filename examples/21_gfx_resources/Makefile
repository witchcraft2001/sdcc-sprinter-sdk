APP        = gfxres
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib

include ../common.mk

$(BUILD)/main.rel: res.h

res.h gfxdemo.gfx: make_assets.py $(SDK_DIR)tools/png2gfx.py
	$(PYTHON) make_assets.py
	$(PYTHON) $(SDK_DIR)tools/png2gfx.py --out gfxdemo.gfx --header res.h --name demo_gfx assets/sprite16.png assets/sprite24.png

clean: clean-gfx-assets

clean-gfx-assets:
	rm -rf assets res.h gfxdemo.gfx
