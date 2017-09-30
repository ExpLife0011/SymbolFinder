#include <tchar.h>  
#include <Windows.h>  
#include <Shlwapi.h>  
#pragma comment( lib, "Shlwapi.lib" )  
#include <strsafe.h>  

BOOL FindAllFilesInDirectory(LPCTSTR pszDirPath)
{
	// �ݹ�߽�������·��ΪNULL�򲻴��ڣ�����FALSE
	if (NULL == pszDirPath || FALSE == PathFileExists(pszDirPath))
	{
		return FALSE;
	}

	// ������·���µ������ļ�  
	WIN32_FIND_DATA FindData;
	TCHAR Directory[MAX_PATH];
	StringCbPrintf(Directory, sizeof(Directory), TEXT("%s\\*.*"), pszDirPath);

	// ��ʼ������ѯ��·���µ������ļ����ļ���
	HANDLE hFile = FindFirstFile(Directory, &FindData);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return FALSE;
	}

	TCHAR szSubItemPath[MAX_PATH];
	do
	{
		// ���˵���ǰĿ¼���ϼ�Ŀ¼���ļ��� 
		if (TEXT('.') == FindData.cFileName[0])
		{
			continue;
		}

		// �õ���ǰ�������·��
		StringCbPrintf(szSubItemPath, sizeof(szSubItemPath), TEXT("%s\\%s"), pszDirPath, FindData.cFileName);

		// �жϵ�ǰ�ҵ������ļ������ļ��У������ļ�����ִ�в��Ҳ���
		if (0 == (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			_tprintf(TEXT("find file: %s size:%d bytes.\n"), szSubItemPath, FindData.nFileSizeLow);
		}
		// �����ļ��У���ݹ���������Ŀ¼
		else
		{
			_tprintf(TEXT("find directory: %s.\n"), szSubItemPath);
			//�����ݹ�����ļ���  
			if (false == FindAllFilesInDirectory(szSubItemPath))
			{
				return FALSE;
			}
		}
	} while (FALSE != FindNextFile(hFile, &FindData));
	//����˳�ѭ�������Ƿ���û���ļ��˷������  
	if (ERROR_NO_MORE_FILES != GetLastError())
	{
		return FALSE;
	}
	FindClose(hFile);
	return TRUE;
}