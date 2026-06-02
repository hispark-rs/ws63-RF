# ws63-RF 架构

本仓库是 [ws63-rs](https://github.com/sanchuanhehe/ws63-rs) monorepo 的子模块。

`ws63-RF` 重分发 WS63 的闭源 Wi-Fi/BT/BLE/SLE 协议栈（`lib/*.a`）与 porting 接口头（`include/port/port_*.h`）。
复用闭源协议栈、只移植 ~70 个 OS/IPC 抽象函数 + HCC 共享内存 IPC，是正确的战略。本仓库本身保持**语言中立**
（只有 `.a` + C 头 + ROM 符号表 + 移植契约），不含任何 Rust。

> **状态更新（旧结论"尚无 Rust 绑定 / 连接性 0%"已过时）。** porting 契约的 Rust 实现现位于上游 monorepo 的
> in-tree crate [`ws63-rf-rs`](https://github.com/sanchuanhehe/ws63-rs/tree/main/ws63-rf-rs)：它把 `port_*.h`
> 实现为 `extern "C"` 符号（osal/oal/log/uapi/frw/hcc + 协作调度器 + 软件计时器 + netif→smoltcp 桥），并把 blob
> 链入镜像。**Wi-Fi-init 的符号闭合已达成**（whole-archive 0 重复符号；`--gc-sections` rooted at
> `uapi_wifi_init` 残留仅 2 个 `__wifi_pkt_ram_*` defsym），数据通路已在 `ws63-qemu` standalone 自测。
> 剩余是真机 bring-up——掩膜 ROM 函数只在硅片上可执行（地址见 `rom/ws63_acore_rom.lds`）、厂商自定义重定位 +
> 把 netif pbuf 布局/TX sink pin 到真实 blob。详见下方 `README.md` 的 Porting Guide 与上游 `ROADMAP.md` 阶段 3-5。

完整架构与评审（集中维护于主仓库）：
- 组件文档：<https://github.com/sanchuanhehe/ws63-rs/blob/main/docs/architecture/ws63-RF.md>
- 总体架构：<https://github.com/sanchuanhehe/ws63-rs/blob/main/docs/architecture/overview.md>
- 整改排期（连接性 bring-up）：<https://github.com/sanchuanhehe/ws63-rs/blob/main/ROADMAP.md>（阶段 3-5）
