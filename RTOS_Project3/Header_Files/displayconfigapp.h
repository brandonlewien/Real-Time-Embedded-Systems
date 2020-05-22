/**************************************************************************//**
 * @file displayconfig.h
 * @brief LCD Display config file
 * @author Brandon Lewien
 * @version 1.00
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2019 Brandon Lewien
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Labs has no
 * obligation to support this Software. Silicon Labs is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Brandon Lewien will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************/

#ifndef __DISPLAYCONFIGAPP_H__
#define __DISPLAYCONFIGAPP_H__

/* Include TEXTDISPLAY support because this example prints text to the
   display. */
#define INCLUDE_TEXTDISPLAY_SUPPORT

/* Select one of the fonts listed below:
   TEXTDISPLAY_FONT_6x8
   TEXTDISPLAY_FONT_8x8
   TEXTDISPLAY_NUMBER_FONT_16x20
 */
#define  TEXTDISPLAY_FONT_8x8

/* Enable or disable scroll mode on the text display. */
#define RETARGETTEXTDISPLAY_SCROLL_MODE  (false)

/* Enable or disable adding Carriage Return (CR) to Line Feed (LF) characters
   on the text display. */
#define RETARGETTEXTDISPLAY_LINE_FEED_MODE  (true)

/* Include pixel matrix allocation support. */
#define PIXEL_MATRIX_ALLOC_SUPPORT

/* Enable allocation of pixel matrices from the static pixel matrix pool.
   NOTE:
   The allocator does not support free'ing pixel matrices. It allocates
   continuosly from the static pool without keeping track of the sizes of
   old allocations. I.e. this is a one-shot allocator, and the  user should
   allocate buffers once at the beginning of the program.
 */
#define USE_STATIC_PIXEL_MATRIX_POOL

/* Specify the size of the static pixel matrix pool. For the clock demo
   we need one pixel matrix (framebuffer) covering the whole display.
 */
#define PIXEL_MATRIX_POOL_SIZE   ( (DISPLAY0_HEIGHT * DISPLAY0_WIDTH / 8) \
                                   + (FONT_HEIGHT * DISPLAY0_WIDTH / 8) )

#endif /* __DISPLAYCONFIGAPP_H__ */
