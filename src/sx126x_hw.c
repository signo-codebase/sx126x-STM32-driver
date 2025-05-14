/**
 * Author: AES
 * maggio 2025
 * 
 * based on Wojciech Domski work on SX1728
 * 
 * 
 */

#include <assert.h>
#include "sx126x_hw.h"

void sx126x_hw_init(sx126x_hw_t *hw) {
	sx126x_hw_SetNSS(hw, 1);
	HAL_GPIO_WritePin(hw->nreset.port, hw->nreset.pin, GPIO_PIN_SET);
}
void sx126x_hw_SetNSS(sx126x_hw_t *hw, int value) {
	HAL_GPIO_WritePin(hw->nss.port, hw->nss.pin,
			(value == 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

BUSY_state_t sx126x_hw_BUSY_state(sx126x_hw_t *hw) {

    GPIO_PinState pin_state = HAL_GPIO_ReadPin(hw->busy.port, hw->busy.pin);

    // Dal datasheet: BUSY è alto (SET), FREE è basso (RESET)
    if (pin_state == GPIO_PIN_SET) {
        return BUSY_STATE_BUSY; // Pin alto = chip occupato
    } else {
        return BUSY_STATE_FREE; // Pin basso = chip libero
    }
}
void sx126x_hw_Reset(sx126x_hw_t *hw) {
	sx126x_hw_SetNSS(hw, 1); // alto così non interferisce con il reset

	HAL_GPIO_WritePin(hw->nreset.port, hw->nreset.pin, GPIO_PIN_RESET); // reset low

	HAL_Delay(1);   // datasheet dice 100us generalmente    

	HAL_GPIO_WritePin(hw->nreset.port, hw->nreset.pin, GPIO_PIN_SET);     // reset high

}
void sx126x_hw_SendCommand_blocking(sx126x_hw_t *hw, uint8_t cmd) {
    
	HAL_SPI_Transmit(hw->hspi, &cmd, 1, 100);   

	while (HAL_SPI_GetState(hw->hspi) != HAL_SPI_STATE_READY)
		;
}
uint8_t sx126x_hw_ReadByte_blocking(sx126x_hw_t *hw) {
	uint8_t txByte = 0x00;
	uint8_t rxByte = 0x00;

	HAL_SPI_TransmitReceive(hw->hspi, &txByte, &rxByte, 1, 100);
	while (HAL_SPI_GetState(hw->hspi) != HAL_SPI_STATE_READY)
		;
	return rxByte;
}
GPIO_PinState sx126x_hw_GetDIOx(sx126x_hw_t *hw, sx126x_DIOx_t DIO) {
    
	GPIO_PinState pin_state;

    switch (DIO) {
        case DIO1:
            pin_state = HAL_GPIO_ReadPin(hw->dio1.port, hw->dio1.pin);
            break;
        case DIO2:
            pin_state = HAL_GPIO_ReadPin(hw->dio2.port, hw->dio2.pin);
            break;
        case DIO3:
            pin_state = HAL_GPIO_ReadPin(hw->dio3.port, hw->dio3.pin);
            break;
        default:
            //! if we're here it's a profound error, but i don't know how to handle
            //! it any better, so i'll make an assert

            assert(0 && "Invalid DIO number passed to sx126x_hw_GetDIOx");
            return 0;
    }

    return pin_state;
}
