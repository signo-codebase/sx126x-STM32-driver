## Structure

The driver is defined as follows:

- sx126x.c: implementation of the driver functions
- sx126x.h: declarations of the driver functions
- sx126x_regs.h: definitions of all useful registers (address and fields)
- sx126x_hal.h: declarations of the HAL functions (to be implemented by the user - see below)
- lr_fhss_mac.c: Transceiver-independent LR-FHSS implementation
- sx126x_lr_fhss.c: Transceiver-dependent LR-FHSS implementation
- lr_fhss_mac.h: Transceiver-independent LR-FHSS declarations
- sx126x_lr_fhss.h: Transceiver-dependent LR-FHSS declarations
- lr_fhss_v1_base_types.h: LR-FHSS type interface