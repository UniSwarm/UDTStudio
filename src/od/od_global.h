#ifndef OD_GLOBAL_H
#define OD_GLOBAL_H


#include <QtCore/qglobal.h>

#if defined(OD_EXPORT_LIB)
    #define OD_EXPORT Q_DECL_EXPORT
#else
    #define OD_EXPORT Q_DECL_IMPORT
#endif

#endif // OD_GLOBAL_H
