#ifndef DATASETDELEGATE_H
#define DATASETDELEGATE_H

#include <QItemDelegate>
#include "SObject.h"

class DataSetDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	DataSetDelegate(QObject *parent);
	~DataSetDelegate();

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;

private:
	
};

#endif // DATASETDELEGATE_H
