/**
 *
 * �� �� �� : ScriptParser.h
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

#ifndef __NAM_SCRIPT_PARSER_H__
#define __NAM_SCRIPT_PARSER_H__

#include "SList.h"
#include "SApi.h"
#include "SStack.h"

//////////////////////////////////////////////////////////////////////////
// ��    ��:  SScriptParser
// ��    ��:  �ۿ���
// ����ʱ��:  2016-10-27 11:19
// ��    ��:  �����ͽű�������������
/* ������C���Ե��﷨�ṹ �﷨���ܣ�
1��֧�ֱ�����������������
2��֧�ָ�ֵ���㡢�����������㡢�Ƚ����㼰�߼����㣻
3��֧��������֧��
4��֧��ѭ����
5��֧��Ƕ�׷�֧��ѭ����Ƕ��ʹ�ã�
6��֧���﷨��飻
7�������������Ľӿڳ��󻯣�
8��֧�ֶ�����ʱ���������Ͱ�����float/int/string/bool����������ʱ�����ͳ���������ã�����������ƥ����ʱ������ƥ��ʧ�ܲŲ��ҳ��������
��:
int fun1()
{
print("into fun1");
int i=0;
int ret=1234-234*100/1000;
while(i<10)
{
print("i=",i,",ret=",ret,",f_add=",f_add(i,ret));
if(i<5)
ret += 1;
else if(i<8)
{
ret += 2;
int a=0;
a = a+1;
}
else
ret += 3;
i += 1;
}
return ret;
}

int f_add(int a,int b)
{
return a*1000/b;
}

*/


//////////////////////////////////////////////////////////////////////////
class SScriptParser
{
public:
	//�������Ͷ���
	enum eStepType
	{
		STEP_UNKNOWN=0,
		STEP_FUNCTION,
		STEP_DEFINE,
		STEP_SET_VAL,
		STEP_IF,
		STEP_ELSEIF,
		STEP_ELSE,
		STEP_WHILE,
		STEP_RETURN,
		STEP_BREAK,
		STEP_CALL,
		STEP_COMMENT,//ע��		
	};
	//ֵ���Ͷ���
	enum eValueType
	{
		VAL_UNKNOWN=0,
		VAL_VOID,
		VAL_INT,
		VAL_FLOAT,
		VAL_STRING,
		VAL_BOOL,
	};
	static eValueType GetValueTypeByStr(SString vt)
	{
		if(vt == "int")
			return VAL_INT;
		else if(vt == "float")
			return VAL_FLOAT;
		else if(vt == "bool")
			return VAL_BOOL;
		else if(vt == "string")
			return VAL_STRING;
		else if(vt == "void")
			return VAL_VOID;
		else
			return VAL_UNKNOWN;
	}
	static SString GetValueTypeStrByVt(eValueType vt)
	{
		switch(vt)
		{
		case VAL_UNKNOWN:
			return "";
		case VAL_VOID:
			return "void";
		case VAL_INT:
			return "int";
		case VAL_FLOAT:
			return "float";
		case VAL_STRING:
			return "string";
		case VAL_BOOL:
			return "bool";
		default:
			return "unknown";
		}
	}
	//��ֵ����
	enum eSETVAL_TYPE
	{
		SETVAL_UNKNOWN=0,	
		SETVAL_EQUAL=1,		// = 
		SETVAL_ADD_EQUAL,	// +=
		SETVAL_DEL_EQUAL,	// -=
		SETVAL_MUL_EQUAL,	// *=
		SETVAL_DIV_EQUAL,	// /=
	};
	static eSETVAL_TYPE GetSetValTypeByStr(SString st)
	{
		if(st == "=")
			return SETVAL_EQUAL;
		else if(st == "+=")
			return SETVAL_ADD_EQUAL;	// +=
		else if(st == "-=")
			return SETVAL_DEL_EQUAL;	// -=
		else if(st == "*=")
			return SETVAL_MUL_EQUAL;	// *=
		else if(st == "/=")
			return SETVAL_DIV_EQUAL;	// /=
		else
			return SETVAL_UNKNOWN;
	}
	static SString GetSetValTypeStrBySvt(eSETVAL_TYPE st)
	{
		switch(st)
		{
		case SETVAL_EQUAL:
			return "=";
		case SETVAL_ADD_EQUAL:
			return "+=";
		case SETVAL_DEL_EQUAL:
			return "-=";
		case SETVAL_MUL_EQUAL:
			return "*=";
		case SETVAL_DIV_EQUAL:
			return "/=";
		default:
			return "unknown";
		}
	}
	enum eCALC_TYPE
	{
		CALC_UNKNOWN=0,
		//��������
		CALC_ADD,			//+
		CALC_DEL,			//-
		CALC_MUL,			//*
		CALC_DIV,			///
		//�߼�����
		CALC_AND,			//&&
		CALC_OR,			//||
		CALC_NOT,			//!,
		//�Ƚ�����
		CALC_BT,			//>
		CALC_BET,			//>=
		CALC_LT,			//<
		CALC_LET,			//<=
		CALC_EQ,			//==
		CALC_NOTEQ,			//!=
	};
	static eCALC_TYPE GetCalcTypeByStr(SString str)
	{
		//��������
		if(str == "+")
			return CALC_ADD;			//+
		else if(str == "-")
			return CALC_DEL;			//-
		else if(str == "*")
			return CALC_MUL;			//*
		else if(str == "/")
			return CALC_DIV;			///
		//�߼�����
		else if(str == "&&")
			return CALC_AND;			//&&
		else if(str == "||")
			return CALC_OR;				//||
		else if(str == "!")
			return CALC_NOT;			//!,
		//�Ƚ�����
		else if(str == ">")
			return CALC_BT;				//>
		else if(str == ">=")
			return CALC_BET;			//>=
		else if(str == "<")
			return CALC_LT;				//<
		else if(str == "<=")
			return CALC_LET;			//<=
		else if(str == "==")
			return CALC_EQ;				//==
		else if(str == "!=")
			return CALC_NOTEQ;			//!=
		else
			return CALC_UNKNOWN;
	}
	static SString GetCalcTypeStrByCt(eCALC_TYPE ct)
	{
		switch(ct)
		{
		case CALC_ADD:return "+";
		case CALC_DEL:return "-";
		case CALC_MUL:return "*";
		case CALC_DIV:return "/";
		case CALC_AND:return "&&";
		case CALC_OR :return "||";
		case CALC_NOT:return "!,";
		case CALC_BT :return ">	";
		case CALC_BET:return ">=";
		case CALC_LT :return "<	";
		case CALC_LET:return "<=";
		case CALC_EQ :return "==";
		case CALC_NOTEQ:return "!=";
		default:return "unknown";
		}
	}
	//ֵ����
	struct stuValue
	{
		SString var_name;
		eValueType val_type;
		SString val;
		stuValue()
		{
			val_type = VAL_VOID;
		}
		stuValue(SString vn,eValueType vt)
		{
			var_name = vn;
			val_type = vt;
		}
		stuValue(SString vn,eValueType vt,SString v)
		{
			var_name = vn;
			val_type = vt;
			val = v;
		}
		////////////////////////////////////////////////////////////////////////
		// ��    ��:  ��ֵ���������
		// ��    ��:  �ۿ���
		// ����ʱ��:  2016-10-31 10:47
		// ����˵��:  @sֵ
		// �� �� ֵ:  *this
		//////////////////////////////////////////////////////////////////////////
		stuValue &operator=(const stuValue &s)
		{
			this->var_name = s.var_name;
			this->val_type = s.val_type;
			this->val = s.val;
			return *this;
		}

		inline void Set(eValueType vt,const char* pValStr)
		{
			this->val_type = vt;
			this->val = pValStr;
		}

		inline void SetType(eValueType vt)
		{
			this->val_type = vt;
		}

		inline void SetValue(SString v)
		{
			val = v;
		}
		inline void SetValue(char* v)
		{
			val = v;
		}
		inline SString GetValue()
		{
			return val;
		}
		inline void SetName(char* n)
		{
			var_name = n;
		}
	};

	//���趨��
	struct stuFunctionInfo
	{
		stuFunctionInfo()
		{
			params.setAutoDelete(true);
		}
		eValueType ret_vt;//��������
		SString fun_name;//��������
		SPtrList<stuValue> params;//�β��б�
	};
	struct stuDefineInfo
	{
		eValueType define_vt;//ֵ����
		SString var_name;//��������
		SString init_val;//��ʼֵ���ձ�ʾû�г�ʼֵ
	};
	struct stuSetvalInfo
	{
		SString var_name;//����ֵ��������
		eSETVAL_TYPE setv_type;//��ֵ����
		SString express;//��ֵ���ʽ
	};
	struct stuConditionInfo
	{
		SString express;//�������ʽ
	};
	struct stuReturnInfo
	{
		SString express;//ֵ���ʽ
	};
	struct stuCallInfo
	{
		SString fun_name;//�����ú�������
		SStringList param_express;//ʵ�ʲ������ʽ
	};
	struct stuStep
	{
		stuStep()
		{
			type = STEP_UNKNOWN;
			info.pFunInfo = NULL;
			sub_step.setAutoDelete(true);
		}
		~stuStep()
		{
			if(info.pFunInfo != NULL)
			{
				switch(type)
				{
				case STEP_FUNCTION:
					delete info.pFunInfo;
					break;
				case STEP_DEFINE:
					delete info.pDefInfo;
					break;
				case STEP_SET_VAL:
					delete info.pSetvInfo;
					break;
				case STEP_IF:
				case STEP_ELSEIF:
				case STEP_WHILE:
					delete info.pCondInfo;
					break;
				case STEP_RETURN:
					delete info.pRetInfo;
					break;
				case STEP_CALL:
					delete info.pCallInfo;
					break;
				case STEP_ELSE:
					break;
				default:
					break;
				}
			}
		}
		inline bool IsFunctionByName(SString &sFunName)
		{
			if(type == STEP_FUNCTION && info.pFunInfo->fun_name == sFunName)
				return true;
			return false;
		}
		eStepType type;//��������
		SString step_text;//�����ı�
		SPtrList<stuStep> sub_step;//�Ӳ���
		union uInfo{
			stuFunctionInfo *pFunInfo;
			stuDefineInfo *pDefInfo;
			stuSetvalInfo *pSetvInfo;
			stuConditionInfo *pCondInfo;//if/else if/while
			stuReturnInfo *pRetInfo;//return
			stuCallInfo *pCallInfo;//call
		} info;
	};


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  SScript�����ص��ӿ�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-31 15:50
	// ����˵��:  @pCbParam��ʾ�ص�����(ע��ʱָ����)
	//         :  @ReturnVal��ʾ����ֵ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	typedef bool (*SScript_Function)(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  stuExtFunction
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 16:53
	// ��    ��:  �ⲿ��չ��������
	//////////////////////////////////////////////////////////////////////////
	struct stuExtFunction
	{
		const char* sFunName;//��������
		SScript_Function pFun;//����ָ��
		eValueType sRetValType;//����ֵ����
		int iParamCount;//��ʽ��������
		const char* sParams;//���е���ʽ������int a,int b
		const char* sComment;//��ע
	};

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  stuExtVariant
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 16:53
	// ��    ��:  �ⲿ��չ��������
	//////////////////////////////////////////////////////////////////////////
	struct stuExtVariant
	{
		const char* sVarName;//��������
		eValueType var_type;//��������
		const char* sDefaultVal;//����ȱʡֵ
		const char* sComment;//��ע
	};

	struct stuExtFunctionParam
	{
		stuExtFunctionParam(){}
		stuExtFunctionParam(SString n,SScript_Function p,void *param=NULL)
		{
			pFun = p;
			name = n;
			pParam = param;
		}
		SScript_Function pFun;
		void *pParam;
		SString name;
	};
	
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  CPredefinedExtFunctionPackage
	// ��    ��:  �ۿ���
	// ����ʱ��:  2017-3-27 15:45
	// ��    ��:  Ԥ������չ������
	//////////////////////////////////////////////////////////////////////////
	class CPredefinedExtFunctionPackage
	{
	public:
		CPredefinedExtFunctionPackage()
		{
			m_bUsed = false;
			m_ExtFunctionPtrs.setAutoDelete(true);
		}
		virtual ~CPredefinedExtFunctionPackage()
		{
			m_ExtFunctionPtrs.clear();
		}
		inline stuExtFunctionParam* SearchExtFunctionByName(SString &sFunName)
		{
			unsigned long pos;
			stuExtFunctionParam* p = m_ExtFunctionPtrs.FetchFirst(pos);
			while(p)
			{
				if(p->name == sFunName)
					return p;
				p = m_ExtFunctionPtrs.FetchNext(pos);
			}
			return NULL;
		}
		bool m_bUsed;//�Ƿ�ʹ�õ�ǰ
		SPtrList<stuExtFunctionParam> m_ExtFunctionPtrs;//Ԥ�ú���ָ������
	};

	SScriptParser();
	virtual ~SScriptParser();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���ı��ַ������ؽű��﷨
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-27 16:34
	// ����˵��:  @pScriptTextΪ�ű�����
	// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool LoadSyntaxText(char *pScriptText);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ָ�����ı��ļ����ؽű��﷨
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-27 16:35
	// ����˵��:  @pScriptFileΪ�ű��ļ���
	// �� �� ֵ:  true��ʾ���سɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool LoadSyntaxFile(char *pScriptFile);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������ת��Ϊ�﷨�ű�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 8:55
	// ����˵��:  void
	// �� �� ֵ:  SString
	//////////////////////////////////////////////////////////////////////////
	SString ToSyntaxScript();

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ���ش���ű����к�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 9:11
	// ����˵��:  void
	// �� �� ֵ:  int, 0��ʾû�д���
	//////////////////////////////////////////////////////////////////////////
	int GetErrorTextRow(){return m_iCurrTextRow;};
	
	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ���ĺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 10:30
	// ����˵��:  @ReturnVal��ʾ����ֵ��������
	//         :  @sFunName��ʾ��������
	//         :  @pParamValues��ʽ�����б�ָ�룬NULL��ʾû���βΣ������������÷�ʽ
	// �� �� ֵ:  true��ʾ���гɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool RunFunction(stuValue &ReturnVal,SString &sFunName,SPtrList<stuValue> *pParamValues=NULL);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ���������ƶ�Ӧ�ı���ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 16:40
	// ����˵��:  @sVarName��ʾ��������
	//         :  @Var��ʾ�ȱ����ı�������
	// �� �� ֵ:  stuValue*, NULL��ʾ�Ҳ���
	//////////////////////////////////////////////////////////////////////////
	inline stuValue* SearchValue(SString &sVarName,SPtrList<stuValue> &Var)
	{
		unsigned long pos;
		stuValue* pVal = Var.FetchFirst(pos);
		while(pVal)
		{
			if(pVal->var_name == sVarName)
				return pVal;
			pVal = Var.FetchNext(pos);
		}
		return NULL;
	};


	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ע��ָ���ĺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-31 15:55
	// ����˵��:  @sFunName��ʾ��������
	//         :  @pFun��ʾ����ָ��
	//         :  @pParam��ʾͨ�Ų���
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ���ʾͬ�������Ѵ���)
	//////////////////////////////////////////////////////////////////////////
	bool RegisterFunction(SString sFunName,SScript_Function pFun,void *pParam);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ָ��������ע��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-31 15:55
	// ����˵��:  @sFunName��ʾ��������
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ���ʾָ������δע��)
	//////////////////////////////////////////////////////////////////////////
	bool UnregisterFunction(SString sFunName);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ע��ָ����ʵ������
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-31 15:57
	// ����˵��:  @pInstValue��ʾʵ������ָ�룬ע��󲻸����ͷ�ָ�룬���ɵ����߸����ͷ�
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��(һ��Ϊͬ�������Ѵ���)
	//////////////////////////////////////////////////////////////////////////
	bool RegisterInstVariant(stuValue *pInstValue);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ȡ��ָ��ʵ��������ע��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-9 16:49
	// ����˵��:  @pInstValue��ʾ����ָ��
	// �� �� ֵ:  true��ʾ�ɹ���false��ʾʧ��
	//////////////////////////////////////////////////////////////////////////
	bool UnregisterInstVariant(stuValue *pInstValue);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ���ĺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 15:58
	// ����˵��:  @sFunNameΪ��������
	// �� �� ֵ:  stuStep*,NULL��ʾ�Ҳ���
	//////////////////////////////////////////////////////////////////////////
	inline stuStep* SearchFunction(SString sFunName)
	{
		unsigned long pos;
		stuStep *pFunStep = m_Functions.FetchFirst(pos);
		while(pFunStep)
		{
			if(pFunStep->IsFunctionByName(sFunName))
			{
				return pFunStep;
			}
			pFunStep = m_Functions.FetchNext(pos);
		}
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����Ѽ��صĺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-13 9:43
	// ����˵��:  void
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void ClearFunctions()
	{
		m_Functions.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ���һ���µĺ���
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-12 15:59
	// ����˵��:  @pFunStepΪ�µĺ���ָ�룬��Ӻ��ϲ㲻��Ҫ�����ͷ�
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	inline void AddFunction(stuStep* pFunStep)
	{
		m_Functions.append(pFunStep);
	}

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��������ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-12-13 18:56
	// ����˵��:  void
	// �� �� ֵ:  SPtrList<stuStep>*
	//////////////////////////////////////////////////////////////////////////
	inline SPtrList<stuStep>* GetFunctionPtr()
	{
		return &m_Functions;
	}

	void AddExtFunPackage(CPredefinedExtFunctionPackage *pNew)
	{
		m_ExtPkgs.append(pNew);
	}

private:

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡһ���ı�����
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-27 16:46
	// ����˵��:  @pTextΪԴ�ı������غ�Դ�ı��ַ���������������һ�ζ�ȡλ�ã�*pText==0��ʾ�����˽�β
	// �� �� ֵ:  �����¶����һ��
	//////////////////////////////////////////////////////////////////////////
	SString ReadLine(char* &pText);

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
	bool LoadStep(stuStep *pParentStep,SString &sLine,char * &pScriptText,stuStep *pStep);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ǰ�Ĳ���ת��Ϊ��Ӧ���﷨�ű�
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 8:56
	// ����˵��:  @iLevel��ʾ��ǰ��κ�
	//         :  @pStep����ָ��
	// �� �� ֵ:  SString
	//////////////////////////////////////////////////////////////////////////
	SString StepToScriptText(int iLevel,stuStep *pStep);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ��ȡһ�������ı��ʽ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 15:04
	// ����˵��:  @pExpressΪ���ʽ�ַ���ָ�룬���÷�����һ�ζ�ȡλ��,*pExpress=='\0'��ʾ�������
	//         :  @sTextΪ���÷��ص�ֵ
	// �� �� ֵ:  void
	//////////////////////////////////////////////////////////////////////////
	void ReadExpress(char * &pExpress,SString &sText);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ������ʽ��ֵ
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-28 16:58
	// ����˵��:  @InnerVarΪ�ڲ���������
	//         :  @expressΪ���ʽ�ַ���
	//         :  @sValue��ʾֵ����
	// �� �� ֵ:  true��ʾ����ɹ���false��ʾ����ʧ��
	//////////////////////////////////////////////////////////////////////////
	bool CalcExpressValue(SPtrList<stuValue> &InnerVar,SString &express,stuValue &sValue);

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
	int RunStep(SPtrList<stuValue> &InnerVar,stuStep *pParentStep,unsigned long &parent_pos,stuStep *pStep,stuValue &ReturnVal);

	//////////////////////////////////////////////////////////////////////////
	// ��    ��:  ����ָ����Ԥ�ú���ָ��
	// ��    ��:  �ۿ���
	// ����ʱ��:  2016-10-31 16:02
	// ����˵��:  
	// �� �� ֵ:  
	//////////////////////////////////////////////////////////////////////////
	inline stuExtFunctionParam* SearchExtFunctionByName(SString &sFunName)
	{
		unsigned long pos;
		stuExtFunctionParam* p = NULL;
		CPredefinedExtFunctionPackage *pPkg = m_ExtPkgs.FetchFirst(pos);
		while(pPkg)
		{
			p = pPkg->SearchExtFunctionByName(sFunName);
			if(p != NULL)
				return p;
			pPkg = m_ExtPkgs.FetchNext(pos);
		}
		
		p = m_ExtFunctionPtrs.FetchFirst(pos);
		while(p)
		{
			if(p->name == sFunName)
				return p;
			p = m_ExtFunctionPtrs.FetchNext(pos);
		}
		return NULL;
	}
// 
// 	static bool SScript_Function_print(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
// 	{
// 		if(pParamValues != NULL)
// 		{
// 			unsigned long pos;
// 			stuValue *pVal = pParamValues->FetchFirst(pos);
// 			while(pVal)
// 			{
// 				printf("%s",pVal->val.data());
// 				pVal = pParamValues->FetchNext(pos);
// 			}
// 			printf("\n");
// 		}
// 		return true;
// 	}

	SPtrList<stuStep> m_Functions;//�����Ӻ���
	int m_iCurrTextRow;//��ǰ�����ı����к�
	SPtrList<stuValue> m_InstVariant;//ʵ����������
	SPtrList<stuExtFunctionParam> m_ExtFunctionPtrs;//Ԥ�ú���ָ������
	stuStep *m_pCurrFunction;//��ǰ���͵ĺ���ָ��
	SPtrList<CPredefinedExtFunctionPackage> m_ExtPkgs;//��չ������
};

//////////////////////////////////////////////////////////////////////////
// ��    ��:  SScriptExtPkg_System
// ��    ��:  �ۿ���
// ����ʱ��:  2017-3-27 15:57
// ��    ��:  ϵͳ����չ������
//////////////////////////////////////////////////////////////////////////
class SScriptExtPkg_System : public SScriptParser::CPredefinedExtFunctionPackage
{
public:
	SScriptExtPkg_System()
	{
		m_ExtFunctionPtrs.append(new SScriptParser::stuExtFunctionParam("usleep",ExtFun_usleep));
		m_ExtFunctionPtrs.append(new SScriptParser::stuExtFunctionParam("print",ExtFun_print));
	};
	virtual ~SScriptExtPkg_System(){};
	static bool ExtFun_usleep(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
	{
		if(pParamValues == NULL || pParamValues->count() == 0)
			return false;
		SApi::UsSleep(pParamValues->at(0)->GetValue().toInt());
		return true;
	}
	static bool ExtFun_print(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues)
	{
		if(pParamValues != NULL)
		{
			unsigned long pos;
			SScriptParser::stuValue *pVal = pParamValues->FetchFirst(pos);
			while(pVal)
			{
				printf("%s",pVal->val.data());
				pVal = pParamValues->FetchNext(pos);
			}
		}
		return true;
	}
};


#endif//__NAM_SCRIPT_PARSER_H__
