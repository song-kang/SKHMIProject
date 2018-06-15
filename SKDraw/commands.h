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
	explicit UnGroupShapeCommand(QGraphicsItemGroup *group, QGraphicsScene *graphicsScene, QUndoCommand *parent = 0);
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
	MoveShapeCommand(DrawScene *scene, const QPointF &delta, QUndoCommand *parent = 0);
	MoveShapeCommand(DrawScene *scene, QGraphicsItem *item, const QPointF &delta , QUndoCommand *parent = 0);
	~MoveShapeCommand();

protected:
	void undo() ;
	void redo() ;

private:
	bool m_bMoved;
	QPointF m_delta;
	QGraphicsItem  *m_pItem;
	DrawScene *m_pScene;
	QList<QGraphicsItem *> m_listItem;

};

///////////////////////// RotateShapeCommand /////////////////////////
class RotateShapeCommand : public QUndoCommand
{
public:
	RotateShapeCommand(DrawScene *scene, QGraphicsItem *item, const qreal oldAngle, QUndoCommand *parent = 0);
	~RotateShapeCommand();

protected:
	void undo() ;
	void redo() ;

private:
	DrawScene *m_pScene;
	QGraphicsItem *m_pItem;
	qreal m_oldAngle;
	qreal m_newAngle;

};

///////////////////////// ResizeShapeCommand /////////////////////////
class ResizeShapeCommand : public QUndoCommand
{
public:
	enum { Id = 1234, };
	ResizeShapeCommand(DrawScene *scene, QGraphicsItem * item, int handle, const QPointF& scale, QUndoCommand *parent = 0 );
	~ResizeShapeCommand();

protected:
	void undo() ;
	void redo() ;

public:
	bool mergeWith(const QUndoCommand *command) ;
	int id() const  { return Id; }

private:
	DrawScene *m_pScene;
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
	ControlShapeCommand(DrawScene *scene, QGraphicsItem * item, int handle, const QPointF& newPos, const QPointF& lastPos, QUndoCommand *parent = 0);
	~ControlShapeCommand();

protected:
	void undo() ;
	void redo() ;

public:
	bool mergeWith(const QUndoCommand *command) ;
	int id() const  { return Id; }

private:
	DrawScene *m_pScene;
	QGraphicsItem  *m_pItem;
	int handle_;
	QPointF lastPos_;
	QPointF newPos_;
	bool bControled;

};

///////////////////////// PenPropertyCommand /////////////////////////
class PenPropertyCommand : public QUndoCommand
{
public:
	PenPropertyCommand(GraphicsItem *item, const QPen oldPen, QUndoCommand *parent = 0);
	~PenPropertyCommand();

protected:
	void undo() ;
	void redo() ;

private:
	DrawScene *m_pScene;
	GraphicsItem *m_pItem;
	QPen m_oldPen;
	QPen m_newPen;

};

///////////////////////// BrushPropertyCommand /////////////////////////
class BrushPropertyCommand : public QUndoCommand
{
public:
	BrushPropertyCommand(GraphicsItem *item, const QBrush oldBrush, QUndoCommand *parent = 0);
	~BrushPropertyCommand();

protected:
	void undo() ;
	void redo() ;

private:
	DrawScene *m_pScene;
	GraphicsItem *m_pItem;
	QBrush m_oldBrush;
	QBrush m_newBrush;

};

///////////////////////// FontPropertyCommand /////////////////////////
class FontPropertyCommand : public QUndoCommand
{
public:
	FontPropertyCommand(GraphicsItem *item, const QFont oldFont, QUndoCommand *parent = 0);
	~FontPropertyCommand();

protected:
	void undo() ;
	void redo() ;

private:
	DrawScene *m_pScene;
	GraphicsItem *m_pItem;
	QFont m_oldFont;
	QFont m_newFont;

};

///////////////////////// ScalePropertyCommand /////////////////////////
class ScalePropertyCommand : public QUndoCommand
{
public:
	ScalePropertyCommand(GraphicsItem *item, const qreal oldScale, QUndoCommand *parent = 0);
	~ScalePropertyCommand();

protected:
	void undo() ;
	void redo() ;

private:
	DrawScene *m_pScene;
	GraphicsItem *m_pItem;
	qreal m_oldScale;
	qreal m_newScale;

};

#endif // COMMANDS_H
