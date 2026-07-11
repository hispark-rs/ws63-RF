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
 * @param key          NV item identifier (platform-defined).
 * @param max_len      Capacity of the output buffer.
 * @param actual_len   Actual number of bytes returned.
 * @param value        Output buffer for the value.
 * @return 0 on success, non-zero on failure.
 */
uint32_t uapi_nv_read(uint16_t key, uint16_t max_len, uint16_t *actual_len, uint8_t *value);

/**
 * Write a value to non-volatile storage.
 *
 * @param key    NV item identifier (platform-defined).
 * @param value  Value to persist.
 * @param len    Number of bytes in value.
 * @return 0 on success, non-zero on failure.
 */
uint32_t uapi_nv_write(uint16_t key, const uint8_t *value, uint16_t len);

/**
 * Get the current chip temperature.
 * Used by the WiFi thermal protection algorithm (temperature-based TX power
 * backoff) and radar calibration.
 *
 * @param temp  Output temperature in degrees Celsius.
 * @return 0 on success, non-zero on failure.
 */
uint32_t uapi_tsensor_get_current_temp(int8_t *temp);

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
