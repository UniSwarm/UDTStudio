# COOD

Console application for CANopen. 
Generates Object Dictionary's .h, .c and .dcf file from an EDS or a DCF file.

## How to use ?

### Options:
```bash
  -h, --help             Displays this help.
  -v, --version          Displays version information.
  -o, --out <out>        Output directory or file.
  -n, --nodeid <nodeid>  CANOpen Node Id.
```

### General use :
```bash
../../../bin/cood.sh in.eds -n 1
```

### Generates only a .h or a .c file
```bash
../../../bin/cood.sh in.eds -n 1 -o od_data.h
../../../bin/cood.sh in.eds -n 1 -o od_data.c
```

### Generates only a .dcf file
```bash
../../../bin/cood.sh in.eds -n 1 -o out.dcf
```
