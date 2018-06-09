#include "commands.h"

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
		GraphicsItemGroup *group = dynamic_cast<GraphicsItemGroup*>(m_pItem);
		if (group && group->type() != GraphicsItemGroup::Type)
			delete m_pItem;
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
		delete item;
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
	delete m_pItemGroup;
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
	foreach (QGraphicsItem *item, m_pItemGroup->childItems())
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
