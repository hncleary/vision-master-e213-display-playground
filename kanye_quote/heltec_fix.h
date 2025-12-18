// Fix for Heltec library compilation errors
// These defines are required by the library even if LoRaWAN is not used
// This header should be included before any Heltec library headers

// Define the board type - Vision Master E213
// This ensures board-config.h will define RADIO_NSS and other pins
#ifndef Vision_Master_E_213
#define Vision_Master_E_213
#endif

#ifndef HELTEC_BOARD
#define HELTEC_BOARD 0
#endif

#ifndef LoRaWAN_DEBUG_LEVEL
#define LoRaWAN_DEBUG_LEVEL 0
#endif

// RADIO_NSS should be defined by board-config.h if Vision_Master_E_213 is defined
// But define a fallback just in case
#ifndef RADIO_NSS
#define RADIO_NSS 8  // Default for Vision Master E213
#endif

#ifndef SLOW_CLK_TPYE
#define SLOW_CLK_TPYE 0
#endif

