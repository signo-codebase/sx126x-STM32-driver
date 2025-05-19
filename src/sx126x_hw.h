/**
 * Author: AES
 * maggio 2025
 * 
 * based on Wojciech Domski work on SX1728
 * 
 * 
 */

#ifndef __SX1278_HW_H
#define __SX1278_HW_H

#include <stdint.h>
#include "stm32f4xx_hal.h" 		// <--- Add this line
#include "stm32xxxx_hal_spi.h"		// change for a particular stm32 microcontroller
#include "stm32xxxx_hal_gpio.h"		// change for a particular stm32 microcontroller

typedef struct sx126x_hw_pin_s{
	uint16_t pin;
	GPIO_TypeDef *port;
} sx126x_hw_pin_t;                             

typedef struct sx126x_hw_s{
	sx126x_hw_pin_t nreset;
	sx126x_hw_pin_t nss;
    sx126x_hw_pin_t busy;
    sx126x_hw_pin_t	dio1;
	sx126x_hw_pin_t dio2;
	sx126x_hw_pin_t dio3;
	SPI_HandleTypeDef *hspi;
} sx126x_hw_t;

typedef enum _DIOx {
    DIO1 = 1, DIO2 = 2, DIO3 = 3
} sx126x_DIOx_t;

typedef enum BUSY_state_e {
	BUSY_STATE_FREE = 0, BUSY_STATE_BUSY = 1
} BUSY_state_t;


/**
 * \brief Initialize hardware layer
 *
 * Clears NSS and resets LoRa module.
 *
 * \param[in]   hw 		Pointer to hardware structure
 */
void sx126x_hw_init(sx126x_hw_t *hw);

/**
 * \brief Control NSS
 *
 * Clears and sets NSS according to passed value.
 *
 * \param[in]   hw 		Pointer to hardware structure.
 * \param[in]   value   1 sets NSS high, other value sets NSS low.
 */
void sx126x_hw_SetNSS(sx126x_hw_t *hw, int value);

/**
 * \brief Get BUSY line state
 *
 * If BUSY is SET, then wait. If BUSY is reset, a command can be send.
 *
 * \param[in]    hw 		Pointer to hardware structure.	      
 */
BUSY_state_t sx126x_hw_BUSY_state(sx126x_hw_t *hw);

/**
 * \brief Resets LoRa module
 *
 * Resets LoRa module.
 *
 * \param[in]   hw 		Pointer to hardware structure
 */
void sx126x_hw_Reset(sx126x_hw_t *hw);

/**
 * \brief Send command via SPI in an incredibly BLOCKING way
 *
 * Send single byte via SPI interface.
 *
 * \param[in]   hw 		Pointer to hardware structure
 * \param[in]   cmd		Command
 */
void sx126x_hw_SendCommand_blocking(sx126x_hw_t *hw, uint8_t cmd);

/**
 * \brief Reads data via SPI
 *
 * Reads data via SPI interface.
 *
 * \param[in]   hw 		Pointer to hardware structure
 *
 * \return				Read value
 */
uint8_t sx126x_hw_ReadByte_blocking(sx126x_hw_t *hw);

/**
 * \brief ms delay
 *
 * Milisecond delay.
 *
 * \param[in]   msec 		Number of milliseconds to wait
 */
void sx126x_hw_DelayMs(uint32_t msec);

/**
 * \brief Reads DIO0 state
 *
 * Reads LoRa DIO0 state using GPIO.
 *
 * \param[in]   hw 		Pointer to hardware structure
 *
 * \return				0 if DIO0 low, 1 if DIO high
 */
GPIO_PinState sx126x_hw_GetDIOx(sx126x_hw_t *hw, sx126x_DIOx_t DIO);


// TODO: sx126x_set_dio2_as_rf_sw_ctrl()


#endif
