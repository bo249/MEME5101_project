#include "load_dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#define DEVICE_FILE_NAME "/dev/buzzer"

int open_dev() {
    int fd = open(DEVICE_FILE_NAME, O_RDWR);
    if (fd < 0) {
        printf("Can't open buzzer device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }
    return fd;
}
