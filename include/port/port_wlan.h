/**
 * @file port_wlan.h
 * @brief WLAN shared-memory ring buffer and RF clock control.
 *
 * These functions manage the shared-memory communication rings between the
 * application CPU (ACORE) and the device CPU (DCORE/WiFi core).
 *
 * The "msg_h2d" (host-to-device) functions send control messages through
 * shared memory descriptor rings.
 *
 * The "oal_ring" functions are lock-free circular buffers used for
 * high-throughput data transfer between cores.
 */

#ifndef PORT_WLAN_H
#define PORT_WLAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- RF / ABB Clock Control ---- */

/** Enable WiFi ABB (Analog Baseband) and RF clocks. Called before WiFi TX/RX. */
int wlan_open_wifi_abb_rf_clk(void);

/** Disable WiFi ABB and RF clocks. Called when WiFi is idle or sleeping. */
int wlan_close_wifi_abb_rf_clk(void);

/* ---- Host-to-Device Message Descriptor Rings ---- */

/** Initialize the RX descriptor ring in shared memory. */
int wlan_msg_h2d_init_rx_dscr(void);

/** Destroy/deallocate the RX descriptor ring. */
int wlan_msg_h2d_destory_rx_dscr(void);

/** Enable the WiFi front-end (RF chain). */
int wlan_msg_h2d_enable_front_end(void);

/** Prepare for idle sleep mode. */
int wlan_msg_h2d_idel_sleep_prepare(void);

/** Request synchronization of the PM (power management) bitmap to device. */
int wlan_msg_h2d_req_sync_pmbit(void);

/** Synchronize sub-work to the RF domain. */
int wlan_msg_h2d_sync_sub_work_to_rf(void);

/** Synchronize the working VAP (Virtual AP) state bitmap. */
int wlan_msg_h2d_sync_work_vap_state(void);

/* ---- Lock-Free Ring Buffer ---- */

/** Ring buffer control structure. */
typedef struct {
    void    *entries_addr;     /**< Base address of the ring buffer in shared memory. */
    uint8_t *read_idx_addr;    /**< Address of the read index (1 byte: 7-bit idx + 1-bit wrap). */
    uint8_t *write_idx_addr;   /**< Address of the write index. */
    uint16_t ring_depth;       /**< Total number of entries. */
    uint16_t ring_entry_size;  /**< Size of each entry in bytes. */
} oal_ring_ctrl;

/**
 * Write one entry to the ring buffer.
 * @return 0 on success, -1 if ring is full.
 */
int oal_ring_write(oal_ring_ctrl *ring, const void *entry);

/**
 * Read one entry from the ring buffer.
 * @return 0 on success, -1 if ring is empty.
 */
int oal_ring_read(oal_ring_ctrl *ring, void *entry);

/** Get the number of filled entries in the ring. */
uint16_t oal_get_ring_element_num(oal_ring_ctrl *ring);

#ifdef __cplusplus
}
#endif

#endif /* PORT_WLAN_H */
