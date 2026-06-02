# WS63 RF - WiFi/BLE Closed-Source Libraries

Precompiled RISC-V static libraries extracted from the HiSilicon WS63 SDK,
with porting stub interfaces for adaptation to any target environment.

## Architecture

```
┌────────────────────────────────────────────────────┐
│  Application                                       │
│  (your code using public API headers)              │
├────────────────────────────────────────────────────┤
│  libbt_host.a + libbt_app.a     (BLE/SLE Host)    │
│  libbth_gle.a                   (SLE/GLE Host)    │
│  libbg_common.a                 (BT Common)       │
│  libbth_sdk.a                   (BT SDK)          │
├────────────────────────────────────────────────────┤
│  libwifi_driver_dmac.a          (WiFi DMAC + HAL) │
│  libwifi_rom_data.a             (WiFi ROM Data)   │
├────────────────────────────────────────────────────┤
│  Porting Layer (you implement these interfaces)    │
│  ┌───────────┬──────────┬──────────┬───────────┐  │
│  │ port_osal │ port_frw │ port_hcc │ port_wlan │  │
│  │ port_oal  │ port_uapi│ port_log │port_linker│  │
│  └───────────┴──────────┴──────────┴───────────┘  │
├────────────────────────────────────────────────────┤
│  RTOS / Hardware                                   │
│  (FreeRTOS, Zephyr, Linux, bare-metal, etc.)       │
└────────────────────────────────────────────────────┘
```

The WS63 is a **single-core** RISC-V SoC — one application CPU (verified against
the fbb_ws63 SDK: "one self-developed RISC-V main CPU"; only an `acore` ROM
config; no `dcore` anywhere). The closed-source `.a` libraries contain the entire
WiFi MAC protocol stack (HMAC + DMAC) and the BLE/SLE host stack, all **linked
into the single application image** and running on that one core.

The HCC layer is HiSilicon's generic host↔device transport abstraction. Two
physical CPUs exist only in the *external-host* topology (a host MCU driving WS63
as a module over SDIO/SPI). On a standalone WS63, HMAC (upper/host MAC) and DMAC
(lower/device MAC) are a *software* split on the one core, and HCC is an on-chip
software/MAC-hardware message path — not a second RISC-V core.

**Key insight (verified by `nm` against the C SDK):** `libwifi_driver_dmac.a`
has ~1080 undefined symbols, and they are **almost all obtainable** — a full
link does NOT require reverse-engineering the radio:

- **~422 are WS63 mask-ROM functions** (`fe_*` RF front-end, `hal_machw_*`,
  `hal_al_rx_*`, `hal_btcoex_*`, …). They live at fixed addresses in the on-chip
  ROM; the C SDK resolves them via a ROM **symbol-address table**. That table is
  now shipped here as [`rom/ws63_acore_rom.lds`](rom/ws63_acore_rom.lds) (3752
  symbols, e.g. `fe_initialize_rf_dev = 0x12825e;`) — link with
  `-T rom/ws63_acore_rom.lds`. (These addresses only execute on real silicon.)
- **~618 are defined by other vendor WiFi `.a` libs** — `libwifi_driver_hmac.a`
  (host-MAC + public Wi-Fi API), `libwifi_driver_tcm.a`, the `libwifi_alg_*.a`.
  These were omitted by the original extraction and are **now also vendored in
  `lib/`** (see `LIB_EXTRACT.md`). The open hostap `libwpa_supplicant.a` (WPA
  auth) is deliberately **not** bundled — see `LIB_EXTRACT.md`.
- **~40 are the runtime's job**: the documented `port_*.h` porting contract
  (`osal_*`/`oal_*`/`log_*`/`uapi_*`), the 64-bit compiler-rt builtins
  (`__udivdi3` …), the `__wifi_pkt_ram_*` linker symbols, and the two ROM-data
  globals `g_dmac_alg_main`/`g_mac_res_etc` (defined by no lib — the runtime
  supplies them).

**So with `rom/ws63_acore_rom.lds` + the full WiFi `.a` set + a porting layer,
the blob's symbol surface closes.** The earlier wording here ("all hal_*
self-contained in dmac.a") was inaccurate; an earlier extraction also omitted
the ROM table and several WiFi `.a` libs — see `LIB_EXTRACT.md`.

## Library Catalog

| Library | Size | Description |
|---------|------|-------------|
| `libwifi_driver_dmac.a` | 614 KB | WiFi device MAC + HAL + RF front-end control |
| `libwifi_driver_hmac.a` | 32 MB | WiFi host MAC + public Wi-Fi API (`wifi_*`) |
| `libwifi_driver_tcm.a` | 6 MB | TCM-resident WiFi driver code |
| `libwifi_alg_*.a` (5) | ~2.5 MB | rate/CCA/EDCA/TxBF/temp-protect algorithms |
| `libwifi_rom_data.a` | 3 KB | WiFi ROM data segment |
| `rom/ws63_acore_rom.lds` | 144 KB | mask-ROM symbol table (3752 symbols; link `-T`) |
| `libbt_host.a` | 1.1 MB | BLE host protocol stack (GAP/GATT/SMP/L2CAP) |
| `libbt_app.a` | 286 KB | BLE application layer (service init, connection mgmt) |
| `libbth_gle.a` | 821 KB | SLE/GLE (StarLight) host protocol stack |
| `libbth_sdk.a` | 90 KB | BT host SDK utilities |
| `libbg_common.a` | 219 KB | BT common utilities |

## Directory Structure

```
ws63-RF/
├── README.md
├── lib/                          # Precompiled RISC-V .a files
│   ├── libwifi_driver_dmac.a     # WiFi device MAC + HAL + RF front-end
│   ├── libwifi_driver_hmac.a     # WiFi host MAC + public wifi_* API
│   ├── libwifi_driver_tcm.a      # TCM-resident WiFi driver code
│   ├── libwifi_alg_*.a           # 5 algo libs: anti_interference/cca_opt/edca_opt/temp_protect/txbf
│   ├── libwifi_rom_data.a        # WiFi ROM data segment
│   ├── libbt_host.a
│   ├── libbt_app.a
│   ├── libbth_gle.a
│   ├── libbth_sdk.a
│   └── libbg_common.a
├── rom/
│   └── ws63_acore_rom.lds        # mask-ROM symbol table (3752 syms; link -T)
├── include/
│   ├── api/                      # Public API headers
│   │   ├── wifi/                 # WiFi device/config/event/scan API
│   │   └── bts/                  # BLE GAP/GATT + SLE SSAP API
│   ├── internal/                 # Internal type headers (included by .a code)
│   │   ├── osal_types.h          # Portable integer types
│   │   ├── td_type.h             # Parallel type system
│   │   ├── td_base.h             # Base types + error codes
│   │   ├── errcode.h             # Error code definitions
│   │   ├── rom/                  # ROM code types
│   │   │   ├── frw_msg_rom.h     # Message passing types
│   │   │   ├── frw_ext_common_rom.h  # Timer system
│   │   │   ├── wlan_msg.h        # WLAN message IDs (100+)
│   │   │   ├── wlan_msg_rom.h    # ROM message IDs
│   │   │   ├── wlan_types_common.h   # WiFi MAC enums
│   │   │   ├── wlan_ring.h       # Ring buffer data structures
│   │   │   ├── oam_ext_if.h      # Logging interface
│   │   │   ├── oal_types_device_rom.h  # Device OAL types
│   │   │   ├── common_error_code_rom.h # ROM error codes
│   │   │   └── share_mem_config.h # Linker symbol declarations
│   │   ├── host/                 # HMAC/DMAC/FRW host headers
│   │   │   ├── oal_types.h       # Host OAL types
│   │   │   ├── oal_ext_if.h      # OAL external interface
│   │   │   ├── oal_util_hcm.h    # Section macros, register access
│   │   │   ├── frw_ext_if.h      # FRW framework API
│   │   │   ├── dmac_ext_if_hcm.h # DMAC external interface
│   │   │   └── hmac_ext_if.h     # HMAC external interface
│   │   └── hcc/                  # HCC IPC headers
│   │       ├── hcc_cfg_comm.h    # Bus/service/queue type enums
│   │       ├── hcc_types.h       # Queue control types
│   │       ├── hcc.h             # HCC handler types
│   │       ├── hcc_queue.h       # Data queue types
│   │       └── hcc_bus.h         # Bus driver interface
│   └── port/                     # PORTING STUBS - implement these
│       ├── port_osal.h           # OS abstraction (24 functions)
│       ├── port_frw.h            # FRW message framework (15 functions)
│       ├── port_wlan.h           # WLAN ring buffer + RF control (11 functions)
│       ├── port_hcc.h            # HCC IPC transport (6 functions)
│       ├── port_oal.h            # OAL buffer pool management (7 functions)
│       ├── port_uapi.h           # Platform services (3 functions)
│       ├── port_log.h            # Logging + safe C lib (7 functions)
│       └── port_linker.h         # Linker symbol declarations (20+ symbols)
```

## Porting Guide

> **Worked reference implementation (Rust).** The upstream monorepo's in-tree crate
> [`ws63-rf-rs`](https://github.com/sanchuanhehe/ws63-rs/tree/main/ws63-rf-rs) implements
> this entire contract for a bare-metal `riscv32imfc` target: `port_*.h` → `extern "C"`
> Rust symbols (OSAL/OAL/log/uapi + cooperative scheduler + FRW worker + HCC FIFO +
> software timers + netif→smoltcp), with **Wi-Fi-init symbol closure achieved** and the
> data path self-tested on `ws63-qemu`. Read it as a concrete, validated example of the
> steps below; the steps themselves stay runtime-neutral (FreeRTOS / Zephyr / Linux /
> bare-metal). What's left there is hardware-in-the-loop (real ROM addresses + pinning
> the pbuf layout / TX sink to the live blob).

### Step 1: Implement the porting stubs

All stubs are documented in `include/port/*.h`. Start with the simplest:

1. **port_linker.h** — Define memory regions in your linker script.
   The WiFi library needs a 48KB packet RAM region and TCM/SRAM sections.
   For a non-WS63 target, allocate these in your system RAM.

2. **port_log.h** — Map to printf() or your logging system.
   Implement `memset_s`/`memcpy_s`/`snprintf_s` using standard libc.

3. **port_uapi.h** — Provide system tick and optional NV read.

4. **port_osal.h** — Adapt to your RTOS (FreeRTOS, Zephyr, etc.).
   Key: interrupt management, thread creation, memory allocation.

5. **port_oal.h** — Initialize the 48KB WiFi packet buffer pool.

6. **port_wlan.h** — Implement ring buffer operations in shared memory.

7. **port_frw.h** — Message dispatch framework. Can start with stubs.

8. **port_hcc.h** — IPC transport. For on-chip use, implement shared memory
   IPC. For external use, implement SDIO/SPI bus driver.

### Step 2: Set up the linker script

Minimum linker script symbols (allocate from your RAM):

```
.wifi_pkt_ram (NOLOAD) : {
    . = ALIGN(4);
    __wifi_pkt_ram_begin__ = .;
    . += 48K;
    __wifi_pkt_ram_end__ = .;
}
```

For TCM/SRAM sections, see `include/port/port_linker.h` for the full
list of required symbols. On non-WS63 targets, all sections can be placed
in the same RAM region.

### Step 3: Build and link

```makefile
# Example Makefile fragment

# Full WiFi set — a real link needs host-MAC + TCM + algos, not just dmac
# (see the "Key insight" section above; the original extraction omitted these).
WIFI_LIBS  = -lwifi_driver_dmac -lwifi_driver_hmac -lwifi_driver_tcm \
             -lwifi_alg_anti_interference -lwifi_alg_cca_opt -lwifi_alg_edca_opt \
             -lwifi_alg_temp_protect -lwifi_alg_txbf -lwifi_rom_data
BT_LIBS    = -lbt_host -lbt_app -lbth_gle -lbth_sdk -lbg_common

LDFLAGS   += -L$(WS63_RF)/lib $(WIFI_LIBS) $(BT_LIBS)

# Required: the mask-ROM symbol table (fe_*/hal_machw_*/… live at fixed ROM
# addresses; these only execute on real silicon).
LDFLAGS   += -T$(WS63_RF)/rom/ws63_acore_rom.lds

# Required: libgcc for __divdi3/__udivdi3
LDFLAGS   += -lgcc

# Include paths
CFLAGS    += -I$(WS63_RF)/include/api/wifi
CFLAGS    += -I$(WS63_RF)/include/api/bts
CFLAGS    += -I$(WS63_RF)/include/api/bts/ble
CFLAGS    += -I$(WS63_RF)/include/api/bts/br
CFLAGS    += -I$(WS63_RF)/include/api/bts/sle
CFLAGS    += -I$(WS63_RF)/include/api/bts/common
CFLAGS    += -I$(WS63_RF)/include/internal
CFLAGS    += -I$(WS63_RF)/include/internal/rom
CFLAGS    += -I$(WS63_RF)/include/internal/host
CFLAGS    += -I$(WS63_RF)/include/internal/hcc
CFLAGS    += -I$(WS63_RF)/include/port
```

### Step 4: Initialize and use

**WiFi initialization sequence:**

```c
#include "wifi_device.h"
#include "wifi_event.h"

void wifi_event_callback(const wifi_event_stru *evt) {
    switch (evt->event) {
    case WIFI_STA_CONNECTED:
        // Connected to AP
        break;
    }
}

void init_wifi(void) {
    // 1. Initialize your porting layer first
    oal_memory_init();
    frw_main_init();
    hcc_dmac_config_bus_ini();
    hcc_dmac_service_adapt_start();

    // 2. Register event callback, then init WiFi
    wifi_register_event_cb(wifi_event_callback);
    wifi_init();

    // 3. Wait for wifi_is_wifi_inited() to return true,
    //    then enable STA mode and connect
    wifi_sta_enable();
    // ... scan and connect ...
}
```

**BLE initialization sequence:**

```c
#include "bts_le_gap.h"
#include "bts_gatt_server.h"

void gap_callback(const gap_ble_cb_event_t *evt) {
    if (evt->event == GAP_BLE_ENABLE_EVENT) {
        // BLE stack ready, register GATT services
        gatts_register_server(server_id);
        gap_ble_start_adv(adv_handle);
    }
}

void init_ble(void) {
    gap_ble_register_callbacks(gap_callback);
    enable_ble();
}
```

## Memory Requirements

| Region | Size | Purpose |
|--------|------|---------|
| WiFi Packet RAM | 48 KB | DMA buffers, netbuf pools, descriptor rings |
| ROM Data | ~4 KB | Initialized ROM variables |
| ROM BSS | ~1 KB | Zero-initialized ROM variables |
| TCM Text | 16-64 KB | Latency-critical WiFi/BT code (optional) |
| TCM Data/BSS | 16-64 KB | Latency-critical WiFi/BT data (optional) |
| SRAM Text | ~128 KB | WiFi protocol code |
| SRAM Data/BSS | ~256 KB | WiFi protocol runtime state |

**Total minimum RAM:** ~500 KB for full WiFi+BLE operation.

## Dependencies Count

| Porting Stub | Functions | Difficulty |
|-------------|-----------|------------|
| port_osal.h | 24 | Medium (adapt to your RTOS) |
| port_frw.h | 15 | Medium (message dispatch logic) |
| port_wlan.h | 11 | Low (ring buffer + clock gate) |
| port_hcc.h | 6 | Medium (IPC transport) |
| port_oal.h | 7 | Low (buffer pool management) |
| port_uapi.h | 3 | Low (platform services) |
| port_log.h | 7 | Low (printf mapping) |
| port_linker.h | 20+ symbols | Low (linker script) |
| **Total** | **~70** | |

## References

- **Hardware register map:** [ws63-svd](https://github.com/sanchuanhehe/ws63-svd) — CMSIS-SVD
  file covering all documented WS63 peripherals
- **Hardware documentation:** [ws63-guide](https://github.com/sanchuanhehe/ws63-guide) —
  System architecture, memory map, interrupt table, peripheral register descriptions
- **Original SDK:** fbb_ws63 — HiSilicon WS63 LiteOS SDK (reference implementation of
  porting stubs is in `src/drivers/chips/ws63/porting/`)

## License

The public API headers and porting stubs in this repository are provided as
interface definitions. The `.a` library files are proprietary HiSilicon
binaries extracted from the WS63 SDK. Consult the original SDK license
(see `../fbb_ws63/LICENSE`) for usage terms.
