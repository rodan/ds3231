#ifndef __ds3231_h_
#define __ds3231_h_

#include <WProgram.h>

static int ds3231_i2c_addr = 0x68;

void DS3231_init(uint8_t creg);
void DS3231_set(uint8_t s, uint8_t mi, uint8_t h, uint8_t dw,
                uint8_t d, uint8_t mo, uint16_t y);
void DS3231_get(unsigned char type, char *buf, size_t len);

// control/status register
void DS3231_set_creg(uint8_t val);
void DS3231_set_sreg(uint8_t mask);
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

#endif
