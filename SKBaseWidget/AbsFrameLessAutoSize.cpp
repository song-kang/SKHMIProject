#include "AbsFrameLessAutoSize.h"

AbsFrameLessAutoSize::AbsFrameLessAutoSize(QWidget *parent)
	: QWidget(parent)
{
	m_state.MousePressed=false;
	m_state.IsPressBorder=false;
	m_border = 6; //此数与可伸缩框与界面框的上下左右边距保持一致，即鼠标可显示出来的区域
	m_bFixed = false;

	setMouseTracking(true);
	setWindowFlags(Qt::FramelessWindowHint);
	setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

void AbsFrameLessAutoSize::setBorder(int border)
{
	m_border=border;
}

void AbsFrameLessAutoSize::mouseMoveRect(const QPoint& p)
{
	if(!m_state.IsPressBorder)
	{
		if (m_bFixed) //固定大小，不显示上下左右可拉动鼠标样式
			return;

		if(p.x()>width()-m_border&&p.y()<height()-m_border&&p.y()>m_border)//right side
		{
			setCursor(Qt::SizeHorCursor);
			m_curPos= Right;
		}
		else if(p.x()<m_border&&p.y()<height()-m_border&&p.y()>m_border)//left side;
		{
			setCursor(Qt::SizeHorCursor);
			m_curPos= Left;
		}
		else if(p.y()>height()-m_border&&p.x()>m_border&&p.x()<width()-m_border)//bottom side;
		{
			setCursor(Qt::SizeVerCursor);
			m_curPos= Bottom;
		}
		else if(p.y()<m_border&&p.x()>m_border&&p.x()<width()-m_border)
		{
			setCursor(Qt::SizeVerCursor);
			m_curPos=Top;
		}
		else if(p.y()<m_border&&p.x()>width()-m_border)
		{
			setCursor(Qt::SizeBDiagCursor);
			m_curPos=TopRight;
		}

		else if(p.y()<m_border&&p.x()<m_border)
		{
			setCursor(Qt::SizeFDiagCursor);
			m_curPos=TopLeft;
		}

		else if(p.x()>m_border&&p.y()>height()-m_border)
		{
			setCursor(Qt::SizeFDiagCursor);
			m_curPos=BottomRight;
		}

		else if(p.x()<m_border&&p.y()>height()-m_border)
		{
			setCursor(Qt::SizeBDiagCursor);
			m_curPos=BottomLeft;
		}
		else
		{
			m_curPos=Default;
			setCursor(Qt::ArrowCursor);
		}
	}
	else
	{
		switch (m_curPos) 
		{
		case Right:
			{
				int setW=QCursor::pos().x()-x();
				if(minimumWidth()<=setW&&setW<=maximumWidth())
					setGeometry(x(),y(),setW,height());
				break;
			}
		case Left:
			{
				int setW=x()+width()-QCursor::pos().x();
				int setX=QCursor::pos().x();
				if(minimumWidth()<=setW&&setW<=maximumWidth())
					setGeometry(setX,y(),setW,height());
				break;
			}
		case Bottom:
			{
				int setH=QCursor::pos().y()-y();
				if(minimumHeight()<=setH&&setH<=maximumHeight())
					setGeometry(x(),y(),width(),setH);
				break;
			}
		case Top:
			{
				int setH=y()-QCursor::pos().y()+height();
				if(minimumHeight()<=setH&&setH<=maximumHeight())
					setGeometry(x(),QCursor::pos().y(),width(),setH);
				break;
			}
		case TopRight:
			{
				int setH=y()+height()-QCursor::pos().y();
				int setW=QCursor::pos().x()-x();
				int setY=QCursor::pos().y();
				if(setH>=maximumHeight())
				{
					setY=m_state.WindowPos.y()+m_state.PressedSize.height()-height();
					setH=maximumHeight();
				}
				if(setH<=minimumHeight())
				{
					setY=m_state.WindowPos.y()+m_state.PressedSize.height()-height();
					setH=minimumHeight();
				}
				setGeometry(m_state.WindowPos.x(),setY,setW,setH);
				break;
			}
		case TopLeft:
			{
				int setY=QCursor::pos().y();
				int setX=QCursor::pos().x();

				int setW=pos().x()+width()-setX;
				int setH=pos().y()+height()-setY;
				int totalW= m_state.WindowPos.x()+m_state.PressedSize.width();
				int totalH=m_state.WindowPos.y()+m_state.PressedSize.height();

				if(totalW-setX>=maximumWidth())
				{
					setX=totalW-maximumWidth();
					setW=maximumWidth();
				}
				if(totalW-setX<=minimumWidth())
				{
					setX=totalW-minimumWidth();
					setW=minimumWidth();
				}
				if(totalH-setY>=maximumHeight())
				{
					setY=totalH-maximumHeight();
					setH=maximumHeight();
				}
				if(totalH-setY<=minimumHeight())
				{
					setY=totalH-minimumHeight();
					setH=minimumHeight();
				}
				setGeometry(setX,setY,setW,setH);
				break;
			}
		case BottomRight:
			{
				int setW=QCursor::pos().x()-x();
				int setH=QCursor::pos().y()-y();
				setGeometry(m_state.WindowPos.x(),m_state.WindowPos.y(),setW,setH);
				break;
			}
		case BottomLeft:
			{
				int setW=x()+width()-QCursor::pos().x();
				int setH=QCursor::pos().y()-m_state.WindowPos.y();
				int setX=QCursor::pos().x();
				int totalW= m_state.WindowPos.x()+m_state.PressedSize.width();
				if(totalW-setX>=maximumWidth())
				{
					setX=totalW-maximumWidth();
					setW=maximumWidth();
				}
				if(totalW-setX<=minimumWidth())
				{
					setX=totalW-minimumWidth();
					setW=minimumWidth();
				}
				setGeometry(setX,m_state.WindowPos.y(),setW,setH);
				break;
			}
		case Default:
			break;
		default:
			break;
		}
	}
}

void AbsFrameLessAutoSize::mousePressEvent(QMouseEvent *event)
{
	m_state.PressedSize=this->size();
	m_state.IsPressBorder=false;
	setFocus();
	if (event->button() == Qt::LeftButton)
	{
		m_state.WindowPos = this->pos();
		if(QRect(m_border+1,m_border+1,width()-(m_border+1)*2,height()-(m_border+1)*2).contains(QPoint(event->globalX()-x(),event->globalY()-y())))
		{
			m_state.MousePos = event->globalPos();
			m_state.MousePressed = true;
		}
		else
			m_state.IsPressBorder=true;
	}
}

void AbsFrameLessAutoSize::mouseMoveEvent(QMouseEvent *event)
{
	mouseMoveRect(mapFromGlobal(event->globalPos()));
	if (m_state.MousePressed)
	{
		this->move(m_state.WindowPos + (event->globalPos() - m_state.MousePos));
	}
}

void AbsFrameLessAutoSize::mouseReleaseEvent(QMouseEvent *event)
{
	m_state.IsPressBorder=false;
	if (event->button() == Qt::LeftButton)
	{
		this->m_state.MousePressed = false;
	}
	//setCursor(Qt::PointingHandCursor);
	setCursor(Qt::ArrowCursor);
}
