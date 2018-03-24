#ifndef __SK_BASE_INC_H__
#define __SK_BASE_INC_H__

#define SSP_DBUSED_MYSQL
#define SSP_DBUSED_ORACLE

#ifdef SSP_DBUSED_MYSQL
#include "SMySQL.h"
#endif
#ifdef SSP_DBUSED_ORACLE
#include "SOracle.h"
#endif

#ifdef WIN32
  #ifdef _DEBUG
    #pragma comment(lib,"sbased.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysqld.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracled.lib")
    #endif
  #else
    #pragma comment(lib,"sbase.lib")
    #ifdef SSP_DBUSED_MYSQL
      #pragma comment(lib,"sbase_mysql.lib")
    #endif
    #ifdef SSP_DBUSED_ORACLE
      #pragma comment(lib,"sbase_oracle.lib")
    #endif
  #endif
  
  #ifdef SKBASE_EXPORTS
    #define SK_BASE_EXPORT __declspec(dllexport)
  #else
    #define SK_BASE_EXPORT __declspec(dllimport)
  #endif
#endif

#endif	//__SK_BASE_INC_H__
