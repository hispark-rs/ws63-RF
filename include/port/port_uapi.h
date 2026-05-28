/**
 * @file port_uapi.h
 * @brief UAPI (User API) - platform services used by WiFi/BLE libs.
 *
 * Minimal platform service layer. Only two functions are needed by the
 * closed-source libraries:
 *
 *  - uapi_nv_read()          : Read a value from non-volatile storage
 *                              (e.g., calibrated RF parameters from flash).
 *  - uapi_tsensor_get_current_temp() : Read the current temperature sensor
 *                              value for thermal protection algorithms.
 */

#ifndef PORT_UAPI_H
#define PORT_UAPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read a value from non-volatile storage.
 * The WiFi driver uses this to read calibrated RF parameters, MAC address,
 * and other persistent configuration stored in flash/eFuse.
 *
 * @param id     NV item identifier (platform-defined).
 * @param buf    Output buffer for the read data.
 * @param len    Length of data to read.
 * @return 0 on success, non-zero on failure.
 */
int uapi_nv_read(uint32_t id, void *buf, uint32_t len);

/**
 * Get the current chip temperature.
 * Used by the WiFi thermal protection algorithm (temperature-based TX power
 * backoff) and radar calibration.
 *
 * @return Temperature in Celsius (or temperature sensor code value).
 */
int uapi_tsensor_get_current_temp(void);

/**
 * Get the current system tick in milliseconds.
 * Used by BLE host stack for timeout calculations.
 *
 * @return System uptime in milliseconds.
 */
uint64_t uapi_systick_get_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_UAPI_H */
