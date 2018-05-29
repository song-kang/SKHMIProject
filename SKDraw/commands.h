#ifndef COMMANDS_H
#define COMMANDS_H

#include "skhead.h"
#include <QUndoCommand>

///////////////////////// AddShapeCommand /////////////////////////
class AddShapeCommand : public QUndoCommand
{
public:
	AddShapeCommand(QGraphicsItem *item, QGraphicsScene *graphicsScene, QUndoCommand *parent = 0);
	~AddShapeCommand();

protected:
	virtual void undo();
	virtual void redo();

private:
	QGraphicsItem *m_pItem;
	QGraphicsScene *m_pScene;
	QPointF m_initialPosition;

};

///////////////////////// RemoveShapeCommand /////////////////////////
class RemoveShapeCommand : public QUndoCommand
{
public:
	explicit RemoveShapeCommand(QGraphicsScene *graphicsScene, QUndoCommand *parent = 0);
	~RemoveShapeCommand();

protected:
	virtual void undo();
	virtual void redo();

private:
	QList<QGraphicsItem *> m_listItem;
	QGraphicsScene *m_pScene;
};

#endif // COMMANDS_H
