/**
 * ay.h - AY/YM sound helper API.
 */

#ifndef _SPRINTER_AY_H
#define _SPRINTER_AY_H

#include <sprinter/types.h>

/**
 * Initialize a PT3 player image loaded into the first page of a DSS memory block.
 *
 * The page must contain a PT3 player assembled for 0xC000 followed immediately
 * by the PT3 module data. The SDK maps the block to WIN3 temporarily and calls
 * the player's START entry.
 *
 * Returns 0 on success, or DSS SETWIN error code if the page cannot be mapped.
 */
u8 ay_pt3_init(u8 block) SPRINTER_NAKED_DECL;

/**
 * Play one PT3 frame/quark. Call once per video frame, typically right after
 * video_vsync() or from a frame interrupt handler.
 *
 * Returns 0 on success, or DSS SETWIN error code if the page cannot be mapped.
 */
u8 ay_pt3_play(u8 block) SPRINTER_NAKED_DECL;

/**
 * Mute the PT3 player/AY channels before pause, stop, or freeing the music page.
 *
 * Returns 0 on success, or DSS SETWIN error code if the page cannot be mapped.
 */
u8 ay_pt3_mute(u8 block) SPRINTER_NAKED_DECL;

#endif /* _SPRINTER_AY_H */
