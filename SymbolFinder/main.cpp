#include <stdio.h>
#include "MapFileToMem.h"
#include "Resolver.h"

int main()
{
	LPTSTR pszTextObjPath = TEXT("G:\\G\\��ҵ���\\��������\\BasicType.obj");
	HandleObjFile(pszTextObjPath);

	HandleLibFile("G:\\G\\��ҵ���\\��������\\LIBC.LIB", "printf");
	
	return 0;
}