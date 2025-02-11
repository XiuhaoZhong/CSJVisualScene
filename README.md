# CSJVisualScene

## 项目简介
    Windows/macOS的渲染器，UI框架基于QT，渲染层基于Vulkan API，提供2D/3D内容渲染功能

## 功能特性
1. 支持通用的obj模型加载
2. 支持2D图形渲染，如jpeg、png等常见图片格式，
3. 支持3D图形渲染，包括基于shadow mapping的阴影处理，Blinn-Phong着色模型
4. 支持图形交互，如缩放、位移、旋转等等操作

## 整体架构
    整体分为Editor和Runtime两个部分。Editor负责用户对渲染对象的编辑，Runtime负责具体的渲染。
    Editor基于QT Gui。
    Runtime中包含功能层(function)、核心层(core)、资源管理(ResourceManager)
    function：主要包括对象管理，对象渲染、
    ResourceManager：主要包括渲染模型加载、管理
    core：包括类型反射、序列化等基础算法库等

## 项目结构
```
CSJVisualScene/
├── src/                # 源代码目录
├── Resources/          # 资源文件
├── README.md           # 项目说明文件
├── CMakeLists.txt      # 项目配置文件
└── LICENSE             # 许可证文件
```