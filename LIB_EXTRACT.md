# ws63-RF extraction manifest

What this delivery contains vs. what a *complete* WiFi link needs, and how to
obtain the rest from the HiSilicon `fbb_ws63` C SDK. Verified by `nm` against
`libwifi_driver_dmac.a` (1080 undefined symbols).

## Currently vendored in `lib/`

- `libwifi_rom_data.a`, `libwifi_driver_dmac.a` (WiFi)
- `libbt_host.a`, `libbt_app.a`, `libbth_gle.a`, `libbth_sdk.a`, `libbg_common.a` (BT/SLE)

## Now also shipped

- `rom/ws63_acore_rom.lds` — the WS63 app-core **mask-ROM symbol table** (3752
  symbols). Resolves ~422 of dmac.a's undefined references (`fe_*` RF front-end,
  `hal_machw_*`, `hal_al_rx_*`, `hal_btcoex_*`, …) to their on-chip ROM
  addresses. Source: `fbb_ws63 drivers/chips/ws63/rom_config/acore/acore.sym`.

## Omitted by the original extraction (obtain from the C SDK to complete WiFi)

These vendor `.a` libs define ~618 of dmac.a's undefined symbols and are **not**
in `lib/`. They live in the C SDK at `src/protocol/wifi/ws63-liteos-app/`
(and `.../ws63-liteos-spi-device/` for btcoex/radar):

| Library | Size | Defines |
|---------|------|---------|
| `libwifi_driver_hmac.a` | ~32 MB | host MAC + public WiFi API (`wifi_*`) |
| `libwifi_driver_tcm.a` | ~6 MB | TCM-resident driver code |
| `libwpa_supplicant.a` | ~13 MB | WPA/auth supplicant |
| `libwifi_btcoex.a` | ~1.7 MB | WiFi/BT coexistence |
| `libwifi_alg_*.a` (5) | ~2.5 MB | rate/CCA/EDCA/TxBF/temp-protect algorithms |

To vendor them (run from the C SDK build that produced the wifi libs):

```sh
SRC=fbb_ws63/src/protocol/wifi/ws63-liteos-app
DST=ws63-RF/lib
cp "$SRC"/libwifi_driver_hmac.a "$SRC"/libwifi_driver_tcm.a \
   "$SRC"/libwpa_supplicant.a "$SRC"/libwifi_alg_*.a "$DST"/
cp fbb_ws63/src/protocol/wifi/ws63-liteos-spi-device/libwifi_btcoex.a "$DST"/
```

## Closure result

With `rom/ws63_acore_rom.lds` + the full WiFi `.a` set above, `libwifi_driver_dmac.a`'s
1080 undefined symbols reduce to **~40**, all of which are the **runtime's**
responsibility (not vendor code):

- ~21 `osal_*`/`oal_*` — the documented `port_*.h` porting contract.
- `log_event_wifi_print*`, `uapi_nv_read`/`uapi_tsensor_get_current_temp`/
  `uapi_efuse_read_*`, `get_tcxo_freq` — platform services (`port_log.h`/`port_uapi.h`).
- `__udivdi3`/`__ashldi3`/… — 64-bit compiler-rt builtins.
- `__wifi_pkt_ram_begin__`/`__wifi_pkt_ram_end__` — linker symbols.
- `g_dmac_alg_main`/`g_mac_res_etc` — two ROM-data globals defined by no lib.

A runtime porting layer (e.g. `ws63-rs`'s `ws63-rf-rs`) implements that ~40-symbol
set. **No radio reverse-engineering is required** — the RF/PHY/MAC-HW primitives
are the mask-ROM functions in `rom/ws63_acore_rom.lds`.
