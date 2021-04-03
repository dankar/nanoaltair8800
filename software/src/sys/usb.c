#include "main.h"
#include "usb.h"
#include "usbd_cdc_if.h"
#include "sys/log.h"
#include "altair8800/serial.h"

LOG_UNIT("USB");
LOG_TYPE(USB_DEBUG, OUTPUT_OFF);

extern uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

void usb_receive_callback(uint8_t* buf, uint32_t *len)
{
    serial1_add_data(buf, *len);
}

void usb_transmit_complete_callback(uint8_t *Buf, uint32_t *len, uint8_t epnum)
{

}

bool usb_transmit(const uint8_t *buf, uint16_t len)
{
    memcpy(UserTxBufferFS, buf, len);
    uint32_t start = HAL_GetTick();

    do
    {
        if(CDC_Transmit_FS(UserTxBufferFS, len) == USBD_OK)
        {
            return true;
        }
    } while((HAL_GetTick() - start) < 20);
    return true;
}