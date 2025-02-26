/*Copyright 2022 Lotier

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

#define VENDOR_ID       0x6A62 // Initials (lowercase) in hex
#define PRODUCT_ID      0x4A42 // JB in hex
#define DEVICE_VER      0x0002
#define MANUFACTURER    lotier
#define PRODUCT         JBOARD

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 16

/*
 * Keyboard Matrix Assignments
 *
 * Change this to how you wired your keyboard
 * COLS: AVR pins used for columns, left to right
 * ROWS: AVR pins used for rows, top to bottom
 * DIODE_DIRECTION: COL2ROW = COL = Anode (+), ROW = Cathode (-, marked on diode)
 *                  ROW2COL = ROW = Anode (+), COL = Cathode (-, marked on diode)
 *
 */

#define MATRIX_ROW_PINS {D6, B7, B6, B5, D7, C6}
// COL_PINS are directly controlled by the SX1509.
// #define MATRIX_COL_PINS { }
#define DIODE_DIRECTION COL2ROW

#define BLUEFRUIT_LE_RST_PIN D2
#define BLUEFRUIT_LE_CS_PIN D3
#define BLUEFRUIT_LE_IRQ_PIN F0

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 1

/* disable debug print */
#ifndef NO_DEBUG
    #define NO_DEBUG
#endif

/* disable print */
#ifndef NO_PRINT
    #define NO_PRINT
#endif
