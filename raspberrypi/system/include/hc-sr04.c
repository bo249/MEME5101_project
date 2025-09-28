#include "hc-sr04.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "../module/hc_sr04/include/hc_sr_ioctl.h"

void sonic_measure(int distances[4]) {
    int trig_pin[4] = {0, 4, 6, 12};
    int echo_pin[4] = {1, 5, 7, 13};
    const char* devs[4] = {
        "/dev/ultrasonic0",
        "/dev/ultrasonic1",
        "/dev/ultrasonic2",
        "/dev/ultrasonic3"
    };

    int fds[4];
    for (int i = 0; i < 4; i++) {
        fds[i] = open(devs[i], O_RDWR);
        if (fds[i] < 0) {
            perror("open");
            distances[i] = -1;
        }
    }

    char buf[32];
    for (int i = 0; i < 4; i++) {
        distances[i] = -1;
        if (fds[i] < 0) continue;

        memset(buf, 0, sizeof(buf));
        ioctl(fds[i], HC_SR04_SET_TRIGGER, trig_pin[i]);
        ioctl(fds[i], HC_SR04_SET_ECHO, echo_pin[i]);
        int n = read(fds[i], buf, sizeof(buf));
        if (n > 0) {
            distances[i] = atoi(buf);
        }

        usleep(60000);
    }

    for (int i = 0; i < 4; i++) {
        if (fds[i] >= 0)
            close(fds[i]);
    }
}
