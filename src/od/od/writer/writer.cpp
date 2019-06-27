#include "writer.h"
#include "dcfwriter.h"

Writer::Writer()
{

}

Writer *Writer::getWriter(const QString &type)
{
    if (type == "dcf")
        return new DcfWriter();
    return nullptr;
}
