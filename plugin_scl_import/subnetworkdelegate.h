#ifndef SUBNETWORKDELEGATE_H
#define SUBNETWORKDELEGATE_H

#include <QItemDelegate>
#include "SObject.h"
class SubNetworkDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	SubNetworkDelegate(QObject *parent);
	~SubNetworkDelegate();

	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex & index) const;
	void setEditorData(QWidget *editor,const QModelIndex &index) const;
	void setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const;

private:
	
};

#endif // SUBNETWORKDELEGATE_H
