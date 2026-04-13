/**
 * types.h — Basic type definitions for ZX Sprinter SDK
 *
 * Standard integer types for Z80 / SDCC.
 */

#ifndef _SPRINTER_TYPES_H
#define _SPRINTER_TYPES_H

/* Fixed-width integer types */
typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned int        uint16_t;
typedef signed int          int16_t;
typedef unsigned long       uint32_t;
typedef signed long         int32_t;

/* Short aliases */
typedef unsigned char       u8;
typedef signed char         i8;
typedef unsigned int        u16;
typedef signed int          i16;
typedef unsigned long       u32;
typedef signed long         i32;

/* Standard definitions */
#include <stddef.h>
#include <stdbool.h>

/* Classic types */
typedef unsigned char       byte;
typedef unsigned int        word;

#endif /* _SPRINTER_TYPES_H */
