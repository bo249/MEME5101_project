#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <sys/select.h>

#include "../module/buzzer/include/buzzer_ioctl.h"
#include "../module/hc_sr04/include/hc_sr_ioctl.h"

#define BUZZER_GPIO 8
#define THRESHOLD_DISTANCE 50 // mm
#define ALERT_DURATION 5 // seconds

void sonic_measure(int distances[4]);
void control_buzzer_on(int fd);
void control_buzzer_off(int fd);
int open_dev();

int main() {
    int distances[4];
    int buzzer_fd = open_dev();
    int abnormal_flag = 0;
    int lock_alarm = 0;
    time_t alert_start_time = 0;

    printf("障礙物偵測中...\n");

    while (1) {
        sonic_measure(distances);
        int is_alert = 0;

        for (int i = 0; i < 4; i++) {
            printf("Distance%d: %d mm\n", i, distances[i]);
            if (distances[i] >= 0 && distances[i] < THRESHOLD_DISTANCE) {
                printf("sonic%d too close!\n", i);
                is_alert = 1;
            }
        }

        if (is_alert) {
            // 首次進入異常，開始計時
            if (!abnormal_flag) {
                alert_start_time = time(NULL);
                abnormal_flag = 1;
            }
            // 判斷異常是否持續超過 10 秒
            time_t now = time(NULL);
            int elapsed = (int)(now - alert_start_time);
            if (elapsed >= ALERT_DURATION) {
                // 超過 10 秒，蜂鳴器常開
                printf("障礙物距離<=50mm，且停留超過5秒，蜂鳴器常開！\n");
                control_buzzer_on(buzzer_fd);
                sleep(1);
            } else {
                // 未超過 10 秒，蜂鳴器間歇警報
                printf("障礙物距離<=50mm，蜂鳴器間歇警報中...\n");
                control_buzzer_on(buzzer_fd);
                usleep(200000);  // 蜂鳴200ms
                control_buzzer_off(buzzer_fd);
                usleep(800000);  // 靜音800ms
            }
        } else {
            if (abnormal_flag) {
                printf("障礙物遠離，警報解除。\n");
            }
            // 沒有異常，重置狀態與蜂鳴器關閉
            abnormal_flag = 0;
            alert_start_time = 0;
            control_buzzer_off(buzzer_fd);
            printf("障礙物偵測中...\n");
            sleep(1);
        }
    }

    close(buzzer_fd);
    return 0;
}

int open_dev() {
    int fd = open(DEVICE_FILE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Can't open buzzer device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }
    return fd;
}

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

//控制蜂鳴器 ON
void control_buzzer_on(int fd) {
    ioctl(fd, IOCTL_SET_BUZZER_ON, BUZZER_GPIO);
}
// 控制蜂鳴器 OFF
void control_buzzer_off(int fd) {
    ioctl(fd, IOCTL_SET_BUZZER_OFF, BUZZER_GPIO);
}