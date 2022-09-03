MCU = atmega32u4
F_CPU = 8000000
ARCH = AVR8
F_USB = $(F_CPU)

# Bootloader selection
BOOTLOADER = caterina

# Build Options
#   change yes to no to disable
#
BLUETOOTH_ENABLE = yes
BLUETOOTH_DRIVER = BluefruitLE

BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = no            # Enable N-Key Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output

CUSTOM_MATRIX = lite        # We have a custom matrix
SRC = matrix.c
QUANTUM_LIB_SRC += i2c_master.c

LAYOUTS = tkl_ansi
