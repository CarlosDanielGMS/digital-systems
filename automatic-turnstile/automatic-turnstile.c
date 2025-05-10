#include <stdio.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#define IN 0
#define OUT 1
#define PRESSED 0
#define NOT_PRESSED 1

#define RESET_BUTTON_PIN 6

bool resetButtonStatus = NOT_PRESSED;

void initializeComponents();
void readButtons();

int main()
{
    initializeComponents();

    while (true) {
        readButtons();

        if (resetButtonStatus == PRESSED)
        {
            reset_usb_boot(0, 0);
        }
        
    }
}

void initializeComponents()
{
    stdio_init_all();

    gpio_init(RESET_BUTTON_PIN);
    gpio_set_dir(RESET_BUTTON_PIN, IN);
    gpio_pull_up(RESET_BUTTON_PIN);
}

void readButtons()
{
    resetButtonStatus = gpio_get(RESET_BUTTON_PIN);
}