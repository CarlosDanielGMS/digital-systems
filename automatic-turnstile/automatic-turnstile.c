#include <stdio.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#define IN 0
#define OUT 1
#define PRESSED 0
#define NOT_PRESSED 1

#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15

#define RESET_BUTTON_PIN 6

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

bool resetButtonStatus = NOT_PRESSED;

void initializeComponents();
void readButtons();
void setDisplay(char *message);

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
    
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    setDisplay("Inicializando");
}

void readButtons()
{
    resetButtonStatus = gpio_get(RESET_BUTTON_PIN);
}

void setDisplay(char *message)
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);
    
    ssd1306_draw_string(ssd, 0, 8, message);
    
    render_on_display(ssd, &frame_area);
}