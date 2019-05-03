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

	combo->addItem(tr("��ֵ"));
	combo->addItem(tr("��ֵ��"));
	combo->addItem(tr("������"));
	combo->addItem(tr("������"));
	combo->addItem(tr("��ѹ��"));
	combo->addItem(tr("����"));
	combo->addItem(tr("�澯"));
	combo->addItem(tr("����"));
	combo->addItem(tr("¼��"));
	//combo->addItem(tr("TeleControlling"));
	//combo->addItem(tr("TeleAdjusting"));
	combo->addItem(tr("���Ӳ�����"));
	combo->addItem(tr("���ӿ�����"));
	combo->addItem(tr("Goose"));
	combo->addItem(tr("SV"));
	combo->addItem(tr("��־"));
	combo->addItem(tr("δ֪"));
	combo->addItem(tr("������"));

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
