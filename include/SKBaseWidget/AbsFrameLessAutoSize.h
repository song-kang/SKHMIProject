#ifndef ABSFRAMELESSAUTOSIZE_H
#define ABSFRAMELESSAUTOSIZE_H

#include <QtCore/qglobal.h>
#include "skhead.h"

#ifdef SKBASEWIDGET_LIB
# define ABSFRAMELESSAUTOSIZE_EXPORT Q_DECL_EXPORT
#else
# define ABSFRAMELESSAUTOSIZE_EXPORT Q_DECL_IMPORT
#endif

enum eCursorPos { Default,Right,Left,Bottom,Top,TopRight,TopLeft,BottomRight,BottomLeft };
struct pressWindowsState
{
    bool    MousePressed;
    bool	IsPressBorder;
    QPoint  MousePos;
    QPoint  WindowPos;
    QSize	PressedSize;
};

class ABSFRAMELESSAUTOSIZE_EXPORT AbsFrameLessAutoSize : public QWidget
{
    Q_OBJECT

public:
    AbsFrameLessAutoSize(QWidget *parent = 0);
    ~AbsFrameLessAutoSize(){}
    inline void setBorder(int border);
    void mouseMoveRect(const QPoint &p);
	void SetFixed(bool b) { m_bFixed = b; }

protected:
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    pressWindowsState m_state;
    int m_border;
    eCursorPos m_curPos;
	bool m_bFixed;

};

#endif // ABSFRAMELESSAUTOSIZE_H
