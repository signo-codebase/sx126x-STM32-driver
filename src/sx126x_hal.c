/**
 * Author: AES
 * maggio 2025
 *
 * based on Wojciech Domski work on SX1728
 *
 */

#include "sx126x_hal.h"
#include "sx126x_hw.h" 

// Definizione del timeout per le operazioni SPI, se non già definito altrove
#ifndef HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS
#define HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS 100
#endif

#define SX126X_HAL_TIMEOUT 20 // Millisecondi per sx126x_hal_wait_on_busy

sx126x_hal_status_t sx126x_hal_wait_on_busy( const void *context, uint32_t timeout_ms ) {
    const sx126x_hw_t *hw = (const sx126x_hw_t*) context;
    uint32_t start_tick = HAL_GetTick( );

    while( sx126x_hw_BUSY_state( (sx126x_hw_t*) hw ) == BUSY_STATE_BUSY ) {
        if( ( HAL_GetTick( ) - start_tick ) > timeout_ms ) {
            return SX126X_HAL_STATUS_ERROR;
        }

        // HAL_Delay(1);
    }
    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {
    sx126x_hw_t *hw = (sx126x_hw_t*) context;
    HAL_StatusTypeDef spi_status;

    if( sx126x_hal_wait_on_busy( hw, SX126X_HAL_TIMEOUT ) != SX126X_HAL_STATUS_OK ) {
        return SX126X_HAL_STATUS_ERROR;
    }

    sx126x_hw_SetNSS( hw, 0 ); // start communication

    // send command buffer
    if( command_length > 0 ) {
        spi_status = HAL_SPI_Transmit( hw->hspi, (uint8_t*)command, command_length, HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS );
        if( spi_status != HAL_OK ) {
            sx126x_hw_SetNSS( hw, 1 ); // Alza NSS in caso di errore
            return SX126X_HAL_STATUS_ERROR;
        }
    }

    // send data buffer if there is any
    if( data_length > 0 ) {
        spi_status = HAL_SPI_Transmit( hw->hspi, (uint8_t*)data, data_length, HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS );
        if( spi_status != HAL_OK ) {
            sx126x_hw_SetNSS( hw, 1 ); // Alza NSS in caso di errore
            return SX126X_HAL_STATUS_ERROR;
        }
    }

    sx126x_hw_SetNSS( hw, 1 ); // Alza NSS per terminare la comunicazione

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
    sx126x_hw_t *hw = (sx126x_hw_t*) context;
    HAL_StatusTypeDef spi_status;

    if( sx126x_hal_wait_on_busy( hw, SX126X_HAL_TIMEOUT ) != SX126X_HAL_STATUS_OK ) {
        return SX126X_HAL_STATUS_ERROR;
    }

    sx126x_hw_SetNSS( hw, 0 ); // Abbassa NSS

    // Invia il buffer dei comandi
    // Per la lettura, alcuni dispositivi richiedono un byte NOP (0x00) dopo il comando prima di restituire i dati.
    // sx126x.c usa un NOP dopo il comando e l'indirizzo per la lettura dei registri.
    // La funzione sx126x_hal_read di Semtech prevede l'invio del comando e poi la ricezione dei dati.
    // Se il comando include già il byte NOP richiesto dal chip per iniziare a inviare dati, va bene così.
    // Altrimenti, potrebbe essere necessario inviare byte NOP aggiuntivi.

    if( command_length > 0 ) {
        spi_status = HAL_SPI_Transmit( hw->hspi, (uint8_t*)command, command_length, HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS );
        if( spi_status != HAL_OK ) {
            sx126x_hw_SetNSS( hw, 1 );
            return SX126X_HAL_STATUS_ERROR;
        }
    }

    // Ricevi i dati
    if( data_length > 0 ) {
        spi_status = HAL_SPI_Receive( hw->hspi, data, data_length, HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS );
        if( spi_status != HAL_OK ) {
            sx126x_hw_SetNSS( hw, 1 );
            return SX126X_HAL_STATUS_ERROR;
        }
    }

    sx126x_hw_SetNSS( hw, 1 ); // Alza NSS

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context ) {
    sx126x_hw_t *hw = (sx126x_hw_t*) context;
    sx126x_hw_Reset( hw ); 
    HAL_Delay(5);
    return sx126x_hal_wait_on_busy(hw, SX126X_HAL_TIMEOUT); // Attendi che il chip sia pronto dopo il reset
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context ) {
    sx126x_hw_t *hw = (sx126x_hw_t*) context;

    sx126x_hw_SetNSS( hw, 0 );
    // Non è necessario inviare un comando specifico qui, il fronte di discesa di NSS è sufficiente
    // per svegliare il chip se era in sleep mode e si aspettava un wakeup SPI.
    // Tuttavia, il driver sx126x.c invia un comando GET_STATUS (0xC0) seguito da un NOP.
    // Per coerenza con il driver sx126x.c, inviamo un NOP (o un comando GetStatus).
    // Il datasheet (8.2.2 SPI Timing When the Transceiver Leaves Sleep Mode) indica che
    // il BUSY va alto e poi basso quando il chip è pronto.
    uint8_t get_status_cmd[] = { 0xC0 }; // GET_STATUS opcode
    HAL_SPI_Transmit( hw->hspi, get_status_cmd, 1, HAL_SPI_TIMEOUT_DEFAULT_VALUE_MS);
    sx126x_hw_SetNSS( hw, 1 );

    return sx126x_hal_wait_on_busy( hw, SX126X_HAL_TIMEOUT );
}


// TODO functions for DIOx implementations