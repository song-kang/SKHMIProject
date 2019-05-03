#include "xmlparsethread.h"
#include "view_plugin_scl_import.h"
#include "QMessageBox"

XmlParseThread::XmlParseThread(QObject *parent)
	: QThread(parent)
{
	sclImport = (view_plugin_scl_import *)parent;
}

XmlParseThread::~XmlParseThread()
{

}

void XmlParseThread::run()
{
	QString			 fileName = sclImport->xmlFile;
	QXmlStreamReader reader;

	QFile file(fileName);
	if (file.open(QFile::ReadOnly | QFile::Text)) 
	{
		sclImport->parseError = QString::null;
		sclImport->parseWarn = QString::null;

		reader.setDevice(&file);

		XmlObject * parent  = 0;
		XmlObject * current = 0;
		while(!reader.atEnd())
		{
			sclImport->progressValue = reader.lineNumber();

			QXmlStreamReader::TokenType token = reader.readNext();
			if(reader.isStartElement())
			{
				current = new XmlObject();
				current->parent = parent;
				current->name   = reader.qualifiedName().toString();
				current->line   = reader.lineNumber();

				if(parent)
					parent->children.append(current);
				else
					sclImport->document.append(current);

				if(current->name == "SCL")
				{
					current->attributes.insert("xsi:schemaLocation","http://www.iec.ch/61850/2003/SCL SCL.xsd");
					current->attributes.insert("xmlns","http://www.iec.ch/61850/2003/SCL");
					current->attributes.insert("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
					current->attributes.insert("xmlns:sac","http://www.sac-china.com");
				}
				else
				{
					QXmlStreamAttributes attributes = reader.attributes();
					for(int i = 0; i < attributes.count(); i++)
					{
						QXmlStreamAttribute attrib = attributes.at(i);
						current->attributes.insert(attrib.qualifiedName().toString(),attrib.value().toString());
					}
				}

				parent = current;
			}
			else if(reader.isEndElement())
			{
				parent = parent->parent;
			}
			else if(token == QXmlStreamReader::Characters)
			{
				if(parent)
				{
					QString text = reader.text().toString().trimmed();
					if(!text.isEmpty())
					{
						parent->text = reader.text().toString();
						parent->attributes.insert("value",text);
					}
				}
			}

			if(reader.hasError())
			{
				sclImport->parseError = tr("XML语法错误！行号：%1，列号：%2，错误：%3。")
					.arg(reader.lineNumber()).arg(reader.columnNumber()).arg(reader.errorString());
				qDeleteAll(sclImport->document);
				sclImport->document.clear();
				break;
			}
		}	
	}
	else
	{
		sclImport->parseError = QObject::tr("只读方式打开文件[%1]失败。").arg(fileName);
	}

	reader.clear();
	file.close();
	sclImport->bProgressDialogStart = false;
}
