# UDTStudio

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.org/UniSwarm/UDTStudio.svg?branch=master)](https://travis-ci.org/UniSwarm/UDTStudio)
[![Build status](https://ci.appveyor.com/api/projects/status/7ts34vord4jnooub?svg=true)](https://ci.appveyor.com/project/sebcaux/udevkit-ide)

Tools for UniSwarm products.

## dependencies
### Qt > 5.9

Qt module used :

- core
- gui
- widgets
- serialport
- serialbus

## Build

```
mkdir build
cd build
qmake ../src
make -j
```

Binaries will be placed in `bin/` directory.
