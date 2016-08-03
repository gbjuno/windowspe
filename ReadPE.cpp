#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "ReadPE.h"
#include "PE.h"
#define READBUF 10240
#define __DEBUG__ true

/*
* int ReadPE(const char* target, char** pTargetFileBuffer)
* 拷贝文件到内存当中
* 失败返回0，成功则返回读取文件的大小(字节)
*
* 使用说明:
* char *pTarget;
* int num = ReadPE(target,&pTarget);
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
	
	memset(fileBuffer, 0, targetLen);

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
* int GetPEInfoFromFileBuf(const char* fileBuffer, struct _IMAGE_PE_INFO ** pToPEInfo)
* 成功返回0,失败返回1	
*
* 使用方式:
* struct _IMAGE_PE_HEADER_INFO* pToPEHeaderInfo = NULL;
* int ret = GetPEHeaderInfoFromFileBuf(fileBuffer,&pToPEHeaderInfo);
* if(ret != 0) {
* ...
* }
* free(pToPEHeaderInfo);
*/


int GetPEHeaderInfoFromFileBuf(const char* fileBuffer, struct _IMAGE_PE_HEADER_INFO ** pToPEHeaderInfo) {
	struct _IMAGE_PE_HEADER_INFO *info = (struct _IMAGE_PE_HEADER_INFO *)malloc(sizeof(struct _IMAGE_PE_HEADER_INFO));
	if(info == NULL) {
		if(__DEBUG__) {
			printf("cannot allocate space for _IMAGE_PE_HEADER_INFO\n");
		}
		return 1;
	}
	memset(info, 0, sizeof(struct _IMAGE_PE_HEADER_INFO));

	info->DosHeader = (struct _IMAGE_DOS_HEADER*)fileBuffer;
	info->Signature = *((int*)(fileBuffer + info->DosHeader->e_lfanew));
	info->PeHeader = (struct _IMAGE_FILE_HEADER*)(fileBuffer + info->DosHeader->e_lfanew + 4);
	info->OpPEHeader = (struct _IMAGE_OPTIONAL_HEADER*)((char *)info->PeHeader + 20);
	info->SectionHeader = (struct _IMAGE_SECTION_HEADER*)((char *)info->OpPEHeader + info->PeHeader->SizeOfOptionalHeader);

	*pToPEHeaderInfo = info;
	return 0;
}

/*
*  int GetImageBufFromFileBuf(const char* fileBuffer, char** imageBuffer)
*  成功返回0,失败则返回1
*  使用说明:
*  char *imageBuffer = NULL;
*  int ret = GetImageBufFromFileBuf(fileBuffer, &imageBuffer)
*  if (ret != 0) {
*      ...
*  }
*  free(imageBuffer)  
*/

int GetImageBufFromFileBuf(const char* fileBuffer, char** imageBuffer) {

	struct _IMAGE_PE_HEADER_INFO* pToPEHeaderInfo = NULL;
	int ret = GetPEHeaderInfoFromFileBuf(fileBuffer, &pToPEHeaderInfo);
	if(ret != 0) {
		return 1;
	}
	
	int sizeOfImage = pToPEHeaderInfo->OpPEHeader->SizeOfImage;
	const char* FileBuf = fileBuffer;
	char* ImageBuf = (char *)malloc(sizeOfImage);
	//申请内存，失败则返回1
	if(ImageBuf == NULL) {
		if(__DEBUG__) {
			printf("cannot allocate memory for ImageBuf,size:%d Bytes\n",sizeOfImage);
		}
		return 1;
	}
	memset(ImageBuf, 0, sizeOfImage);

	//拷贝头部(头部+节表)
	int sizeOfHeader = pToPEHeaderInfo->OpPEHeader->SizeOfHeaders;
	memcpy(ImageBuf, FileBuf, sizeOfHeader);
	
	//按顺序拷贝每个节
	int numOfSections = pToPEHeaderInfo->PeHeader->NumberOfSections;
	struct _IMAGE_SECTION_HEADER * firstHeader = pToPEHeaderInfo->SectionHeader;
	for(int i=0;i<numOfSections;i++) {
		memcpy(ImageBuf + firstHeader->VirtualAddress, FileBuf + firstHeader->PointerToRawData,firstHeader->SizeOfRawData);
		firstHeader ++;
	}
	*imageBuffer = ImageBuf;

	return 0;
}


/* 
* 
* int GetPEInfoFromImageBuf(const char* imageBuffer, struct _IMAGE_PE_INFO ** pToPEInfo)
* 成功返回0,失败返回1	
*
* 使用方式:
* struct _IMAGE_PE_HEADER_INFO* pToPEHeaderInfo = NULL;
* int ret = GetPEHeaderInfoFromImageBuf(imageBuffer,&pToPEHeaderInfo);
* if(ret != 0) {
* ...
* }
* free(pToPEHeaderInfo);
*/


int GetPEHeaderInfoFromImageBuf(const char* imageBuffer, struct _IMAGE_PE_HEADER_INFO ** pToPEHeaderInfo) {
	struct _IMAGE_PE_HEADER_INFO *info = (struct _IMAGE_PE_HEADER_INFO *)malloc(sizeof(struct _IMAGE_PE_HEADER_INFO));
	if(info == NULL) {
		if(__DEBUG__) {
			printf("cannot allocate space for _IMAGE_PE_HEADER_INFO\n");
		}
		return 1;
	}
	info->DosHeader = (struct _IMAGE_DOS_HEADER*)imageBuffer;
	info->Signature = *((int*)(imageBuffer + info->DosHeader->e_lfanew));
	info->PeHeader = (struct _IMAGE_FILE_HEADER*)(imageBuffer + info->DosHeader->e_lfanew + 4);
	info->OpPEHeader = (struct _IMAGE_OPTIONAL_HEADER*)((char *)info->PeHeader + 20);
	info->SectionHeader = (struct _IMAGE_SECTION_HEADER*)((char *)info->OpPEHeader + info->PeHeader->SizeOfOptionalHeader);

	*pToPEHeaderInfo = info;
	return 0;
}

/*
* int GetFileBufFromImageBuf(const char * imageBuffer, char ** fileBuffer) 
* 成功返回0,失败返回1
*
* 使用说明:
* char* fileBuffer = NULL;
* int ret = GetFileBufFromImageBuf(imageBuffer, &fileBuffer) 
* if(ret != 0) {
*     ...
* }
* free(fileBuffer)
*
*/
int GetFileBufFromImageBuf(const char * imageBuffer, char ** fileBuffer) {
	struct _IMAGE_PE_HEADER_INFO *info = NULL;
	int ret = GetPEHeaderInfoFromImageBuf(imageBuffer, &info);
	if(ret != 0) {
		if(__DEBUG__) {
			printf("GetPEHeaderInfoFromImageBuf return 1\n");
		}
		return 1;
	}

	struct _IMAGE_SECTION_HEADER *endHeader = info->SectionHeader + info->PeHeader->NumberOfSections - 1;
	char* endAddress = (char*)endHeader->PointerToRawData + endHeader->SizeOfRawData;
	if(__DEBUG__) {
		printf("beginning allocate memory for FileBuf,size:%d Bytes\n",(int)endAddress);
	}

	char* FileBuf = (char*)malloc((int)endAddress);
	//申请内存，失败则返回1
	if(FileBuf == NULL) {
		if(__DEBUG__) {
			printf("cannot allocate memory for FileBuf,size:%d Bytes\n",(int)endAddress);
		}
		return 1;
	}
	memset(FileBuf, 0, (int)endAddress);
	if(__DEBUG__) {
		printf("allocate memory for FileBuf, size:%d Bytes\n", (int)endAddress);
	}

	//拷贝头部(头部+节表)
	int sizeOfHeader = info->OpPEHeader->SizeOfHeaders;
	memcpy(FileBuf, imageBuffer, sizeOfHeader);
	
	//按顺序拷贝每个节
	int numOfSections = info->PeHeader->NumberOfSections;
	struct _IMAGE_SECTION_HEADER * firstHeader = info->SectionHeader;
	for(int i=0;i<numOfSections;i++) {
		printf("copying header %s: Addr:%x, total %x Bytes\n", firstHeader->Name, firstHeader->PointerToRawData, firstHeader->SizeOfRawData);
		memcpy(FileBuf + firstHeader->PointerToRawData, imageBuffer + firstHeader->VirtualAddress, firstHeader->SizeOfRawData);
		firstHeader ++;
	}
	*fileBuffer = FileBuf;
	return 0;
}

/*
* int SaveFileFromBuf(const char *fileName, const char* buffer, long size)
* 成功返回0,失败返回1
*
* 使用说明:
* char* fileName = "C:/123.txt"
* int ret = SaveFileFromBuf(filename, buffer, buffer_size) 
* if(ret != 0) {
*     ...
* }
*
*/

int SaveFileFromBuf(const char *fileName, const char* buffer, long size) {
	FILE* fileHandler = fopen(fileName,"wb");
	if(fileHandler == NULL) {
		if(__DEBUG__) {
			printf("can not write file %s\n", fileName);
		}
		return 1;
	}

	int ret = fwrite(buffer, 1, size, fileHandler);
	if(ret != size) {
		if(__DEBUG__) {
			printf("write file %s incomplete, expected written Bytes:%d; actual written Bytes:%d\n", fileName, size, ret);
		}
		fclose(fileHandler);
		return 1;
	}
	
	fclose(fileHandler);
	return 0;
}
