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

///////////////////////// MoveShapeCommand /////////////////////////
class MoveShapeCommand : public QUndoCommand
{
public:
	MoveShapeCommand(QGraphicsScene *graphicsScene, const QPointF &delta, QUndoCommand *parent = 0);
	MoveShapeCommand(QGraphicsItem *item, const QPointF &delta , QUndoCommand *parent = 0);
	~MoveShapeCommand();

protected:
	void undo() ;
	void redo() ;

private:
	bool m_bMoved;
	QPointF m_delta;
	QGraphicsItem  *m_pItem;
	QGraphicsScene *m_pScene;
	QList<QGraphicsItem *> m_listItem;

};

///////////////////////// RotateShapeCommand /////////////////////////
class RotateShapeCommand : public QUndoCommand
{
public:
	RotateShapeCommand(QGraphicsItem *item, const qreal oldAngle, QUndoCommand *parent = 0);
	~RotateShapeCommand();

protected:
	void undo() ;
	void redo() ;

private:
	QGraphicsItem *m_pItem;
	qreal m_oldAngle;
	qreal m_newAngle;
};

///////////////////////// ResizeShapeCommand /////////////////////////
class ResizeShapeCommand : public QUndoCommand
{
public:
	enum { Id = 1234, };
	ResizeShapeCommand(QGraphicsItem * item, int handle, const QPointF& scale, QUndoCommand *parent = 0 );
	~ResizeShapeCommand();

protected:
	void undo() ;
	void redo() ;

public:
	bool mergeWith(const QUndoCommand *command) ;
	int id() const  { return Id; }

private:
	QGraphicsItem *m_pItem;
	int handle_;
	int opposite_;
	QPointF scale_;
	bool bResized;
};

///////////////////////// ControlShapeCommand /////////////////////////
class ControlShapeCommand : public QUndoCommand
{
public:
	enum { Id = 1235, };
	ControlShapeCommand(QGraphicsItem * item, int handle, const QPointF& newPos, const QPointF& lastPos, QUndoCommand *parent = 0);
	~ControlShapeCommand();

protected:
	void undo() ;
	void redo() ;

public:
	bool mergeWith(const QUndoCommand *command) ;
	int id() const  { return Id; }

private:
	QGraphicsItem  *m_pItem;
	int handle_;
	QPointF lastPos_;
	QPointF newPos_;
	bool bControled;

};

#endif // COMMANDS_H
