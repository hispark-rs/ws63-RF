/**
 * @file port_log.h
 * @brief Logging and safe C library functions for WiFi/BLE diagnostics.
 *
 * The closed-source libraries call log_event_wifi_print*() to emit
 * diagnostic messages, and safe C functions (memset_s, memcpy_s, snprintf_s)
 * for bounded memory operations.
 */

#ifndef PORT_LOG_H
#define PORT_LOG_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * WiFi event/diagnostic print functions (printf-style).
 * Numbered variants correspond to log levels:
 *   0 = Error/Fatal
 *   1 = Warning
 *   2 = Info
 *   4 = Verbose/Trace
 */
int log_event_wifi_print0(const char *fmt, ...);
int log_event_wifi_print1(const char *fmt, ...);
int log_event_wifi_print2(const char *fmt, ...);
int log_event_wifi_print4(const char *fmt, ...);

/** Safe snprintf with bounds checking. */
int snprintf_s(char *buf, size_t size, const char *fmt, ...);

/** Safe memset with bounds checking. */
int memset_s(void *dest, size_t dest_max, int c, size_t count);

/** Safe memcpy with bounds checking. */
int memcpy_s(void *dest, size_t dest_max, const void *src, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* PORT_LOG_H */
