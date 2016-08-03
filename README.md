# PE文件读取

###PE头部信息
```
struct _IMAGE_PE_HEADER_INFO {
	struct _IMAGE_DOS_HEADER *DosHeader;           //pointer To DOSHeader
	int Signature;                                 //Signature
	struct _IMAGE_FILE_HEADER *PeHeader;           //pointer To PEHeader
	struct _IMAGE_OPTIONAL_HEADER *OpPEHeader;     //pointer To OptionPEHeader
	struct _IMAGE_SECTION_HEADER *SectionHeader;   //pointer To First Secion Header
};
```

###int ReadPE(const char* target, char** pTargetFileBuffer)
1.作用:拷贝文件到内存当中
2.状态:失败返回0，成功则返回读取文件的大小(字节)
```
char *pTarget;
int num = ReadPE(target,&pTarget);
...
free(*pTarget);
```

###int GetPEHeaderInfoFromBuf(const char* buffer, struct _IMAGE_PE_INFO *pToPEInfo)
1.作用:根据文件镜像获取PE头部信息
2.状态:成功返回0,失败返回1	
```
struct _IMAGE_PE_HEADER_INFO *pToPEHeaderInfo = NULL;
int ret = GetPEHeaderInfoFromBuf(buffer,&pToPEHeaderInfo);
if(ret != 0) {
   ...
}
free(pToPEHeaderInfo);
```

###int GetFileBufFromImageBuf(const char imageBuffer, char *fileBuffer) 
1.作用:根据内存镜像获取文件镜像
2.状态:成功返回0,失败返回1
```
char *fileBuffer = NULL;
int ret = GetFileBufFromImageBuf(imageBuffer, &fileBuffer) 
if(ret != 0) {
    ...
}
free(fileBuffer)
```

###int SaveFileFromBuf(const char *fileName, const charbuffer, long size)
1.作用:保存文件至内存中
2.状态:成功返回0,失败返回1
```
char *fileName = "C:/123.txt"
int ret = SaveFileFromBuf(filename, buffer, buffer_size) 
if(ret != 0) {
    ...
}
```

