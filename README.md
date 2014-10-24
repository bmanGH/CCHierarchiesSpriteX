CCHierarchiesSpriteX
====================

render flash animation in cocos2d-x, a animation sprite extension for cocos2d-x, base on hierarchies node like bone.

![Demo Vedio 1](http://cl.ly/image/111k3Y0N0R3r/CCHierarchiesSpriteXDemo_Video_1_optimized.gif)
![Demo Vedio 2](http://cl.ly/image/1z1M2t1d3x3v/CCHierarchiesSpriteXDemo_Video_2_optimized.gif)


依赖
----------------------
   * cocos2d-x 3.3rc0 (请将cocos2d-x复制到cocos2d-x目录下才能运行demo)
   * rapidxml v1.13
   * 建议使用Flash Professional CC


实现的Flash功能
----------------------
   * 2D Transform 2D变换
   * KeyFrame 关键帧
   * Layer 层
   * MovieClip 影片剪辑
   * Graphic 图形元件
   * Classic Tween 传统补间 (但不支持补间的Ease, Rotate, Snap, Sync, Orient to path, Scale)
   * Loop Mode
   * Color Effect 变色效果
   * MovieClip或Graphic嵌套
   * XFL文件格式


未实现的Flash功能
----------------------
   * 3D Transform 3D变换
   * Shape 矢量图
   * Button
   * Filter 滤镜
   * Blend 图层混合
   * Motion Tween 非传统补间
   * Shape Morph
   * 引导层
   * Mask 蒙版
   * ActionScript


计划实现的功能
----------------------
   * Lua绑定
   * Sound
   * Sprite Mesh
   * Tween Ease, Tween Rotate


Flash中注意的地方
----------------------
   * Flash中只有两个组成补间动画的KeyFrame都只含有一个Element时动画才正常
   * 如果对2个关键帧中的Image创建补间动画，Flash会自动生成两个只包含Image的Graphic (建议针对每个Image先创建一个Graphic或MovieClip，再用这些来创建动画)
   * 只有MoviceClip的SymbolInstance才支持Filter, 3D View, Blend
   * 只有Graphic的SymbolInstance才支持LoopMode
   * Flash对2个使用同样的MovieClip并且连续的KeyFrame播放动画时会自动连续播放，Graphic由于有LoopMode所以不会自动连续播放
   * 引导层可以通过预先转换成关键帧来实现同样的动画效果
   * 建议不要在一个MoviceClip或Graphic中使用重复的Layer名
   * 自定义事件是通过将第一层命名为EVENT_LAYER来定义事件层，其中的KeyFrame名为事件名
   * 建议将主要的动画MovieClip或Graphic放在Animation文件夹中
   * 建议将所有的Image放在Image文件夹中 (可以方便生成SpriteSheet)


Sprite类
-------------------
   * CCHierarchiesSpriteStatic类更注重动画播放性能，通过缓存动画数据获得更高的运行速度，但不支持动态换装，只能在创建时设置换装
   * CCHierarchiesSpriteDynamic类支持动态换装，但性能一般，不适合同屏大量使用
   * 将MovieClip的原点每帧更新为CCNode的AnchorPoint坐标
   * 每帧更新BoundingBox
   * 支持自定义动画事件
   * 支持Avatar


Action类
----------------
   * CCHierarchiesAnimate类用于播放动画
   * CCHierarchiesFlipX, CCHierarchiesFlipY用于翻转
   * CCHierarchiesAvatarMapReset, CCHierarchiesAvatarMapInsert, CCHierarchiesAvatarMapSet用于换装操作(只能用于CCHierarchiesSpriteDynamic)


使用方法
-----------------
   * 在程序启动时调用`CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime();`
   * 通过`CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->releaseUnusedResource();`释放缓存的数据
   * 具体播放动画和Action的使用方法见demo的测试代码
   * 制作Flash FLA动画资源时，可换装部位的动画需要用Movie Clip来制作
   * 通过在创建CCHierarchiesSprite, CCHierarchiesSpriteDynamic时使用带有AvatarMapType参数的构造函数，或者通过CCHierarchiesSpriteDynamic的setAvatarMap函数来设置换装部位名字映射(AvatarMapType类型用于设置换装部位名字的映射，实质是std::unordered_map<std::string, std::string>类型)
   * 注意由于CCHierarchiesSprite使用了动画数据缓存，所以如果多个对象即使是同一个动画但使用了不同换装配置的话，会缓存多份动画数据消耗更多的内存，相同换装配置会共用一份缓存


工具
----------------------
   * HierarchiesSpriteSheet.plugin.jsfl 是用于Flash导出SpriteSheet到.hsheet格式的脚本插件

