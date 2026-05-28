/**
 * @file port_hcc.h
 * @brief HCC (Host-Controller Communication) - IPC message layer.
 *
 * The HCC layer provides the transport abstraction between the host CPU
 * (running HMAC/BLE host stack) and the device CPU (running DMAC/BT controller).
 *
 * On WS63, the transport is IPC (shared memory). When porting to a different
 * architecture (e.g., external MCU driving WS63 over SDIO/SPI), implement
 * the corresponding HCC bus driver instead.
 *
 * Minimum implementation for WS63-like IPC:
 *  - hcc_dmac_config_bus_ini()  : Initialize shared memory bus configuration.
 *  - hcc_msg_open_wlan()         : Open WLAN message channel.
 *  - hcc_msg_close_wlan()        : Close WLAN message channel.
 *  - hcc_wifi_msg_register()     : Register WiFi message handler.
 *  - hcc_wifi_msg_send()         : Send a WiFi message.
 *  - hcc_dmac_service_adapt_start() : Start the DMAC service adapter.
 */

#ifndef PORT_HCC_H
#define PORT_HCC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward: message payload type - defined in internal/rom/frw_msg_rom.h */
struct frw_msg;

/**
 * Initialize the DMAC bus configuration.
 * This sets up the shared memory layout for IPC communication.
 */
int hcc_dmac_config_bus_ini(void);

/**
 * Start the DMAC service adaptation layer.
 * This connects the DMAC message processing to the HCC transport.
 */
int hcc_dmac_service_adapt_start(void);

/**
 * Open a WLAN message channel.
 * @return 0 on success.
 */
int hcc_msg_open_wlan(void);

/**
 * Close the WLAN message channel.
 * @return 0 on success.
 */
int hcc_msg_close_wlan(void);

/**
 * Register a handler for inbound WLAN messages.
 * @param handler  Callback invoked when a WLAN message arrives.
 * @return 0 on success.
 */
int hcc_wifi_msg_register(void (*handler)(struct frw_msg*));

/**
 * Send a WLAN message to the device core.
 * @param msg  The message to send. Ownership transfers to HCC.
 * @return 0 on success.
 */
int hcc_wifi_msg_send(struct frw_msg *msg);

#ifdef __cplusplus
}
#endif

#endif /* PORT_HCC_H */
