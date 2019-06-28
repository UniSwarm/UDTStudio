#ifndef UDTGUI_GLOBAL_H
#define UDTGUI_GLOBAL_H


#include <QtCore/qglobal.h>

#if defined(UDTGUI_EXPORT_LIB)
    #define UDTGUI_EXPORT Q_DECL_EXPORT
#else
    #define UDTGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // UDTGUI_GLOBAL_H
