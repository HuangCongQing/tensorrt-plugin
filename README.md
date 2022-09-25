<!--
 * @Description: 
 * @Author: HCQ
 * @Company(School): UCAS
 * @Email: 1756260160@qq.com
 * @Date: 2022-08-31 22:34:38
 * @LastEditTime: 2022-09-25 18:15:56
 * @FilePath: /tensorrt-plugin/README.md
-->
# tensorrt-plugin
实现TensorRT自定义插件(plugin)

@[双愚](https://github.com/HuangCongQing/) , 若fork或star请注明来源


## cookbook Example
环境安装
```
pip install -r cookbook/requirements.txt
```

### cookbook/04-Parser
> [cookbook/04-Parser/README.md](cookbook/04-Parser/README.md)

### 05-Plugin
> [cookbook/05-Plugin/README.md](cookbook/05-Plugin/README.md)

*入门例子：给输入张量所有元素加上同一个值 [05-Plugin/usePluginV2DynamicExt/AddScalarPlugin.cu](05-Plugin/usePluginV2DynamicExt/AddScalarPlugin.cu)

### 06-PluginAndParser
> [cookbook/06-PluginAndParser/README.md](cookbook/06-PluginAndParser/README.md)

## 参考资料

* 官方B站视频：TensorRT 教程 | 基于 8.2.3 版本 | 第三部分
    * 视频：https://www.bilibili.com/video/BV1t34y1s7mo/
    * 配套的代码库：https://github.com/NVIDIA/trt-samples-for-hackathon-cn/tree/master/cookbook

* 知户好文：实现TensorRT自定义插件(plugin)自由！https://zhuanlan.zhihu.com/p/297002406