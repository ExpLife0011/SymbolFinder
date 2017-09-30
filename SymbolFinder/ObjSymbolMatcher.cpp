#include <windows.h>
#include <stdio.h>
#include "MapFileToMem.h"
#define		TYPE_NOT_CARE						0
#define		TYPE_EXTERN_GLOBAL_VAR				1
#define		TYPE_EXTERN_FUNC					2
#define		TYPE_LOCAL_FUNC						3
#define		TYPE_LOCAL_GLOBAL_VAR				4
#define		TYPE_LOCAL_GLOBAL_VAR_INSEC			5
#define		TYPE_LOCAL_STATIC_VAR				6
#define		TYPE_LOCAL_LABEL					7

typedef struct _STOBJFILEINFO
{
	PIMAGE_SECTION_HEADER	pFirstSecHeader;				//ָ���һ�����ε��ײ�
	DWORD					dwSecNum;						//�ܵ�������
	PIMAGE_SYMBOL			pSymbolTable;					//ָ����ű�
	DWORD					dwSymbolNum;					//�ܹ��ķ�����
	LPSTR					pSymbolStrTable;				//ָ������������
}STOBJFILEINFO, *PSTOBJFILEINFO;


BOOL GetObjHeaderInfo(IN PIMAGE_FILE_HEADER pObjFile, OUT PSTOBJFILEINFO pstObjFileInfo)
{
	if ((pObjFile->Machine != IMAGE_FILE_MACHINE_I386))		//��OBJ�ļ����ڴ����ɹ�
	{
		return FALSE;
		//ErrorHandler(TEXT("�����OBJ�ļ���ʽ�Ƿ���"), ERROR_EXIT);
	}
	pstObjFileInfo->dwSecNum = pObjFile->NumberOfSections;
	pstObjFileInfo->dwSymbolNum = pObjFile->NumberOfSymbols;
	pstObjFileInfo->pSymbolTable = (PIMAGE_SYMBOL)((LPSTR)pObjFile + pObjFile->PointerToSymbolTable);
	pstObjFileInfo->pSymbolStrTable = (LPSTR)pstObjFileInfo->pSymbolTable + ((pstObjFileInfo->dwSymbolNum) * sizeof(IMAGE_SYMBOL));
}

LPSTR GetSymbolName(LPSTR lpNameOrIndex, LPSTR lpSymStrTable)
{
	if (*(DWORD*)lpNameOrIndex)		//��������,�ض��ַ����ֹ�ĩβ���NULL,�����ƻ���Value��ֵ,�ɵ��÷���ǰ����
	{
		lpNameOrIndex[8] = (CHAR)NULL;
		return lpNameOrIndex;
	}
	else								//��������,ֱ�Ӳ鵽�������������ַ
	{
		return (LPSTR)(lpSymStrTable + (*(PUINT32)(lpNameOrIndex + 4)));
	}
}

DWORD CheckSymbolType(PIMAGE_SYMBOL pSymTable)			//����������Ŀ�����ģ���ں���VA�ͱ����ڴ�ķ���
{
	if (pSymTable->StorageClass == IMAGE_SYM_CLASS_EXTERNAL)			//���ⲿ����
	{
		if (pSymTable->Type == IMAGE_SYM_DTYPE_FUNCTION << 4)			//Ϊ��������
		{
			if (pSymTable->SectionNumber == 0)							//��������OBJ�еĺ���
			{
				return TYPE_EXTERN_FUNC;
			}
			else if (pSymTable->SectionNumber > 0)						//��OBJ�ڶ���ĺ���
			{
				return TYPE_LOCAL_FUNC;
			}
		}
		else if (pSymTable->Type == IMAGE_SYM_DTYPE_NULL)
		{
			if (pSymTable->SectionNumber == 0)
			{
				if (pSymTable->Value == 0)
				{
					return TYPE_EXTERN_GLOBAL_VAR;			//��������OBJ�еı���
				}
				else
				{
					return TYPE_LOCAL_GLOBAL_VAR;			//��OBJ�ڶ���ı���,�������κ�����
				}
			}
			if (pSymTable->SectionNumber > 0)
			{
				return TYPE_LOCAL_GLOBAL_VAR_INSEC;			 //��OBJ�ڶ���ı���, �����ݶ���
			}
		}
	}
	else if (pSymTable->StorageClass == IMAGE_SYM_CLASS_STATIC)
	{
		if (pSymTable->Type == IMAGE_SYM_DTYPE_NULL)
		{
			if (pSymTable->SectionNumber > 0)
			{
				//return TYPE_LOCAL_GLOBAL_VAR_INSEC;			//��OBJ�ڶ���ı���,�������κ�����
				return TYPE_LOCAL_STATIC_VAR;
			}
		}
	}
	else if (pSymTable->StorageClass == IMAGE_SYM_CLASS_LABEL)
	{
		//return TYPE_LOCAL_GLOBAL_VAR_INSEC;
		return TYPE_LOCAL_LABEL;
	}
	return 0;
}

DWORD BuildSymbolItemTable(PIMAGE_SYMBOL pSymTable, LPSTR lpSymStrTable)
{
	DWORD dwSymType = CheckSymbolType(pSymTable);

	DWORD dwValue = pSymTable->Value;	
	DWORD dwSection = pSymTable->SectionNumber;
	LPSTR lpszSymbolName = GetSymbolName((LPSTR)&(pSymTable->N), lpSymStrTable);
	switch (dwSymType)
	{
	case	TYPE_EXTERN_GLOBAL_VAR:					//��������OBJ�еı���
		printf("������ģ���ⶨ���ȫ�ֱ���:%s\n", lpszSymbolName);
		break;
	case	TYPE_EXTERN_FUNC:
		printf("������ģ���ⶨ��ĺ���:%s\n", lpszSymbolName);
		break;
	case	TYPE_LOCAL_FUNC:						//��OBJ�ڶ���ĺ���,�����VA
		printf("�����ڱ�ģ���ж���ĺ���:%s\n", lpszSymbolName);
		break;
	case	TYPE_LOCAL_GLOBAL_VAR:
		printf("���ֵ��������ȫ��δ��ʼ������:%s\n", lpszSymbolName);
		break;
	case	TYPE_LOCAL_GLOBAL_VAR_INSEC:			//��OBJ�ڶ���ı���,�����ݶ���,������Ϊ��ʼ�������ݶ��ڴ�ʱҲ�������˿ռ�,ֱ������VA����
	case	TYPE_LOCAL_STATIC_VAR:
	case	TYPE_LOCAL_LABEL:
		printf("�����ڱ�ģ�����ݶ��е�ȫ�ֱ���:%s\n", lpszSymbolName);
		break;
	}
	return dwSymType;
}

DWORD ResolveSymbolTable(PSTOBJFILEINFO pstObjFileInfo)
{
	DWORD			dwSymNum = pstObjFileInfo->dwSymbolNum;				//��ģ��ķ�����
	PIMAGE_SYMBOL	pSymTable = pstObjFileInfo->pSymbolTable;			//��ģ��ķ��ű�ָ��
	LPSTR			pSymStrTable = pstObjFileInfo->pSymbolStrTable;		//��ģ��ĳ��������ַ�����

	for (DWORD i = 0; i < dwSymNum; i++)									//�������ű�,�ҳ������ͱ���
	{
		BuildSymbolItemTable(&pSymTable[i], pstObjFileInfo->pSymbolStrTable);
		i = i + pSymTable[i].NumberOfAuxSymbols;							//��������������
	}
	return 0;
}

BOOL HandleObjFile(LPTSTR pszObjFilePath)
{
	PSTMEMFILEHANDLE hObjFile = FileToMemMap(pszObjFilePath);
	STOBJFILEINFO stObjFileInfo;
	GetObjHeaderInfo((PIMAGE_FILE_HEADER)GetMemFilePointer(hObjFile), &stObjFileInfo);
	ResolveSymbolTable(&stObjFileInfo);
	CloseFileMap(hObjFile);
	return TRUE;
}