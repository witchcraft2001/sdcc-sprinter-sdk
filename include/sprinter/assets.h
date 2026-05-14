/**
 * assets.h - resource file loading helpers.
 */

#ifndef _SPRINTER_ASSETS_H
#define _SPRINTER_ASSETS_H

#include <sprinter/types.h>

#define ASSET_PACK_MAGIC_0  'S'
#define ASSET_PACK_MAGIC_1  'P'
#define ASSET_PACK_MAGIC_2  'K'
#define ASSET_PACK_MAGIC_3  '1'
#define ASSET_PACK_VERSION  1

/** Load a raw or SDK packed resource file into consecutive DSS pages.
 *  Packed resources are produced by tools/pack_asset.py and contain HRUST
 *  compressed 16K chunks. Plain files are loaded page-by-page unchanged.
 *
 *  Returns the number of destination pages touched, or -1 on error.
 */
i16 asset_load_pages(const char *path, u8 block, u8 page_count);

/** Depack one HRUST ZX-header block from src to dst. */
void hrust_depack(const void *src, void *dst) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_ASSETS_H */
