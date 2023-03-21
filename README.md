# 3gpp amrnb

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

[中文](README.zh-CN.md)

3gpp amrnb codec library

This warehouse is a RT thread ported version of 3gpp AMR, providing software encoding and decoding for AMR. In order to adapt to the MCU environment, all floats are modified to float to apply the acceleration function of the fpu, which may cause some audio distortion because some calculation accuracy is reduced from double to float. All MCUs containing fpus can use this warehouse to obtain a good encoding and decoding experience, such as stm32f407, stm32f429, stm32h750, and so on.

## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Example Readmes](#example-readmes)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Install

Open this software package using env package management, and the installation is complete.

## Usage

You can learn how to use decoder.c and encoder.c.

[README] (README_upstream.md) This is the upstream READDME, described in some sections.

## Example Readmes

There are two MSH commands in decoder.c and encoder.c to encode and decode audio files from the command line.

## Maintainers

[@myshowtogo](https://github.com/myshowtogo)
[@llyhwdmx](https://github.com/llyhwdmx)

## Contributing

Feel free to dive in! [Open an issue](https://github.com/RichardLitt/standard-readme/issues/new) or submit PRs.

Standard Readme follows the [Contributor Covenant](http://contributor-covenant.org/version/1/3/0/) Code of Conduct.

### Contributors

This project exists thanks to all the people who contribute. 

[@llyhwdmx](https://github.com/llyhwdmx)

## License

[Apache](LICENSE) © Richard Littauer
