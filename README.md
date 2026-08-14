# RimeDictHelper2

用于管理Rime输入法词库的小工具，基于c++实现。功能简单，受众小众，主要自用，顺便分享，有缘遇到可自取。

## 简介

如果你也：

+ 使用Rime输入法
+ 使用五笔

那么大概率`RimeDictHelper2`对你有帮助。如果你恰好还

+ 喜欢手动管理词库而抗拒自动造词
+ 使用Git管理自己的词库

那么这个小工具简直就是为你定制的了。

## 基本功能

+ 图形化造词操作
+ 自动生成词组编码（限五笔词组编码规则）

## 附加功能

后置自定义脚本。最基本地，可以在里面调用`WeaselDeployer.exe`实现自动部署。我还会在里面添加git相关命令

## 程序目录结构

```bash
RimeDictHelper2
 ├─after.bat  # 可选
 ├─config.ini
 └─RimeDictHelper2.exe
```

## 配置方法

按上述目录结构部署好应用即可。

### 1. config.ini

`config.ini`文件中需要配置如下内容：

```ini
[Settings]
# BaseDictPath为${Rime用户文件夹}\xxx.dict.yaml 
BaseDictPath = D:\ProgramData\rime\wubi86_jidian.dict.yaml
# UserDictPath为${Rime用户文件夹}\xxx_user.dict.yaml
UserDictPath = D:\ProgramData\rime\wubi86_jidian_user.dict.yaml
```

其中`BaseDictPath`用于读取单字编码，以便后续自动生成词组编码。

### 2. after.bat

`after.bat`为可选项。建议至少添加如下内容：

```bat
rem 前半部分替换为自己Rime的安装目录即可。
rem 也可在“开始菜单-小狼毫输入法-【小狼毫】重新部署”快捷方式的属性窗口，将“目标”一栏的内容直接复制到下面
"D:\Program Files\Rime\weasel-0.17.4\WeaselDeployer.exe" /deploy
```

这样每次添加后就免去手动重新部署，可立即生效