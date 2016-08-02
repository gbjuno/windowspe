#include<stdio.h>
#include<stdlib.h>

#include "ReadPE.h"
#include "PE.h"
#define READBUF 10240
#define __DEBUG__ true

/*
* int ReadPE(const char* target, char* pTargetFileBuffer)
* 拷贝文件到内存当中
* 失败返回0，成功则返回读取文件的大小(字节)
*
* 使用说明:
* char *pTarget;
* int num = ReadPE(target,pTarget);
* ...
* free(*pTarget);
*/

int ReadPE(const char* target, char** pTargetFileBuffer) {
	FILE* targetHandler = fopen(target,"rb");
	//打开文件失败则返回
	if(targetHandler == NULL) {
		if(__DEBUG__) {
			printf("cannot open file %s\n",target);
		}
		return 0;
	}
	
	
	if(fseek(targetHandler, 0, SEEK_END) !=0 ){
		if(__DEBUG__) {
			perror("cannot seek file to end");
		}
		return 0;
	}
	
	long targetLen = ftell(targetHandler);
	printf("targetlen:%d\n",targetLen);
	if(fseek(targetHandler, 0, SEEK_SET) !=0 ){
		if(__DEBUG__) {
			perror("cannot seek file to start");
		}
		return 0;
	}

	//申请空间大小为文件大小的内存用于读取数据,申请失败则返回
	char* fileBuffer = (char*)malloc(targetLen);
	if(fileBuffer == NULL) {
		if(__DEBUG__) {
			printf("cannot allocate memory :%s bytes\n", targetLen);
		}
		return 0;
	}
	
	//拷贝文件到内存
	int readNum;
	while((readNum = fread(fileBuffer, 1, targetLen, targetHandler)) != targetLen) {
		if(__DEBUG__) {
			printf("readnum:%d\n",readNum);
			perror("cannot copy file to memory (read incomplete)");
		}
		free(fileBuffer);
		return 0;	
	}

	*pTargetFileBuffer = fileBuffer;
	return targetLen;
}

/* 
* 
* int GetPEInfoFromFileBuf(const char* fileBuffer, struct _IMAGE_PE_INFO * pToPEInfo)
* 成功返回0,失败返回1	
*
* 使用方式:
* struct _IMAGE_PE_HEADER_INFO* pToPEHeaderInfo = NULL;
* int ret = GetPEHeaderInfoFromFileBuf(fileBuffer,pToPEHeaderInfo);
* if(ret != 0) {
* ...
* }
* free(pToPEHeaderInfo);
*/


int GetPEHeaderInfoFromFileBuf(const char* fileBuffer, struct _IMAGE_PE_HEADER_INFO ** pToPEHeaderInfo) {
	struct _IMAGE_PE_HEADER_INFO *info = (struct _IMAGE_PE_HEADER_INFO *)malloc(sizeof(struct _IMAGE_PE_HEADER_INFO));
	if(info == NULL) {
		printf("cannot allocate space for _IMAGE_PE_HEADER_INFO\n");
		return 1;
	}
	info->DosHeader = (struct _IMAGE_DOS_HEADER*)fileBuffer;
	info->Signature = *((int*)(fileBuffer + info->DosHeader->e_lfanew));
	info->PeHeader = (struct _IMAGE_FILE_HEADER*)(fileBuffer + info->DosHeader->e_lfanew + 4);
	info->OpPEHeader = (struct _IMAGE_OPTIONAL_HEADER*)((char *)info->PeHeader + 20);
	info->SectionHeader = (struct _IMAGE_SECTION_HEADER*)((char *)info->OpPEHeader + info->PeHeader->SizeOfOptionalHeader);

	*pToPEHeaderInfo = info;
	return 0;
}

/*
*  int GetImageBufFromFileBuf(const char* fileBuffer, char* imageBuffer)
*  成功返回0,失败返回其他值
*  使用说明:
*  char *imageBuffer = NULL;
*  int ret = GetImageBufFromFileBuf(fileBuffer, imageBuffer)
*  if (ret != 0) {
*      ...
*  }
*  free(imageBuffer)  
*/

int GetImageBufFromFileBuf(const char* fileBuffer, char** imageBuffer) {
	
	return 0;
}