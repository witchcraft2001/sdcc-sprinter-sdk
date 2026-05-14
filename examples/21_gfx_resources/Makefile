APP        = gfxres
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib
PACK_ASSETS ?= 0

include ../common.mk

GFX_RAW = $(BUILD)/gfxdemo.raw

all: gfxdemo.gfx

$(BUILD)/main.rel: res.h

res.h $(GFX_RAW): make_assets.py $(SDK_DIR)tools/png2gfx.py | $(BUILD)
	$(PYTHON) make_assets.py
	$(PYTHON) $(SDK_DIR)tools/png2gfx.py --out $(GFX_RAW) --header res.h --name demo_gfx assets/sprite16.png assets/sprite24.png

ifeq ($(PACK_ASSETS),1)
gfxdemo.gfx: $(GFX_RAW) $(SDK_DIR)tools/pack_asset.py
	$(PYTHON) $(SDK_DIR)tools/pack_asset.py $< $@
else
gfxdemo.gfx: $(GFX_RAW)
	cp $< $@
endif

clean: clean-gfx-assets

clean-gfx-assets:
	rm -rf assets res.h gfxdemo.gfx
