APP        = multiimg
SRCS       = main.c
EXTRA_LIBS = $(SDK_DIR)build/gfx.lib

include ../common.mk

GFX_FILES = splash.gfx menu.gfx loading.gfx won.gfx over.gfx

all: $(GFX_FILES)

$(BUILD)/main.rel: res.h

res.h $(GFX_FILES): make_assets.py | $(BUILD)
	$(PYTHON) make_assets.py

clean: clean-multi-assets

clean-multi-assets:
	rm -f res.h $(GFX_FILES)
