/*******************************************************************************************
 * Class: EEL5862
 * Author: Hector Pena
 * Assignment No: 02
 * Date: September 2026
 * Description: Blinks an LED on GPIO2 on/off every 1 second.
*******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"
#include "driver/uart.h"

const uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
};

uint32_t i;
float fi;
char str[] = "                    ";

/* Task that prints out converted numbers from UART input */
void Task_1(void *pvParameters);

/* Task that prints a message every second */
void Task_2(void *pvParameters);

/* Helper to initialize the UART0 component*/
void uart0_init(void);

/* Helper to convert a string to integer and float numbers */
int32_t convert_string_to_numbers(char *str);

/* Helper to read input from UART */
int32_t read_uart_input(void);

void app_main(void)
{
    uart0_init();

    /* Create blinky task */
    xTaskCreate(&Task_1,        // Task address
        "Task_1",               // Task name
        4000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        NULL);                  // Handle
    
    /* Create hello world task */
    xTaskCreate(&Task_2,        // Task address
        "Task_2",               // Task name
        4000,                   // Stack size in bytes
        NULL,                   // Parameters
        1,                      // Priority
        NULL);                  // Handle
}

void Task_1(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        int32_t ret = read_uart_input();

        if(ret == 0)
        {
            /* Print message to UART */
            printf("Task 1, i = %ld, fi = %f\n", i, fi);
        }
    }
}

void Task_2(void *pvParameters)
{
    /* Infinite loop */
    while(1)
    {
        /* Print message to UART */
        printf("Task 2.\n");

        /* Wait 100 milliseconds */
        vTaskDelay(pdMS_TO_TICKS(100));
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