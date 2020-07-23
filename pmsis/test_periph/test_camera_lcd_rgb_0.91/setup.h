#ifndef __SETUP_H__
#define __SETUP_H__

#define SILENT
#ifdef SILENT
#define PRINTF(...) ((void) 0)
#else
#define PRINTF printf
#endif  /* DEBUG */

#define QVGA 1

#if defined(QVGA)
#define CAMERA_WIDTH  ( 320 )
#define CAMERA_HEIGHT ( 240 )
#else
#define CAMERA_WIDTH  ( 160 )
#define CAMERA_HEIGHT ( 120 )
#endif  /* QVGA */

#define LCD_WIDTH     ( 80 )
#define LCD_HEIGHT    ( 160 )

#endif /* __SETUP_H__ */
