/**
 *
 * 文 件 名 : view_plugin_modbus_rtu.cpp
 * 创建日期 : 2021-08-03 18:52
 * 作    者 : SspAssist(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : ModbusRtu配置界面
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author    Comments
 * ---  ----------  --------  -------------------------------------------
 * 001	 2021-08-03	SspAssist　创建文件
 *
 **/
#include "view_plugin_modbus_rtu.h"
#include "skgui.h"
#include "dlgnode.h"
#include "dlgied.h"
#include "dlggrp.h"
#include "dlgtable.h"

view_plugin_modbus_rtu::view_plugin_modbus_rtu(QWidget *parent)
: CBaseView(parent)
{
	ui.setupUi(this);

	Init();
	InitUi();
	InitSlot();
	Start();
}

view_plugin_modbus_rtu::~view_plugin_modbus_rtu()
{

}

void view_plugin_modbus_rtu::Init()
{
	SetBackgroundColor();
	m_mapGroupType.insert(GROUP_TYPE_ANA,"遥测组");
	m_mapGroupType.insert(GROUP_TYPE_DGT,"遥信组");
	m_mapGroupType.insert(GROUP_TYPE_CTL,"遥控组");
	m_mapGroupType.insert(GROUP_TYPE_SET,"定值组");
	m_pCurrentItem = NULL;
	m_iCurrentTableType = 0;
	m_isItemFixed = false;
}

void view_plugin_modbus_rtu::InitUi()
{
	ui.splitter->setStretchFactor(0,2);
	ui.splitter->setStretchFactor(1,8);
	ui.treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->verticalHeader()->setDefaultSectionSize(25);
}

void view_plugin_modbus_rtu::InitSlot()
{
	connect(ui.treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), 
		this, SLOT(SlotTreeItemClicked(QTreeWidgetItem*, int)));
	connect(ui.treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), 
		this, SLOT(SlotTreeCustomContextMenuRequested(const QPoint&)));
	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)), 
		this, SLOT(SlotTableCustomContextMenuRequested(const QPoint&)));
}

void view_plugin_modbus_rtu::Start()
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select node_no,description from t_oe_run_node where module_name='%s'",NODE_NAME);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		for (int i = 0; i < cnt; i++) {
			int nodeNo = rs.GetValue(i,0).toInt();
			QString desc = rs.GetValue(i,1);
			QTreeWidgetItem* pNodeItem = new QTreeWidgetItem(ui.treeWidget,eNode);
			pNodeItem->setText(0, desc);
			pNodeItem->setData(0, Qt::UserRole, nodeNo);
			pNodeItem->setIcon(0, QIcon(":/images/database"));
			StartIed(nodeNo, pNodeItem);
		}
	}
}

void view_plugin_modbus_rtu::StartIed(quint32 nodeNo, QTreeWidgetItem* pItem)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select ied_no from t_oe_run_device where node_no=%d",nodeNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		for (int i = 0; i < cnt; i++) {
			int iedNo = rs.GetValue(i,0).toInt();
			sql.sprintf("select name from t_oe_ied where ied_no=%d",iedNo);
			SString name = DB->SelectInto(sql);
			QTreeWidgetItem* pIedItem = new QTreeWidgetItem(pItem,eIed);
			pIedItem->setText(0, name.data());
			pIedItem->setData(0, Qt::UserRole, iedNo);
			pIedItem->setIcon(0, QIcon(":/images/device"));
			StartGroup(iedNo, pIedItem);
		}
	}
}

void view_plugin_modbus_rtu::StartGroup(quint32 iedNo, QTreeWidgetItem* pItem)
{
	SString sql;
	SRecordset rs;
	sql.sprintf("select group_no,name from t_oe_group where ied_no=%d and cpu_no=1",iedNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		for (int i = 0; i < cnt; i++) {
			int groupNo = rs.GetValue(i,0).toInt();
			SString name = rs.GetValue(i,1);
			QTreeWidgetItem* pGroupItem = new QTreeWidgetItem(pItem,eGroup);
			pGroupItem->setText(0, name.data());
			pGroupItem->setData(0, Qt::UserRole, groupNo);
			pGroupItem->setIcon(0, QIcon(":/images/open"));
		}
	}
}

void view_plugin_modbus_rtu::SlotTableItemChanged(QTableWidgetItem* pItem)
{
	m_isItemFixed = true;
}

void view_plugin_modbus_rtu::SlotTreeItemClicked(QTreeWidgetItem* pItem, int iColumn)
{
	if (m_isItemFixed && QMessageBox::question(this, "询问", 
		QString("有修改未保存，是否保存？").arg(m_pCurrentItem->text(0)),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
		SlotSaveItem();

	if (pItem->type() != eGroup) {
		ui.tableWidget->clear();
		ui.tableWidget->setColumnCount(0);
		ui.tableWidget->setRowCount(0);
		m_iCurrentTableType = 0;
		return;
	}

	disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), 
		this, SLOT(SlotTableItemChanged(QTableWidgetItem*)));
	m_pCurrentItem = pItem;
	ShowGroupData(pItem->parent()->data(0, Qt::UserRole).toUInt(),
		pItem->data(0, Qt::UserRole).toUInt());
	connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), 
		this, SLOT(SlotTableItemChanged(QTableWidgetItem*)));
}

void view_plugin_modbus_rtu::SlotTreeCustomContextMenuRequested(const QPoint& pos)
{
	m_pCurrentItem = ui.treeWidget->itemAt(pos);
	if(m_pCurrentItem == NULL) {
		QMenu menu(this);
		QAction* pAddNode = menu.addAction(QIcon(":/images/add"), "增加节点(&D)");
		connect(pAddNode, SIGNAL(triggered()), this, SLOT(SlotAddNode()));
		menu.exec(QCursor::pos());
	}
	else if (m_pCurrentItem->type() == eNode){
		QMenu menu(this);
		QAction* pAddIed = menu.addAction(QIcon(":/images/add"), "增加装置(&D)");
		menu.addSeparator();
		QAction* pFixNode = menu.addAction(QIcon(":/images/edit"), "修改节点(&F)");
		QAction* pDelNode = menu.addAction(QIcon(":/images/cancel"), "删除节点(&E)");
		connect(pAddIed, SIGNAL(triggered()), this, SLOT(SlotAddIed()));
		connect(pFixNode, SIGNAL(triggered()), this, SLOT(SlotFixNode()));
		connect(pDelNode, SIGNAL(triggered()), this, SLOT(SlotDelNode()));
		menu.exec(QCursor::pos());
	}
	else if (m_pCurrentItem->type() == eIed) {
		QMenu menu(this);
		QAction* pAddGroup = menu.addAction(QIcon(":/images/add"), "增加组(&D)");
		menu.addSeparator();
		QAction* pFixIed = menu.addAction(QIcon(":/images/edit"), "修改装置(&F)");
		QAction* pDelIed = menu.addAction(QIcon(":/images/cancel"), "删除装置(&E)");
		connect(pAddGroup, SIGNAL(triggered()), this, SLOT(SlotAddGroup()));
		connect(pFixIed, SIGNAL(triggered()), this, SLOT(SlotFixIed()));
		connect(pDelIed, SIGNAL(triggered()), this, SLOT(SlotDelIed()));
		menu.exec(QCursor::pos());
	}
	else if (m_pCurrentItem->type() == eGroup) {
		QMenu menu(this);
		QAction* pFixGroup = menu.addAction(QIcon(":/images/edit"), "修改组(&F)");
		QAction* pDelGroup = menu.addAction(QIcon(":/images/cancel"), "删除组(&E)");
		connect(pFixGroup, SIGNAL(triggered()), this, SLOT(SlotFixGroup()));
		connect(pDelGroup, SIGNAL(triggered()), this, SLOT(SlotDelGroup()));
		menu.exec(QCursor::pos());
	}
}

void view_plugin_modbus_rtu::SlotAddNode()
{
	DlgNode dlg(this);
	dlg.Start();
	if (dlg.exec() == QDialog::Accepted) {
		QTreeWidgetItem* pItem = new QTreeWidgetItem(ui.treeWidget,eNode);
		pItem->setText(0, dlg.GetDesc());
		pItem->setData(0, Qt::UserRole, dlg.GetNo());
		pItem->setIcon(0, QIcon(":/images/database"));
	}
}

void view_plugin_modbus_rtu::SlotFixNode()
{
	DlgNode dlg(this);
	dlg.StartFix(m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (dlg.exec() == QDialog::Accepted) {
		m_pCurrentItem->setText(0, dlg.GetDesc());
	}
}

void view_plugin_modbus_rtu::SlotDelNode()
{
	int ret = QMessageBox::question(this, "询问", 
		QString("确认删除【%1】及以下所有内容？").arg(m_pCurrentItem->text(0)),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret != QMessageBox::Yes)
		return;

	SString sql;
	SRecordset rs;
	sql.sprintf("select ied_no from t_oe_run_device where node_no=%d",
		m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		for (int i = 0; i < cnt; i++) {
			int iedNo = rs.GetValue(i,0).toInt();
			DelIed(iedNo);
		}
	}

	sql.sprintf("delete from t_oe_run_node where node_no=%d",
		m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	delete m_pCurrentItem;
	m_pCurrentItem = NULL;
}

void view_plugin_modbus_rtu::SlotAddIed()
{
	DlgIed dlg(this);
	dlg.Start(m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (dlg.exec() == QDialog::Accepted) {
		QTreeWidgetItem* pItem = new QTreeWidgetItem(m_pCurrentItem,eIed);
		pItem->setText(0, dlg.GetName());
		pItem->setData(0, Qt::UserRole, dlg.GetNo());
		pItem->setIcon(0, QIcon(":/images/device"));

		AddGroup(pItem, 1, GROUP_TYPE_DGT);
		AddGroup(pItem, 2, GROUP_TYPE_ANA);
		AddGroup(pItem, 3, GROUP_TYPE_CTL);
		AddGroup(pItem, 4, GROUP_TYPE_SET);
		m_pCurrentItem->setExpanded(true);
	}
}

void view_plugin_modbus_rtu::SlotFixIed()
{
	DlgIed dlg(this);
	dlg.StartFix(m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (dlg.exec() == QDialog::Accepted) {
		m_pCurrentItem->setText(0, dlg.GetName());
	}
}

void view_plugin_modbus_rtu::SlotDelIed()
{
	int ret = QMessageBox::question(this, "询问", 
		QString("确认删除【%1】及以下所有内容？").arg(m_pCurrentItem->text(0)),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret != QMessageBox::Yes)
		return;

	DelIed(m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	delete m_pCurrentItem;
	m_pCurrentItem = NULL;
}

void view_plugin_modbus_rtu::SlotAddGroup()
{
	DlgGrp dlg(this);
	dlg.Start(m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (dlg.exec() == QDialog::Accepted) {
		QTreeWidgetItem* pItem = new QTreeWidgetItem(m_pCurrentItem,eGroup);
		pItem->setText(0, dlg.GetName());
		pItem->setData(0, Qt::UserRole, dlg.GetNo());
		pItem->setIcon(0, QIcon(":/images/open"));
	}
}

void view_plugin_modbus_rtu::SlotFixGroup()
{
	DlgGrp dlg(this);
	dlg.StartFix(m_pCurrentItem->parent()->data(0, Qt::UserRole).toUInt(), 
		m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	if (dlg.exec() == QDialog::Accepted) {
		m_pCurrentItem->setText(0, dlg.GetName());
	}
}

void view_plugin_modbus_rtu::SlotDelGroup()
{	
	int ret = QMessageBox::question(this, "询问", 
		QString("确认删除【%1】及以下所有内容？").arg(m_pCurrentItem->text(0)),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret != QMessageBox::Yes)
		return;

	DelGroup(m_pCurrentItem->parent()->data(0, Qt::UserRole).toUInt(),
		m_pCurrentItem->data(0, Qt::UserRole).toUInt());
	delete m_pCurrentItem;
	m_pCurrentItem = NULL;
}

void view_plugin_modbus_rtu::DelIed(quint32 iedNo)
{
	SString sql;
	sql.sprintf("delete from t_oe_element_state where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_element_general where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_element_control where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_group where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_cpu where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_ied where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_run_device where ied_no=%d",iedNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
}

void view_plugin_modbus_rtu::DelGroup(quint32 iedNo, quint32 groupNo)
{
	SString sql;
	sql.sprintf("delete from t_oe_element_state where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_element_general where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_element_control where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
	sql.sprintf("delete from t_oe_group where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}
}

void view_plugin_modbus_rtu::AddGroup(QTreeWidgetItem* pItem, quint32 groupNo, quint32 groupType)
{
	SString sql;
	sql.sprintf("insert into t_oe_group(ied_no,cpu_no,group_no,name,type) "
		"values (%d,%d,%d,'%s',%d)",
		pItem->data(0,Qt::UserRole).toUInt(),
		1,groupNo,
		m_mapGroupType.value(groupType).toStdString().data(),
		groupType);
	if (!DB->Execute(sql)) {
		QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(sql.data()));
		return;
	}

	QTreeWidgetItem* ppItem = new QTreeWidgetItem(pItem,eGroup);
	ppItem->setText(0, m_mapGroupType.value(groupType));
	ppItem->setData(0, Qt::UserRole, groupNo);
	ppItem->setIcon(0, QIcon(":/images/open"));
}

void view_plugin_modbus_rtu::SlotTableCustomContextMenuRequested(const QPoint& pos)
{
	if (m_iCurrentTableType == 0)
		return;

	QMenu menu(this);
	QAction* pAddItem = menu.addAction(QIcon(":/images/add"), "增加条目(&D)");
	QAction* pDelItem = menu.addAction(QIcon(":/images/cancel"), "删除条目(&E)");
	menu.addSeparator();
	QAction* pComItem = menu.addAction(QIcon(":/images/edit"), "设同一值(&O)");
	QAction* pSeqItem = menu.addAction(QIcon(":/images/edit"), "设顺序值(&U)");
	menu.addSeparator();
	QAction* pSaveItem = menu.addAction(QIcon(":/images/save"), "保存(&S)");
	connect(pAddItem, SIGNAL(triggered()), this, SLOT(SlotAddItem()));
	connect(pDelItem, SIGNAL(triggered()), this, SLOT(SlotDelItem()));
	connect(pComItem, SIGNAL(triggered()), this, SLOT(SlotComItem()));
	connect(pSeqItem, SIGNAL(triggered()), this, SLOT(SlotSeqItem()));
	connect(pSaveItem, SIGNAL(triggered()), this, SLOT(SlotSaveItem()));
	menu.exec(QCursor::pos());
}

void view_plugin_modbus_rtu::SlotAddItem()
{
	DlgTable dlg(this);
	dlg.Start(eOperAdd);
	if (dlg.exec() == QDialog::Accepted) {
		int number = dlg.GetVal().toUInt();
		if (number == 0)
			return;

		int start = ui.tableWidget->rowCount();
		switch (m_iCurrentTableType) {
		case GROUP_TYPE_ANA:
			ui.tableWidget->setRowCount(start + number);
			for (int i = start; i < start + number; i++) {
				QTableWidgetItem* pItem = new QTableWidgetItem(QString("遥测%1").arg(i+1));
				ui.tableWidget->setItem(i,0,pItem);
				for (int j = 1; j < 4; j++) {
					pItem = new QTableWidgetItem;
					ui.tableWidget->setItem(i,j,pItem);
				}
			}
			break;
		case GROUP_TYPE_DGT:
			ui.tableWidget->setRowCount(start + number);
			for (int i = start; i < start + number; i++) {
				QTableWidgetItem* pItem = new QTableWidgetItem(QString("遥信%1").arg(i+1));
				ui.tableWidget->setItem(i,0,pItem);
				for (int j = 1; j < 3; j++) {
					pItem = new QTableWidgetItem;
					ui.tableWidget->setItem(i,j,pItem);
				}
			}
			break;
		case GROUP_TYPE_CTL:
			break;
		case GROUP_TYPE_SET:
			break;
		default:
			return;
		}
		m_isItemFixed = true;
	}
}

void view_plugin_modbus_rtu::SlotDelItem()
{
	if (ui.tableWidget->columnCount() == 0)
		return;

	QMap<int,int> mapRow;
	QItemSelectionModel *selections = ui.tableWidget->selectionModel();
	QModelIndexList selectedsList = selections->selectedIndexes();
	foreach (QModelIndex index, selectedsList) {
		QMap<int,int>::iterator iter = mapRow.find(index.row());
		if (iter == mapRow.end()) {
			mapRow.insert(index.row(),index.row());
		}
	}
	if (mapRow.count() == 0)
		return;

	QMap<int,int>::iterator iter;
	for (iter = --mapRow.end(); iter != --mapRow.begin(); iter--) {
		ui.tableWidget->removeRow(iter.value());
	}
	m_isItemFixed = true;
}

void view_plugin_modbus_rtu::SlotComItem()
{
	if (ui.tableWidget->columnCount() == 0)
		return;

	DlgTable dlg(this);
	dlg.Start(eOperCommon);
	if (dlg.exec() == QDialog::Accepted) {
		QString val = dlg.GetVal();
		QItemSelectionModel *selections = ui.tableWidget->selectionModel();
		QModelIndexList selectedsList = selections->selectedIndexes();
		foreach (QModelIndex index, selectedsList) {
			ui.tableWidget->item(index.row(),index.column())->setText(val);
		}
		m_isItemFixed = true;
	}
}

void view_plugin_modbus_rtu::SlotSeqItem()
{
	if (ui.tableWidget->columnCount() == 0)
		return;

	DlgTable dlg(this);
	dlg.Start(eOperSequence);
	if (dlg.exec() == QDialog::Accepted) {
		int val = dlg.GetVal().toUInt();
		QItemSelectionModel *selections = ui.tableWidget->selectionModel();
		QModelIndexList selectedsList = selections->selectedIndexes();
		foreach (QModelIndex index, selectedsList) {
			ui.tableWidget->item(index.row(),index.column())->setText(QString("%1").arg(val++));
		}
		m_isItemFixed = true;
	}
}

void view_plugin_modbus_rtu::SlotSaveItem()
{
	if (!m_pCurrentItem || 
		!m_pCurrentItem->parent() || 
		ui.tableWidget->rowCount() == 0)
		return;

	SString sql;
	switch (m_iCurrentTableType) {
	case GROUP_TYPE_ANA:
		sql.sprintf("delete from t_oe_element_general "
			"where ied_no=%d and cpu_no=1 and group_no=%d",
			m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt(),
			m_pCurrentItem->data(0,Qt::UserRole).toUInt());
		DB->Execute(sql);
		SlotSaveItemAna();
		break;
	case GROUP_TYPE_DGT:
		sql.sprintf("delete from t_oe_element_state "
			"where ied_no=%d and cpu_no=1 and group_no=%d",
			m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt(),
			m_pCurrentItem->data(0,Qt::UserRole).toUInt());
		DB->Execute(sql);
		SlotSaveItemDgt();
		break;
	case GROUP_TYPE_CTL:
		sql.sprintf("delete from t_oe_element_control "
			"where ied_no=%d and cpu_no=1 and group_no=%d",
			m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt(),
			m_pCurrentItem->data(0,Qt::UserRole).toUInt());
		DB->Execute(sql);
		SlotSaveItemCtl();
		break;
	case GROUP_TYPE_SET:
		sql.sprintf("delete from t_oe_element_general "
			"where ied_no=%d and cpu_no=1 and group_no=%d",
			m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt(),
			m_pCurrentItem->data(0,Qt::UserRole).toUInt());
		DB->Execute(sql);
		SlotSaveItemSet();
		break;
	default:
		break;
	}

	m_isItemFixed = false;
}

void view_plugin_modbus_rtu::SlotSaveItemAna()
{
	quint32 iedNo = m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt();
	quint32 groupNo = m_pCurrentItem->data(0,Qt::UserRole).toUInt();

	int cnt = 0;
	SString sql,sqlData;
	sql.sprintf("insert into t_oe_element_general(ied_no,cpu_no,group_no,entry,name,valtype,"
		"maxval,minval,stepval,precision_n,precision_m,factor,offset,threshold,smooth,read_fun_code,read_address) values ");
	for (int row = 0; row < ui.tableWidget->rowCount(); row++) {
		QString name = ui.tableWidget->item(row,0)->text().trimmed();
		quint32 type = ui.tableWidget->item(row,1)->text().trimmed().toUInt();
		quint32 code = ui.tableWidget->item(row,2)->text().trimmed().toUInt();
		quint32 addr = ui.tableWidget->item(row,3)->text().trimmed().toUInt();
		SString s;
		s.sprintf("(%d,1,%d,%d,'%s',%d,65535.00,0.00,0.01,6,%d,1.0,0.0,1.0,0.0,%d,%d),",
			iedNo,groupNo,row+1,name.toStdString().data(),type,type==7?3:0,code,addr);
		sqlData += s;
		if (++cnt == 100) {
			SString ss = sql + sqlData;
			ss = ss.left(ss.size()-1);
			if (!DB->Execute(ss)) {
				QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(ss.data()));
				return;
			}
			DB->Execute("commit;");
			sqlData = "";
			cnt = 0;
		}
	}
	if (cnt > 0) {
		SString ss = sql + sqlData;
		ss = ss.left(ss.size()-1);
		if (!DB->Execute(ss)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(ss.data()));
			return;
		}
		DB->Execute("commit;");
	}
	QMessageBox::information(this, "提示", "保存成功");
}

void view_plugin_modbus_rtu::SlotSaveItemDgt()
{
	quint32 iedNo = m_pCurrentItem->parent()->data(0,Qt::UserRole).toUInt();
	quint32 groupNo = m_pCurrentItem->data(0,Qt::UserRole).toUInt();

	int cnt = 0;
	SString sql,sqlData;
	sql.sprintf("insert into t_oe_element_state(ied_no,cpu_no,group_no,entry,name,"
		"fun,inf,type,val_type,read_fun_code,read_address) values ");
	for (int row = 0; row < ui.tableWidget->rowCount(); row++) {
		QString name = ui.tableWidget->item(row,0)->text().trimmed();
		quint32 code = ui.tableWidget->item(row,1)->text().trimmed().toUInt();
		quint32 addr = ui.tableWidget->item(row,2)->text().trimmed().toUInt();
		SString s;
		s.sprintf("(%d,1,%d,%d,'%s',100,%d,4,2,%d,%d),",
			iedNo,groupNo,row+1,name.toStdString().data(),row+1,code,addr);
		sqlData += s;
		if (++cnt == 100) {
			SString ss = sql + sqlData;
			ss = ss.left(ss.size()-1);
			if (!DB->Execute(ss)) {
				QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(ss.data()));
				return;
			}
			DB->Execute("commit;");
			sqlData = "";
			cnt = 0;
		}
	}
	if (cnt > 0) {
		SString ss = sql + sqlData;
		ss = ss.left(ss.size()-1);
		if (!DB->Execute(ss)) {
			QMessageBox::critical(this,tr("错误"),tr("SQL语句执行错误。\n\n%1").arg(ss.data()));
			return;
		}
		DB->Execute("commit;");
	}
	QMessageBox::information(this, "提示", "保存成功");
}

void view_plugin_modbus_rtu::SlotSaveItemCtl()
{

}

void view_plugin_modbus_rtu::SlotSaveItemSet()
{

}

void view_plugin_modbus_rtu::ShowGroupData(quint32 iedNo, quint32 groupNo)
{
	ui.tableWidget->clear();
	ui.tableWidget->setColumnCount(0);
	ui.tableWidget->setRowCount(0);

	SString sql;
	sql.sprintf("select type from t_oe_group where ied_no=%d and cpu_no=1 and group_no=%d",
		iedNo,groupNo);
	m_iCurrentTableType = DB->SelectIntoI(sql);
	switch (m_iCurrentTableType) {
	case GROUP_TYPE_ANA:
		ShowGroupDataAna(iedNo, groupNo);
		break;
	case GROUP_TYPE_DGT:
		ShowGroupDataDgt(iedNo, groupNo);
		break;
	case GROUP_TYPE_CTL:
		ShowGroupDataCtl(iedNo, groupNo);
		break;
	case GROUP_TYPE_SET:
		ShowGroupDataSet(iedNo, groupNo);
		break;
	default:
		break;
	}
}

void view_plugin_modbus_rtu::ShowGroupDataAna(quint32 iedNo, quint32 groupNo)
{
	QStringList header;
	header << tr("名称") << tr("值类型") << tr("功能码") << tr("地址");
	ui.tableWidget->setColumnCount(4);
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->setColumnWidth(0,300);
	ui.tableWidget->setColumnWidth(1,100);
	ui.tableWidget->setColumnWidth(2,150);
	ui.tableWidget->setColumnWidth(3,150);

	SString sql;
	SRecordset rs;
	sql.sprintf("select name,valtype,read_fun_code,read_address from t_oe_element_general "
		"where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		ui.tableWidget->setRowCount(cnt);
		for (int i = 0; i < cnt; i++) {
			QTableWidgetItem* pItem = new QTableWidgetItem(rs.GetValue(i,0).data());
			ui.tableWidget->setItem(i,0,pItem);
			pItem = new QTableWidgetItem(rs.GetValue(i,1).data());
			ui.tableWidget->setItem(i,1,pItem);
			pItem = new QTableWidgetItem(rs.GetValue(i,2).data());
			ui.tableWidget->setItem(i,2,pItem);
			pItem = new QTableWidgetItem(rs.GetValue(i,3).data());
			ui.tableWidget->setItem(i,3,pItem);
		}
	}
}

void view_plugin_modbus_rtu::ShowGroupDataDgt(quint32 iedNo, quint32 groupNo)
{
	QStringList header;
	header << tr("名称") << tr("功能码") << tr("地址");
	ui.tableWidget->setColumnCount(3);
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->setColumnWidth(0,300);
	ui.tableWidget->setColumnWidth(1,150);
	ui.tableWidget->setColumnWidth(2,150);

	SString sql;
	SRecordset rs;
	sql.sprintf("select name,read_fun_code,read_address from t_oe_element_state "
		"where ied_no=%d and cpu_no=1 and group_no=%d",iedNo,groupNo);
	int cnt = DB->Retrieve(sql,rs);
	if (cnt > 0) {
		ui.tableWidget->setRowCount(cnt);
		for (int i = 0; i < cnt; i++) {
			QTableWidgetItem* pItem = new QTableWidgetItem(rs.GetValue(i,0).data());
			ui.tableWidget->setItem(i,0,pItem);
			pItem = new QTableWidgetItem(rs.GetValue(i,1).data());
			ui.tableWidget->setItem(i,1,pItem);
			pItem = new QTableWidgetItem(rs.GetValue(i,2).data());
			ui.tableWidget->setItem(i,2,pItem);
		}
	}
}

void view_plugin_modbus_rtu::ShowGroupDataCtl(quint32 iedNo, quint32 groupNo)
{

}

void view_plugin_modbus_rtu::ShowGroupDataSet(quint32 iedNo, quint32 groupNo)
{

}
