# ws63-RF 架构

本仓库是 [ws63-rs](https://github.com/sanchuanhehe/ws63-rs) monorepo 的子模块。

`ws63-RF` 重分发 WS63 的闭源 Wi-Fi/BT/BLE/SLE 协议栈（`lib/*.a`）与 porting 接口头（`include/port/port_*.h`）。
复用闭源协议栈、只移植 ~70 个 OS/IPC 抽象函数 + HCC 共享内存 IPC，是正确的战略；但目前**尚无任何 Rust 绑定/链接**，
连接性交付为 0%——这是 ws63-rs 到"可用产品"的最大缺口。

完整架构与评审（集中维护于主仓库）：
- 组件文档：<https://github.com/sanchuanhehe/ws63-rs/blob/main/docs/architecture/ws63-RF.md>
- 总体架构：<https://github.com/sanchuanhehe/ws63-rs/blob/main/docs/architecture/overview.md>
- 整改排期（连接性 bring-up）：<https://github.com/sanchuanhehe/ws63-rs/blob/main/ROADMAP.md>（阶段 3-5）
