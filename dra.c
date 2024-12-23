/*
 -- dra.c
 -- DRA818V/U Library for Flipper Zero
 -- Tyler H. Jones - inquirewue@gmail.com
*/

#include <flipper.h>
#include <furi.h>
#include <gpio.h>
#include <spi.h>

#define DRA818_CS_PIN  GPIO_PIN_0 // Chip Select pin for DRA818
#define DRA818_RST_PIN GPIO_PIN_1 // Reset pin for DRA818
#define DRA818_INT_PIN GPIO_PIN_2 // Interrupt pin for DRA818 (if applicable)

#define SPI_SPEED 1000000 // SPI speed (1 MHz for example)

SPI_HandleTypeDef hspi1; // SPI handler

void dra818_spi_init() {
    // SPI configuration structure
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;

    if(HAL_SPI_Init(&hspi1) != HAL_OK) {
        FURI_LOG(FURI_LOG_ERROR, "SPI Init failed!");
    }

    // Configure GPIO pins for DRA818
    gpio_init(DRA818_CS_PIN, GPIO_MODE_OUTPUT);
    gpio_init(DRA818_RST_PIN, GPIO_MODE_OUTPUT);
    gpio_init(DRA818_INT_PIN, GPIO_MODE_INPUT);
}

void dra818_select() {
    gpio_set(DRA818_CS_PIN, 0); // Set CS low to select DRA818
}

void dra818_deselect() {
    gpio_set(DRA818_CS_PIN, 1); // Set CS high to deselect DRA818
}

void dra818_reset() {
    gpio_set(DRA818_RST_PIN, 0); // Reset DRA818 (low)
    furi_delay_ms(100); // Wait for reset to complete
    gpio_set(DRA818_RST_PIN, 1); // Release reset (high)
    furi_delay_ms(100);
}

uint8_t dra818_send(uint8_t data) {
    uint8_t received_data;
    HAL_SPI_TransmitReceive(&hspi1, &data, &received_data, 1, 100);
    return received_data;
}

void dra818_write(uint8_t reg, uint8_t value) {
    dra818_select();
    dra818_send(reg); // Send the register address
    dra818_send(value); // Send the value to write to the register
    dra818_deselect();
}

uint8_t dra818_read(uint8_t reg) {
    dra818_select();
    dra818_send(reg); // Send the register address
    uint8_t data = dra818_send(0x00); // Read the data from the register
    dra818_deselect();
    return data;
}

void dra818_init() {
    dra818_reset(); // Perform hardware reset

    // Set default frequency (example: 433 MHz)
    dra818_write(0x00, 0x57); // Set frequency registers according to datasheet

    // Set mode (transmit/receive, etc.)
    dra818_write(0x01, 0x80); // Example register for setting mode (check datasheet)

    // Set modulation, power, etc.
    dra818_write(0x02, 0x02); // Example modulation configuration

    // Set power level
    dra818_write(0x03, 0x0F); // Example power setting

    // Enable or disable other features like filters, squelch, etc.
    dra818_write(0x04, 0x00); // Example filter setting
}

void dra818_transmit(uint8_t data) {
    dra818_select();
    dra818_send(0x00); // Send the transmit register address
    dra818_send(data); // Transmit the data
    dra818_deselect();
}

uint8_t dra818_receive() {
    dra818_select();
    dra818_send(0x01); // Send the receive register address
    uint8_t data = dra818_send(0x00); // Read received data
    dra818_deselect();
    return data;
}
