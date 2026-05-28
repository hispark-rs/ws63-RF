/**
 * @file port_osal.h
 * @brief OS Abstraction Layer - porting interface for WiFi/BLE closed-source libs.
 *
 * Implement these functions to adapt the WS63 WiFi/BLE libraries to your
 * target RTOS or bare-metal environment.
 *
 * All functions return 0 on success, non-zero on failure unless otherwise noted.
 */

#ifndef PORT_OSAL_H
#define PORT_OSAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Types ---- */

typedef uint8_t  osal_u8;
typedef uint16_t osal_u16;
typedef uint32_t osal_u32;
typedef uint64_t osal_u64;
typedef int8_t   osal_s8;
typedef int16_t  osal_s16;
typedef int32_t  osal_s32;
typedef int64_t  osal_s64;
typedef void*    osal_handle;
typedef void     osal_void;
typedef int      osal_bool;

#define OSAL_TRUE  1
#define OSAL_FALSE 0
#define OSAL_OK    0
#define OSAL_NOK   1
#define OSAL_SYS_WAIT_FOREVER 0xFFFFFFFF

/* ---- Interrupt Management ---- */

/**
 * Request an IRQ. handler is called with (irq, arg) when the interrupt fires.
 * Returns 0 on success.
 */
int osal_irq_request(uint32_t irq, void (*handler)(uint32_t, void*), void *arg);

/** Release a previously requested IRQ. */
int osal_irq_free(uint32_t irq, void *arg);

/** Enable the specified IRQ. */
int osal_irq_enable(uint32_t irq);

/** Disable the specified IRQ. */
int osal_irq_disable(uint32_t irq);

/**
 * Lock interrupts and return previous lock state. Use with osal_irq_restore().
 * Returns the previous interrupt mask/state to be passed to osal_irq_restore.
 */
unsigned long osal_irq_lock(void);

/** Restore interrupts to previous state. */
void osal_irq_restore(unsigned long state);

/* ---- Thread/Kernel Thread ---- */

/** Thread function signature. arg is the user argument passed at creation. */
typedef void* (*osal_kthread_func)(void *arg);

/**
 * Create a kernel-level thread.
 * @param thread_func  Thread entry function.
 * @param arg          Argument passed to thread_func.
 * @param stack_size   Stack size in bytes.
 * @param priority     Thread priority (platform-specific scale).
 * @param name         Thread name (for debug).
 * @return Thread handle, or NULL on failure.
 */
osal_handle osal_kthread_create(osal_kthread_func thread_func, void *arg,
                                size_t stack_size, int priority, const char *name);

/** Destroy/terminate a kernel thread. */
int osal_kthread_destroy(osal_handle thread);

/** Acquire thread lock (prevent preemption/scheduling). */
int osal_kthread_lock(osal_handle thread);

/** Release thread lock. */
int osal_kthread_unlock(osal_handle thread);

/** Set thread priority. */
int osal_kthread_set_priority(osal_handle thread, int priority);

/* ---- Memory Management ---- */

/** Allocate kernel memory. Semantics: non-pageable, zero-initialized. */
void *osal_kmalloc(size_t size);

/** Free memory allocated by osal_kmalloc. */
void osal_kfree(void *ptr);

/* ---- Wait/Signal ---- */

/** Opaque wait object handle. */
typedef void* osal_wait_handle;

/** Create a wait object. */
osal_wait_handle osal_wait_init(void);

/** Destroy a wait object. */
int osal_wait_destroy(osal_wait_handle wait);

/** Wake up a wait object (signal). */
int osal_wait_wakeup(osal_wait_handle wait);

/* ---- Delay ---- */

/** Busy-wait or sleep for @p usec microseconds. */
void osal_udelay(uint32_t usec);

/* ---- Cache ---- */

/** Flush (clean + invalidate) a data cache range. */
void osal_flush_cache(void *addr, size_t size);

/* ---- Debug Print ---- */

/** Debug printf. Implement with your platform's logging mechanism. */
int osal_printk(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* PORT_OSAL_H */
