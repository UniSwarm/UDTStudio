# UDTStudio

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)
[![Build Status](https://travis-ci.org/UniSwarm/UDTStudio.svg?branch=master)](https://travis-ci.org/UniSwarm/UDTStudio)
[![Build status](https://ci.appveyor.com/api/projects/status/7ts34vord4jnooub?svg=true)](https://ci.appveyor.com/project/sebcaux/udevkit-ide)

CANOpen tools for UniSwarm products.

![Screenshot UMC](screenshot-umc.png)

![Screenshot PDO](screenshot-pdo.png)

## dependencies
### Qt > 5.9

Qt module used :

- core
- gui
- widgets
- charts

## QT5 for UDTStudio : charts

```
sudo apt install git make g++ qtbase5-dev libqt5charts5-dev
```

## Build

Do not forget to init and update submodule before build ore clone recursively.

```
git clone https://github.com/UniSwarm/UDTStudio.git --recursive
cd UDTStudio
mkdir build
cd build
qmake ../src
make -j`nproc`
```

Binaries will be placed in `bin/` directory.

## EDS files

UDTStudio use an environment variable `EDS_PATH` to read EDS files :

`export EDS_PATH="pathToEdsRepertory"`

You can permanently set it by pushing it to your ~/.profile file.

A directory is provided with eds files of all UniSwarm products.
