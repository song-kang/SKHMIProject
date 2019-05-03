#include "oe_ied_model_version.h"
#include "view_plugin_scl_import.h"

oe_ied_model_version::oe_ied_model_version(QList<XmlObject*> list,QString ied)
{
	model = "NULL";
	version = "NULL";
	crc = "NULL";
	scene = "NULL";
	defect = "NULL";

	document = list;
	iedName = ied;
	object = document.at(0)->findChildDeep("IED","name",iedName);
}

oe_ied_model_version::~oe_ied_model_version(void)
{
}

bool oe_ied_model_version::execute(QString & error)
{
	bool b = true;
	bool bInsert = false;

	if (!object)
	{
		LOGFAULT("��t_oe_ied_model_version�������ʱ��װ�ö���δ���֣�%s.", iedName.toLatin1().data());
		error = QObject::tr("��t_oe_ied_model_version�������ʱ��װ�ö���δ���֣�%s.").arg(iedName.toLatin1().data());
		return false;
	}

	if (!get_version())
		return false;
	if (!get_crc())
		return false;
	if (!get_model(error,&bInsert))
		return false;
	if (!get_scene())
		return false;
	if (!get_defect())
		return false;
	if (!bInsert)
		return true;

	SString sql = SString::toFormat("INSERT INTO t_oe_ied_model_version (model,version,crc,scene,defect) VALUES "
		"('%s','%s','%s','%s','%s')",
		model.data(),
		version.data(),
		crc.data(),
		scene.data(),
		defect.data());

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

bool oe_ied_model_version::get_model(QString & error,bool * bInsert)
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
		SString sql = SString::toFormat("SELECT model FROM t_oe_ied_model_version where model='%s' and version='%s' and crc='%s'",
			model.data(),
			version.data(),
			crc.data());
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

bool oe_ied_model_version::get_version()
{
	bool		b = true;

	version = object->attrib("configVersion").toStdString();
	if (version.isEmpty())
		version = "NULL";

	return b;
}

bool oe_ied_model_version::get_crc()
{
	bool		b = true;

	crc = "NULL";

	return b;
}

bool oe_ied_model_version::get_scene()
{
	bool		b = true;

	scene = "NULL";

	return b;
}

bool oe_ied_model_version::get_defect()
{
	bool		b = true;

	defect = "NULL";

	return b;
}