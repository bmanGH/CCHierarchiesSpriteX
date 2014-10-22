CCHierarchiesSpriteX
====================

render flash animation in cocos2d-x, a animation sprite extension for cocos2d-x, base on hierarchies node like bone.

![Demo Vedio 1](http://cl.ly/image/111k3Y0N0R3r/CCHierarchiesSpriteXDemo_Video_1_optimized.gif)
![Demo Vedio 2](http://cl.ly/image/1z1M2t1d3x3v/CCHierarchiesSpriteXDemo_Video_2_optimized.gif)


cocos2d-x版本
----------------------
   * 3.3rc0
   * 请将cocos2d-x复制到cocos2d-x目录下才能运行demo


工具
----------------------
   * jsfl


Sprite类
-------------------
   * CCHierarchiesSprite类更注重动画播放性能，通过缓存动画数据获得更高的运行速度，但不支持动态换装，只能在创建时设置换装
   * CCHierarchiesSpriteDynamic类支持动态换装，但性能一般，不适合同屏大量使用


Action类
----------------
   * CCHierarchiesAnimate类用于播放动画
   * CCHierarchiesFlipX, CCHierarchiesFlipY用于翻转
   * CCHierarchiesAvatarMapReset, CCHierarchiesAvatarMapInsert, CCHierarchiesAvatarMapSet用于换装操作(只能用于CCHierarchiesSpriteDynamic)

功能
-------------------
   * 支持Flash的经典插值(Motion)动画
   * 支持Flash的引导层动画(通过JSFL进行转换为关键帧实现)
   * 支持Flash的所有Color Effect
   * 支持MovieClip和Graphics的2D矩阵变换(缩放，旋转，切变，偏移，锚点)
   * 自动将MovieClip的原点每帧更新为CCNode的AnchorPoint坐标
   * 自动更新每帧的BoundingBox
   * 播放动画操作实现为Action类
   * 通过缓存提高加载和动画计算的速度
   * 支持Flash中MovieClip的嵌套
   * 支持Graphics的所有循环播放模式
   * 支持自定义动画事件
   * 支持动画Avatar


局限
-------------------
   * 不支持Flash的非经典插值动画
   * 不支持Flash的蒙版
   * 不支持Flash的滤镜
   * 不支持Flash的shape变形动画
   * 不支持Flash的blend模式设置
   * 不支持插值动画的tweening设置


开发计划
-------------------
   * 支持内嵌声音播放数据
   * 支持滤镜
   * 支持骨骼的CCNode插接
   * 支持button
   * 支持action script version 1
   * ~~合并同一个fla导出的所有.hanims数据到一个文件~~
   * ~~改善Mesh的数据结构，更有效的使用vertex index~~


依赖
-------------------
   * cocos2d-x v2.1.4 以上
   * rapidxml v1.13 以上


性能
-------------------
   * iPad3同屏250个精灵平均50FPS


编译demo
------------------
   * 使用`git submodule init`和`git submodule update`下载cocos2d-x
   * 由于代码使用了部分c++11的功能，请将cocos2d-x编译配置也改为使用c++11和c++11标准库


整合cocos2d-x
-----------------
   * 将CCHierarchiesSprite目录复制到extensions目录下
   * 将rapidxml库加入工程
   * 在cocos-ext.h中加入
   
```c++
// CCHierarchiesSprite
#include "CCHierarchiesSprite/CCHierarchiesSprite.h"
#include "CCHierarchiesSprite/CCHierarchiesSpriteDynamic.h"
#include "CCHierarchiesSprite/CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSprite/CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSprite/CCHierarchiesSpriteAnimationCache.h"
#include "CChierarchiesSprite/CCHierarchiesSpriteRuntime.h"
```


使用方法
-----------------
   * 在程序启动时调用`CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime();`
   * 通过`CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->releaseUnusedResource();`释放缓存的数据
   * 具体播放动画和Action的使用方法见demo的测试代码
   * 制作Flash FLA动画资源时，可换装部位的动画需要用Movie Clip来制作
   * 通过在创建CCHierarchiesSprite, CCHierarchiesSpriteDynamic时使用带有AvatarMapType参数的构造函数，或者通过CCHierarchiesSpriteDynamic的setAvatarMap函数来设置换装部位名字映射(AvatarMapType类型用于设置换装部位名字的映射，实质是std::unordered_map<std::string, std::string>类型)
   * 注意由于CCHierarchiesSprite使用了动画数据缓存，所以如果多个对象即使是同一个动画但使用了不同换装配置的话，会缓存多份动画数据消耗更多的内存，相同换装配置会共用一份缓存


工具
----------------
   * 详见tools/README.md的说明


FLA动画资源
------------------
   * 感谢肥熊工作室提供的动画资源
