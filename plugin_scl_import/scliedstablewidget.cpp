#include "scliedstablewidget.h"

SCLIedsTableWidget::SCLIedsTableWidget(QWidget *parent)
	: QTableWidget(parent)
{
	init();

	connect(horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(slotHeaderClicked(int)));
}

SCLIedsTableWidget::~SCLIedsTableWidget()
{

}

void SCLIedsTableWidget::init()
{
	horizontalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}"); 
	verticalHeader()->setStyleSheet("QHeaderView::section{background:yellow;}");
	setStyleSheet("QTableCornerButton::section{background:yellow;}");

	setSelectionBehavior(QAbstractItemView::SelectRows);		//����ѡ��ģʽ
	setEditTriggers(QAbstractItemView::NoEditTriggers);		//���ɱ༭
	setSelectionMode(QAbstractItemView::SingleSelection);		//��ѡģʽ
	setFocusPolicy(Qt::NoFocus);								//ȥ�����㣬�����
	//horizontalHeader()->setStretchLastSection(true);			//���ó�������
	setStyleSheet("selection-background-color:lightblue;");	//����ѡ�б���ɫ
	verticalHeader()->setDefaultSectionSize(20);				//�����и�
	horizontalHeader()->setHighlightSections(false);			//�����ʱ���Ա�ͷ�й���
	setAlternatingRowColors(true);								//���ý�����ɫ
}

void SCLIedsTableWidget::slotHeaderClicked(int column)
{
	int		i;
	bool	bInvert = true;

	if (!rowCount())
		return;

	if (column != TABLE_IEDS_NAME)
		return;

	for (i = 0; i < rowCount(); i++)
	{
		if (item(i,column)->checkState() != Qt::Checked)
		{
			item(i,column)->setCheckState(Qt::Checked);
			bInvert = false;
		}
	}

	if (bInvert)
	{
		for (i = 0; i < rowCount(); i++)
		{
			item(i,column)->setCheckState(Qt::Unchecked);
		}
	}
}

void SCLIedsTableWidget::reloadIedsTableWidget()
{
	row = 0;
	setRowCount(0);
	clearContents();

	show(document); 
	slotHeaderClicked(TABLE_IEDS_NAME);
}

void SCLIedsTableWidget::show(QList<XmlObject*> list)
{
	foreach(XmlObject* obj,list)
	{
		//Header,Substation�����٣��˴��ͺ�����
		if (obj->name == "Communication")
		{
			continue;	//�������ƽ�������ʾ��������children
		}

		if (obj->name == "IED")
		{
			if (isAappend(obj))
				appendTableWidget(obj);
			continue;	//�������ƽ�������ʾ��children
		}

		if (obj->name == "DataTypeTemplates")
		{
			continue;	//�������ƽ�������ʾ��������children
		}

		show(obj->children);	//���ݹ�
	}
}

bool SCLIedsTableWidget::isAappend(XmlObject * object)
{
	XmlObject *obj = object->findChildDeep("AccessPoint");
	if (obj->attrib("name") != "S1" && obj->attrib("name") != "G1" && obj->attrib("name") != "M1")	//��S1,G1,M1��apName�����ж�
	{
		setRowCount(++row);
		return true;
	}

	if (checkStateS1 == Qt::Checked)
	{
		obj = object->findChildDeep("AccessPoint","name","S1");
		if (obj)
		{
			setRowCount(++row);
			return true;
		}
	}

	if (checkStateG1 == Qt::Checked)
	{
		obj = object->findChildDeep("AccessPoint","name","G1");
		if (obj)
		{
			setRowCount(++row);
			return true;
		}
	}

	if (checkStateM1 == Qt::Checked)
	{
		obj = object->findChildDeep("AccessPoint","name","M1");
		if (obj)
		{
			setRowCount(++row);
			return true;
		}
	}

	return false;
}

void SCLIedsTableWidget::appendTableWidget(XmlObject * object)
{
	int column;
	QMap<QString,QString>::iterator iter;
	for(iter = object->attributes.begin(); iter != object->attributes.end(); iter++)
	{
		if (iter.key() == "name")
		{
			column = TABLE_IEDS_NAME;
		}
		else if (iter.key() == "desc")
		{
			column = TABLE_IEDS_DESC;
		}
		else if (iter.key() == "type")
		{
			column = TABLE_IEDS_TYPE;
		}
		else if (iter.key() == "manufacturer")
		{
			column = TABLE_IEDS_MANFACTURER;
		}
		else
			continue;

		QTableWidgetItem * item = new QTableWidgetItem(iter.value());
		item->setTextAlignment(Qt::AlignCenter);
		setItem(row-1,column,item);
	}
}
