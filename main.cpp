#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "PE.h"
#include "ReadPE.h"

#define SrcFile "C:/ipmsg.exe"
#define DestFile   "C:/ipmsg_modify.exe"

int Test_ReadPE() {
	char* target = SrcFile;
	char* pTargetFileBuffer = NULL;

	int num = ReadPE(target, &pTargetFileBuffer);
	printf("size of file: %d\n", num);
	free(pTargetFileBuffer);
	return 0;
}

int Test_GetPEHeaderInfoFromFileBuf() {
	char* target = SrcFile;
	char* pTargetFileBuffer = NULL;
	struct _IMAGE_PE_HEADER_INFO * pToPEHeaderInfo = NULL;

	int num = ReadPE(target, &pTargetFileBuffer);
	if(pTargetFileBuffer == NULL) {
		printf("pTargetFileBuffer is NULL\n");
		return 1;
	}

	int ret = GetPEHeaderInfoFromFileBuf(pTargetFileBuffer, &pToPEHeaderInfo);
	if (ret != 0) {
		free(pTargetFileBuffer);
		printf("GetPEHeaderInfoFromFileBuf error!\n");
		return 1;
	}

	printf("%x\n",pToPEHeaderInfo->Signature);
	printf("%x\n",pToPEHeaderInfo->PeHeader->SizeOfOptionalHeader);
	free(pTargetFileBuffer);
	free(pToPEHeaderInfo);
	return 0;

}

int main(int argc, char ** argv) {
	//Test_ReadPE();
	Test_GetPEHeaderInfoFromFileBuf();
	return 0;
}