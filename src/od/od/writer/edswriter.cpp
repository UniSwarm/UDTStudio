#include "edswriter.h"

#include "deviceiniwriter.h"

EdsWriter::EdsWriter()
{
}

EdsWriter::~EdsWriter()
{
}

void EdsWriter::write(const DeviceDescription *deviceDescription, const QString &dir) const
{
    QFile dcfFile(dir + "/" + deviceDescription->getFileName());

    if (!dcfFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&dcfFile);
    DeviceIniWriter writer(&out);

    writer.writeFileInfo(deviceDescription->fileInfos());
    writer.writeDeviceInfo(deviceDescription->deviceInfos());
    writer.writeDummyUsage(deviceDescription->dummyUsages());

    writer.writeObjects(deviceDescription);

    dcfFile.close();
}
