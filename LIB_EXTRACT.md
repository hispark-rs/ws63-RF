# ws63-RF extraction manifest

What this delivery contains and how it was completed from the HiSilicon
`fbb_ws63` C SDK. Verified by `nm` against `libwifi_driver_dmac.a` (1080
undefined symbols).

## WiFi `.a` libraries vendored in `lib/`

| Library | Size | Defines |
|---------|------|---------|
| `libwifi_rom_data.a` | 3 KB | WiFi ROM data segment (config globals) |
| `libwifi_driver_dmac.a` | 614 KB | WiFi device MAC + HAL + RF front-end control |
| `libwifi_driver_hmac.a` | 32 MB | host MAC + public WiFi API (`wifi_*`) |
| `libwifi_driver_tcm.a` | 6 MB | TCM-resident driver code |
| `libwifi_alg_*.a` (txbf/cca_opt/edca_opt/temp_protect/anti_interference) | ~2.5 MB | rate/PHY algorithms |

The hmac/tcm/alg_* set was **omitted by the original extraction** and added
here, copied from the C SDK app build `src/protocol/wifi/ws63-liteos-app/`:

```sh
SRC=fbb_ws63/src/protocol/wifi/ws63-liteos-app
cp "$SRC"/libwifi_driver_{hmac,tcm}.a "$SRC"/libwifi_alg_*.a ws63-RF/lib/
```

## WPA supplicant — intentionally NOT vendored

`libwpa_supplicant.a` is **open-source hostap** (`Copyright Jouni Malinen`,
BSD; `open_source/wpa_supplicant/...`), not proprietary, and the WiFi MAC driver
does **not** depend on it (verified by `nm`: 0 MAC→wpa references). It is the
auth layer for connecting to *secured* APs, and it drags in lwip (TCP/IP),
mbedtls (crypto) and libc — ~50 MB and a large C porting surface.

It is therefore left out: the ws63-rs path does **open-network MVP first** (no
supplicant needed), with WPA later via a Rust supplicant (RustCrypto) or by
re-adding the open lib (`cp "$SRC"/libwpa_supplicant.a ws63-RF/lib/`). This is
the same supplicant esp-idf uses (open hostap), just not bundled here.

Other build variants ship extra optional libs (`libwifi_btcoex.a`,
`libwifi_radar_sensor.a`, `libwifi_csa.a`, …) under
`src/protocol/wifi/ws63-liteos-spi-device/`; the btcoex/radar primitives the app
build needs are otherwise resolved from the ROM table below.

## Mask-ROM symbol table

- `rom/ws63_acore_rom.lds` — the WS63 app-core ROM symbol table (3752 symbols).
  Resolves ~422 of dmac.a's undefined references (`fe_*` RF front-end,
  `hal_machw_*`, `hal_al_rx_*`, `hal_btcoex_*`, …) to their on-chip ROM
  addresses. Source: `fbb_ws63 drivers/chips/ws63/rom_config/acore/acore.sym`.
  Link with `-T rom/ws63_acore_rom.lds`. (Real-silicon addresses — an emulator
  that does not populate the mask ROM cannot execute them.)

## Closure result (verified by `nm`)

With `rom/ws63_acore_rom.lds` + the vendored WiFi `.a` set, `libwifi_driver_dmac.a`'s
1080 undefined symbols reduce to exactly **40**, all the **runtime's** job
(compiler-rt / linker / the porting layer — no vendor code, no radio
reverse-engineering):

- **23 OSAL/OAL** — `osal_kmalloc`/`kfree`, `osal_irq_*` (lock/restore/enable/
  disable/free/request/**set_priority**), `osal_kthread_*`, `osal_wait_*`,
  `osal_udelay`, `osal_flush_cache`, `osal_printk`, **`osal_get_current_pid`**.
- **10 platform** — `log_event_wifi_print{0,1,2,3,4}`, `uapi_nv_read`,
  `uapi_tsensor_get_current_temp`, **`uapi_efuse_read_bit`/`_buffer`**,
  **`get_tcxo_freq`**.
- **7 compiler-rt** — `__udivdi3`/`__divdi3`/`__ashldi3`/`__ashrdi3`/`__lshrdi3`,
  `memcpy`, `memset`.
- **2 linker** — `__wifi_pkt_ram_begin__`/`__wifi_pkt_ram_end__`.

**Bold** symbols are referenced by the blob but are NOT in the documented
`port_*.h` contract (`print3`, `osal_get_current_pid`, `osal_irq_set_priority`,
`uapi_efuse_read_*`, `get_tcxo_freq`) — the real porting surface is slightly
larger than the headers document. `ws63-rs`'s `ws63-rf-rs` implements the OSAL/
OAL/log/uapi set (compiler-rt is provided by the Rust toolchain); the linker
symbols and a real `.wifi_pkt_ram` region are the firmware's job.
