/**
 * @file port_oal.h
 * @brief OAL (OS Adaptation Layer) - WiFi buffer pool and memory management.
 *
 * These functions manage the WiFi packet buffer pool and network buffer
 * (netbuf/skb) infrastructure. The DMAC library uses these to allocate
 * RX/TX buffers within the dedicated WiFi packet RAM region.
 *
 * The WiFi packet RAM is defined by the linker symbols:
 *   __wifi_pkt_ram_begin__  (start)
 *   __wifi_pkt_ram_end__    (start + 48KB)
 *
 * All OAL memory operations operate within this 48KB pool.
 */

#ifndef PORT_OAL_H
#define PORT_OAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Linker Symbols (defined in your linker script) ---- */

/** Start of the 48KB WiFi packet buffer RAM region. */
extern uint8_t __wifi_pkt_ram_begin__;
/** End of the 48KB WiFi packet buffer RAM region. */
extern uint8_t __wifi_pkt_ram_end__;

/* ---- Memory Pool Configuration ---- */

/**
 * Get the zero-copy header size for network buffers.
 * This is used to align buffer data payloads.
 */
size_t oal_mem_get_zerocopy_hdr_size(void);

/**
 * Reserve memory within the WiFi packet pool.
 * Used for static allocations at init time.
 */
void *oal_mem_rsv(size_t size);

/**
 * Set the total buffer size for the network buffer pool.
 * Called once during WiFi initialization.
 */
void oal_mem_set_buf_size(size_t size);

/**
 * Set the skb (socket buffer) size.
 */
void oal_mem_set_skb_size(size_t size);

/**
 * Initialize the OAL memory pool. Must be called before any WiFi operations.
 * This carves up the 48KB WiFi packet RAM region into sub-pools.
 */
int oal_memory_init(void);

/**
 * Teardown the OAL memory pool.
 */
int oal_memory_exit(void);

/**
 * Get the total number of entries in the network buffer pool.
 */
int oal_get_netbuf_pool_len(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_OAL_H */
