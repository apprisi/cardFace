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
//���ܹ���֤
int AuthorityDog(const char* strExePath, const char* strRegItemName, int isDisplayDlg = 1);
//��������֤
int AuthorityMachineCode(const char* strExePath, const char* strRegItemName, int isDisplayDlg = 1);
//ʹ���޴�
int AuthorityUseTimes(const char* strExePath, const char* strRegItemName, const char* strRegKeyName, int iTotalTimes, int isDisplayDlg = 1);
//���ܹ��������롢ʹ���޴�һ����֤
int AuthorityAll(const char* strExePath, const char* strRegItemName, const char* strRegKeyName, int iTotalTimes,int isDisplayDlg = 1);


#ifdef __cplusplus
	}
#endif

#endif
