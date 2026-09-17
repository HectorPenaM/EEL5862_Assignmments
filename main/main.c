/*******************************************************************************************
 * Class: EEL5862
 * Author: Hector Pena
 * Assignment No: 02
 * Date: September 2026
 * Description: Blinks an LED on GPIO2 on/off every 1 second.
*******************************************************************************************/

#include <stdio.h>
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

/* Task that blinks an LED every second */
void Task_1_Blink(void *pvParameters);

/* Task that prints a message every second */
void Task_2_PrintMessage(void *pvParameters);

/* Helper to set UART0 baud rate */
void set_uart0_baud_rate(void);

void app_main(void)
{
    /* Configure the LED pin */
    gpio_config(&led_pin_config);

    set_uart0_baud_rate();

    /* Create blinky task */
    xTaskCreate(&Task_1_Blink,  // Task address
        "Task_1_Blink",         // Task name
        1000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        NULL);                  // Handle
    
    /* Create hello world task */
    xTaskCreate(&Task_2_PrintMessage,  // Task address
        "Task_2_PrintMessage",         // Task name
        2000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        NULL);                  // Handle
}

void Task_1_Blink(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        /* Set LED pin high */
        gpio_set_level(GPIO_NUM_2, 1);

        /* Wait 1000 milliseconds */
        vTaskDelay(pdMS_TO_TICKS(1000));

        /* Set LED pin low */
        gpio_set_level(GPIO_NUM_2, 0);

        /* Wait 1000 milliseconds */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Task_2_PrintMessage(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        /* Print message to UART */
        printf("Hello World!\n");

        /* Wait 1000 milliseconds */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void set_uart0_baud_rate(void)
{
    /* Empty the stream buffer*/
    fflush(stdout); 

    /* Wait for UART to finish transmitting any previous data */
    uart_wait_tx_idle_polling(UART_NUM_0); 

    /* Configure the UART */
    uart_param_config(UART_NUM_0, &uart_config);
}