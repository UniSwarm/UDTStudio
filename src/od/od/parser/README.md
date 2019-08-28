# Parser

## EDS Parser

```c
DeviceDescription *parse(const QString &path) const;
```
Returns a DeviceDescription * completed by the parser.

## DCF Parser
```c
DeviceConfiguration *parse(const QString &path) const;
```
Returns a DeviceConfiguration * completed by the parser.

## Extension
A XDD parser can be added by extended the DeviceDescriptionParser class.

A XDC parer can be added by extended the DeviceConfigurationParser class.
