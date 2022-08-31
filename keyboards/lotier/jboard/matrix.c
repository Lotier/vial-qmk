#include <stdint.h>
#include <stdbool.h>
#include "wait.h"
#include "util.h"
#include "matrix.h"
#include "quantum.h"
#include "i2c_master.h"
#include "print.h"

#define SX1509_ADDR 0x3E << 1 // Configurable with jumpers
#define SX1509_TIMEOUT 200 // milliseconds

#define DATAA 0x11
#define DATAB 0x10
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
static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;

/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values

static void select_row(uint8_t row) {
    setPinOutput(row_pins[row]);
    writePinLow(row_pins[row]);
}

static void unselect_row(uint8_t row) { setPinInputHigh(row_pins[row]); }

static bool read_cols_on_row(matrix_row_t current_matrix[], uint8_t current_row) {
    // Store last value of row prior to reading
    matrix_row_t last_row_value = current_matrix[current_row];

    // Clear data in matrix row
    current_matrix[current_row] = 0;

    uint8_t buf[2];

    uint8_t data = DATAB;

    // Select row and wait for row selecton to stabilize
    select_row(current_row);
    wait_us(30);

    // For each col...
    // for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
    //     // Select the col pin to read (active low)
    //     uint8_t pin_state = readPin(col_pins[col_index]);

    //     // Populate the matrix row with the state of the col pin
    //     current_matrix[current_row] |= pin_state ? 0 : (MATRIX_ROW_SHIFTER << col_index);
    // }
    sx1509_status = i2c_start(SX1509_ADDR, SX1509_TIMEOUT);
    if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting start"); }
    sx1509_status = i2c_transmit(SX1509_ADDR, &data, 1, SX1509_TIMEOUT);
    if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting transmit"); }
    sx1509_status = i2c_receive(SX1509_ADDR, buf, 2, SX1509_TIMEOUT);
    if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting receive"); }
    i2c_stop();

    // Unselect row
    unselect_row(current_row);

    current_matrix[current_row] = ~((buf[0] << 8) | buf[1]);

    return (last_row_value != current_matrix[current_row]);
}

void matrix_init_custom(void) {
  uint8_t reset[] = {0x12, 0x34};
  uint8_t allOn = 0xff;
  // Software reset
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegReset, &reset[0], 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting reset 1"); }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegReset, &reset[1], 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting reset 2"); }

  // Set all the pins as inputs
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegDirA, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting writing 1"); }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegDirB, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting writing 2"); }


  // Turn on internal pull-ups
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegPullUpA, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting pullup 1"); }
  sx1509_status = i2c_writeReg(SX1509_ADDR, RegPullUpB, &allOn, 1, SX1509_TIMEOUT);
  if (sx1509_status != I2C_STATUS_SUCCESS) { print("error setting pullup 1"); }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;

    // Set row, read cols
    for (uint8_t current_row = 0; current_row < MATRIX_ROWS; current_row++) {
        matrix_has_changed |= read_cols_on_row(raw_matrix, current_row);
    }

    return matrix_has_changed;
}