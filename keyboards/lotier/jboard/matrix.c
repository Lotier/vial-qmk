#include <stdint.h>
#include "quantum.h"
#include "i2c_master.h"

#define SX1509_ADDR 0x3E << 1 // Configurable with jumpers
#define SX1509_TIMEOUT 200 // milliseconds

enum sx1509_registers {
  RegReset = 0x7d,
	RegDirA = 0x0f,
  RegDirB = 0x0e,
  RegPullUpA = 0x07,
  RegPullUpB = 0x06,
  DataA = 0x11,
  DataB = 0x10,
};

static i2c_status_t sx1509_status;
static bool sx1509_init = false;
static uint8_t reinit_counter;

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;

static void select_row(uint8_t row) {
  setPinOutput(row_pins[row]);
  writePinLow(row_pins[row]);
}

static void unselect_row(uint8_t row) { 
  setPinInputHigh(row_pins[row]); 
}

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
  // Store last value of row prior to reading
  matrix_row_t last_row_value = current_matrix[current_row];

  if(!sx1509_init) {
    goto done;
  }

  // Clear data in matrix row
  current_matrix[current_row] = 0;

  // Set up for reading SX1509 pin data.
  uint8_t buf[2];
  uint8_t data = DataB;

  // Select row and wait for row selecton to stabilize
  select_row(current_row);
  wait_us(30);

  sx1509_status = i2c_start(SX1509_ADDR, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }
  sx1509_status = i2c_transmit(SX1509_ADDR, &data, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }
  sx1509_status = i2c_receive(SX1509_ADDR, buf, 2, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }

  done:
    i2c_stop();

    // Unselect row
    unselect_row(current_row);

    if (!sx1509_init) {
      dprint("failed to read SX1509, will re-init.\n");
    } else {
      current_matrix[current_row] = ~((buf[0] << 8) | buf[1]);
    }

    return (last_row_value != current_matrix[current_row]);
}

void sx1509_startup(void) {
  uint8_t reset[] = {0x12, 0x34};
  uint8_t allOn = 0xff;

  sx1509_init = false;
  
  // Software reset
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegReset, &reset[0], 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegReset, &reset[1], 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }

  // Set all the pins as inputs
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegDirA, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegDirB, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }

  // Turn on internal pull-ups
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegPullUpA, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegPullUpB, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { sx1509_init = false; goto done; } 

  sx1509_init = true;

  done:
    if (!sx1509_init) {
      dprint("Failed to init SX1509.\n");
    } else {
      dprint("SX1509 initialized!\n");
    }
}

void matrix_init_custom(void) {
  sx1509_startup();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    if (!sx1509_init) {
      // Will be true on every overflow event, aka 255 matrix scans
      if (reinit_counter++ != 0) {
        sx1509_startup();
      }
    }

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
      matrix_has_changed |= read_cols_on_row(current_matrix, current_row);
    }

    dprint("matrix_scan_custom complete\n");

    return matrix_has_changed;
}