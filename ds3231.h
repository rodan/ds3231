#ifndef __ds3231_h_
#define __ds3231_h_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

// i2c slave address of the DS3231 chip
#define DS3231_I2C_ADDR             0x68

// timekeeping registers
#define DS3231_TIME_CAL_ADDR        0x00
#define DS3231_ALARM1_ADDR          0x07
#define DS3231_ALARM2_ADDR          0x0B
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F
#define DS3231_AGING_OFFSET_ADDR    0x10
#define DS3231_TEMPERATURE_ADDR     0x11

struct ts {
    uint8_t sec;         /* seconds */
    uint8_t min;         /* minutes */
    uint8_t hour;        /* hours */
    uint8_t mday;        /* day of the month */
    uint8_t mon;         /* month */
    int year;            /* year */
    uint8_t wday;        /* day of the week */
    uint8_t yday;        /* day in the year */
    uint8_t isdst;       /* daylight saving time */
    uint8_t year_s;      /* year in short notation*/
};

void DS3231_init(uint8_t creg);
void DS3231_set(struct ts t);
void DS3231_get(struct ts *t);

void DS3231_set_addr(uint8_t addr, uint8_t val);
uint8_t DS3231_get_addr(uint8_t addr);

// control/status register
void DS3231_set_creg(uint8_t val);
void DS3231_set_sreg(uint8_t val);
uint8_t DS3231_get_sreg();

// aging offset register
void DS3231_set_aging(int8_t val);
int8_t DS3231_get_aging();

// temperature register
float DS3231_get_treg();

// alarms
void DS3231_set_a1(uint8_t s, uint8_t mi, uint8_t h, uint8_t d,
                   boolean * flags);
void DS3231_get_a1(char *buf, size_t len);
void DS3231_set_a2(uint8_t mi, uint8_t h, uint8_t d, boolean * flags);
void DS3231_get_a2(char *buf, size_t len);

// helpers
uint8_t dectobcd(uint8_t val);
uint8_t bcdtodec(uint8_t val);
uint8_t inp2toi(char *cmd, uint16_t seek);

#endif
