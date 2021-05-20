#include <main.h>
#include "sys/log.h"
#include "switches.h"

LOG_UNIT("GPIO");
LOG_TYPE(GPIO_DEBUG, OUTPUT_OFF);

typedef struct switch_t
{
    bool has_been_pressed;
    switch_state_t state;
    uint32_t timestamp;
    GPIO_TypeDef *gpio;
    uint16_t pin;
} switch_t;

#define SWITCH(name) [SWITCH_ ## name] = {.gpio = name ## _SW_GPIO_Port, .pin = name ## _SW_Pin}

switch_t _switches[SWITCH_NUM_SWITCHES] = 
{
    SWITCH(STOP),
    SWITCH(RUN),
    SWITCH(STEP),
    SWITCH(RESET),
    SWITCH(EXAMINE),
    SWITCH(EXAMINE_NEXT),
    SWITCH(DEPOSIT),
    SWITCH(DEPOSIT_NEXT)
};

// Convert pin to switch
switches_t pin_to_switch(uint16_t pin)
{
    switch(pin)
    {
        case STOP_SW_Pin:          return SWITCH_STOP;
        case RUN_SW_Pin:           return SWITCH_RUN;
        case STEP_SW_Pin:          return SWITCH_STEP;
        case RESET_SW_Pin:         return SWITCH_RESET;
        case EXAMINE_SW_Pin:       return SWITCH_EXAMINE;
        case EXAMINE_NEXT_SW_Pin:  return SWITCH_EXAMINE_NEXT;
        case DEPOSIT_SW_Pin:       return SWITCH_DEPOSIT;
        case DEPOSIT_NEXT_SW_Pin:  return SWITCH_DEPOSIT_NEXT;
        default:                   return SWITCH_NUM_SWITCHES;
    }
}

bool switch_settled(switches_t sw)
{
    return HAL_GetTick() - _switches[sw].timestamp > SWITCH_DEBOUNCE_MS;
}

// Get current switch state
bool switch_get(switches_t sw)
{
    return HAL_GPIO_ReadPin(_switches[sw].gpio, _switches[sw].pin) == GPIO_PIN_RESET;
}

// Interrupt callback with debounce
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switches_t sw = pin_to_switch(GPIO_Pin);
    
    if(sw == SWITCH_NUM_SWITCHES)
    {
        return;
    }
    
    if(HAL_GPIO_ReadPin(_switches[sw].gpio, _switches[sw].pin) == GPIO_PIN_RESET)
    {
        if(_switches[sw].state != SWITCH_STATE_DOWN)
        {
            _switches[sw].state = SWITCH_STATE_DOWN;
            if(switch_settled(sw))
            {
                _switches[sw].has_been_pressed = true;
            }
            _switches[sw].timestamp = HAL_GetTick();
        }
    }
    else
    {
        if(_switches[sw].state != SWITCH_STATE_UP)
        {
            _switches[sw].state = SWITCH_STATE_UP;
            _switches[sw].timestamp = HAL_GetTick();
        }
    }   
}

// Return whether the key has been pressed since the last check
bool switch_was_pressed(switches_t sw)
{
    bool result = _switches[sw].has_been_pressed;
    _switches[sw].has_been_pressed = false;
    return result;
}

// Reset any presses. This is used when entering run mode to not cause
// any switch presses done durin run mode to suddenly all have effect.
void switch_clear_all()
{
    for(int i = 0; i < SWITCH_NUM_SWITCHES; i++)
    {
        _switches[i].has_been_pressed = false;
    }
}

// Get address entered in the DIP switches
#define ADDR(x) (HAL_GPIO_ReadPin(ADDRESS_ ## x ## _GPIO_Port, ADDRESS_ ## x ## _Pin) == GPIO_PIN_RESET ? (1 << (x - 1)) : 0)
uint16_t switch_get_address()
{
    return ADDR(16) | ADDR(15) | ADDR(14) | ADDR(13) |ADDR(12) | ADDR(11) | ADDR(10) | ADDR(9) | ADDR(8) | 
        ADDR(7) | ADDR(6) | ADDR(5) | ADDR(4) |ADDR(3) | ADDR(2) | ADDR(1);
}

// Get sense switches state
bool sense(uint8_t *b)
{
    *b = switch_get_address() >> 8;
	return true;
}