#ifndef COMMANDS_H
#define COMMANDS_H

#include "skhead.h"
#include <QUndoCommand>
#include "drawscene.h"

///////////////////////// AddShapeCommand /////////////////////////
class AddShapeCommand : public QUndoCommand
{
public:
	explicit AddShapeCommand(QGraphicsItem *item, QGraphicsScene *graphicsScene, QUndoCommand *parent = 0);
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

///////////////////////// GroupShapeCommand /////////////////////////
class GroupShapeCommand : public QUndoCommand
{
public:
	explicit GroupShapeCommand(QGraphicsItemGroup *group, QGraphicsScene *graphicsScene,QUndoCommand *parent = 0);
	~GroupShapeCommand();

protected:
	virtual void undo();
	virtual void redo();

private:
	QList<QGraphicsItem *> m_listItem;
	QGraphicsItemGroup *m_pItemGroup;
	QGraphicsScene *m_pScene;
	bool b_undo;

};

///////////////////////// UnGroupShapeCommand /////////////////////////
class UnGroupShapeCommand : public QUndoCommand
{
public:
	explicit UnGroupShapeCommand( QGraphicsItemGroup *group, QGraphicsScene *graphicsScene, QUndoCommand *parent = 0);
	~UnGroupShapeCommand();

protected:
	void undo();
	void redo();

private:
	QList<QGraphicsItem *> m_listItem;
	QGraphicsItemGroup *m_pItemGroup;
	QGraphicsScene *m_pScene;

};

#endif // COMMANDS_H
