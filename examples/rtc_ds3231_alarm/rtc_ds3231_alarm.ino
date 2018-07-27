
// during an alarm the INT pin of the RTC is pulled low
//
// this is handy for minimizing power consumption for sensor-like devices, 
// since they can be started up by this pin on given time intervals.

#include <Wire.h>
#include "ds3231.h"

#define BUFF_MAX 256

// time when to wake up
uint8_t wake_HOUR = 15;
uint8_t wake_MINUTE = 49;
uint8_t wake_SECOND = 9;

// how often to refresh the info on stdout (ms)
unsigned long prev = 5000, interval = 5000;

void set_alarm(void)
{

    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm - see datasheet
    // A1M1 (seconds) (0 to enable, 1 to disable)
    // A1M2 (minutes) (0 to enable, 1 to disable)
    // A1M3 (hour)    (0 to enable, 1 to disable) 
    // A1M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[5] = { 0, 0, 0, 1, 1 };

    // set Alarm1
    DS3231_set_a1(wake_SECOND, wake_MINUTE, wake_HOUR, 0, flags);

    // activate Alarm1
    DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A1IE);
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    DS3231_init(DS3231_CONTROL_INTCN);
    DS3231_clear_a1f();
    set_alarm();
}

void loop()
{
    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t;

    // once a while show what is going on
    if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(&t);

        // display current time
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec);
        Serial.println(buff);

        // display a1 debug info
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);

        if (DS3231_triggered_a1()) {
            // INT has been pulled low
            Serial.println(" -> alarm1 has been triggered");
            // clear a1 alarm flag and let INT go into hi-z
            DS3231_clear_a1f();
        }
        prev = now;
    }
}

