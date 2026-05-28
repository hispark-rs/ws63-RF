/**
 * @file port_frw.h
 * @brief FRW (Framework) - WiFi message dispatch and timer system.
 *
 * The FRW layer is the WiFi internal message routing framework. It routes
 * messages between HMAC (host-side MAC) and DMAC (device-side MAC), manages
 * timer callbacks, and processes the WiFi event loop.
 *
 * Key abstraction: messages are tagged with module_id (HMAC/DMAC/HAL/OAM/etc.)
 * and dispatched through frw_send/frw_post calls. Timers are created per-module
 * and fire frw_timeout_func callbacks.
 */

#ifndef PORT_FRW_H
#define PORT_FRW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations for types defined in internal headers */
struct frw_msg;
struct frw_timeout_stru;

/* ---- Initialization ---- */

/** Initialize the FRW framework. Returns 0 on success. */
int frw_main_init(void);

/** Shutdown the FRW framework. */
int frw_main_exit(void);

/** Return current init state (0=not ready, >0=init phase). */
int frw_get_init_state(void);

/* ---- Message Management ---- */

/** Allocate a message node from the FRW message pool. Returns NULL on failure. */
struct frw_msg *frw_fetch_msg_node(void);

/** Free a previously allocated message node. */
void frw_free_msg_node(struct frw_msg *msg);

/** Post a message to the DMAC module's processing queue. */
int frw_send_msg_to_device(struct frw_msg *msg);

/** Register/unregister a DMAC message hook callback. */
int frw_dmac_msg_hook_register(void (*hook)(struct frw_msg*));
int frw_dmac_msg_hook_unregister(void (*hook)(struct frw_msg*));

/** Configure DMAC receive processing. */
int frw_dmac_rcv_cfg(void *cfg);

/** Post a received netbuf to the WiFi RX path. */
int frw_rx_netbuf(void *netbuf);

/** Post a WiFi netbuf from RX for host processing. */
int frw_rx_wifi_post_netbuf(void *netbuf);

/* ---- Timer System ---- */

typedef void (*frw_timeout_func)(unsigned long arg);

/** Initialize the DMAC timer subsystem. */
int frw_dmac_timer_init(void);

/** Shutdown the DMAC timer subsystem. */
int frw_dmac_timer_exit(void);

/** Process expired DMAC timers. Called from periodic event loop. */
void frw_dmac_timer_timeout_proc(void);

/** Generic timer timeout callback. */
void frw_timer_timeout_proc_event(unsigned long arg);

/* ---- Event Processing ---- */

/** The FRW event processing loop. Call repeatedly to service WiFi events.
 *  Returns when all pending events are processed. */
void frw_event_process_all_event_etc(void);

/** The FRW scheduling task thread. This is the main WiFi worker thread.
 *  Returns when the framework shuts down. */
void* frw_task_thread(void *arg);

/** Wait for FRW thread events. */
int frw_thread_get_wait(void);

/* ---- ROM Callbacks ---- */

/** Register a ROM callback function. */
int frw_rom_cb_register(void *cb);

/* ---- HCC Service Integration ---- */

/** Deinitialize the FRW HCC service adapter. */
int frw_hcc_service_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_FRW_H */
