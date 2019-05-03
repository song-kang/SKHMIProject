#include "datasetdelegate.h"
#include <QComboBox>

DataSetDelegate::DataSetDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

DataSetDelegate::~DataSetDelegate()
{
	
}

QWidget * DataSetDelegate::createEditor(QWidget * parent,const QStyleOptionViewItem & ,const QModelIndex & index) const
{
	S_UNUSED(index);
	QComboBox * combo = new QComboBox(parent);

	combo->addItem(tr("定值"));
	combo->addItem(tr("定值区"));
	combo->addItem(tr("测量量"));
	combo->addItem(tr("开关量"));
	combo->addItem(tr("软压板"));
	combo->addItem(tr("动作"));
	combo->addItem(tr("告警"));
	combo->addItem(tr("故障"));
	combo->addItem(tr("录波"));
	//combo->addItem(tr("TeleControlling"));
	//combo->addItem(tr("TeleAdjusting"));
	combo->addItem(tr("复杂测量量"));
	combo->addItem(tr("复杂开关量"));
	combo->addItem(tr("Goose"));
	combo->addItem(tr("SV"));
	combo->addItem(tr("日志"));
	combo->addItem(tr("未知"));
	combo->addItem(tr("不导入"));

	return combo;
}

void DataSetDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,const QModelIndex &) const
{
	if(editor)
	{
		editor->setGeometry(option.rect);
	}
}

void DataSetDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
	if(!index.isValid()) 
		return;

	QString text = index.data(Qt::DisplayRole).toString();

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo)
		combo->setCurrentIndex(combo->findText(text));;
}

void DataSetDelegate::setModelData(QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
	QString text = QString::null;

	QComboBox * combo = qobject_cast<QComboBox*>(editor);
	if(combo) 
		text = combo->itemText(combo->currentIndex());

	if(model) 
		model->setData(index,QVariant(text),Qt::DisplayRole);
}
