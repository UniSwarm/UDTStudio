# Generator

## C Generator

### Generate from a DeviceConfiguration
```c
bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath) const;
```
It's an overide method, returns true because it's implemented.

### Generate from a DeviceDescription
```c
void generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId) const;
``` 

## Extension
A new type of generator can be added in extension of the Generator class.
