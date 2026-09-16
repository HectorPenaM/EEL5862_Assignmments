/*******************************************************************************************
 * Class: EEL5862
 * Author: Hector Pena
 * Assignment No: 01
 * Date: September 2026
 * Description: Blinks an LED on GPIO2 on/off every 1 second.
*******************************************************************************************/

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"

#define LED_PIN (GPIO_NUM_2)

/* Parameters for the LED Pin*/
gpio_config_t led_pin_config = {
    .pin_bit_mask = (1ULL << LED_PIN),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
};

/* Task that blinks an LED every second */
void Task_1_Blink(void *pvParameters);

void app_main(void)
{
    /* Configure the LED pin */
    gpio_config(&led_pin_config);

    /* Create a task */
    xTaskCreate(&Task_1_Blink,  // Task address
        "Task_1_Blink",         // Task name
        1000,                   // Stack size in bytes
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