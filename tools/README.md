CCHierarchiesSprite工具
=====================


CCHierarchiesSpriteTool
----------------------------

   * HierarchiesSpriteExporter.zxp是Flash工具的GUI界面扩展插件包(mxp assets和mxp FLA是插件开发用的源文件)(Flash CS6以上才能支持使用Sprite Sheet Exporter排序导出功能)
       * 在装有Adobe Extension Manager的情况下双击安装
       * 在没有安装Adobe Extension Manager的情况下:
           1. 将 mxp assets/jsfl/HierarchiesSpriteAnimationPreprocess.jsfl 文件放置到 ***(Mac)*** [User Home目录]/Library/Application Support/Adobe/Flash CS6/en_US/Configuration/HierarchiesSpriteExporter/jsfl/ 目录下
           2. 将 mxp assets/jsfl/HierarchiesSpriteAnimationExporter.jsfl 文件放置到 ***(Mac)*** [User Home目录]/Library/Application Support/Adobe/Flash CS6/en_US/Configuration/HierarchiesSpriteExporter/jsfl/ 目录下
           3. 将 mxp assets/locale/en.xml 文件放置到 ***(Mac)*** [User Home目录]/Library/Application Support/Adobe/Flash CS6/en_US/Configuration/HierarchiesSpriteExporter/locale/ 目录下
           4. 将 mxp assets/locale/zh-CN.xml 文件放置到 ***(Mac)*** [User Home目录]/Library/Application Support/Adobe/Flash CS6/en_US/Configuration/HierarchiesSpriteExporter/locale/ 目录下
           5. 将 mxp assets/HierarchiesSpriteExporterPanel.swf 文件放置到 ***(Mac)*** [User Home目录]/Library/Application Support/Adobe/Flash CS6/en_US/Configuration/WindowSWF/ 目录下
       * batch convert script 对png图片进行后期处理的脚本
           * ***(Mac)*** convertPNGtoPVRCCZ.sh会对目录下所有png图片进行alpha bleed处理并转换成pvr.ccz格式
           * ***(Mac)*** convertPNGtoPNG.sh会对目录下所有png图片进行alpha bleed处理再进行png大小优化处理 ***(注意关闭XCode中的Compress PNG Files选项)***

   * HierarchiesSpriteSheet.plugin.jsfl是Flash工具的自定义SpriteSheet导出插件
       1. 将此文件放置在 ***(Mac)*** [Flash安装目录]/Common/Configuration/Sprite Sheet Plugins/ 目录下
       2. 在Flash Sprite Sheet Exporter中选择data formate为HierarchiesSpriteSheet即可

   * TexturePackerExporter是TexturePacker的SpriteSheet导出插件
       1. 将文件夹放置在 ***(Mac)*** [TexturePacker安装目录]/Resources/exporters/ 目录下
       2. 在导出选项中选择HierarchiesSpriteSheet即可

   * ~~HierarchiesSpriteSheetPacker build step:~~
       1. ~~install ImageMagick package from http://cactuslab.com/imagemagick/ or http://www.imagemagick.org/script/binary-releases.php~~
       2. ~~install pkg-config from HomeBrew or MacPort~~
       3. ~~open termial with command first: PKG_CONFIG_PATH=/opt/ImageMagick/lib/pkgconfig/~~
       4. ~~copy result from command: echo `Magick++-config --cppflags`~~
       5. ~~replace HierarchiesSpriteSheetPacker's XCode project -> Build Settings -> Other C++ Flags~~
       6. ~~copy result from command: echo `Magick++-config --ldflags --libs`~~
       7. ~~replace HierarchiesSpriteSheetPacker's XCode project -> Build Settings -> Other Linker Flags~~
       8. ~~set HierarchiesSpriteSheetPacker's XCode project -> C++ Standard Library -> libstdc++ (GNU C++ standard library)~~


ConvertShaderToCString
----------------------------

   * ConvertShaderToCString Sublime Text 2 的插件，用来将Shader代码转换为C的字符串常量，方便将Shader代码嵌入到程序中
   * 安装方法同其他Sublime Text 2插件的安装方法


ccz
----------------------------

   * Cocos2d ccz格式压缩工具


AlphaUtilityCL
----------------------------

   * 对png图片进行alpha bleed处理的工具，避免纹理采样时出现的黑边现象
   
   