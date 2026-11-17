# EFR32MG24 Bluetooth Mesh 与 Bootloader

## English overview

This repository contains a Bluetooth Mesh application and a Gecko Bootloader for the Silicon Labs EFR32MG24. Both projects target EFR32MG24B210F1536IM48 and BRD4186C and specify Gecko SDK 4.5.0.

- `btmesh_soc_empty_SDK450/`: Bluetooth Mesh application with a serial-to-Mesh interface. Despite its name, `zigbee_transfer.c` does not by itself implement a complete Zigbee stack.
- `bootloader_SDK450_MG24/`: Gecko Bootloader configured with a single internal-flash storage slot for firmware update images.

The application includes test Mesh keys and self-provisioning logic. Review security and provisioning before deployment. The SDK is not included; install it separately. This snapshot has not been rebuilt or validated on hardware in the current environment.

## 中文简介

本仓库保存两个 Silicon Labs 工程：

- `btmesh_soc_empty_SDK450/`：Bluetooth Mesh 应用。主要逻辑位于 `app.c`，`zigbee_transfer.c` 实现串口与 Mesh 数据转发接口；文件名不代表包含完整 Zigbee 协议栈。
- `bootloader_SDK450_MG24/`：Gecko Bootloader，使用内部 Flash 的单个存储槽保存升级镜像。

两个工程的 .slcp 都指定 Gecko SDK 4.5.0、EFR32MG24B210F1536IM48 和 BRD4186C。实际硬件不同必须检查芯片、板卡与引脚配置。应用中有测试用 Mesh 密钥和自配网逻辑，仅供测试，实际部署需重新设计密钥和配网流程。

## 环境准备

1. 安装 Simplicity Studio 6，在 Studio 中安装并识别 Gecko SDK 4.5.0、GCC ARM 工具链及相关工具。VS Code 扩展本身不包含完整 SDK。
2. 安装 VS Code 的 Simplicity Studio for VS Code 扩展。
3. 克隆仓库或从 GitHub 下载并解压。SDK 不包含在本仓库内。

## 在 VS Code 打开

1. 选择 **File > Open Folder**，打开 `btmesh_soc_empty_SDK450` 文件夹。
2. 要同时打开另一个工程，选择 **File > Add Folder to Workspace**，加入 `bootloader_SDK450_MG24`，可用 **File > Save Workspace As** 保存工作区。
3. 点击左侧 **Si** 图标，在 Workspace 中找到工程。工程已经包含 VS Code 生成器的 CMake 文件与 vscode.conf。
4. 若未显示，刷新 Si 视图；检查 SDK 与工具链是否可用。必要时在 Simplicity Studio 中导入工程，打开 .slcp，核对 SDK、芯片及 VS Code generator，重新生成后再打开。

官方文档：[打开已有工程](https://docs.silabs.com/ss-vscode/latest/ss-vscode-projects/import-open-existing-project)；[Si 面板与编译、烧录、调试](https://docs.silabs.com/ss-vscode/latest/ss-vscode-getting-started-overview/explore-vscode-extension)。

## 编译和使用

1. 先在 Si 面板选择 Bootloader 工程的 **Build**，再选择应用工程的 **Build**。有多个配置时按提示选择。检查输出窗口中的错误，不要把历史 artifact 当成新编译结果。
2. 用调试器连接硬件，确认设备型号。烧录前检查 Bootloader 与应用的链接地址、Flash 布局、擦除范围和具体镜像，防止覆盖已有固件或设备数据。使用 **Flash** 选择相应构建产物；.gbl 是升级镜像，不能当作普通应用镜像随意直接烧录。
3. 选择应用工程的 **Debug** 进行调试。带非零启动地址且与 Bootloader 共存的应用可根据插件提示选择 Run Directly，具体取决于实际启动和链接配置。
4. 改业务逻辑编辑 app.c、app.h、zigbee_transfer.c/.h；组件、引脚与协议配置优先通过 Simplicity Studio 的 .slcp 配置器修改并重新生成。autogen 中的文件可能被生成器覆盖。

Bootloader 配置的槽大小为 737280 字节。自带 README 的文字与实际配置存在差异；烧录或 OTA 前需核对配置和应用 Flash 分区，确认不重叠。原有 artifact 为历史产物，未验证与当前源码一致。

本仓库的整理版本排除了本机编译缓存、旧路径和备份；未在当前环境完成重新编译或硬件验证。
