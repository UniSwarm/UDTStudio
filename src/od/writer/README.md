# Writer

## EDS Writer

### Write an EDS file from a DeviceDescription class.
```c
void write(const DeviceDescription *deviceDescription, const QString &filePath) const;
```

## DCF Writer

### Write a DCF file from a DeviceConfiguration class.
```c
void write(DeviceConfiguration *deviceConfiguration, const QString &filePath) const;
```

### Write a DCF file from a DeviceDescription class.
```c
void write(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId) const;
```
