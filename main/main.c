/*******************************************************************************************
 * Class: EEL5862
 * Author: Hector Pena
 * Assignment No: 04
 * Date: September 2026
 * Description: Multithreading and its applications
*******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"
#include "driver/uart.h"

#define LED_PIN (GPIO_NUM_2)

/* Parameters for the LED Pin*/
gpio_config_t led_pin_config = {
    .pin_bit_mask = (1ULL << LED_PIN),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

const uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

uint32_t i;
uint32_t x = 0;
float fi;
char str[100] = {0};

/* Task that prints out converted numbers from UART input */
void Task_1(void *pvParameters);
TaskHandle_t Task_Handle_1 = NULL;

/* Task that prints a message every second */
void Task_2(void *pvParameters);
TaskHandle_t Task_Handle_2 = NULL;

void toggle_led(void)
{
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(GPIO_NUM_2, led_state);
}

/* Helper to initialize the UART0 component*/
void uart0_init(void);

/* Helper to convert a string to integer and float numbers */
int32_t convert_string_to_numbers(char *str);

/* Helper to read input from UART */
int32_t read_uart_input(void);

void app_main(void)
{
    /* Configure the LED pin */
    gpio_config(&led_pin_config);
    
    uart0_init();

    /* Create blinky task */
    xTaskCreatePinnedToCore(&Task_1,        // Task address
        "Task_1",               // Task name
        4000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        &Task_Handle_1,         // Handle
        0);                     // Core ID
    
    /* Create hello world task */
    xTaskCreatePinnedToCore(&Task_2,        // Task address
        "Task_2",               // Task name
        4000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        &Task_Handle_2,         // Handle
        1);                     // Core ID
}

void Task_1(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        toggle_led();

        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 1 second
    }
}

void Task_2(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        sprintf(str, "Project 4 - Task 2 %ld", i++);

        /* Print message to UART */
        printf("%s\n", str);

        if(i == 10)
        {
            vTaskDelete(Task_Handle_1);
        }

        for(x = 0; x < 40000000; x++)
        {
            /* Do nothing, just waste time */
            x++;
        }

        vTaskDelay(1);
    }
}

void uart0_init(void)
{
    /* Empty the stream buffer*/
    fflush(stdout); 

    /* Wait for UART to finish transmitting any previous data */
    uart_wait_tx_idle_polling(UART_NUM_0); 

    /* Configure the UART */
    uart_param_config(UART_NUM_0, &uart_config);

    /* Install UART driver */
    uart_driver_install(
        UART_NUM_0, // UART port number
        1024,       // RX buffer size
        0,          // TX buffer size
        0,          // Queue size
        NULL,       // Event queue handle
        0           // Interrupt allocation flags
    );
}

int32_t read_uart_input(void)
{
    char temp_buff[20] = {0};
    int32_t ret = -1;
    static size_t pos = 0;

    /* Read UART TX Buffer */
    int32_t len = uart_read_bytes(UART_NUM_0, temp_buff, sizeof(temp_buff), pdMS_TO_TICKS(20));
    
    /* If no message was received, return an error*/
    if (len <= 0)
    {
        return -1;
    }

    /* Loop through the message looking for an Enter keystroke */
    for(uint32_t k = 0; k < len; k++)
    {
        char c = temp_buff[k];

        /* Check if the received character is a newline or carriage return */
        if((c == '\r' || c == '\n') && (pos > 0))
        {
            str[pos] = '\0';
            pos = 0;
            ret = convert_string_to_numbers(str);
        }
        else if (pos < sizeof(str) - 1)
        {
            str[pos++] = c;
        }
    }
    
    return ret;
}

int32_t convert_string_to_numbers(char *str)
{
    char *endptr;
    
    i = (uint32_t)strtol(str, &endptr, 10);
    if (endptr == str)
    {
        printf("Failed to convert string to number: %s\n", str);
        return -1;
    }

    fi = strtof(str, &endptr);
    if (endptr == str)
    {
        printf("Failed to convert string to float: %s\n", str);
        return -1;
    }

    return 0;
}