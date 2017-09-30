#pragma once
#include	<tchar.h>
#include	"ErrorHandler.h"

#define		INVALID_HANDLE		-1
#define		INVALID_FILE		-2


typedef struct _STMEMFILEHANDLE			//FileToMemMap���صĽṹ����
{
	LPTSTR		lpszFilePath;			//�ļ�ȫ·���������ļ���
	HANDLE		hFile;					//�ļ����
	HANDLE		hMapFile;				//�ļ��ڴ�ӳ����
	LPSTR		lpFile;					//�ļ�ָ��
	DWORD		dwFileSize;				//�ļ���С
}STMEMFILEHANDLE, *PSTMEMFILEHANDLE;


PSTMEMFILEHANDLE	FileToMemMap(LPTSTR lpszFilePath);
VOID				CloseFileMap(PSTMEMFILEHANDLE pHMemHandle);
DWORD	GetMemFilePointer(PSTMEMFILEHANDLE pMemFileHandle);
LPTSTR				SelectFile(PDWORD	dwExtNamePos); 
