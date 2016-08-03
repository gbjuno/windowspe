#ifndef __ReadPE_H__
#define __ReadPE_H__


struct _IMAGE_PE_HEADER_INFO {
	struct _IMAGE_DOS_HEADER *DosHeader;
	int Signature;
	struct _IMAGE_FILE_HEADER *PeHeader; 
	struct _IMAGE_OPTIONAL_HEADER *OpPEHeader;
	struct _IMAGE_SECTION_HEADER *SectionHeader;
};

int GetPEHeaderInfoFromBuf(const char* fileBuffer, struct _IMAGE_PE_HEADER_INFO ** pToPEHeaderInfo);

int ReadPE(const char* target, char** pTargetFileBuffer);

int GetImageBufFromFileBuf(const char* fileBuffer, char** imageBuffer);

int GetFileBufFromImageBuf(const char * imageBuffer, char ** fileBuffer);

int SaveFileFromBuf(const char *fileName, const char* buffer, long size);
#endif
