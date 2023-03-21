# 3gpp amrnb

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

[English](README.md)

3gpp amrnb编解码库

本仓库为3gpp amr的rt-thread移植版本，提供amr的软件编解码。为了适应mcu环境，所有浮点均修改为了float，以应用fpu的加速功能，可能会造成一定程度的音频失真，因为部分计算精度从double降低为了float。所有包含fpu的mcu均可使用本仓库得到良好的编解码体验，例如stm32f407，stm32f429，stm32h750等。

## 内容列表

- [安装](#安装)
- [使用说明](#使用说明)
- [示例](#示例)
- [维护者](#维护者)
- [如何贡献](#如何贡献)
- [使用许可](#使用许可)

## 安装

使用env的包管理开启本软件包，即完成了安装。

## 使用说明

可通过decoder.c和encoder.c学习具体的使用。
[README](README_upstream.md)这是上游的README，有部分介绍。

## 示例

在decoder.c和encoder.c中包含两条msh的命令，可通过命令行对音频文件进行编解码。

## 维护者

[@myshowtogo](https://github.com/myshowtogo)
[@llyhwdmx](https://github.com/llyhwdmx)

## 如何贡献

非常欢迎你的加入！[提一个 Issue](https://github.com/RichardLitt/standard-readme/issues/new) 或者提交一个 Pull Request。

标准 Readme 遵循 [Contributor Covenant](http://contributor-covenant.org/version/1/3/0/) 行为规范。

### 贡献者

感谢以下参与项目的人：

[@llyhwdmx](https://github.com/llyhwdmx)

## 使用许可

[Apache](LICENSE) © Richard Littauer
