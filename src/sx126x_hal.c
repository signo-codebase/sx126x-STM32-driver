/**
 * Author: AES
 * maggio 2025
 * 
 * based on Wojciech Domski work on SX1728
 * 
 * 
 */

#include "sx126x_hal.h"


sx126x_hal_status_t sx126x_hal_wait_on_busy(const sx126x_hw_t *hw, uint32_t timeout_ms) {

    uint32_t start_tick = HAL_GetTick();

    while (sx126x_hw_BUSY_state(hal_context) == BUSY_STATE_BUSY) {
        
        if ((HAL_GetTick() - start_tick) > timeout_ms) {
            
            return SX126X_HAL_STATUS_ERROR;
        }
        // HAL_Delay(1);
    }

    // Se il loop è terminato, significa che il pin BUSY è diventato libero
    return SX126X_HAL_STATUS_OK;
}