#ifndef SKGUI_GLOBAL_H
#define SKGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SKGUI_LIB
# define SKGUI_EXPORT Q_DECL_EXPORT
#else
# define SKGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // SKGUI_GLOBAL_H
