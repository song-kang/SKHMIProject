#include "oe_ied_model.h"
#include "dlgmanufacturer.h"
#include "view_plugin_scl_import.h"

oe_ied_model::oe_ied_model(QList<XmlObject*> list,QString ied)
{
	model = "NULL";
	manufacturer = "NULL";
	model_id = 0;
	maf_no = 0;

	document = list;
	iedName = ied;
	object = document.at(0)->findChildDeep("IED","name",iedName);
}


oe_ied_model::~oe_ied_model(void)
{
}

bool oe_ied_model::execute(QString & error)
{
	bool b = false;
	bool bInsert = false;

	if (!object)
	{
		LOGFAULT("��t_oe_ied_model�������ʱ��װ�ö���δ���֣�%s.", iedName.toLatin1().data());
		error = QObject::tr("��t_oe_ied_model�������ʱ��װ�ö���δ���֣�%s.").arg(iedName.toLatin1().data());
		return false;
	}
	if (!get_model(error,&bInsert))
		return false;
	if (!bInsert)  //����װ���ͺŲ���Ҫ����
		return true;
	if (!get_manufacturer(error))
		return false;
	if (!get_model_id(error))
		return false;
	if (!get_maf_no(error))
		return false;

	SString sql = SString::toFormat("INSERT INTO t_oe_ied_model(model,manufacturer,model_id,maf_no) VALUES ('%s','%s',%d,%d)",
		model.data(),manufacturer.data(),model_id,maf_no);

	b = DB->Execute(sql);
	if (!b)
	{
		error = "SQL���ִ�д���" + sql;
		return b;
	}

	if (m_bMDB)
	{
		b = MDB->Execute(sql);
		if (!b)
			error = "�ڴ��SQL���ִ�д���" + sql;
	}

	return b;
}

bool oe_ied_model::get_model(QString & error,bool * bInsert)
{
	bool		b = true;
	SRecordset	rs;

	model = object->attrib("type").toStdString();
	if (model.isEmpty())
	{
		//error = "δ��ȡ��IED��type��Ϣ.";
		//b = false;
		*bInsert = false;
	}
	else
	{
		SString sql = SString::toFormat("SELECT model FROM t_oe_ied_model where model='%s'",model.data());
		int iRet = DB->Retrieve(sql,rs);
		if (iRet < 0)	//����ʧ��
		{
			error = "SQL���ִ�д���" + sql;
			b = false;
		}
		else if (iRet == 0)	//����û�У��ɲ���
			*bInsert = true;
		else if (iRet > 0)	//���д��ڣ�������
			*bInsert = false;
	}
	
	return b;
}

bool oe_ied_model::get_manufacturer(QString & error)
{
	bool		b = true;

	manufacturer = object->attrib("manufacturer").toStdString();

	return b;
}

bool oe_ied_model::get_model_id(QString & error)
{
	SString		sql;
	SRecordset	rs;

	sql.sprintf("select max(model_id) from t_oe_ied_model ");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt <0)
	{
		error = "SQL���ִ�д���" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		model_id = 1;
	}
	else if (cnt > 0)
	{
		model_id = rs.GetValue(0,0).toInt();
		model_id ++;
	}

	return true;
}

bool oe_ied_model::get_maf_no(QString & error)
{
	SString sql;
	SRecordset rs;
	bool bFind = false;

	sql.sprintf("select maf_no,maf_name,maf_fullname,wildcard from t_oe_manufacturer_def");
	int cnt = DB->Retrieve(sql,rs);
	if (cnt < 0)
	{
		error = "SQL���ִ�д���" + sql;
		return false;
	}
	else if (cnt == 0)
	{
		//error = "���ҷ����t_oe_manufacturer_defΪ�գ������豸��������ʧ�ܡ�";
		//return false;
	}
	else if (cnt > 0)
	{
		for (int i = 0; i < cnt; i++)
		{
			int m_maf_no = rs.GetValue(i,0).toInt();
			QString m_maf_name = rs.GetValue(i,1).data();
			QString m_maf_fullname = rs.GetValue(i,2).data();
			QString m_wildcard = rs.GetValue(i,3).data();

			QRegExp rx(m_wildcard);
			QString m_manufacturer = object->attrib("manufacturer");
			if (rx.indexIn(m_manufacturer) >= 0)
			{
				maf_no = m_maf_no;
				bFind = true;
				break;
			}
		}
	}

	if (!bFind) //��t_oe_manufacturer_def��δ���ֳ���
	{
		DlgManufacturer dlg;
		QString m_manufacturer = object->attrib("manufacturer");
		dlg.setModel(model.data());
		dlg.setWildCard(m_manufacturer);
		dlg.start();
		if (dlg.exec() == QDialog::Accepted)
		{
			if (!dlg.getError().isEmpty())
			{
				error = dlg.getError();
				return false;
			}

			maf_no = dlg.getMafNo();
		}
	}

	return true;
}
