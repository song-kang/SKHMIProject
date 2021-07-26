/**
 *
 * 文 件 名 : ScriptParser.h
 * 创建日期 : 2016-10-27 11:10
 * 作    者 : 邵凯田(skt001@163.com)
 * 修改日期 : $Date: $
 * 当前版本 : $Revision: $
 * 功能描述 : C语法脚本解释器
 * 修改记录 : 
 *            $Log: $
 *
 * Ver  Date        Author  Comments
 * ---  ----------  ------  -------------------------------------------
 * 001	2016-10-27	邵凯田　创建文件
 *
 **/

#ifndef __NAM_SCRIPT_PARSER_H__
#define __NAM_SCRIPT_PARSER_H__

#include "SList.h"
#include "SApi.h"
#include "SStack.h"

//////////////////////////////////////////////////////////////////////////
// 名    称:  SScriptParser
// 作    者:  邵凯田
// 创建时间:  2016-10-27 11:19
// 描    述:  解析型脚本基础解释器类
/* 采用类C语言的语法结构 语法介绍：
1）支持变量、常量、函数；
2）支持赋值运算、常见算术运算、比较运算及逻辑运算；
3）支持条件分支；
4）支持循环；
5）支持嵌套分支及循环的嵌套使用；
6）支持语法检查；
7）变量及函数的接口抽象化；
8）支持定义临时变量（类型包括：float/int/string/bool），允许临时变量和抽象变量混用，解析器优先匹配临时变量，匹配失败才查找抽象变量；
如:
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
	//步骤类型定义
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
		STEP_COMMENT,//注释		
	};
	//值类型定义
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
	//赋值类型
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
		//算术运算
		CALC_ADD,			//+
		CALC_DEL,			//-
		CALC_MUL,			//*
		CALC_DIV,			///
		//逻辑运算
		CALC_AND,			//&&
		CALC_OR,			//||
		CALC_NOT,			//!,
		//比较运算
		CALC_BT,			//>
		CALC_BET,			//>=
		CALC_LT,			//<
		CALC_LET,			//<=
		CALC_EQ,			//==
		CALC_NOTEQ,			//!=
	};
	static eCALC_TYPE GetCalcTypeByStr(SString str)
	{
		//算术运算
		if(str == "+")
			return CALC_ADD;			//+
		else if(str == "-")
			return CALC_DEL;			//-
		else if(str == "*")
			return CALC_MUL;			//*
		else if(str == "/")
			return CALC_DIV;			///
		//逻辑运算
		else if(str == "&&")
			return CALC_AND;			//&&
		else if(str == "||")
			return CALC_OR;				//||
		else if(str == "!")
			return CALC_NOT;			//!,
		//比较运算
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
	//值定义
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
		// 描    述:  赋值运算符重载
		// 作    者:  邵凯田
		// 创建时间:  2016-10-31 10:47
		// 参数说明:  @s值
		// 返 回 值:  *this
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

	//步骤定义
	struct stuFunctionInfo
	{
		stuFunctionInfo()
		{
			params.setAutoDelete(true);
		}
		eValueType ret_vt;//返回类型
		SString fun_name;//函数名称
		SPtrList<stuValue> params;//形参列表
	};
	struct stuDefineInfo
	{
		eValueType define_vt;//值类型
		SString var_name;//变量名称
		SString init_val;//初始值，空表示没有初始值
	};
	struct stuSetvalInfo
	{
		SString var_name;//被赋值变量名称
		eSETVAL_TYPE setv_type;//赋值类型
		SString express;//赋值表达式
	};
	struct stuConditionInfo
	{
		SString express;//条件表达式
	};
	struct stuReturnInfo
	{
		SString express;//值表达式
	};
	struct stuCallInfo
	{
		SString fun_name;//被调用函数名称
		SStringList param_express;//实际参数表达式
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
		eStepType type;//步骤类型
		SString step_text;//步骤文本
		SPtrList<stuStep> sub_step;//子步骤
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
	// 描    述:  SScript函数回调接口
	// 作    者:  邵凯田
	// 创建时间:  2016-10-31 15:50
	// 参数说明:  @pCbParam表示回调参数(注册时指定的)
	//         :  @ReturnVal表示返回值内容引用
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	typedef bool (*SScript_Function)(void *pCbParam,SScriptParser::stuValue &ReturnVal,SPtrList<SScriptParser::stuValue> *pParamValues);


	//////////////////////////////////////////////////////////////////////////
	// 名    称:  stuExtFunction
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 16:53
	// 描    述:  外部扩展函数定义
	//////////////////////////////////////////////////////////////////////////
	struct stuExtFunction
	{
		const char* sFunName;//函数名称
		SScript_Function pFun;//函数指针
		eValueType sRetValType;//返回值类型
		int iParamCount;//形式参数数量
		const char* sParams;//所有的形式参数：int a,int b
		const char* sComment;//备注
	};

	//////////////////////////////////////////////////////////////////////////
	// 名    称:  stuExtVariant
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 16:53
	// 描    述:  外部扩展变量定义
	//////////////////////////////////////////////////////////////////////////
	struct stuExtVariant
	{
		const char* sVarName;//变量名称
		eValueType var_type;//变量类型
		const char* sDefaultVal;//变量缺省值
		const char* sComment;//备注
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
	// 名    称:  CPredefinedExtFunctionPackage
	// 作    者:  邵凯田
	// 创建时间:  2017-3-27 15:45
	// 描    述:  预定义扩展函数包
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
		bool m_bUsed;//是否使用当前
		SPtrList<stuExtFunctionParam> m_ExtFunctionPtrs;//预置函数指针链表
	};

	SScriptParser();
	virtual ~SScriptParser();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从文本字符串加载脚本语法
	// 作    者:  邵凯田
	// 创建时间:  2016-10-27 16:34
	// 参数说明:  @pScriptText为脚本内容
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadSyntaxText(char *pScriptText);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  从指定的文本文件加载脚本语法
	// 作    者:  邵凯田
	// 创建时间:  2016-10-27 16:35
	// 参数说明:  @pScriptFile为脚本文件名
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadSyntaxFile(char *pScriptFile);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将对象转换为语法脚本
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 8:55
	// 参数说明:  void
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString ToSyntaxScript();

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取加载错误脚本的行号
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 9:11
	// 参数说明:  void
	// 返 回 值:  int, 0表示没有错误
	//////////////////////////////////////////////////////////////////////////
	int GetErrorTextRow(){return m_iCurrTextRow;};
	
	//////////////////////////////////////////////////////////////////////////
	// 描    述:  运行指定的函数
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 10:30
	// 参数说明:  @ReturnVal表示返回值内容引用
	//         :  @sFunName表示函数名称
	//         :  @pParamValues形式参数列表指针，NULL表示没有形参，参数采用引用方式
	// 返 回 值:  true表示运行成功，false表示运行失败
	//////////////////////////////////////////////////////////////////////////
	bool RunFunction(stuValue &ReturnVal,SString &sFunName,SPtrList<stuValue> *pParamValues=NULL);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  检索指定变量名称对应的变量指针
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 16:40
	// 参数说明:  @sVarName表示变量名称
	//         :  @Var表示等遍历的变量集合
	// 返 回 值:  stuValue*, NULL表示找不到
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
	// 描    述:  注册指定的函数
	// 作    者:  邵凯田
	// 创建时间:  2016-10-31 15:55
	// 参数说明:  @sFunName表示函数名称
	//         :  @pFun表示函数指针
	//         :  @pParam表示通信参数
	// 返 回 值:  true表示成功，false表示失败(一般表示同名函数已存在)
	//////////////////////////////////////////////////////////////////////////
	bool RegisterFunction(SString sFunName,SScript_Function pFun,void *pParam);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取消指定函数的注册
	// 作    者:  邵凯田
	// 创建时间:  2016-10-31 15:55
	// 参数说明:  @sFunName表示函数名称
	// 返 回 值:  true表示成功，false表示失败(一般表示指定函数未注册)
	//////////////////////////////////////////////////////////////////////////
	bool UnregisterFunction(SString sFunName);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  注册指定的实例变量
	// 作    者:  邵凯田
	// 创建时间:  2016-10-31 15:57
	// 参数说明:  @pInstValue表示实例变量指针，注册后不负责释放指针，仍由调用者负责释放
	// 返 回 值:  true表示成功，false表示失败(一般为同名变量已存在)
	//////////////////////////////////////////////////////////////////////////
	bool RegisterInstVariant(stuValue *pInstValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  取消指定实例变量的注册
	// 作    者:  邵凯田
	// 创建时间:  2016-12-9 16:49
	// 参数说明:  @pInstValue表示变量指针
	// 返 回 值:  true表示成功，false表示失败
	//////////////////////////////////////////////////////////////////////////
	bool UnregisterInstVariant(stuValue *pInstValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  搜索指定的函数
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 15:58
	// 参数说明:  @sFunName为函数名称
	// 返 回 值:  stuStep*,NULL表示找不到
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
	// 描    述:  清除已加载的函数
	// 作    者:  邵凯田
	// 创建时间:  2016-12-13 9:43
	// 参数说明:  void
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void ClearFunctions()
	{
		m_Functions.clear();
	}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  添加一个新的函数
	// 作    者:  邵凯田
	// 创建时间:  2016-12-12 15:59
	// 参数说明:  @pFunStep为新的函数指针，添加后上层不需要进行释放
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	inline void AddFunction(stuStep* pFunStep)
	{
		m_Functions.append(pFunStep);
	}

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  函数队列指针
	// 作    者:  邵凯田
	// 创建时间:  2016-12-13 18:56
	// 参数说明:  void
	// 返 回 值:  SPtrList<stuStep>*
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
	// 描    述:  读取一行文本内容
	// 作    者:  邵凯田
	// 创建时间:  2016-10-27 16:46
	// 参数说明:  @pText为源文本，返回后，源文本字符串依次向跳到下一次读取位置，*pText==0表示读到了结尾
	// 返 回 值:  返回新读入的一行
	//////////////////////////////////////////////////////////////////////////
	SString ReadLine(char* &pText);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  加载步骤到Step中
	// 作    者:  邵凯田
	// 创建时间:  2016-10-27 18:18
	// 参数说明:  @pParentStep表示父节点指针，NULL表示没有父节点
	//         :  @sLine表示步骤文本
	//         :  @pScriptText表示脚本文本，处理完成后引用返回到下一步骤位置
	//         :  @pStep表示当前步骤的指针
	// 返 回 值:  true表示加载成功，false表示加载失败
	//////////////////////////////////////////////////////////////////////////
	bool LoadStep(stuStep *pParentStep,SString &sLine,char * &pScriptText,stuStep *pStep);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  将当前的步骤转换为对应的语法脚本
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 8:56
	// 参数说明:  @iLevel表示当前层次号
	//         :  @pStep步骤指针
	// 返 回 值:  SString
	//////////////////////////////////////////////////////////////////////////
	SString StepToScriptText(int iLevel,stuStep *pStep);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  读取一个完整的表达式
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 15:04
	// 参数说明:  @pExpress为表达式字符串指针，引用返回下一次读取位置,*pExpress=='\0'表示处理完毕
	//         :  @sText为引用返回的值
	// 返 回 值:  void
	//////////////////////////////////////////////////////////////////////////
	void ReadExpress(char * &pExpress,SString &sText);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  计算表达式的值
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 16:58
	// 参数说明:  @InnerVar为内部变量集合
	//         :  @express为表达式字符串
	//         :  @sValue表示值内容
	// 返 回 值:  true表示计算成功，false表示计算失败
	//////////////////////////////////////////////////////////////////////////
	bool CalcExpressValue(SPtrList<stuValue> &InnerVar,SString &express,stuValue &sValue);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  运行步骤指令，及其下的子步骤
	// 作    者:  邵凯田
	// 创建时间:  2016-10-28 17:30
	// 参数说明:  @InnerVar为内部变量集合
	//         :  @pParentStep为父步骤指针
	//         :  @parent_pos为父指针遍历位置，引用返回
	//         :  @pStep为步骤指针
	//         :  @ReturnVal为引用返回的值，返回0时将被赋值
	// 返 回 值:  <-1表示失败，>0表示成功，=0表示返回，-1表示break
	//////////////////////////////////////////////////////////////////////////
	int RunStep(SPtrList<stuValue> &InnerVar,stuStep *pParentStep,unsigned long &parent_pos,stuStep *pStep,stuValue &ReturnVal);

	//////////////////////////////////////////////////////////////////////////
	// 描    述:  查找指定的预置函数指针
	// 作    者:  邵凯田
	// 创建时间:  2016-10-31 16:02
	// 参数说明:  
	// 返 回 值:  
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

	SPtrList<stuStep> m_Functions;//所有子函数
	int m_iCurrTextRow;//当前解释文本的行号
	SPtrList<stuValue> m_InstVariant;//实例变量集合
	SPtrList<stuExtFunctionParam> m_ExtFunctionPtrs;//预置函数指针链表
	stuStep *m_pCurrFunction;//当前解释的函数指针
	SPtrList<CPredefinedExtFunctionPackage> m_ExtPkgs;//扩展函数包
};

//////////////////////////////////////////////////////////////////////////
// 名    称:  SScriptExtPkg_System
// 作    者:  邵凯田
// 创建时间:  2017-3-27 15:57
// 描    述:  系统类扩展函数包
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
