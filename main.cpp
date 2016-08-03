#include <cstdlib>
#include <stdio.h>
#include <string.h>

#include "PE.h"
#include "ReadPE.h"

#define SrcFile "C:/notepad.exe"
#define DestFile "C:/notepad_m.exe"

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

int Test_GetImageBufFromFileBuf() {
	char* target = SrcFile;
	char* pTargetFileBuffer = NULL;
	char* imageBuffer = NULL;
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

	ret = GetImageBufFromFileBuf(pTargetFileBuffer, &imageBuffer);
	if(ret != 0) {
		return 1;
	}

	printf("signature is %x\n", *((int *)(imageBuffer+pToPEHeaderInfo->DosHeader->e_lfanew)));
	free(pToPEHeaderInfo);
	free(pTargetFileBuffer);
	free(imageBuffer);
	return 1;

}

int Test_GetPEHeaderInfoFromImageBuf() {
	char* target = SrcFile;
	char* pTargetFileBuffer = NULL;
	char* imageBuffer = NULL;
	char* NewFileBuffer = NULL;
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

	ret = GetImageBufFromFileBuf(pTargetFileBuffer, &imageBuffer);
	if(ret != 0) {
		return 1;
	}

	ret = GetFileBufFromImageBuf(imageBuffer, &NewFileBuffer);
	if(ret !=0) {
		return 1;
	}

	printf("signature is %x\n", *((int *)(imageBuffer+pToPEHeaderInfo->DosHeader->e_lfanew)));
	printf("signature is %x\n", *((int *)(NewFileBuffer+pToPEHeaderInfo->DosHeader->e_lfanew)));

	char* oldFile = pTargetFileBuffer;
	char* newFile = NewFileBuffer;
	
	for(int i=0;i<num;i++) {
		if(*oldFile!=*newFile) {
			printf("%d %x %x\n",i, *oldFile, * newFile);
		}
		oldFile++;
		newFile++;	
	}
	
	free(pToPEHeaderInfo);
	free(pTargetFileBuffer);
	free(imageBuffer);
	free(NewFileBuffer);
	return 1;
}


char RunCode[18] = {
	0x6A,0x00,0x6A,0x00,0x6A,0x00,0x6A,0x00,
	0xE8,0x00,0x00,0x00,0x00,
	0xE9,0x00,0x00,0x00,0x00,
};

#define MessageBoxAddr 0x77d507ea
int insertCode(char** pImageBuf) {
	struct _IMAGE_PE_HEADER_INFO * info = NULL;
	GetPEHeaderInfoFromImageBuf(*pImageBuf,&info);

	int numOfSections = info->PeHeader->NumberOfSections;
	struct _IMAGE_SECTION_HEADER *firstHeader = info->SectionHeader;

	int firstOkHeader = -1;
	bool * sectionStatus = (bool*) malloc(numOfSections);

	struct _IMAGE_SECTION_HEADER *thisHeader = firstHeader;
	for(int i=0;i<numOfSections;i++) {
		sectionStatus[i] = false;
		if((thisHeader->SizeOfRawData - thisHeader->Misc.VirtualSize) > 18) {
			sectionStatus[i] = true;
			if(firstOkHeader == -1) {
					firstOkHeader = i;
			}
		}
		thisHeader++;
	}

	if(firstOkHeader == -1) {
		return 1;
	}

	for(i=0;i<numOfSections;i++) {
		printf("section[%s]:%d\n",(firstHeader + i)->Name,sectionStatus[i]);
	}
	
	char* startAddr = *pImageBuf + (firstHeader + firstOkHeader)->VirtualAddress + (firstHeader + firstOkHeader)->Misc.VirtualSize;
	memcpy(startAddr, RunCode, 18);
	(firstHeader + firstOkHeader)->SizeOfRawData += 18;
	printf("sizeOfRawHeader:%x\n", 	(firstHeader + firstOkHeader)->SizeOfRawData);

	int msgBoxAddr = MessageBoxAddr - (info->OpPEHeader->ImageBase + (firstHeader + firstOkHeader)->VirtualAddress + (firstHeader + firstOkHeader)->Misc.VirtualSize + 13);
	int jmpAddr = (info->OpPEHeader->ImageBase + info->OpPEHeader->AddressOfEntryPoint) - (info->OpPEHeader->ImageBase + (firstHeader + firstOkHeader)->VirtualAddress + (firstHeader + firstOkHeader)->Misc.VirtualSize + 18);
	info->OpPEHeader->AddressOfEntryPoint = (firstHeader + firstOkHeader)->VirtualAddress + (firstHeader + firstOkHeader)->Misc.VirtualSize;
	printf("MSGBOXAddr: %x, JMPAddr: %x\n",msgBoxAddr, jmpAddr);
    memcpy(startAddr+9,&msgBoxAddr,4);
	memcpy(startAddr+14,&jmpAddr,4);
	return 0;
}

int main(int argc, char ** argv) {
	//Test_ReadPE();
	//Test_GetPEHeaderInfoFromFileBuf();
	//Test_GetImageBufFromFileBuf();
	//Test_GetPEHeaderInfoFromImageBuf();
	
	char* oldFileBuf = NULL;
	char* imageBuf = NULL;
	char* newFileBuf = NULL;
	int ret = 0;
	int num = 0;
	
	num = ReadPE(SrcFile, &oldFileBuf);
	if(oldFileBuf==NULL){
		return 1;
	}
	
	ret = GetImageBufFromFileBuf(oldFileBuf, &imageBuf);
	if (ret != 0) {
		free(oldFileBuf);
		return 1;
	}

	insertCode(&imageBuf);

	GetFileBufFromImageBuf(imageBuf, &newFileBuf);

	char* oldFile = oldFileBuf;
	char* newFile = newFileBuf;

	for(int i=0;i<num;i++) {
		if(*oldFile!=*newFile) {
			printf("%d %x %x\n",i, *oldFile, * newFile);
		}
		oldFile++;
		newFile++;	
	}

	ret = SaveFileFromBuf(DestFile, newFileBuf, num);
	free(oldFileBuf);
	free(imageBuf);
	free(newFileBuf);

	if(ret != 0) {
		return 1;
	}
	return 0;
}