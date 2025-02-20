/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "user_defines.h"
#include "string.h"

// setup UART buffered IO with event queue
const int uart_buffer_size = (1024*2);
QueueHandle_t uart_queue;
uint8_t data[128];
const char* expected_data = "test break";

void setting_communication_parameters()
{
    uart_set_baudrate(UART_NUM, 115200);
    uart_set_word_length(UART_NUM, DATA_BITS);
    uart_set_parity(UART_NUM, PARITY_MODE);
    uart_set_stop_bits(UART_NUM, STOP_BITS);
}

void setting_communication_pins()
{
    uart_set_pin(UART_NUM, GPIO_TX, GPIO_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void driver_installation()
{
    uart_driver_install(UART_NUM, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0); 
}

void communication_mode_selection()
{
    uart_set_mode(UART_NUM, UART_MODE);
}

// preparing the data for transmission by taking reference from technical docs
// Write data to UART.
void write_Tx_buffer()
{
    // char* test_str = "This is a test string.\n";
    // if(uart_buffer_size>0)
    // {
    //     uart_write_bytes(UART_NUM, (const char*)test_str,strlen(test_str));
    // }


    // Write data to UART, end with a break signal.
    const char *test_str = "test break"; // String to transmit
    uart_write_bytes(UART_NUM, test_str, strlen(test_str)); // Send string with break signal
}


void read_rx_buffer()
{
    // Read data from UART.
    
    // ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_NUM, (size_t*)&length));
    int length = uart_read_bytes(UART_NUM, data, sizeof(data), 100);
    data[length]='\0';
    printf("sending data: %s\n",data);
}

void unity_run_menu()
{
    write_Tx_buffer();
    vTaskDelay(100/portTICK_PERIOD_MS);
    read_rx_buffer();
    vTaskDelay(100/portTICK_PERIOD_MS);
    TEST_ASSERT_EQUAL_STRING((char*)expected_data,(char*) data);
    printf("expected data is : %s and sent data is :%s\n",expected_data,data);
    fflush(stdout);
}

void app_main(void)
{
    setting_communication_parameters();
    setting_communication_pins();
    driver_installation();
    communication_mode_selection();
    UNITY_BEGIN();
    RUN_TEST(unity_run_menu);
    UNITY_END();
}