#include "subnetworkdelegate.h"
#include <QComboBox>

SubNetworkDelegate::SubNetworkDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

SubNetworkDelegate::~SubNetworkDelegate()
{
	
}

QWidget * SubNetworkDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & ,const QModelIndex & index) const
{
	S_UNUSED(index);
	QComboBox * combo = new QComboBox(parent);

	combo->addItem("");
	combo->addItem(tr("MMS A NET"));
	combo->addItem(tr("MMS B NET"));

	return combo;
}

void SubNetworkDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void SubNetworkDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if(!index.isValid()) 
		return;

	QString text = index.data(Qt::DisplayRole).toString();

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo)
		combo->setCurrentIndex(combo->findText(text));;
}

void SubNetworkDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
	QString text = QString::null;

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo) 
		text = combo->itemText(combo->currentIndex());

	if(model) 
		model->setData(index,QVariant(text),Qt::DisplayRole);
}
