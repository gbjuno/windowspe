# PE�ļ���ȡ

###PEͷ����Ϣ
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
1.����:�����ļ����ڴ浱��
2.״̬:ʧ�ܷ���0���ɹ��򷵻ض�ȡ�ļ��Ĵ�С(�ֽ�)
```
char *pTarget;
int num = ReadPE(target,&pTarget);
...
free(*pTarget);
```

###int GetPEHeaderInfoFromBuf(const char* buffer, struct _IMAGE_PE_INFO *pToPEInfo)
1.����:�����ļ������ȡPEͷ����Ϣ
2.״̬:�ɹ�����0,ʧ�ܷ���1	
```
struct _IMAGE_PE_HEADER_INFO *pToPEHeaderInfo = NULL;
int ret = GetPEHeaderInfoFromBuf(buffer,&pToPEHeaderInfo);
if(ret != 0) {
   ...
}
free(pToPEHeaderInfo);
```

###int GetFileBufFromImageBuf(const char imageBuffer, char *fileBuffer) 
1.����:�����ڴ澵���ȡ�ļ�����
2.״̬:�ɹ�����0,ʧ�ܷ���1
```
char *fileBuffer = NULL;
int ret = GetFileBufFromImageBuf(imageBuffer, &fileBuffer) 
if(ret != 0) {
    ...
}
free(fileBuffer)
```

###int SaveFileFromBuf(const char *fileName, const charbuffer, long size)
1.����:�����ļ����ڴ���
2.״̬:�ɹ�����0,ʧ�ܷ���1
```
char *fileName = "C:/123.txt"
int ret = SaveFileFromBuf(filename, buffer, buffer_size) 
if(ret != 0) {
    ...
}
```

