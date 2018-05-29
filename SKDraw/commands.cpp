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
		delete m_pItem;
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
