#ifndef _USB_H_
#define _USB_H_

#include <stdbool.h>
#include "sys/buffer.h"

void usb_receive_callback(uint8_t* buf, uint32_t *len);
void usb_transmit_complete_callback(uint8_t *Buf, uint32_t *Len, uint8_t epnum);
bool usb_transmit(const uint8_t * buf, uint16_t len);

#endif