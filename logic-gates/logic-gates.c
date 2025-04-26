#include <stdio.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#define IN 0
#define OUT 1
#define PRESSED 0
#define NOT_PRESSED 1

#define RESET_BUTTON_PIN 22

void initializeComponents();

int main()
{
    initializeComponents();
    
    while (true) {
        if (gpio_get(RESET_BUTTON_PIN) == PRESSED)
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