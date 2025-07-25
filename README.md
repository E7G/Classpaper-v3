# ClassPaper v3

基于 C++ 和 WebUI 技术重构的智能桌面课表壁纸应用，提供沉浸式的桌面学习体验。

## 简介

ClassPaper 是一个专为学生设计的桌面课表应用，将课程表以美观的方式显示在桌面背景上。应用采用现代 Web 技术构建界面，通过 C++ 后端实现桌面集成，提供流畅的用户体验和丰富的自定义功能。

## 主要特性

### 🎯 核心功能

- **智能课表显示**：实时显示当前课程、下节课程和课程进度
- **桌面壁纸集成**：将课表作为动态桌面壁纸，支持桌面穿透
- **多样化壁纸**：支持自动切换背景壁纸，可自定义切换间隔
- **实时时钟**：显示当前时间、日期、星期和学期周次

### 📅 时间管理

- **学期进度条**：可视化显示学期进度百分比
- **课程提醒**：支持音频通知（上课和下课提醒）
- **事件日历**：集成日程管理功能
- **周次计算**：自动计算并显示当前学期周次

### 🎨 界面定制

- **响应式设计**：适配不同屏幕尺寸
- **暗色主题**：现代化的深色界面设计
- **动画效果**：流畅的壁纸切换和界面过渡动画
- **自定义配置**：通过配置文件和设置界面个性化定制

### ⚙️ 系统集成

- **系统托盘**：最小化到系统托盘，提供快捷操作菜单
- **桌面穿透**：支持点击穿透到桌面底层元素
- **自动启动**：可设置开机自动启动
- **独立设置程序**：专门的设置界面用于配置管理

## 使用方法

### 快速开始

1. **下载程序**

   ```bash
   git clone https://github.com/your-repo/classpaper-v3.git
   cd classpaper-v3
   ```
2. **编译程序**

   ```bash
   # 编译主程序
   build.bat

   # 编译设置程序
   build.bat settings
   ```
3. **运行程序**

   ```bash
   # 直接运行
   main.exe

   # 或使用批处理文件
   run.bat
   ```

### 系统托盘操作

程序运行后会在系统托盘显示图标，右键点击可访问以下功能：

- **页面重载**：刷新课表显示
- **桌面穿透**：重新设置桌面壁纸模式
- **设置**：打开设置界面
- **重启**：重启程序
- **退出**：关闭程序

## 配置说明

### 主配置文件 (config.toml)

```toml
[default]
browser_path = ""                    # 浏览器路径（可选）
url = "./res/index.html"            # 显示的页面路径
```

### 课表配置 (res/config/config.js)

```javascript
const CONFIG = {
    // 学期时间设置
    lessons: {
        times: {
            semester: {
                begin: "2024-09-01",     // 学期开始时间
                end: "2025-01-20"        // 学期结束时间
            }
        }
    },
  
    // 周次偏移设置
    weekOffset: {
        enabled: true,
        offset: 35                       // 周次偏移量
    },
  
    // 壁纸设置
    wallpaperInterval: 30,               // 壁纸切换间隔（秒）
  
    // 进度显示设置
    progressPercentMode: "left",         // 进度模式：left(剩余) / passed(已过)
    progressDescription: "高三剩余"       // 进度描述文字
};

// 壁纸列表
const wallpaperlist = [
    "wallpaper/bg1.jpg",
    "wallpaper/bg2.jpg"
    // 添加更多壁纸...
];
```

## 项目结构

```
classpaper-v3/
├── main.cpp                    # 主程序源码
├── settings.cpp                # 设置程序源码
├── config.toml                 # 主配置文件
├── build.bat                   # 编译脚本
├── run.bat                     # 运行脚本
├── include/                    # 头文件目录
│   ├── webui.h                # WebUI 库头文件
│   ├── webui.hpp              # WebUI C++ 头文件
│   └── toml.hpp               # TOML 解析库
├── lib/                       # 库文件目录
│   ├── libwebui-2-static.a   # WebUI 静态库
│   └── webui-2.dll           # WebUI 动态库
└── res/                       # 资源文件目录
    ├── index.html             # 主界面
    ├── settings.html          # 设置界面
    ├── css/                   # 样式文件
    ├── js/                    # JavaScript 文件
    ├── audio/                 # 音频文件
    ├── config/                # 配置文件
    └── wallpaper/             # 壁纸目录
```

## 技术架构

### 核心技术栈

- **后端**：C++17 + WebUI 2.5.0
- **前端**：HTML5 + CSS3 + JavaScript
- **配置管理**：TOML11 库
- **界面框架**：Pico CSS
- **构建工具**：GCC + MinGW

### 关键组件

- **WallpaperManager**：桌面壁纸管理
- **WindowManager**：窗口状态管理
- **TrayIcon**：系统托盘集成
- **ConfigManager**：配置文件管理

## 编译要求

### 系统要求

- Windows 7 及以上版本
- MinGW-w64 或 Visual Studio 2019+
- C++17 标准支持

### 依赖库

- **WebUI 2.5.0**：跨平台 Web 界面库
- **TOML11**：现代 C++ TOML 解析库
- **Windows API**：系统集成功能

### 编译选项

```bash
# 静态链接版本（推荐）
g++ -O3 -std=c++17 main.cpp -o main.exe -lwebui-2-static -luser32 -lole32 -lws2_32 -static

# 动态链接版本
g++ -O3 -std=c++17 main.cpp -o main-dyn.exe webui-2.dll -luser32 -lole32 -lws2_32

# 调试版本
g++ -O0 -g -DDEBUG -std=c++17 main.cpp -o main-debug.exe -lwebui-2-static
```

## 功能详解

### 课表显示系统

- 支持一周7天的课程安排显示
- 实时高亮当前进行的课程
- 显示课程名称、时间和进度
- 支持早读、正课、晚修等不同时段

### 壁纸管理系统

- 自动扫描 `res/wallpaper/` 目录下的图片文件
- 支持 JPG、PNG、GIF 等常见格式
- 平滑的淡入淡出切换效果
- 可配置的自动切换间隔

### 时间管理功能

- 精确的学期进度计算
- 支持自定义学期起止时间
- 周次自动计算和偏移调整
- 多种进度显示模式

### 音频通知系统

- 上课提醒音效
- 下课提醒音效
- 支持自定义音频文件
- 可在设置中开启/关闭

## 常见问题

### Q: 程序无法启动？

A: 请检查是否安装了必要的 Visual C++ 运行库，确保 `webui-2.dll` 文件存在。

### Q: 桌面穿透不生效？

A: 右键系统托盘图标，选择"桌面穿透"重新设置，或重启程序。

### Q: 如何添加自定义壁纸？

A: 将图片文件放入 `res/wallpaper/` 目录，程序会自动扫描并加入轮播列表。

### Q: 课表时间不准确？

A: 在 `res/config/config.js` 中调整学期起止时间和周次偏移设置。

### Q: 如何自定义课程表？

A: 编辑 `res/config/config.js` 文件中的课程安排配置。

## 更新日志

### v3.0.0

- 使用 C++ 重构整个应用
- 集成 WebUI 2.5.0 框架
- 新增独立设置程序
- 优化壁纸切换效果
- 改进系统托盘交互
- 支持 TOML 配置格式

## 许可证

此项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 联系方式

如有任何疑问或建议，请通过以下方式联系：

- GitHub Issues: [提交问题](https://github.com/e7g/classpaper-v3/issues)
- 作者: [e7g](https://github.com/e7g/)

## 致谢

- [WebUI](https://github.com/webui-dev/webui) - 跨平台 Web 界面框架
- [TOML11](https://github.com/ToruNiina/toml11) - 现代 C++ TOML 解析库
- [Pico CSS](https://picocss.com/) - 轻量级 CSS 框架
- [实现桌面动态壁纸（一）\_动态壁纸原理-CSDN博客](https://blog.csdn.net/qq_59075481/article/details/125361650) - 提供桌面壁纸实现原理
