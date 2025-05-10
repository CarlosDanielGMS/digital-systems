#include <stdio.h>
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/uart.h"

#define IN 0
#define OUT 1
#define PRESSED 0
#define NOT_PRESSED 1

#define NONE 0
#define UP 1
#define RIGHT 2
#define DOWN 3
#define LEFT 4

#define RESET_BUTTON_PIN 6
#define DISPLAY_SDA_PIN 14
#define DISPLAY_SCL_PIN 15
#define JOYSTICK_BUTTON_PIN 22
#define JOYSTICK_X_AXIS_PIN 26
#define JOYSTICK_Y_AXIS_PIN 27

#define JOYSTICK_X_AXIS_ADC_CHANNEL 1
#define JOYSTICK_Y_AXIS_ADC_CHANNEL 0

#define GR 0
#define HO 1
#define DI 2
#define PT 3

struct render_area frame_area = {
    start_column : 0,
    end_column : ssd1306_width - 1,
    start_page : 0,
    end_page : ssd1306_n_pages - 1
};

bool resetButtonStatus = NOT_PRESSED;
bool joystickButtonStatus = NOT_PRESSED;

unsigned char joystickDirection = NONE;

unsigned char previousEntry = PT;
unsigned char currentEntry = GR;

bool grStatus = false;
bool hoStatus = false;
bool diStatus = false;
bool ptStatus = false;
bool ctStatus = false;

void wait(unsigned long int milliseconds);
void initializeComponents();
void readButtons();
void setDisplay(char *message);
void updateDisplay();
void readJoystickDirection();

int main()
{
    initializeComponents();

    while (true)
    {
        updateDisplay();
        readButtons();

        if (resetButtonStatus == PRESSED)
        {
            reset_usb_boot(0, 0);
        }
        else if (joystickButtonStatus == PRESSED)
        {
            // Next
        }
        else
        {
            readJoystickDirection();

            if (joystickDirection == RIGHT)
            {
                if (currentEntry == PT)
                {
                    currentEntry = GR;
                }
                else
                {
                    currentEntry++;
                }
            }
            else if (joystickDirection == LEFT)
            {
                if (currentEntry == GR)
                {
                    currentEntry = PT;
                }
                else
                {
                    currentEntry--;
                }
            }

            wait(150);
        }
    }
}

void wait(unsigned long int milliseconds)
{
    unsigned long int previousTime = to_ms_since_boot(get_absolute_time());
    unsigned long int currentTime = previousTime;

    while (currentTime - previousTime <= milliseconds)
    {
        currentTime = to_ms_since_boot(get_absolute_time());
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

    adc_init();
    adc_gpio_init(JOYSTICK_X_AXIS_PIN);
    adc_gpio_init(JOYSTICK_Y_AXIS_PIN);
}

void readButtons()
{
    resetButtonStatus = gpio_get(RESET_BUTTON_PIN);
    joystickButtonStatus = !gpio_get(JOYSTICK_BUTTON_PIN);
}

void setDisplay(char *message)
{
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    ssd1306_draw_string(ssd, 0, 8, message);

    render_on_display(ssd, &frame_area);
}

void updateDisplay()
{
    if (currentEntry != previousEntry)
    {
        switch (currentEntry)
        {
        case 0:
            setDisplay("0 - Giro");
            break;

        case 1:
            setDisplay("1 - Horario");
            break;

        case 2:
            setDisplay("2 - Dia");
            break;

        case 3:
            setDisplay("3 - Portaria");
            break;

        default:
            setDisplay("Erro");
            break;
        }

        previousEntry = currentEntry;
    }
}

void readJoystickDirection()
{
    unsigned short int joystickXValue = 0;
    unsigned short int joystickYValue = 0;

    adc_select_input(JOYSTICK_X_AXIS_ADC_CHANNEL);
    sleep_us(2);
    joystickXValue = adc_read();

    adc_select_input(JOYSTICK_Y_AXIS_ADC_CHANNEL);
    sleep_us(2);
    joystickYValue = adc_read();

    if (joystickYValue > 3584)
    {
        joystickDirection = UP;
    }
    else if (joystickYValue < 512)
    {
        joystickDirection = DOWN;
    }
    else if (joystickXValue > 3584)
    {
        joystickDirection = RIGHT;
    }
    else if (joystickXValue < 512)
    {
        joystickDirection = LEFT;
    }
    else
    {
        joystickDirection = NONE;
    }
}