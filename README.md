# adiosy_engine

#### 介绍
一个简易的Vulkan渲染框架实现。
- 主页：[Learn_Vulkan02_渲染框架实现_开篇](http://www.adiosy.com/posts/learn_vulkan/learn_vulkan02_%E6%B8%B2%E6%9F%93%E6%A1%86%E6%9E%B6%E5%AE%9E%E7%8E%B0_%E5%BC%80%E7%AF%87.html)
- 视频：[XINGCHEN0085的个人空间-哔哩哔哩](https://b23.tv/ObMaagI)
#### 功能列表
- 支持Windows、Macos和Linux三个PC平台运行。
- Vulkan基本图元渲染，包括：点、线、面和三维物体。
- 物体点击选中。
- 贴图和材质的渲染。
- Phong光照模型，法线贴图、天空盒。
- 简单阴影。
- 场景图ECS系统、场景保存和加载。
- obj和glTF模型导入。
- 蒙皮动画。
- 场景编辑器。
##### TODO list:
- PBR、HDR、泛光和后处理等效果。
- 添加是适配安卓平台。
- …

#### 软件架构
![img.png](img.png)
- 平台层，提供不同平台的差异化实现，包括在Windows、Macos和Linux三个PC平台的实现；另外对图形API（OpenGL、Vulkan…），提供上层窗口系统和Gui支持。
- 核心层，这一层包括一些公共工具、事件处理、配置和渲染、动画和场景管理几个模块。
- 应用层，简单实现一个场景编辑器，还有使用的sample，sandbox等。

#### 安装教程
```shell
mkdir build
cd build
cmake ..
make -j3
```
#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request

