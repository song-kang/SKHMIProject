/**
 *
 * �� �� �� : ScriptParser.cpp
 * �������� : 2016-10-27 11:10
 * ��    �� : �ۿ���(skt001@163.com)
 * �޸����� : $Date: $
 * ��ǰ�汾 : $Revision: $
 * �������� : C�﷨�ű�������
 * �޸ļ�¼ : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-10-27	�ۿ�������ļ�
 *
 **/

#include "SScriptParser.h"

SScriptParser::SScriptParser()
{
	m_Functions.setAutoDelete(true);
	m_InstVariant.setAutoDelete(false);
	m_ExtFunctionPtrs.setAutoDelete(true);
	m_ExtPkgs.setAutoDelete(true);
	AddExtFunPackage(new SScriptExtPkg_System());
	//RegisterFunction("print",SScript_Function_print,NULL);
}

SScriptParser::~SScriptParser()
{

}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ȡһ���ı�����
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 16:46
// ����˵��:  @pTextΪԴ�ı������غ�Դ�ı��ַ���������������һ�ζ�ȡλ�ã�*pText==0��ʾ�����˽�β
// �� �� ֵ:  �����¶����һ��
//////////////////////////////////////////////////////////////////////////
SString SScriptParser::ReadLine(char* &pText)
{
	char *pNext = strstr(pText,"\n");
	if(pNext == NULL)
	{
		//����ĩβ
		pNext = pText + strlen(pText);
	}
	else
		pNext ++;
    SString sLine = SString::toString(pText,pNext-pText).trim();

	pText = pNext;
	m_iCurrTextRow++;
	return sLine;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ز��赽Step��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 18:18
// ����˵��:  @pParentStep��ʾ���ڵ�ָ�룬NULL��ʾû�и��ڵ�
//         :  @sLine��ʾ�����ı�
//         :  @pScriptText��ʾ�ű��ı���������ɺ����÷��ص���һ����λ��
//         :  @pStep��ʾ��ǰ�����ָ��
// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::LoadStep(stuStep *pParentStep,SString &sLine,char * &pScriptText,SScriptParser::stuStep *pStep)
{
	SString sLineLower = sLine.toLower();
	if(pStep->type == STEP_UNKNOWN)
	{
		//����sLine�жϵ�ǰ�Ĳ�������
		if(sLineLower.left(3) == "if " || sLineLower.left(3) == "if(")
			pStep->type = STEP_IF;
		else if(sLineLower.left(8) == "else if " || sLineLower.left(8) == "else if(")
			pStep->type = STEP_ELSEIF;
		else if(sLineLower == "else" || sLineLower.left(5) == "else " || sLineLower.left(5) == "else{")
			pStep->type = STEP_ELSE;
		else if(sLineLower.left(6) == "while " || sLineLower.left(6) == "while(")
			pStep->type = STEP_WHILE;
		else if(sLineLower.left(4) == "int " || sLineLower.left(5) == "bool " || sLineLower.left(6) == "float " || sLineLower.left(7) == "string ")
			pStep->type = STEP_DEFINE;
		else if(sLineLower.left(6) == "return" && (sLineLower.at(6) == ' ' || sLineLower.at(6) == ';'))
			pStep->type = STEP_RETURN;
		else if(sLineLower.left(5) == "break" && (sLineLower.at(5) == ' ' || sLineLower.at(5) == ';'))
			pStep->type = STEP_BREAK;
		else if(sLine.left(2) == "//")
			pStep->type = STEP_COMMENT;
		else
		{
			//�����ַ��������ж��Ǹ�ֵ���Ǻ�������
			const char *pStr = sLineLower.data();
			while(*pStr != '\0')
			{
				if(*pStr < 0)
				{
					//��Ϊ�Ǻ���
					pStr += 2;
					continue;
				}
				if((*pStr >= 'a' && *pStr <= 'z') ||
					(*pStr >= '0' && *pStr <= '9') ||
					*pStr == '_' || *pStr == '@' || *pStr == '$' || *pStr == ' ' || *pStr == '\t')
				{
					pStr ++;
					continue;
				}
				if(*pStr == '(')
					pStep->type = STEP_CALL;
				else if(*pStr == '=' || (pStr[1] == '=' && (*pStr == '+' || *pStr == '-' || *pStr == '*' || *pStr == '/')))
					pStep->type = STEP_SET_VAL;
				else
					return false;
				break;
			}
		}
	}

	pStep->step_text = sLine;
	if(pStep->type == STEP_FUNCTION || pStep->type == STEP_IF || pStep->type == STEP_ELSEIF || pStep->type == STEP_ELSE || pStep->type == STEP_WHILE)
	{
		//����Ƕ�׵��Ӳ���
		stuStep *pSubStep;
		SString sSubLine = ReadLine(pScriptText);
		while(sSubLine.length() == 0 && *pScriptText != '\0')
			sSubLine = ReadLine(pScriptText);
		if(sSubLine.left(2) != "//" && (sLine.right(1) == "{"  || sSubLine.left(1) == "{"))
		{
			//�������
			sSubLine = sSubLine.mid(1);
			bool bEnd = false;
			while(!bEnd && *pScriptText != '\0')
			{
				if(sSubLine.length() == 0)
				{
					sSubLine = ReadLine(pScriptText);
					continue;
				}

				if(sSubLine.left(2) != "//" && sSubLine.right(1) == "}")
				{
					//��������
					bEnd = true;
					sSubLine = sSubLine.left(sSubLine.length()-1);
					if(sSubLine.length() == 0)
						continue;
				}

				pSubStep = new stuStep();
				if(!LoadStep(pStep,sSubLine,pScriptText,pSubStep))
				{
					delete pSubStep;
					return false;
				}
				pStep->sub_step.append(pSubStep);
				if(bEnd)
					break;
				sSubLine = ReadLine(pScriptText);
			}
		}
		else
		{
			//���������
			pSubStep = new stuStep();
			if(!LoadStep(pStep,sSubLine,pScriptText,pSubStep))
			{
				delete pSubStep;
				return false;
			}
			pStep->sub_step.append(pSubStep);
		}
	}
	else
	{
		//ȥ�����Ķ���
		if(pStep->step_text.right(1) == ";")
			pStep->step_text = pStep->step_text.left(pStep->step_text.length()-1);
	}

	SString sTemp,sTemp2,vt,vn;
	int i,cnt,p1,p2;
	//����ǰ�������ϸ��Ϣ����Ϊ����ǰ��Ԥ����
	switch(pStep->type)
	{
	case STEP_FUNCTION:
		pStep->info.pFunInfo = new stuFunctionInfo();
		sTemp = SString::GetIdAttribute(1,sLineLower," ");
		pStep->info.pFunInfo->ret_vt = GetValueTypeByStr(sTemp);
		if(pStep->info.pFunInfo->ret_vt == VAL_UNKNOWN)
			return false;
		sTemp = sLine.mid(sTemp.length() + 1).trim();
		pStep->info.pFunInfo->fun_name = SString::GetIdAttribute(1,sTemp,"(");
		if(pStep->info.pFunInfo->fun_name.length() == 0)
			return false;
		p1 = sTemp.find("(");
		p2 = sTemp.find(")",p1);
		if(p1 < 0 || p2 < 0)
			return false;
		//������ʽ�������壺 int param1,float param2
		sTemp = sTemp.mid(p1+1,p2-p1-1).trim();
		if(sTemp.length() == 0)
			cnt = 0;
		else
			cnt = SString::GetAttributeCount(sTemp,",");
		for(i=1;i<=cnt;i++)
		{
			sTemp2 = SString::GetIdAttribute(i,sTemp,",").trim();
			if(sTemp2.length() == 0)
				return false;
			vt = SString::GetIdAttribute(1,sTemp2," ");
			vn = SString::GetIdAttribute(2,sTemp2," ");
			if(vn.length() == 0)
				return false;
			stuValue *pVal = new stuValue(vn,GetValueTypeByStr(vt));
			pStep->info.pFunInfo->params.append(pVal);
			if(pVal->val_type == VAL_UNKNOWN)
				return false;
		}
		break;
	case STEP_DEFINE:
		//int a=0;
		//int a;
		pStep->info.pDefInfo = new stuDefineInfo();
		sTemp = SString::GetIdAttribute(1,pStep->step_text," ").toLower();
		sTemp2 = pStep->step_text.mid(sTemp.length()+1).trim();
		pStep->info.pDefInfo->define_vt = GetValueTypeByStr(sTemp);
		if(pStep->info.pDefInfo->define_vt == VAL_UNKNOWN)
			return false;
		if(sTemp2.find("=") >= 0)
		{
			pStep->info.pDefInfo->var_name = SString::GetIdAttribute(1,sTemp2,"=");
			pStep->info.pDefInfo->init_val = SString::GetIdAttribute(2,sTemp2,"=");
			if(pStep->info.pDefInfo->init_val.length() == 0)
				return false;
		}
		else
		{
			pStep->info.pDefInfo->var_name = sTemp2;
		}
		if(pStep->info.pDefInfo->var_name.length() == 0)
			return false;
		break;
	case STEP_SET_VAL:
		//a = 1;
		//a += b+c;
		i = pStep->step_text.find("=");
		if(i <= 0)
			return false;
		i--;
		switch(pStep->step_text.at(i))
		{
		case '+':
			vt = "+=";
			break;
		case '-':
			vt = "-=";
			break;
		case '*':
			vt = "*=";
			break;
		case '/':
			vt = "/=";
			break;
		default:
			vt = "=";
			i++;
			break;
		}
		pStep->info.pSetvInfo = new stuSetvalInfo();
		pStep->info.pSetvInfo->var_name = pStep->step_text.left(i).trim();
		pStep->info.pSetvInfo->setv_type = GetSetValTypeByStr(vt);
		pStep->info.pSetvInfo->express = pStep->step_text.mid(i+(pStep->info.pSetvInfo->setv_type==SETVAL_EQUAL?1:2)).trim();
		if(pStep->info.pSetvInfo->var_name.length() == 0 || pStep->info.pSetvInfo->express.length() == 0)
			return false;
		break;
	case STEP_IF:
	case STEP_ELSEIF:
	case STEP_WHILE:
		pStep->info.pCondInfo = new stuConditionInfo();
		i = pStep->step_text.find("(");
		if(i <= 0)
			return false;
		pStep->info.pCondInfo->express = pStep->step_text.mid(i+1,pStep->step_text.length()-i-2).trim();
		if(pStep->info.pCondInfo->express.length() == 0)
			return false;
		break;
	case STEP_ELSE:
		break;
	case STEP_RETURN:
		pStep->info.pRetInfo = new stuReturnInfo();
		pStep->info.pRetInfo->express = pStep->step_text.mid(7).trim();
		if(pStep->info.pRetInfo->express.length() == 0)
		{
			if(m_pCurrFunction != NULL && m_pCurrFunction->step_text.left(4) == "void")
				break;
			return false;
		}
		break;
	case STEP_CALL:
		pStep->info.pCallInfo = new stuCallInfo();
		i = pStep->step_text.find("(");
		if(i <= 0)
			return false;
		{
			char *pExpress = pStep->step_text.data()+i+1;
			while(*pExpress != '\0')
			{
				ReadExpress(pExpress,sTemp2);
				sTemp2 = sTemp2.trim();
				pStep->info.pCallInfo->param_express.append(sTemp2);
				if(*pExpress != ',')
					break;
				pExpress ++;
			}
			pExpress = SString::SkipSpaceChar(pExpress);
			if(*pExpress != ')')
				return false;
		}
// 		pStep->info.pCallInfo->fun_name = pStep->step_text.left(i).trim();
// 		sTemp = pStep->step_text.mid(i+1,pStep->step_text.length()-i-2).trim();
// 		if(sTemp.length() > 0)
// 		{
// 			//�в����б������ַ������ó��������ʽ�б�
// 			char *pExpress = sTemp.data();
// 			while(*pExpress != '\0')
// 			{
// 				sTemp2 = ReadExpress(pExpress).trim();
// 				pStep->info.pCallInfo->param_express.append(sTemp2);
// 				if(*pExpress != ',')
// 					break;
// 				pExpress ++;
// 			}
// 			pExpress = SString::SkipSpaceChar(pExpress);
// 			if(*pExpress != ')')
// 				return false;
// 		}
		break;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���ı��ַ������ؽű��﷨
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 16:34
// ����˵��:  @pScriptTextΪ�ű�����
// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::LoadSyntaxText(char *pScriptText)
{
	m_iCurrTextRow = 0;
	SString sLine;
	while(*pScriptText != '\0')
	{
		sLine = ReadLine(pScriptText);
		if(sLine.length() == 0)
			continue;
		//�жϺ�������ǰ׺
		if(sLine.left(4).toLower() == "int " || sLine.left(5).toLower() == "void " || sLine.left(6).toLower() == "float " || sLine.left(7).toLower() == "string ")
		{
			stuStep *pNewFunStep = new stuStep();
			pNewFunStep->type = STEP_FUNCTION;
			pNewFunStep->step_text = sLine;
			m_pCurrFunction = pNewFunStep;
			if(!LoadStep(NULL,sLine,pScriptText,pNewFunStep))
			{
				delete pNewFunStep;
				return false;
			}
			m_Functions.append(pNewFunStep);
		}
		else
			return false;
	}
	m_iCurrTextRow = 0;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ָ�����ı��ļ����ؽű��﷨
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 16:35
// ����˵��:  @pScriptFileΪ�ű��ļ���
// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::LoadSyntaxFile(char *pScriptFile)
{
	SFile f(pScriptFile);
	if(!f.open(IO_ReadOnly))
		return false;
	int fsize = f.size();
	if(fsize == 0)
		return 0;
	BYTE *pScriptText = new BYTE[fsize+1];
	pScriptText[fsize] = '\0';
	f.seekBegin();
	f.readBlock(pScriptText,fsize);
	f.close();
	bool ret = LoadSyntaxText((char*)pScriptText);
	delete[] pScriptText;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ������ת��Ϊ�﷨�ű�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 8:55
// ����˵��:  void
// �� �� ֵ:  SString
//////////////////////////////////////////////////////////////////////////
SString SScriptParser::ToSyntaxScript()
{
	SString sText = "";
	unsigned long pos;
	int iLevel = 0;
	stuStep *pFunStep = m_Functions.FetchFirst(pos);
	while(pFunStep)
	{
		sText += StepToScriptText(iLevel,pFunStep);
		sText += "\r\n";
		pFunStep = m_Functions.FetchNext(pos);
	}

	return sText;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����ǰ�Ĳ���ת��Ϊ��Ӧ���﷨�ű�
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 8:56
// ����˵��:  @iLevel��ʾ��ǰ��κ�
//         :  @pStep����ָ��
// �� �� ֵ:  SString
//////////////////////////////////////////////////////////////////////////
SString SScriptParser::StepToScriptText(int iLevel,SScriptParser::stuStep *pStep)
{
	SString sText = "";
	sText += SString::toFillString("",iLevel*2,' ') + pStep->step_text;
	if(pStep->type == STEP_FUNCTION || pStep->type == STEP_IF || pStep->type == STEP_ELSEIF || 
		pStep->type == STEP_ELSE || pStep->type == STEP_WHILE)
	{
		sText += "\r\n";
		sText += SString::toFillString("",iLevel*2,' ') + "{\r\n";
		unsigned long pos;
		stuStep *pSubStep = pStep->sub_step.FetchFirst(pos);
		while(pSubStep)
		{
			sText += StepToScriptText(iLevel+1,pSubStep);
			pSubStep = pStep->sub_step.FetchNext(pos);
		}
		sText += SString::toFillString("",iLevel*2,' ') + "}\r\n";
	}
	else if(pStep->type == STEP_COMMENT)
	{
		sText += "\r\n";
	}
	else
	{
		sText += ";\r\n";
	}
	return sText;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ��ȡһ�������ı��ʽ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 15:04
// ����˵��:  @pExpressΪ���ʽ�ַ���ָ�룬���÷�����һ�ζ�ȡλ��,*pExpress=='\0'��ʾ�������
//         :  @sTextΪ���÷��ص�ֵ
// �� �� ֵ:  void
//////////////////////////////////////////////////////////////////////////
void SScriptParser::ReadExpress(char * &pExpress,SString &sText)
{
	int in_level=0;//�̺��ڵĲ���
	bool in_str=false;//�Ƿ������ַ�����
	char prev_ch=' ',ch;
	char *pStart = pExpress;
	while((ch = *pExpress) != '\0')
	{
		if(ch == '\"' && prev_ch != '\\')
		{
			in_str = !in_str;
		}
		else if(!in_str)
		{
			if(ch == '(')
				in_level ++;
			else if(ch == ')')
			{
				if(in_level == 0)
					break;
				in_level --;
			}
			else if(ch == ',' && in_level == 0)
				break;//������һ�����ʽ
		}

		sText += ch;
		prev_ch = ch;
		pExpress++;
	}
	sText = SString::toString(pStart,pExpress-pStart);
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ������ʽ��ֵ
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 16:58
// ����˵��:  @InnerVarΪ�ڲ���������
//         :  @expressΪ���ʽ�ַ���
//         :  @sValue��ʾֵ����
// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::CalcExpressValue(SPtrList<stuValue> &InnerVar,SString &express,stuValue &sValue)
{
	sValue.SetValue("");
	char *pExpress = express.data();
	stuValue sSubValue;
	eCALC_TYPE calc_type = CALC_UNKNOWN;
	bool bNot = false;
	SString str;
	//����зǲ���
	while(*pExpress)
	{
		pExpress = SString::SkipSpaceChar(pExpress);
		if(*pExpress == '!')
		{
			bNot = true;
			pExpress ++;
			pExpress = SString::SkipSpaceChar(pExpress);
			continue;
		}
		if(*pExpress == '(')
		{
			//Ƕ�ױ��ʽ
			//ȡ���������ӱ��ʽ
			pExpress ++;
			ReadExpress(pExpress,str);
			if(*pExpress != ')')
				return false;
			if(!CalcExpressValue(InnerVar,str,sSubValue))
				return false;
		}
		else if(*pExpress == '\"')
		{
			//�ַ���
			pExpress++;
			char *pStartChar = pExpress;
			while(*pExpress != '\0')
			{
				if(*pExpress == '\"' && *pExpress != '\\')
					break;
				pExpress ++;
				
			}
			if(*pExpress != '\"')
				return false;
			sSubValue.val_type = VAL_STRING;
			sSubValue.SetValue(SString::toString(pStartChar,pExpress-pStartChar));
			pExpress ++;


// 			ReadExpress(pExpress,str);
// 			if(str.at(0) != '\"' || str.at(str.length()-1) != '\"')
// 				return false;
// 			sSubValue.val_type = VAL_STRING;
// 			sSubValue.SetValue(str.mid(1,str.length()-2));

// 			pExpress ++;
// 			char *pStartChar = pExpress;
// 			while(*pExpress != '\0')
// 			{
// 				if(*pExpress == '\"' && pExpress[-1] != '\\')
// 					break;
// 				pExpress ++;
// 			}
// 			if(*pExpress != '\"')
// 				return false;
// 			sSubValue.val_type = VAL_STRING;
// 			sSubValue.SetValue(SString::toString(pStartChar,pExpress-pStartChar));
// 			pExpress ++;
		}
		else if(*pExpress >= '0' && *pExpress <= '9')
		{
			//����
			char *pStartChar = pExpress;
			if(*pExpress == '0' && (pExpress[1] == 'x') || (pExpress[1] == 'X'))
			{
				//16����
				pExpress += 2;
			}
			while(*pExpress != '\0')
			{
				if(*pExpress == '.' || (*pExpress >= '0' && *pExpress <= '9'))
				{
					pExpress ++;
					continue;
				}
				break;
			}
			sSubValue.SetValue(SString::toString(pStartChar,pExpress-pStartChar));
			if(sSubValue.GetValue().find(".") >= 0)
				sSubValue.val_type = VAL_FLOAT;
			else
				sSubValue.val_type = VAL_INT;
		}
		else if(*pExpress == '_' || *pExpress == '@' || *pExpress == '$' || (*pExpress >= 'a' && *pExpress <= 'z') || (*pExpress >= 'A' && *pExpress <= 'Z'))
		{
			//������ȡ��������
			char *pStartChar = pExpress;
			while(*pExpress != '\0')
			{
				if(*pExpress < 0)
				{
					//����
					pExpress += 2;
					continue;
				}
				if(*pExpress == '_' || *pExpress == '@' || *pExpress == '$' || (*pExpress >= 'a' && *pExpress <= 'z') || (*pExpress >= 'A' && *pExpress <= 'Z') || (*pExpress >= '0' && *pExpress <= '9'))
				{
					pExpress ++;
					continue;
				}
				break;
			}			
			SString sFunName = SString::toString(pStartChar,pExpress-pStartChar);
			int level=0;
			bool bCalc=false;
			SPtrList<stuValue> param_vals;
			stuValue RetValue;

			pExpress = SString::SkipSpaceChar(pExpress);
			if(*pExpress != '(')
			{
				//true/false
				if(sFunName.toLower() == "true")
				{
					sSubValue.val_type = VAL_BOOL;
					sSubValue.SetValue("1");
				}
				else if(sFunName.toLower() == "false")
				{
					sSubValue.val_type = VAL_BOOL;
					sSubValue.SetValue("0");
				}
				else
				{
					//�����Ǳ���
					stuValue *pV = SearchValue(sFunName,InnerVar);
					if(pV == NULL)
						pV = SearchValue(sFunName,m_InstVariant);
					if(pV == NULL)
						return false;
					sSubValue = *pV;
				}
				goto aga;
			}
			pExpress++;
			//ȡ�����б�ֱ��������Ч�عر�
			pExpress = SString::SkipSpaceChar(pExpress);
			while(*pExpress != '\0')
			{
				ReadExpress(pExpress,str);
				if(str.length() > 0)
				{
					//��sSub_Expr�����Ľ�����뵽�����б���
					stuValue *pValue = new stuValue();
					param_vals.append(pValue);
					if(!CalcExpressValue(InnerVar,str, *pValue))
						return false;
				}
				pExpress = SString::SkipSpaceChar(pExpress);
				if(*pExpress != ',')
					break;
				pExpress ++;
			}

// 			param_vals.setAutoDelete(true);
// 			bool bIn=false;
// 			while(*pExpress != '\0')
// 			{
// 				if(*pExpress == '\"')
// 					bIn = !bIn;
// 				if(!bIn)
// 				{
// 					if(*pExpress == '(')
// 						level ++;
// 					else if(*pExpress == ')')
// 					{
// 						if(level == 0)
// 						{
// 							bCalc = true;
// 						}
// 						else
// 							level --;
// 					}
// 					else if(*pExpress == ',' && level == 0)
// 					{
// 						bCalc = true;
// 						pExpress ++;
// 					}
// 					if(bCalc && sSub_Expr.length() > 0)
// 					{
// 						//��sSub_Expr�����Ľ�����뵽�����б���
// 						stuValue *pValue = new stuValue();
// 						param_vals.append(pValue);
// 						if(!CalcExpressValue(InnerVar,sSub_Expr, *pValue))
// 							return false;
// 					
// 						sSub_Expr = "";
// 						bCalc = false;
// 					}
// 					if(*pExpress == ')' && level == 0)
// 						break;
// 				}
// 
// 				sSub_Expr += *pExpress;
// 				pExpress ++;
// 			}
			if(*pExpress != ')')
				return false;
			pExpress ++;
			if(!RunFunction(RetValue,sFunName,param_vals.count() > 0?&param_vals:NULL))
				return false;
			sSubValue.SetValue(RetValue.GetValue());
		}
		else
			return false;//�Ƿ����ʽ
	aga:
		if(bNot)
		{
			//����зǲ����������߼�ȡ��
			if(sSubValue.val_type != VAL_BOOL)
				return false;
			if(sSubValue.GetValue() == "1")
				sSubValue.SetValue("0");
			else
				sSubValue.SetValue("1");
			bNot = true;
		}

		//����һ�ε��������ͣ�ȷ����ǰֵ���ӱ��ʽֵ����Ϸ�ʽ
		switch(calc_type)
		{
		case CALC_UNKNOWN:
			sValue = sSubValue;
			break;
		//��������
		case CALC_ADD:			//+
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(SString::toString(sValue.GetValue().toInt()+sSubValue.GetValue().toInt()));
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(SString::toString(sValue.GetValue().toFloat()+sSubValue.GetValue().toFloat()));
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()+sSubValue.GetValue());
			else
				return false;
			break;
		case CALC_DEL:			//-
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(SString::toString(sValue.GetValue().toInt()-sSubValue.GetValue().toInt()));
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(SString::toString(sValue.GetValue().toFloat()-sSubValue.GetValue().toFloat()));
			else
				return false;
			break;
		case CALC_MUL:			//*
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(SString::toString(sValue.GetValue().toInt()*sSubValue.GetValue().toInt()));
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(SString::toString(sValue.GetValue().toFloat()*sSubValue.GetValue().toFloat()));
			break;
		case CALC_DIV:			///
			if(sSubValue.GetValue().toInt() == 0)//div zero error
				return false;
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(SString::toString(sValue.GetValue().toInt()/sSubValue.GetValue().toInt()));
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(SString::toString(sValue.GetValue().toFloat()/sSubValue.GetValue().toFloat()));
			else
				return false;
			break;
		//�߼�����
		case CALC_AND:			//&&
			if(sValue.val_type != VAL_BOOL || sSubValue.val_type != VAL_BOOL)
				return false;
			if(sValue.GetValue() == "1" && sSubValue.GetValue() == "1")
				sValue.SetValue("1");
			else
				sValue.SetValue("0");
			break;
		case CALC_OR:			//||
			if(sValue.val_type != VAL_BOOL || sSubValue.val_type != VAL_BOOL)
				return false;
			if(sValue.GetValue() == "1" || sSubValue.GetValue() == "1")
				sValue.SetValue("1");
			else
				sValue.SetValue("0");
			break;
//		case CALC_NOT:			//!,
		//�Ƚ�����
		case CALC_BT:			//>
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(sValue.GetValue().toInt()>sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()>sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()>sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
		case CALC_BET:			//>=
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(sValue.GetValue().toInt()>=sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()>=sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()>=sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
		case CALC_LT:			//<
			if(sValue.val_type == VAL_INT)
  				sValue.SetValue(sValue.GetValue().toInt()<sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()<sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()<sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
		case CALC_LET:			//<=
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(sValue.GetValue().toInt()<=sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()<=sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()<=sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
		case CALC_EQ:			//==
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(sValue.GetValue().toInt()==sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()==sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()==sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
		case CALC_NOTEQ:			//!=
			if(sValue.val_type == VAL_INT)
				sValue.SetValue(sValue.GetValue().toInt()!=sSubValue.GetValue().toInt()?"1":"0");
			else if(sValue.val_type == VAL_FLOAT)
				sValue.SetValue(sValue.GetValue().toFloat()!=sSubValue.GetValue().toFloat()?"1":"0");
			else if(sValue.val_type == VAL_STRING)
				sValue.SetValue(sValue.GetValue()!=sSubValue.GetValue()?"1":"0");
			else
				return false;
			sValue.val_type = VAL_BOOL;
			break;
			break;
		default:
			return false;
		}

		SString::SkipSpaceChar(pExpress);
		if(*pExpress == '\0')
			break;
		//ȡ��һ�ε������
		if(*pExpress == '+')
		{
			calc_type = CALC_ADD;
			pExpress ++;
		}
		else if(*pExpress == '-')
		{
			calc_type = CALC_DEL;			//-
			pExpress ++;
		}
		else if(*pExpress == '*')
		{
			calc_type = CALC_MUL;			//*
			pExpress ++;
		}
		else if(*pExpress == '/')
		{
			calc_type = CALC_DIV;			///
			pExpress ++;
		}
		//�߼�����
		else if(*pExpress == '&' && pExpress[1] == '&')
		{
			calc_type = CALC_AND;			//&&
			pExpress += 2;
		}
		else if(*pExpress == '|' && pExpress[1] == '|')
		{
			calc_type = CALC_OR;				//||
			pExpress += 2;
		}
		//�Ƚ�����
		else if(*pExpress == '>' && pExpress[1] == '=')
		{
			calc_type = CALC_BET;			//>=
			pExpress += 2;
		}
		else if(*pExpress == '>')
		{
			calc_type = CALC_BT;				//>
			pExpress ++;
		}
		else if(*pExpress == '<' && pExpress[1] == '=')
		{
			calc_type = CALC_LET;			//<=
			pExpress += 2;
		}
		else if(*pExpress == '<')
		{
			calc_type = CALC_LT;				//<
			pExpress ++;
		}
		else if(*pExpress == '=' && pExpress[1] == '=')
		{
			calc_type = CALC_EQ;				//==
			pExpress += 2;
		}
		else if(*pExpress == '!' && pExpress[1] == '=')
		{
			calc_type = CALC_NOTEQ;			//!=
			pExpress += 2;
		}
		else
			return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ���в���ָ������µ��Ӳ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 17:30
// ����˵��:  @InnerVarΪ�ڲ���������
//         :  @pParentStepΪ������ָ��
//         :  @parent_posΪ��ָ�����λ�ã����÷���
//         :  @pStepΪ����ָ��
//         :  @ReturnValΪ���÷��ص�ֵ������0ʱ������ֵ
// �� �� ֵ:  <-1��ʾʧ�ܣ�>0��ʾ�ɹ���=0��ʾ���أ�-1��ʾbreak
//////////////////////////////////////////////////////////////////////////
int SScriptParser::RunStep(SPtrList<stuValue> &InnerVar,stuStep *pParentStep,unsigned long &parent_pos,stuStep *pStep,stuValue &ReturnVal)
{
	unsigned long pos;
	stuValue *pVal;
	stuValue sValue;
	stuStep *pStep2,*pIfSubStep,*pSubStep;
	unsigned long prev_parent_pos;
	switch(pStep->type)
	{
	case STEP_DEFINE:
		if(!CalcExpressValue(InnerVar,pStep->info.pDefInfo->init_val,sValue))
			return -2;
		InnerVar.append(new stuValue(pStep->info.pDefInfo->var_name,pStep->info.pDefInfo->define_vt,sValue.GetValue()));
		break;
	case STEP_SET_VAL:
		pVal = SearchValue(pStep->info.pSetvInfo->var_name,InnerVar);
		if(pVal == NULL)
			pVal = SearchValue(pStep->info.pSetvInfo->var_name,m_InstVariant);
		if(pVal == NULL)
			return -3;
		if(!CalcExpressValue(InnerVar,pStep->info.pSetvInfo->express,sValue))
			return -4;
		switch (pStep->info.pSetvInfo->setv_type)
		{
		case SETVAL_EQUAL:		// =
			pVal->SetValue(sValue.GetValue());
			break;
		case SETVAL_ADD_EQUAL:	// +=
			if(pVal->val_type == VAL_INT)
				pVal->SetValue(SString::toString(pVal->GetValue().toInt() + sValue.GetValue().toInt()));
			else if(pVal->val_type == VAL_FLOAT)
				pVal->SetValue(SString::toString(pVal->GetValue().toFloat() + sValue.GetValue().toFloat()));
			else if(pVal->val_type == VAL_STRING)
				pVal->SetValue(pVal->GetValue() + sValue.GetValue());
			else
				return -5;
			break;
		case SETVAL_DEL_EQUAL:	// -=
			if(pVal->val_type == VAL_INT)
				pVal->SetValue(SString::toString(pVal->GetValue().toInt() - sValue.GetValue().toInt()));
			else if(pVal->val_type == VAL_FLOAT)
				pVal->SetValue(SString::toString(pVal->GetValue().toFloat() - sValue.GetValue().toFloat()));
			else
				return -6;
			break;
		case SETVAL_MUL_EQUAL:	// *=
			if(pVal->val_type == VAL_INT)
				pVal->SetValue(SString::toString(pVal->GetValue().toInt() * sValue.GetValue().toInt()));
			else if(pVal->val_type == VAL_FLOAT)
				pVal->SetValue(SString::toString(pVal->GetValue().toFloat() * sValue.GetValue().toFloat()));
			else
				return -7;
			break;
		case SETVAL_DIV_EQUAL:	// /=
			if(sValue.GetValue().toInt() == 0)
				return -8;//div zero error
			if(pVal->val_type == VAL_INT)
				pVal->SetValue(SString::toString(pVal->GetValue().toInt() / sValue.GetValue().toInt()));
			else if(pVal->val_type == VAL_FLOAT)
				pVal->SetValue(SString::toString(pVal->GetValue().toFloat() / sValue.GetValue().toFloat()));
			else
				return -9;
			break;
		default:
			return -10;
		}
		break;
	case STEP_IF:
		pIfSubStep = NULL;
		if(!CalcExpressValue(InnerVar,pStep->info.pCondInfo->express,sValue))
			return -11;
		if(sValue.GetValue() == "1")//true
		{
			//ִ���Ӳ���
			pIfSubStep = pStep;
		}
		//�����������е�ELSEIF��ELSE
		prev_parent_pos = parent_pos;
		pStep2 = pParentStep->sub_step.FetchNext(parent_pos);
		while(pStep2 && (pStep2->type == STEP_ELSEIF || pStep2->type == STEP_ELSE))
		{
			if(pIfSubStep == NULL)
			{
				if(pStep2->type == STEP_ELSE)
				{
					pIfSubStep = pStep2;
				}
				else
				{
					//��δƥ�䣬�����ж�
					if(!CalcExpressValue(InnerVar,pStep2->info.pCondInfo->express,sValue))
						return -1;
					if(sValue.GetValue() == "1")//true
					{
						//ִ���Ӳ���
						pIfSubStep = pStep2;
					}
				}
			}
			prev_parent_pos = parent_pos;
			pStep2 = pParentStep->sub_step.FetchNext(parent_pos);
		}
		parent_pos = prev_parent_pos;
		if(pIfSubStep != NULL)
		{
			//��ƥ���֧
			pSubStep = pIfSubStep->sub_step.FetchFirst(pos);
			while(pSubStep)
			{
				int ret = RunStep(InnerVar,pIfSubStep,pos,pSubStep,ReturnVal);
				if(ret <= 0)
					return ret;
				pSubStep = pIfSubStep->sub_step.FetchNext(pos);
			}
		}

		break;
	case STEP_WHILE:
		while(1)
		{
			if(!CalcExpressValue(InnerVar,pStep->info.pCondInfo->express,sValue))
				return -12;
			if(sValue.GetValue() == "0")//false
			{
				break;//loop end
			}
			pSubStep = pStep->sub_step.FetchFirst(pos);
			while(pSubStep)
			{
				int ret = RunStep(InnerVar,pStep,pos,pSubStep,ReturnVal);
				if(ret == -1)
					break;//break the loop
				if(ret <= 0)
					return ret;
				pSubStep = pStep->sub_step.FetchNext(pos);
			}
		}
		break;
	case STEP_BREAK:
		return -1;
	case STEP_RETURN:
		if(!CalcExpressValue(InnerVar,pStep->info.pRetInfo->express,sValue))
			return -13;
		ReturnVal.SetValue(sValue.GetValue());
		return 0;
	case STEP_CALL:
		if(!CalcExpressValue(InnerVar,pStep->step_text,sValue))
			return -14;
		break;
	case STEP_COMMENT:
		return 1;
	default:
		break;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ����ָ���ĺ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-28 10:30
// ����˵��:  @ReturnVal��ʾ����ֵ��������
//         :  @sFunName��ʾ��������
//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::RunFunction(stuValue &ReturnVal,SString &sFunName,SPtrList<stuValue> *pParamValues)
{
	SPtrList<stuValue> InnerVar;//�ֲ�����
	InnerVar.setAutoDelete(false);	
	stuExtFunctionParam *pExtFun = SearchExtFunctionByName(sFunName);
	if(pExtFun != NULL)
		return pExtFun->pFun(pExtFun->pParam,ReturnVal,pParamValues);

	//ʵ�μ����ڲ������б�
	if(pParamValues != NULL)
		pParamValues->copyto(InnerVar);
	unsigned long pos,pos2;
	stuStep *pStep;

	stuStep *pFunStep = m_Functions.FetchFirst(pos);
	while(pFunStep)
	{
		if(pFunStep->IsFunctionByName(sFunName))
		{
			break;
		}
		pFunStep = m_Functions.FetchNext(pos);
	}
	if(pFunStep == NULL)
		goto err;
	if((pParamValues == NULL?0:pParamValues->count()) != pFunStep->info.pFunInfo->params.count())
		return false;//����������ƥ��
	if(pParamValues != NULL)
	{
		stuValue *pParam = pParamValues->FetchFirst(pos);
		stuValue *pDefParam = pFunStep->info.pFunInfo->params.FetchFirst(pos2);
		while(pParam && pDefParam)
		{
			pParam->var_name = pDefParam->var_name;
			pParam = pParamValues->FetchNext(pos);
			pDefParam = pFunStep->info.pFunInfo->params.FetchNext(pos2);
		}
	}
	ReturnVal.val_type = pFunStep->info.pFunInfo->ret_vt;
	
	int ret;
	//���к���pFunStep
	pStep = pFunStep->sub_step.FetchFirst(pos);
	while(pStep)
	{
		ret = RunStep(InnerVar,pFunStep,pos,pStep,ReturnVal);
		if(ret < 0)
			goto err;
		if(ret == 0)
			goto ok;
		pStep = pFunStep->sub_step.FetchNext(pos);
	}

ok:
	//���ڲ�������ȥ����ʽ����
	if(pParamValues != NULL)
	{
		stuValue *pV = pParamValues->FetchFirst(pos);
		while(pV)
		{
			InnerVar.remove(pV);
			pV = pParamValues->FetchNext(pos);
		}
	}
	
	InnerVar.setAutoDelete(true);
	return true;
err:
	//���ڲ�������ȥ����ʽ����
	if(pParamValues != NULL)
	{
		stuValue *pV = pParamValues->FetchFirst(pos);
		while(pV)
		{
			InnerVar.remove(pV);
			pV = pParamValues->FetchNext(pos);
		}
	}
	InnerVar.setAutoDelete(true);
	return false;
}


//////////////////////////////////////////////////////////////////////////
// ��    ��:  ע��ָ���ĺ���
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-31 15:55
// ����˵��:  @sFunName��ʾ��������
//         :  @pFun��ʾ����ָ��
//         :  @pParam��ʾͨ�Ų���
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ���ʾͬ�������Ѵ���)
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::RegisterFunction(SString sFunName,SScript_Function pFun,void *pParam)
{
	stuExtFunctionParam *p = SearchExtFunctionByName(sFunName);
	if(p != NULL)
		return false;
	p = new stuExtFunctionParam;
	p->name = sFunName;
	p->pFun = pFun;
	p->pParam = pParam;
	m_ExtFunctionPtrs.append(p);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��ָ��������ע��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-31 15:55
// ����˵��:  @sFunName��ʾ��������
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ���ʾָ������δע��)
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::UnregisterFunction(SString sFunName)
{
	stuExtFunctionParam *p = SearchExtFunctionByName(sFunName);
	if(p == NULL)
		return false;
	m_ExtFunctionPtrs.remove(p);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ע��ָ����ʵ������
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-31 15:57
// ����˵��:  @pInstValue��ʾʵ������ָ�룬ע��󲻸����ͷ�ָ�룬���ɵ����߸����ͷ�
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ��Ϊͬ�������Ѵ���)
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::RegisterInstVariant(stuValue *pInstValue)
{
	m_InstVariant.append(pInstValue);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// ��    ��:  ȡ��ָ��ʵ��������ע��
// ��    ��:  �ۿ���
// ����ʱ��:  2016-12-9 16:49
// ����˵��:  @pInstValue��ʾ����ָ��
// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
//////////////////////////////////////////////////////////////////////////
bool SScriptParser::UnregisterInstVariant(stuValue *pInstValue)
{
	m_InstVariant.remove(pInstValue);
	return true;
}
