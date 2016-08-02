#ifndef __PE_H__
#define __PE_H__

#define IMAGE_SIZEOF_SHORT_NAME 8

struct _IMAGE_DOS_HEADER {
    short e_magic;
    short e_cblp;
    short e_cp;
    short e_crlc;
    short e_cparhdr;
    short e_minalloc;
    short e_maxalloc;
    short e_ss;
    short e_sp;
    short e_csum;
    short e_ip;
    short e_cs;
    short e_lfarlc;
    short e_ovno;
    short e_res[4];
    short e_oemid;
    short e_oeminfo;
    short e_res2[10];
    int e_lfanew;
};


struct _IMAGE_DATA_DIRECTORY{
    int VirtualAddress;
    int Size;
};

struct _IMAGE_FILE_HEADER {
    short Machine;
    short NumberOfSections;
    int TimeDateStamp;
    int PointerToSymbolTable;
    int NumberOfSymbols;
    short SizeOfOptionalHeader;
    short Characteristics;
};

struct _IMAGE_OPTIONAL_HEADER {
    short Magic;
    char MajorLinkerVersion;
    char MinorLinkerVersion;
    int SizeOfCode;
    int SizeOfInitializedData;
    int SizeOfUninitializedData;
    int AddressOfEntryPoint;
    int BaseOfCode;
    int BaseOfData;
    int ImageBase;
    int SectionAlignment;
    int FileAlignment;
    short MajorOperatingSystemVersion;
    short MinorOperatingSystemVersion;
    short MajorImageVersion;
    short MinorImageVersion;
    short MajorSubsystemVersion;
    short MinorSubsystemVersion;
    int Win32VersionValue;
    int SizeOfImage;
    int SizeOfHeaders;
    int CheckSum;
    short Subsystem;
    short DllCharacteristics;
    int SizeOfStackReserve;
    int SizeOfStackCommit;
    int SizeOfHeapReserve;
    int SizeOfHeapCommit;
    int LoaderFlags;
    int NumberOfRvaAndSizes;
    struct _IMAGE_DATA_DIRECTORY DataDirectory[16];
};

struct _IMAGE_NT_HEADERS {
    int Signature;
    struct _IMAGE_FILE_HEADER FileHeader;
    struct _IMAGE_OPTIONAL_HEADER OptionalHeader;
};

struct _IMAGE_SECTION_HEADER {
    char Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        int PhysicalAddress;
        int VirtualSize;
    } Misc;
    int VirtualAddress;
    int SizeOfRawData;
    int PointerToRawData;
    int PointerToRelocations;
    int PointerToLinenumbers;
    short NumberOfRelocations;
    short NumberOfLinenumbers;
    int Characteristics;
};

#endif