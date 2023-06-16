#include <nuttx/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define UART_READ_TIMEOUT      60000
#define UART_PATH              "/dev/ttyS1"

int fd;

/* Data processing thread */
int proc_func(int argc, FAR char *argv[]) {
    for (;;) {
        fxfer_parser();
    }
}

int filexfer_main(int argc, FAR char *argv[]) {
    /* Open UART */
    printf("File transfer test application\n");
    fd = open(UART_PATH, O_RDWR); //Opening the uart with read and write permission
    if (fd < 0) {
        printf("UART open error\n");
    }

    /* Run processing thread */
    printf("Run data processing thread\n");
    int ret = task_create("fxfer_process", 100, 2048, proc_func, NULL);
    if (ret < 0) {
        printf("Processing thread start failed, err: %d\n", errno);
    }

    return 0;
}

/* Platform specific functions */
uint32_t platform_get_tick() {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return current_time.tv_sec * 1000 + current_time.tv_usec / 1000;
}

int read_from_uart(uint8_t *buffer, int num) {
    fd_set rd;
    struct timeval tv;
    int err;

    FD_ZERO(&rd);
    FD_SET(fd, &rd);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = select(fd + 1, &rd, NULL, NULL, &tv);
    return read(fd, buffer, num);
}

int read_from_uart_blocking(uint8_t *buffer, int num, uint32_t to_ms) {
    int data_read = 0;
    uint32_t start_tick = platform_get_tick();
    while ((data_read != num) && (platform_get_tick() - start_tick <= to_ms)) {
        data_read += read_from_uart(&buffer[data_read], num - data_read);
    }
    return data_read;
}

void platform_send(uint8_t* data, uint16_t len) {
    if (write(fd, data, len) < 0) {
        printf("UART write error\n");
    }
}

uint16_t platform_read(uint8_t* data, uint16_t len) {
    return (uint16_t)read_from_uart_blocking(data, len, UART_READ_TIMEOUT);
}

void platform_sleep(uint32_t ms) {
    usleep(ms*1000);
}

void log_info(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vprintf(str, list);
    va_end(list);
}

void log_debug(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vprintf(str, list);
    va_end(list);
}

void log_error(const char* str, ...) {
    va_list list;
    va_start(list, str);
    vprintf(str, list);
    va_end(list);
}
