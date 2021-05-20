#include "spi.h"
#include "log.h"

LOG_UNIT("SPI");
LOG_TYPE(SPI_DEBUG, OUTPUT_OFF);

uint8_t led_data[4] __attribute__((aligned (4)));

// Start an update of the shift registers by pulling the LATCH line low
// and then sending the data over SPI via DMA. The LATCH line will be pulled
// high again in the DMA completion callbacks, triggering the actual
// update of the LEDs.
bool SPI_write_leds(uint16_t address, uint8_t data, uint8_t status)
{
    led_data[0] = status;
    led_data[1] = data;
    led_data[2] = (address >> 8) & 0xff;
    led_data[3] = address & 0xff;

    HAL_GPIO_WritePin(LED_LATCH_GPIO_Port, LED_LATCH_Pin, GPIO_PIN_RESET);
    return HAL_SPI_Transmit_DMA(&hspi1, led_data, 4) == HAL_OK;
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi1)
    {
        HAL_GPIO_WritePin(LED_LATCH_GPIO_Port, LED_LATCH_Pin, GPIO_PIN_SET);
    }    
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if(hspi == &hspi1)
    {
        HAL_GPIO_WritePin(LED_LATCH_GPIO_Port, LED_LATCH_Pin, GPIO_PIN_SET);
    }    
}

void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi)
{

}