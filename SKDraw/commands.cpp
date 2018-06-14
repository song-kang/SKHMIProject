#include "commands.h"

static QMap<int, int> c_mapPtr;
///////////////////////// AddShapeCommand /////////////////////////
AddShapeCommand::AddShapeCommand(QGraphicsItem *item, QGraphicsScene *scene, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pItem = item;
	m_pScene = scene;
	m_initialPosition = item->pos();
}

AddShapeCommand::~AddShapeCommand()
{
	if (m_pItem)
	{
		if (c_mapPtr.value((int)m_pItem) == 0)
		{
			c_mapPtr.insert((int)m_pItem, (int)m_pItem);
			delete m_pItem;
		}
	}
}

void AddShapeCommand::undo()
{
	m_pScene->removeItem(m_pItem);
	m_pScene->update();
}

void AddShapeCommand::redo()
{
	if (m_pItem->scene() == NULL)
		m_pScene->addItem(m_pItem);

	m_pItem->setPos(m_initialPosition);
	m_pScene->update();
}

///////////////////////// RemoveShapeCommand /////////////////////////
RemoveShapeCommand::RemoveShapeCommand(QGraphicsScene *scene, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pScene = scene;
	m_listItem = m_pScene->selectedItems();
}

RemoveShapeCommand::~RemoveShapeCommand()
{
	foreach (QGraphicsItem *item, m_listItem)
	{
		if (c_mapPtr.value((int)item) == 0)
		{
			c_mapPtr.insert((int)item, (int)item);
			delete item;
		}
	}
	m_listItem.clear();
}

void RemoveShapeCommand::undo()
{
	foreach (QGraphicsItem *item, m_listItem)
	{
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (!g)
			m_pScene->addItem(item);
	}

	m_pScene->update();
}

void RemoveShapeCommand::redo()
{
	foreach (QGraphicsItem *item, m_listItem)
	{
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (!g)
			m_pScene->removeItem(item);
	}
}

///////////////////////// GroupShapeCommand /////////////////////////
GroupShapeCommand::GroupShapeCommand(QGraphicsItemGroup *group, QGraphicsScene *graphicsScene, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pScene = graphicsScene;
	m_pItemGroup = group;
	m_listItem = group->childItems();
	b_undo = false;
}

GroupShapeCommand::~GroupShapeCommand()
{
	//foreach (QGraphicsItem *item, m_listItem)
	//{
	//	if (c_mapPtr.value((int)item) == 0)
	//	{
	//		c_mapPtr.insert((int)item, (int)item);
	//		delete item;
	//	}
	//}

	if (c_mapPtr.value((int)m_pItemGroup) == 0)
	{
		c_mapPtr.insert((int)m_pItemGroup, (int)m_pItemGroup);
		delete m_pItemGroup;
	}
}

void GroupShapeCommand::undo()
{
	m_pItemGroup->setSelected(false);

	QList<QGraphicsItem*> plist = m_pItemGroup->childItems();
	foreach (QGraphicsItem *item, plist)
	{
		item->setSelected(true);
		m_pItemGroup->removeFromGroup(item);
	}

	m_pScene->removeItem(m_pItemGroup);
	m_pScene->update();
	b_undo = true;
}

void GroupShapeCommand::redo()
{
	if (b_undo)
	{
		foreach (QGraphicsItem *item, m_listItem)
		{
			item->setSelected(false);
			QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
			if (!g)
				m_pItemGroup->addToGroup(item);
		}
	}

	m_pItemGroup->setSelected(true);
	if (m_pItemGroup->scene() == NULL)
		m_pScene->addItem(m_pItemGroup);

	m_pScene->update();
}

///////////////////////// UnGroupShapeCommand /////////////////////////
UnGroupShapeCommand::UnGroupShapeCommand(QGraphicsItemGroup *group, QGraphicsScene *graphicsScene, QUndoCommand *parent)
	:QUndoCommand(parent)
{
	m_pScene = graphicsScene;
	m_pItemGroup = group;
	m_listItem = group->childItems();
}

UnGroupShapeCommand::~UnGroupShapeCommand()
{
	//foreach (QGraphicsItem *item, m_listItem)
	//{
	//	if (c_mapPtr.value((int)item) == 0)
	//	{
	//		c_mapPtr.insert((int)item, (int)item);
	//		delete item;
	//	}
	//}

	if (c_mapPtr.value((int)m_pItemGroup) == 0)
	{
		c_mapPtr.insert((int)m_pItemGroup, (int)m_pItemGroup);
		delete m_pItemGroup;
	}
}

void UnGroupShapeCommand::undo()
{
	foreach (QGraphicsItem *item, m_listItem)
	{
		item->setSelected(false);
		QGraphicsItemGroup *g = dynamic_cast<QGraphicsItemGroup*>(item->parentItem());
		if (!g)
			m_pItemGroup->addToGroup(item);
	}

	m_pItemGroup->setSelected(true);
	if (m_pItemGroup->scene() == NULL)
		m_pScene->addItem(m_pItemGroup);

	m_pScene->update();
}

void UnGroupShapeCommand::redo()
{
	m_pItemGroup->setSelected(false);
	foreach (QGraphicsItem *item, m_listItem)
	{
		item->setSelected(true);
		m_pItemGroup->removeFromGroup(item);

		AbstractShape *ab = qgraphicsitem_cast<AbstractShape*>(item);
		if (ab && !qgraphicsitem_cast<SizeHandleRect*>(ab))
			ab->UpdateCoordinate();
	}

	m_pScene->removeItem(m_pItemGroup);
	m_pScene->update();
}

///////////////////////// MoveShapeCommand /////////////////////////
MoveShapeCommand::MoveShapeCommand(QGraphicsScene *graphicsScene, const QPointF &delta, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pItem = NULL;
	m_listItem = graphicsScene->selectedItems();
	m_pScene = graphicsScene;
	m_delta = delta;
	m_bMoved = true;
}

MoveShapeCommand::MoveShapeCommand(QGraphicsItem *item, const QPointF &delta, QUndoCommand *parent)
	: QUndoCommand(parent)
{
	m_pScene = 0;
	m_pItem = item;
	m_delta = delta;
	m_bMoved = true;
}

MoveShapeCommand::~MoveShapeCommand()
{

}

void MoveShapeCommand::undo()
{
	if (m_pItem)
	{
		m_pItem->moveBy(-m_delta.x(),-m_delta.y());
	}
	else if (m_listItem.count() > 0)
	{
		foreach (QGraphicsItem *item, m_listItem)
			item->moveBy(-m_delta.x(),-m_delta.y());
	}

	m_bMoved = false;
}

void MoveShapeCommand::redo()
{
	if (!m_bMoved)
	{
		if (m_pItem)
		{
			m_pItem->moveBy(m_delta.x(),m_delta.y());
			m_pItem->scene()->update();
		}
		else if (m_listItem.count() > 0)
		{
			foreach (QGraphicsItem *item, m_listItem)
				item->moveBy(m_delta.x(),m_delta.y());
			m_pScene->update();
		}
	}
}

///////////////////////// RotateShapeCommand /////////////////////////
RotateShapeCommand::RotateShapeCommand(QGraphicsItem *item, const qreal oldAngle, QUndoCommand *parent)
	:QUndoCommand(parent)
{
	m_pItem = item;
	m_oldAngle = oldAngle;
	m_newAngle = item->rotation();
}

RotateShapeCommand::~RotateShapeCommand()
{

}

void RotateShapeCommand::undo()
{
	m_pItem->setRotation(m_oldAngle);
	m_pItem->scene()->update();
}

void RotateShapeCommand::redo()
{
	m_pItem->setRotation(m_newAngle);
	m_pItem->update();
}

///////////////////////// ResizeShapeCommand /////////////////////////
ResizeShapeCommand::ResizeShapeCommand(QGraphicsItem *item, int handle, const QPointF& scale, QUndoCommand *parent)
{
	m_pItem = item;
	handle_ = handle;
	scale_  = QPointF(scale);
	opposite_ = eHandleNone;
	bResized = true;       
}

ResizeShapeCommand::~ResizeShapeCommand()
{

}

void ResizeShapeCommand::undo()
{
	int handle = handle_;
	AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(m_pItem);
	if (item)
	{
		if (eHandleNone != opposite_)
			handle = opposite_;

		item->Stretch(handle, 1./scale_.x(), 1./scale_.y(), item->Opposite(handle));
		item->UpdateCoordinate();
		item->update();
	}

	bResized = false;
}

void ResizeShapeCommand::redo()
{
	int handle = handle_;
	if (!bResized)
	{
		AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(m_pItem);
		if (item)
		{
			item->Stretch(handle, scale_.x(), scale_.y(), item->Opposite(handle));
			item->UpdateCoordinate();
			item->update();
		}
	}
}

bool ResizeShapeCommand::mergeWith(const QUndoCommand *command)
{
	if (command->id() != ResizeShapeCommand::Id)
		return false;

	const ResizeShapeCommand *cmd = static_cast<const ResizeShapeCommand *>(command);

	QGraphicsItem *item = cmd->m_pItem;
	if (m_pItem != item)
		return false;

	if (cmd->handle_ != handle_)
		return false;

	AbstractShape * ab = qgraphicsitem_cast<AbstractShape*>(item);

	opposite_ = ab->SwapHandle(cmd->handle_, cmd->scale_);

	handle_ = cmd->handle_;
	scale_ = cmd->scale_;

	return true;
}

///////////////////////// ControlShapeCommand /////////////////////////
ControlShapeCommand::ControlShapeCommand(QGraphicsItem *item, int handle, const QPointF& newPos, const QPointF& lastPos, QUndoCommand *parent)
{
	m_pItem = item;
	handle_ = handle;
	lastPos_  = QPointF(lastPos) ;
	newPos_ = QPointF(newPos);
	bControled = true;
}

ControlShapeCommand::~ControlShapeCommand()
{

}

void ControlShapeCommand::undo()
{
	AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(m_pItem);
	if (item)
	{
		item->Control(handle_,lastPos_);
		item->UpdateCoordinate();
		item->update();
	}

	bControled = false;
}

void ControlShapeCommand::redo()
{
	if (!bControled)
	{
		AbstractShape *item = qgraphicsitem_cast<AbstractShape*>(m_pItem);
		if (item)
		{
			item->Control(handle_,newPos_);
			item->UpdateCoordinate();
			item->update();
		}
	}
}

bool ControlShapeCommand::mergeWith(const QUndoCommand *command)
{
	if (command->id() != ControlShapeCommand::Id)
		return false;

	const ControlShapeCommand *cmd = static_cast<const ControlShapeCommand *>(command);
	QGraphicsItem *item = cmd->m_pItem;

	if (m_pItem != item )
		return false;
	if ( cmd->handle_ != handle_ )
		return false;

	handle_ = cmd->handle_;
	lastPos_ = cmd->lastPos_;
	newPos_  = cmd->newPos_;

	return true;
}
