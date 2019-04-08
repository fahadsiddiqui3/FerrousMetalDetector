#include "fileop.h"
#include "string.h"




void FILEOP_Get_Filename(char *FileName,uint16_t Length, uint16_t Counter)
{
	sprintf(FileName,"0:Log_%dcmHor_%dcmVer.csv",Length,Counter);
}

