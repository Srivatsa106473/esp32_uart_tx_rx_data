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
#include "esp_err.h"
#include "esp_log.h"

// setup UART buffered IO with event queue
const int uart_buffer_size = (1024*2);
QueueHandle_t uart_queue;
uint8_t data[128];
const char* expected_data = "test break";
static const char *TAG = "UART_TEST";

esp_err_t err;

void setting_communication_parameters()
{
    // DATA_BITS
    err=uart_set_word_length(UART_NUM, DATA_BITS);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"Setting DATA_BITS failed: %s in setting_communication_parameters()", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG,"DATA_BITS set successfully in setting_communication_parameters() !");
    }


    // BAUD_RATE
    err = uart_set_baudrate(UART_NUM, UART_BAUDRATE);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "Setting Baudrate failed: %s in setting_communication_parameters()", esp_err_to_name(err));
    }
    else if(UART_BAUDRATE != 115200)
    {
        ESP_LOGE(TAG, "setting baudrate failed %d",UART_BAUDRATE);
    }
    else
    {
        ESP_LOGI(TAG," Baudrate set successfully to %d!",UART_BAUDRATE);
    }


    // PARITY_BITS
    err = uart_set_parity(UART_NUM, PARITY_MODE);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG," setting Parity bits failed: %s in setting_communication_parameters()", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG," setting parity_bits successfully ");
    }


    // STOP_BITS
    err= uart_set_stop_bits(UART_NUM, STOP_BITS);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "setting STOP_BITS failed: %s in setting_communication_parameters()", esp_err_to_name(err));
    }
    else
    {
        ESP_LOGI(TAG, " setting STOP_BITS successfully");
    }
}

void setting_communication_pins()
{
    err = uart_set_pin(UART_NUM, GPIO_TX, GPIO_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "uart_set_pin() not set successfully");
    }
    else
    {
        ESP_LOGI(TAG, "uart_pins setr successfully...");
    }
}

void driver_installation()
{
    err = uart_driver_install(UART_NUM, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0); 
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "uart driver installation failed !");
    }
    else
    {
        ESP_LOGI(TAG, "uart driver installation success..");
    }
}

void communication_mode_selection()
{
    err = uart_set_mode(UART_NUM, UART_MODE);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "UART mode not set successfully");
    }
    else
    {
        ESP_LOGI(TAG, "UART mode set to UART_MODE_UART");
    }
}

// preparing the data for transmission by taking reference from technical docs
// Write data to UART.
void write_Tx_buffer()
{
    // Write data to UART
    const char *test_str = "test break"; // String to transmit
    err = uart_write_bytes(UART_NUM, test_str, strlen(test_str)); // Send string with break signal

    if(err != ESP_OK && uart_buffer_size<1)
    {
        ESP_LOGE(TAG, "data: %s, transmission buffer ERROR!", test_str);
    }
    else
    {
        ESP_LOGI(TAG, "Data transmitted successfully....");
    }
}


void read_rx_buffer()
{
    // Read data from UART.
    int length = uart_read_bytes(UART_NUM, data, sizeof(data), 100);
    if (length > 0)
    {
        data[length] = '\0'; // Null-terminate the string
        ESP_LOGI(TAG, "Data received: %s", data);
    }
    else
    {
        ESP_LOGE(TAG, "No data received.");
    }
}

void unity_run_menu()
{
    ESP_LOGI(TAG, "Running UART test...");

    write_Tx_buffer();
    vTaskDelay(100/portTICK_PERIOD_MS);
    read_rx_buffer();
    vTaskDelay(100/portTICK_PERIOD_MS);

    if (strcmp((char *)data, expected_data) == 0)
    {
        ESP_LOGI(TAG, "UART test PASSED.");
    }
    else
    {
        ESP_LOGE(TAG, "UART test FAILED. Expected: %s, Got: %s", expected_data, data);
    }

    TEST_ASSERT_EQUAL_STRING((char*)expected_data,(char*) data);
    printf("expected data is : %s and sent data is :%s\n",expected_data,data);
    fflush(stdout);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting UART setup...");
    esp_log_level_set("*", ESP_LOG_DEBUG);

    setting_communication_parameters();
    setting_communication_pins();
    driver_installation();
    communication_mode_selection();
    UNITY_BEGIN();
    RUN_TEST(unity_run_menu);
    UNITY_END();
}