#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define BUTTON0_PIN "/sys/class/gpio/gpio67/value"   // P8_8
#define BUTTON1_PIN "/sys/class/gpio/gpio68/value"   // P8_9
#define PWM_PIN "/sys/class/pwm/pwmchip2/pwm0"       // P8_13
#define TEMPERATURE_PIN "/sys/bus/iio/devices/iio:device0/in_voltage0_raw" // P9_39 (AIN0)

bool button0_pressed = false;
bool button1_pressed = false;
pthread_t button_thread;
pthread_t temperature_thread;

void* button_event_handler(void* arg)
{
    int button0_fd, button1_fd;
    char button0_value, button1_value;

    button0_fd = open(BUTTON0_PIN, O_RDONLY);
    button1_fd = open(BUTTON1_PIN, O_RDONLY);

    while (true) {
        lseek(button0_fd, 0, SEEK_SET);
        lseek(button1_fd, 0, SEEK_SET);
        read(button0_fd, &button0_value, 1);
        read(button1_fd, &button1_value, 1);

        if (button0_value == '1') {
            button0_pressed = true;
            printf("Event 1: Button 0 pressed\n");

            // Generate PWM of 1kHz
            FILE* pwm_fd = fopen(PWM_PIN "/period", "w");
            fprintf(pwm_fd, "1000000\n");   // 1ms period (1kHz)
            fclose(pwm_fd);

            printf("PWM frequency: 1kHz\n");
        }

        if (button1_value == '1') {
            button1_pressed = true;
            printf("Event 2: Button 1 pressed\n");

            // Generate PWM of 10kHz
            FILE* pwm_fd = fopen(PWM_PIN "/period", "w");
            fprintf(pwm_fd, "100000\n");    // 0.1ms period (10kHz)
            fclose(pwm_fd);

            printf("PWM frequency: 10kHz\n");
        }

        usleep(100000);   // Sleep for 100ms
    }

    close(button0_fd);
    close(button1_fd);

    return NULL;
}

void* temperature_thread_handler(void* arg)
{
    int temperature_fd;
    char temperature_value[5];

    temperature_fd = open(TEMPERATURE_PIN, O_RDONLY);

    while (true) {
        lseek(temperature_fd, 0, SEEK_SET);
        read(temperature_fd, &temperature_value, 4);
        temperature_value[4] = '\0';

        uint32_t temperature_raw = atoi(temperature_value);
        float temperature_celsius = (temperature_raw * 1.8f / 4095.0f) - 50.0f;

        struct timespec timestamp;
        clock_gettime(CLOCK_MONOTONIC, &timestamp);
        uint64_t milliseconds = (uint64_t)timestamp.tv_sec * 1000 + (uint64_t)timestamp.tv_nsec / 1000000;

        printf("Temperature: %.2f°C, Timestamp: %lu\n", temperature_celsius, milliseconds);

        sleep(1);   // Sleep for 1 second
    }

    close(temperature_fd);

    return NULL;
}

int main()
{
    pthread_create(&button_thread, NULL, button_event_handler, NULL);
    pthread_create(&temperature_thread, NULL, temperature_thread_handler, NULL);

    sleep(10);   // Run the experiment for 10 seconds

    pthread_cancel(button_thread);
    pthread_cancel(temperature_thread);
    pthread_join(button_thread, NULL);
    pthread_join(temperature_thread, NULL);

    return 0;
}
