#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
LD_LIBRARY_PATH=/opt/Qt5.13.0/5.13.0/gcc_64/lib/ "$DIR/udtstudio"

