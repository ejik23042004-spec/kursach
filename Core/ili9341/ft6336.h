#ifndef _FT6336_H
#define _FT6336_H

#include <stm32l4xx_hal.h>

// Используем I2C1 из main.c
// Пины I2C1: PB8 (SCL), PB9 (SDA) - для Nucleo L452RE
extern I2C_HandleTypeDef hi2c1;

#define FT6336_ADDR               0x38
#define FT6336_I2C_TIMEOUT        100  // таймаут в мс

#define FT6336_ADDR_DEVICE_MODE   0x00
#define FT6336_ADDR_TD_STATUS     0x02

#define FT6336_ADDR_TOUCH1_EVENT  0x03
#define FT6336_ADDR_TOUCH1_ID     0x05
#define FT6336_ADDR_TOUCH1_XH     0x03
#define FT6336_ADDR_TOUCH1_XL     0x04
#define FT6336_ADDR_TOUCH1_YH     0x05
#define FT6336_ADDR_TOUCH1_YL     0x06

#define FT6336_ADDR_TOUCH2_EVENT  0x09
#define FT6336_ADDR_TOUCH2_ID     0x0B
#define FT6336_ADDR_TOUCH2_XH     0x09
#define FT6336_ADDR_TOUCH2_XL     0x0A
#define FT6336_ADDR_TOUCH2_YH     0x0B
#define FT6336_ADDR_TOUCH2_YL     0x0C

#define FT6336_ADDR_FIRMARE_ID    0xA6

// Регистры конфигурации FT6336
#define FT6336_REG_DEVICE_MODE    0x00
#define FT6336_REG_GEST_ID        0x01
#define FT6336_REG_TD_STATUS      0x02
#define FT6336_REG_THRESHOLD      0x80
#define FT6336_REG_FILTER_COEFF   0x85
#define FT6336_REG_CTRL           0x86
#define FT6336_REG_TIME_ENTER_MONITOR 0x87
#define FT6336_REG_PERIOD_ACTIVE  0x88
#define FT6336_REG_PERIOD_MONITOR 0x89
#define FT6336_REG_RADIAN_VALUE  0x91
#define FT6336_REG_OFFSET_LEFT_RIGHT  0x92
#define FT6336_REG_OFFSET_UP_DOWN     0x93
#define FT6336_REG_DISTANCE_LEFT_RIGHT 0x94
#define FT6336_REG_DISTANCE_UP_DOWN   0x95
#define FT6336_REG_LIB_VER_H     0xA1
#define FT6336_REG_LIB_VER_L     0xA2
#define FT6336_REG_CHIP_ID       0xA3
#define FT6336_REG_G_MODE        0xA4
#define FT6336_REG_PWR_MODE      0xA5
#define FT6336_REG_FIRMARE_ID    0xA6
#define FT6336_REG_FOCALTECH_ID  0xA8
#define FT6336_REG_RELEASE_CODE_ID 0xAF

// Значения для регистров
#define FT6336_DEVICE_MODE_NORMAL    0x00
#define FT6336_DEVICE_MODE_TEST      0x04
#define FT6336_G_MODE_INTERRUPT_POLLING 0x00
#define FT6336_G_MODE_INTERRUPT_TRIGGER 0x01
#define FT6336_PWR_MODE_ACTIVE       0x00
#define FT6336_PWR_MODE_MONITOR      0x01
#define FT6336_PWR_MODE_HIBERNATE    0x03

// Ожидаемые значения ID (разные версии чипов FocalTech)
#define FT6336_CHIP_ID_VALUE_1      0x51  // Стандартный FT6336
#define FT6336_CHIP_ID_VALUE_2      0x54  // Альтернативная версия
#define FT6336_CHIP_ID_VALUE_3      0x64  // Возможная версия (видно на осциллограмме)
#define FT6336_FOCALTECH_ID_VALUE   0x11

// Функции I2C (используют стандартный HAL)
unsigned char ft6336_write_byte(unsigned char addr, unsigned char reg, unsigned char data);
unsigned char ft6336_read_byte(unsigned char addr, unsigned char reg);

// Функции FT6336
unsigned char ft6336_init(void);
unsigned char ft6336_get_td_status(void);
void ft6336_get_touch1_position(unsigned int *x, unsigned int *y);
void ft6336_get_touch2_position(unsigned int *x, unsigned int *y);
unsigned char ft6336_read_chip_id(void);
void ft6336_set_interrupt_mode(unsigned char enable);
void ft6336_clear_interrupt(void);

#endif

