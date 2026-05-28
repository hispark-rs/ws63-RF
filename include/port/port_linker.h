/**
 * @file port_linker.h
 * @brief Linker symbols and memory layout declarations.
 *
 * The WS63 closed-source libraries expect specific linker symbols and
 * memory regions to be defined. Your linker script must provide these.
 *
 * Memory layout (WS63-like system with TCM/SRAM):
 *
 *   ITCM (Instruction TCM)  : 0x00100000 - 0x0017FFFF (512KB)
 *   DTCM (Data TCM)         : 0x00180000 - 0x001C7FFF (288KB)
 *   SHARE_RAM (SRAM)        : 0x00A00000 - 0x00A98FFF (612KB)
 *     ├── WiFi Packet RAM   : __wifi_pkt_ram_begin__ .. __wifi_pkt_ram_end__
 *     │   (48KB reserved for WiFi DMA buffers)
 *     ├── SRAM text         : __sram_text_begin__ .. __sram_text_end__
 *     ├── SRAM data         : __data_begin__ .. __data_end__
 *     └── SRAM BSS          : __bss_begin__ .. __bss_end__
 *   NOR_FLASH               : 0x00200000 - 0x009FFFFF (8MB)
 *
 * For a non-WS63 target, you can allocate these regions in your target's
 * system RAM. The WiFi libraries only need the regions to exist - they don't
 * require specific physical addresses as long as the linker resolves the
 * symbols correctly.
 */

#ifndef PORT_LINKER_H
#define PORT_LINKER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- WiFi Packet RAM (48KB, must be in shared/uncached memory) ---- */
extern uint8_t __wifi_pkt_ram_begin__;
extern uint8_t __wifi_pkt_ram_end__;

/* ---- ROM Data (copied from flash to RAM at boot) ---- */
extern uint8_t __rom_data_begin__;
extern uint8_t __rom_data_load__;
extern uint8_t __rom_data_size__;
extern uint8_t __rom_bss_begin__;
extern uint8_t __rom_bss_end__;

/* ---- ROM Patch Table ---- */
extern uint8_t __rom_patch_begin__;
extern uint8_t __rom_patch_load__;

/* ---- TCM Text (instructions in ITCM) ---- */
extern uint8_t __tcm_text_begin__;
extern uint8_t __tcm_text_end__;
extern uint8_t __tcm_text_load__;

/* ---- TCM Data (data in DTCM) ---- */
extern uint8_t __tcm_data_begin__;
extern uint8_t __tcm_data_end__;
extern uint8_t __tcm_data_load__;
extern uint8_t __tcm_bss_begin__;
extern uint8_t __tcm_bss_end__;

/* ---- SRAM Text (WiFi code in shared SRAM) ---- */
extern uint8_t __sram_text_begin__;
extern uint8_t __sram_text_end__;
extern uint8_t __sram_text_load__;

/* ---- General Data/BSS (SRAM) ---- */
extern uint8_t __data_begin__;
extern uint8_t __data_end__;
extern uint8_t __data_load__;
extern uint8_t __bss_begin__;
extern uint8_t __bss_end__;

/* ---- libgcc helpers ---- */
int64_t __divdi3(int64_t a, int64_t b);
uint64_t __udivdi3(uint64_t a, uint64_t b);

#ifdef __cplusplus
}
#endif

#endif /* PORT_LINKER_H */
