#ifndef DOGAUTHORITY_H
#define DOGAUTHORITY_H

//typedef char * PCHAR;
//typedef unsigned char * PBYTE;
//typedef short SHORT;
//typedef int BOOL;

#ifdef __cplusplus
extern "C" {
#endif

int test(const char* str);
void StrtoHex(char * pch,unsigned char * pbHex);
void HextoStr(unsigned char * pbHex, short sLen, char * pch);
//加密狗验证
int AuthorityDog(const char* strExePath, const char* strRegItemName, int isDisplayDlg = 1);
//机器码验证
int AuthorityMachineCode(const char* strExePath, const char* strRegItemName, int isDisplayDlg = 1);
//使用限次
int AuthorityUseTimes(const char* strExePath, const char* strRegItemName, const char* strRegKeyName, int iTotalTimes, int isDisplayDlg = 1);
//加密狗、机器码、使用限次一起验证
int AuthorityAll(const char* strExePath, const char* strRegItemName, const char* strRegKeyName, int iTotalTimes,int isDisplayDlg = 1);


#ifdef __cplusplus
	}
#endif

#endif
