
#include "ds3231.h"
#include "rtc_ds3231.h"

#define H_MAX 256

uint8_t time[8];
char recv[H_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 5000;

void setup()
{
    Serial.begin(9600);
    DS3231_init(0x06);
    memset(recv, 0, H_MAX - 1);
    Serial.println("GET time");
}

void loop()
{
    char buf[200];
    unsigned long now = millis();
    int in;

    // show time once in a while
    if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(3, &buf[0], 59);
        Serial.println(buf);
        prev = now;
    }

    if (Serial.available() > 0) {
        in = Serial.read();

        //snprintf(buf,200,"%d",in);
        //Serial.println(buf);

        if ((in == 10 || in == 13) && (recv_size > 0)) {
            parse_cmd(recv, recv_size);
            recv_size = 0;
            recv[0] = 0;
        } else if (in < 48 || in > 122) { // ~[0-9A-Za-z]
            // ignore 
        } else if (recv_size > H_MAX - 2) {
            // drop
            recv_size = 0;
            recv[0] = 0;
        } else if (recv_size < H_MAX - 2) {
            recv[recv_size] = in;
            recv[recv_size + 1] = 0;
            //snprintf(buf,200,"partial,%d: %s,%d,%d\n",recv_size,recv,recv[recv_size],in);
            //Serial.print(buf);
            recv_size += 1;
        }

    }
}

void parse_cmd(char *cmd, int cmdsize)
{
    uint8_t i;
    uint8_t reg_val;
    char buf[H_MAX];

    //snprintf(buf, 200, "cmd was '%s' %d\n", cmd, cmdsize);
    //Serial.print(buf);

    // TssmmhhWDDMMYYYY aka set time
    if (cmd[0] == 84 && cmdsize == 16) {
        for (i = 0; i < 3; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh
        }
        time[3] = cmd[7] - 48;  // day of week
        for (i = 4; i < 8; i++) {
            time[i] = (cmd[2 * i] - 48) * 10 + cmd[2 * i + 1] - 48;     // DD, MM, YY, YY
        }
        DS3231_set(time[0], time[1], time[2], time[3], time[4], time[5],
                   time[6] * 100 + time[7]);
        Serial.println("Ok");
    } else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
        DS3231_get_a1(&buf[0], 59);
        Serial.println(buf);
    } else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
        DS3231_get_a2(&buf[0], 59);
        Serial.println(buf);
    } else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
        Serial.print("aging reg is ");
        Serial.println(DS3231_get_aging(), DEC);
    } else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
        DS3231_set_creg(0x05);
        //ASSMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
        }
        boolean flags[5] = { 0, 0, 0, 0, 0 };
        DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
        DS3231_get_a1(&buf[0], 59);
        Serial.println(buf);
    } else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
        DS3231_set_creg(0x06);
        //BMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
        }
        boolean flags[5] = { 0, 0, 0, 0 };
        DS3231_set_a2(time[0], time[1], time[2], flags);
        DS3231_get_a2(&buf[0], 59);
        Serial.println(buf);
    } else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
        Serial.print("temperature reg is ");
        Serial.println(DS3231_get_treg(), DEC);
    } else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
        reg_val = DS3231_get_sreg();
        reg_val &= B11111100;
        DS3231_set_sreg(reg_val);
    } else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
        DS3231_set_aging(0);
    } else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
        Serial.print("status reg is ");
        Serial.println(DS3231_get_sreg(), DEC);
    } else {
        Serial.print("unknown command prefix ");
        Serial.println(cmd[0]);
        Serial.println(cmd[0], DEC);
    }
}
