/**
 *
 * 文 件 名 : view_plugin_modbus_rtu.h
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

#ifndef __VIEW_PLUGIN_MODBUS_RTU_H__
#define __VIEW_PLUGIN_MODBUS_RTU_H__

#include "cbaseview.h"
#include "sk_database.h"
#include "ui_view_plugin_modbus_rtu.h"

enum eTreeItem {
	eNode = 1,
	eIed,
	eCpu,
	eGroup
};

class CNode;
class CIed;
class CCpu;
class CGroup;
class CElement
{

};

class CGroup
{

};

class CCpu
{

};

class CIed
{

};

class CNode
{
public:
	CNode() {
		m_iNo = 0;
	};

	CNode(quint32 no, QString name, QString desc, QString param) {
		m_iNo = no;
		m_sName = name;
		m_sDesc = desc;
		m_sParam = param;
	}

	~CNode() {
		foreach (CIed* pIed, m_lstIed) {
			delete pIed;
			pIed = NULL;
		}
	};

public:
	quint32 m_iNo;
	QString m_sName;
	QString m_sDesc;
	QString m_sParam;
	QList<CIed*> m_lstIed;
};

class view_plugin_modbus_rtu : public CBaseView
{
	Q_OBJECT

public:
	view_plugin_modbus_rtu(QWidget *parent = 0);
	~view_plugin_modbus_rtu();

	virtual void SetBackgroundColor(int red = 240, int yellow = 240, int blue = 240, int alpha = 230)
	{
		setStyleSheet(tr("QWidget#%1{background:rgb(%2,%3,%4,%5);}").arg(objectName()).arg(red).arg(yellow).arg(blue).arg(alpha));
	}

private:
	Ui::view_plugin_modbus_rtu ui;

	QTreeWidgetItem* m_pCurrentItem;
	QMap<int, QString> m_mapGroupType;
	int m_iCurrentTableType;
	bool m_isItemFixed;

private:
	void Init();
	void InitUi();
	void InitSlot();
	void Start();
	void StartIed(quint32 nodeNo, QTreeWidgetItem* pItem);
	void StartGroup(quint32 iedNo, QTreeWidgetItem* pItem);
	void DelIed(quint32 iedNo);
	void DelGroup(quint32 iedNo, quint32 groupNo);
	void AddGroup(QTreeWidgetItem* pItem, quint32 groupNo, quint32 groupType);
	void ShowGroupData(quint32 iedNo, quint32 groupNo);
	void ShowGroupDataAna(quint32 iedNo, quint32 groupNo);
	void ShowGroupDataDgt(quint32 iedNo, quint32 groupNo);
	void ShowGroupDataCtl(quint32 iedNo, quint32 groupNo);
	void ShowGroupDataSet(quint32 iedNo, quint32 groupNo);

public slots:
	void SlotTableItemChanged(QTableWidgetItem*);
	void SlotTreeItemClicked(QTreeWidgetItem*, int);
	void SlotTreeCustomContextMenuRequested(const QPoint& pos);
	void SlotAddNode();
	void SlotFixNode();
	void SlotDelNode();
	void SlotAddIed();
	void SlotFixIed();
	void SlotDelIed();
	void SlotAddGroup();
	void SlotFixGroup();
	void SlotDelGroup();
	void SlotTableCustomContextMenuRequested(const QPoint& pos);
	void SlotAddItem();
	void SlotDelItem();
	void SlotComItem();
	void SlotSeqItem();
	void SlotSaveItem();
	void SlotSaveItemAna();
	void SlotSaveItemDgt();
	void SlotSaveItemCtl();
	void SlotSaveItemSet();

};

#endif // __VIEW_PLUGIN_MODBUS_RTU_H__

