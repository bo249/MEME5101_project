#include "buzzer.h"
#include "../module/buzzer/include/buzzer_ioctl.h"

#define BUZZER_GPIO 8

void control_buzzer_on(int fd) {
    ioctl(fd, IOCTL_SET_BUZZER_ON, BUZZER_GPIO);
}
void control_buzzer_off(int fd) {
    ioctl(fd, IOCTL_SET_BUZZER_OFF, BUZZER_GPIO);
}