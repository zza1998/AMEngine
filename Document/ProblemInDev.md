
## 问题之- Imgui导入问题：
为了不影响GUI代码和主要渲染的逻辑耦合，额外进行解耦处理，导入过程中出现了许多问题
- GUI和主要屏幕内容交替渲染只存在其1
- 渲染后只有GUI的内容
## 解决方案- Imgui导入问题
- GUI的渲染在模型渲染之后，需要禁止颜色清除，同时在VK_IMAGE_LAYOUT_PRESENT_SRC_KHR后进行渲染
- 查看RenderDoc发现模型部分renderpass其实已经渲染出来, 只是GUI渲染了全屏的内容挡住了，阻塞了好一会，

## 问题之 skybox导入
skybox的渲染需要再场景渲染之前，取消写深度，并且进行cubeSampler

