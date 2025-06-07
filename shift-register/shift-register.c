#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "inc/ssd1306.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include <stdio.h>

#define IN 0
#define OUT 1
#define PRESSED 0
#define NOT_PRESSED 1
#define LOW 0
#define HIGH 1

#define RESET_BUTTON_PIN 5
#define SHOW_BUTTON_PIN 6

#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15

#define IC_OUTPUT_PIN 16
#define IC_CLOCK_PIN 17
#define IC_LOAD_PIN 28

bool resetButtonStatus = NOT_PRESSED;
bool showButtonStatus = NOT_PRESSED;

bool bits[8];
unsigned char result = 0;

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

void initializeComponents();
void readButtons();
void resetIntoBootselMode();
void setDisplay(char *message);
void clearResults();
void readRegister();

int main()
{
    initializeComponents();

    setDisplay("Press B");

    while (true)
    {
        readButtons();

        if (resetButtonStatus == PRESSED)
        {
            resetIntoBootselMode();
        }
        else if (showButtonStatus == PRESSED)
        {
            readRegister();

            char message[8];
            sprintf(message, "%u", result);
            setDisplay(message);
        }
    }
}

void initializeComponents()
{
    stdio_init_all();

    gpio_init(RESET_BUTTON_PIN);
    gpio_set_dir(RESET_BUTTON_PIN, IN);
    gpio_pull_up(RESET_BUTTON_PIN);

    gpio_init(SHOW_BUTTON_PIN);
    gpio_set_dir(SHOW_BUTTON_PIN, IN);
    gpio_pull_up(SHOW_BUTTON_PIN);

    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(DISPLAY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA_PIN);
    gpio_pull_up(DISPLAY_SCL_PIN);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    setDisplay("Inicializando");

    gpio_init(IC_OUTPUT_PIN);
    gpio_set_dir(IC_OUTPUT_PIN, IN);
    gpio_init(IC_CLOCK_PIN);
    gpio_set_dir(IC_CLOCK_PIN, OUT);
    gpio_put(IC_CLOCK_PIN, LOW);
    gpio_init(IC_LOAD_PIN);
    gpio_set_dir(IC_LOAD_PIN, OUT);
    gpio_put(IC_LOAD_PIN, HIGH);
}

void readButtons()
{
    resetButtonStatus = gpio_get(RESET_BUTTON_PIN);
    showButtonStatus = gpio_get(SHOW_BUTTON_PIN);
}

void resetIntoBootselMode()
{
    reset_usb_boot(0, 0);
}

void setDisplay(char *message)
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    ssd1306_draw_string(ssd, 0, 8, message);

    render_on_display(ssd, &frame_area);
}

void clearResults()
{
    for (unsigned char i = 0; i < 8; i++)
    {
        bits[i] = LOW;
    }
    
    result = LOW;
}

void readRegister()
{
    clearResults();
    
    gpio_put(IC_LOAD_PIN, LOW);
    sleep_us(1);

    gpio_put(IC_CLOCK_PIN, HIGH);
    sleep_us(1);
    gpio_put(IC_CLOCK_PIN, LOW);
    sleep_us(1);

    gpio_put(IC_LOAD_PIN, HIGH);
    sleep_us(1);

    for (unsigned char i = 0; i < 8; i++)
    {
        bits[i] = gpio_get(IC_OUTPUT_PIN);

        gpio_put(IC_CLOCK_PIN, HIGH);
        sleep_us(1);
        gpio_put(IC_CLOCK_PIN, LOW);
        sleep_us(1);
    }

    for (unsigned char i = 0; i < 8; i++)
    {
        if (bits[i] == HIGH)
        {
            result |= (1 << (7 - i));
        }
    }
}