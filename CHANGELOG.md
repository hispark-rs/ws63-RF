# Changelog

All notable changes to this project are documented here.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [Initial Release] — 2026-06-02

### Added
- Vendored WiFi .a libraries: `libwifi_driver_hmac.a` (32 MB, host MAC + public WiFi API), `libwifi_driver_tcm.a` (6 MB, TCM-resident code), five `libwifi_alg_*.a` (2.5 MB combined, rate/CCA/EDCA/TxBF/temp-protect algorithms)
- Mask-ROM symbol table `rom/ws63_acore_rom.lds` (3752 symbols mapping to on-chip ROM addresses; resolves ~422 undefined references in dmac.a)
- `LIB_EXTRACT.md` documenting all vendored libraries, closure analysis, and intentional omissions
- `ARCHITECTURE.md` describing the WS63 single-core RISC-V topology and software HMAC/DMAC split
- Complete header catalog: 45 public API headers (WiFi, BLE/SLE/BR), 24 internal type headers (ROM, Host, HCC), 8 porting stub headers (~70 functions total)
- Porting contract: `port_osal.h` (24 funcs), `port_frw.h` (15 funcs), `port_wlan.h` (11 funcs), `port_hcc.h` (6 funcs), `port_oal.h` (7 funcs), `port_uapi.h` (3 funcs), `port_log.h` (7 funcs), `port_linker.h` (20+ symbols)
- Precompiled BLE/SLE libraries: `libbt_host.a`, `libbt_app.a`, `libbth_gle.a`, `libbth_sdk.a`, `libbg_common.a` (3.5 MB combined)
- Initial README.md with library catalog, memory requirements, porting guide, and link to hardware references

### Changed
- Documentation: corrected WS63 as single-core (not dual-core); clarified that HMAC and DMAC are software libraries linked into one application image, not separate physical CPUs
- Documentation: corrected inaccurate claim that `libwifi_driver_dmac.a` is self-contained in all `hal_*` symbols; verified and documented residual undefined symbols
- README: spelled out the full symbol closure: with ROM symbol table + all WiFi libs, `libwifi_driver_dmac.a`'s 1080 undefined symbols reduce to exactly 40 (compiler-rt, linker symbols, osal/oal/log/uapi porting contract)
- `port_oal.h`: removed duplicate `__wifi_pkt_ram_begin__`/`__wifi_pkt_ram_end__` declarations (canonical location in `port_linker.h`)
- README: updated library catalog and memory layout to include the hmac/tcm/alg_* libraries and ROM data sizes

### Removed
- `libwpa_supplicant.a` (open-source hostap, BSD license): intentionally excluded per open-network MVP plan; WiFi MAC driver does not depend on it (verified by nm: zero MAC→WPA references); will be re-added later via Rust supplicant or as an optional layer

### Fixed
- Symbol closure: shipped the WS63 mask-ROM symbol table (`rom/ws63_acore_rom.lds`) that the initial extraction had omitted, enabling the vendored WiFi libraries to resolve ~422 RF front-end (`fe_*`), HAL (`hal_machw_*`, `hal_al_rx_*`, `hal_btcoex_*`) and algorithm symbols to on-chip ROM addresses (link with `-T rom/ws63_acore_rom.lds`)
- Documented additional porting symbols required but not in the stub headers (`log_event_wifi_print3`, `osal_get_current_pid`, `osal_irq_set_priority`, `uapi_efuse_read_bit`/`_buffer`, `get_tcxo_freq`)
