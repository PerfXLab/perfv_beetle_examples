#ifndef __SETUP_H__
#define __SETUP_H__

#define SILENT
#ifdef SILENT
#define PRINTF(...) ((void) 0)
#else
#define PRINTF printf
#endif  /* DEBUG */

#if defined(USE_BRIDGE)
#include "bridge_stubs.h"
#else
#define HAVE_DISPLAY
#endif  /* USE_BRIDGE */

#define USE_UDMA_FOR_DISPLAY
#define DISPLAY_SIZE_320X240

#if defined(DISPLAY_SIZE_320X240)
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#else
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 160 )
#endif  /* QVGA */

#define LCD_WIDTH     ( 80 )
#define LCD_HEIGHT    ( 160 )

#endif /* __SETUP_H__ */
