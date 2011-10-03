
#include <Wire.h>
#include "ds3231.h"

/* control register 0Eh/8Eh
bit7 EOSC   Enable Oscillator (1 if oscillator must be stopped when on battery)
bit6 BBSQW  Battery Backed Square Wave
bit5 CONV   Convert temperature (1 forces a conversion NOW)
bit4 RS2    Rate select - frequency of square wave output
bit3 RS1    Rate select
bit2 INTCN  Interrupt control (1 for use of the alarms and to disable square wave)
bit1 A2IE   Alarm2 interrupt enable (1 to enable)
bit0 A1IE   Alarm1 interrupt enable (1 to enable)
*/

void DS3231_init(uint8_t ctrl_reg)
{
    Wire.begin();
    DS3231_set_creg(ctrl_reg);
}

void DS3231_set(uint8_t s, uint8_t mi, uint8_t h, uint8_t dw,
                uint8_t d, uint8_t mo, uint16_t y)
{
    uint8_t i, century, year_short;

    if (y > 2000) {
        century = B10000000;
        year_short = y - 2000;
    } else {
        century = 0;
        year_short = y - 1900;
    }

    uint8_t TimeDate[7] = { s, mi, h, dw, d, mo, year_short };

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_TIME_CAL_ADDR);
    for (i = 0; i <= 6; i++) {
        TimeDate[i] = dectobcd(TimeDate[i]);
        if (i == 5)
            TimeDate[5] += century;
        Wire.send(TimeDate[i]);
    }
    Wire.endTransmission();
}

// type==0 returns hh:mm:ss YYMMDD
// type==1 returns YYYYMMDD
// type==2 returns D
// type==3 returns YYYYMMDD hh:mm:ss
void DS3231_get(unsigned char type, char *buf, size_t len)
{
    uint8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
    uint8_t century = 0;
    uint8_t i, n;
    uint16_t year_full;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_TIME_CAL_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 7);

    for (i = 0; i <= 6; i++) {
        n = Wire.receive();
        if (i == 5) {
            TimeDate[5] = bcdtodec(n & 0x1F);
            century = (n & 0x80) >> 7;
        } else
            TimeDate[i] = bcdtodec(n);
    }

    if (century == 1)
        year_full = 2000 + TimeDate[6];
    else
        year_full = 1900 + TimeDate[6];

    if (type == 0) {
        snprintf(buf, len, "%02d:%02d:%02d %d%02d%02d", TimeDate[2],
                 TimeDate[1], TimeDate[0], year_full, TimeDate[5], TimeDate[4]);
    } else if (type == 1) {
        snprintf(buf, len, "%d%02d%02d", year_full, TimeDate[5], TimeDate[4]);
    } else if (type == 2) {
        snprintf(buf, len, "%d", TimeDate[4]);
    } else if (type == 3) {
        snprintf(buf, len, "%d%02d%02d %02d:%02d:%02d", year_full,
                 TimeDate[5], TimeDate[4], TimeDate[2], TimeDate[1],
                 TimeDate[0]);
    }
}

void DS3231_get(struct ts *t)
{
    uint8_t TimeDate[7];        //second,minute,hour,dow,day,month,year
    uint8_t century = 0;
    uint8_t i, n;
    uint16_t year_full;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_TIME_CAL_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 7);

    for (i = 0; i <= 6; i++) {
        n = Wire.receive();
        if (i == 5) {
            TimeDate[5] = bcdtodec(n & 0x1F);
            century = (n & 0x80) >> 7;
        } else
            TimeDate[i] = bcdtodec(n);
    }

    if (century == 1)
        year_full = 2000 + TimeDate[6];
    else
        year_full = 1900 + TimeDate[6];

    t->sec = TimeDate[0];
    t->min = TimeDate[1];
    t->hour = TimeDate[2];
    t->mday = TimeDate[4];
    t->mon = TimeDate[5];
    t->year = TimeDate[6];
    t->wday = TimeDate[3];
}

void DS3231_set_addr(uint8_t addr, uint8_t val)
{
    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send(addr);
    Wire.send(val);
    Wire.endTransmission();
}

uint8_t DS3231_get_addr(uint8_t addr)
{
    uint8_t rv;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) addr);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 1);
    rv = Wire.receive();

    return rv;
}



// control register

void DS3231_set_creg(uint8_t val)
{
    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_CONTROL_ADDR);
    Wire.send(val);
    Wire.endTransmission();
}

// status register 0Fh/8Fh

/*
bit7 OSF      Oscillator Stop Flag (if 1 then oscillator has stopped and date might be innacurate)
bit3 EN32kHz  Enable 32kHz output (1 if needed)
bit2 BSY      Busy with TCXO functions
bit1 A2F      Alarm 2 Flag - (1 if alarm2 was triggered)
bit0 A1F      Alarm 1 Flag - (1 if alarm1 was triggered)
*/

void DS3231_set_sreg(uint8_t val)
{
    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_STATUS_ADDR);
    Wire.send(val);
    Wire.endTransmission();
}

uint8_t DS3231_get_sreg()
{
    uint8_t rv;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_STATUS_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 1);
    rv = Wire.receive();

    return rv;
}

// aging register

void DS3231_set_aging(int8_t val)
{
    uint8_t reg;

    if (val >= 0)
        reg = val;
    else
        reg = ~(-val) + 1;      // 2C

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_AGING_OFFSET_ADDR);
    Wire.send(reg);
    Wire.endTransmission();
}

int8_t DS3231_get_aging()
{
    uint8_t reg;
    int8_t rv;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_AGING_OFFSET_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 1);
    reg = Wire.receive();

    if ((reg & B10000000) != 0)
        rv = reg | ~((1 << 8) - 1);     // if negative get two's complement
    else
        rv = reg;

    return rv;
}

// temperature register

float DS3231_get_treg()
{
    float rv;
    uint8_t temp_msb, temp_lsb;
    int8_t nint;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_TEMPERATURE_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 2);
    temp_msb = Wire.receive();
    temp_lsb = Wire.receive() >> 6;

    if ((temp_msb & B10000000) != 0)
        nint = temp_msb | ~((1 << 8) - 1);      // if negative get two's complement
    else
        nint = temp_msb;

    rv = 0.25 * temp_lsb + nint;

    return rv;
}

// alarms

// flags are: A1M1 (seconds), A1M2 (minutes), A1M3 (hour), 
// A1M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0)
void DS3231_set_a1(uint8_t s, uint8_t mi, uint8_t h, uint8_t d, boolean * flags)
{
    uint8_t t[4] = { s, mi, h, d };
    uint8_t i;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_ALARM1_ADDR);

    for (i = 0; i <= 3; i++) {
        if (i == 3) {
            Wire.send(dectobcd(t[3]) | (flags[3] << 7) | (flags[4] << 6));
        } else
            Wire.send(dectobcd(t[i]) | (flags[i] << 7));
    }

    Wire.endTransmission();
}

void DS3231_get_a1(char *buf, size_t len)
{
    uint8_t n[4];
    uint8_t t[4];               //second,minute,hour,day
    boolean f[5];               // flags
    uint8_t i;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_ALARM1_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 4);

    for (i = 0; i <= 3; i++) {
        n[i] = Wire.receive();
        f[i] = (n[i] & B10000000) >> 7;
        t[i] = bcdtodec(n[i] & 0x7F);
    }

    f[4] = (n[3] & B01000000) >> 6;
    t[3] = bcdtodec(n[3] & 0x3F);

    snprintf(buf, len,
             "s%02d m%02d h%02d d%02d fs%d m%d h%d d%d wm%d %d %d %d %d",
             t[0], t[1], t[2], t[3], f[0], f[1], f[2], f[3], f[4], n[0],
             n[1], n[2], n[3]);

}

// flags are: A2M2 (minutes), A2M3 (hour), A2M4 (day) 0 to enable, 1 to disable, DY/DT (dayofweek == 1/dayofmonth == 0) - 
void DS3231_set_a2(uint8_t mi, uint8_t h, uint8_t d, boolean * flags)
{
    uint8_t t[3] = { mi, h, d };
    uint8_t i;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_ALARM2_ADDR);

    for (i = 0; i <= 2; i++) {
        if (i == 2) {
            Wire.send(dectobcd(t[2]) | (flags[2] << 7) | (flags[3] << 6));
        } else
            Wire.send(dectobcd(t[i]) | (flags[i] << 7));
    }

    Wire.endTransmission();
}

void DS3231_get_a2(char *buf, size_t len)
{
    uint8_t n[3];
    uint8_t t[3];               //second,minute,hour,day
    boolean f[4];               // flags
    uint8_t i;

    Wire.beginTransmission((uint8_t) DS3231_I2C_ADDR);
    Wire.send((uint8_t) DS3231_ALARM2_ADDR);
    Wire.endTransmission();

    Wire.requestFrom((uint8_t) DS3231_I2C_ADDR, (uint8_t) 3);

    for (i = 0; i <= 2; i++) {
        n[i] = Wire.receive();
        f[i] = (n[i] & B10000000) >> 7;
        t[i] = bcdtodec(n[i] & 0x7F);
    }

    f[3] = (n[2] & B01000000) >> 6;
    t[2] = bcdtodec(n[2] & 0x3F);

    snprintf(buf, len, "m%02d h%02d d%02d fm%d h%d d%d wm%d %d %d %d", t[0],
             t[1], t[2], f[0], f[1], f[2], f[3], n[0], n[1], n[2]);

}

// helpers

uint8_t dectobcd(uint8_t val)
{
    return ((val / 10 * 16) + (val % 10));
}

uint8_t bcdtodec(uint8_t val)
{
    return ((val / 16 * 10) + (val % 16));
}
