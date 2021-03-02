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
- charts
- serialbus

## QT5 for UDTStudio : charts + serial buses
```
sudo apt install make g++ qt5-default libqt5serialbus5-dev libqt5serialbus5-plugins libqt5charts5-dev
```

## Build

Do not forget to init and update submodule before build :

```
git submodule init
git submodule update
```

```
mkdir build
cd build
qmake ../src
make -j
```

Binaries will be placed in `bin/` directory.

## EDS File

UDT Studio use an environment variable `EDS_PATH` to read EDS files :

`export EDS_PATH="pathToEdsRepertory"`

You can permanently set it by pushing it to your ~/.profile file.
