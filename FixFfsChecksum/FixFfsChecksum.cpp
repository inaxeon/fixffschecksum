// FixFfsChecksum.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <stdio.h>

#include "ffs.h"

UINT8 CalculateSum8(
	IN      CONST UINT8              *Buffer,
	IN      UINT                     Length
)
{
	UINT8     Sum;
	UINT     Count;


	for (Sum = 0, Count = 0; Count < Length; Count++) {
		Sum = (UINT8)(Sum + *(Buffer + Count));
	}

	return Sum;
}

UINT8 CalculateCheckSum8(
	IN      CONST UINT8              *Buffer,
	IN      UINT                     Length
)
{
	UINT8     CheckSum;

	CheckSum = CalculateSum8(Buffer, Length);

	//
	// Return the checksum based on 2's complement.
	//
	return (UINT8)(0x100 - CheckSum);
}

int main(int argc, char *argv[])
{
	BYTE oldHeaderState;
	BYTE* payloadBuffer;
	FILE *fileHandle;
	int payloadSize;
	int recordsRead;
	EFI_FFS_FILE_HEADER ffsHeader;
	errno_t openResult;

	if (argc < 2)
	{
		fprintf(stderr, "Error: No input file specified\r\n");
		return 1;
	}

	openResult = fopen_s(&fileHandle, argv[1], "rb");

	if (openResult)
	{
		fprintf(stderr, "Error: Failed to open input file specified\r\n");
		return 1;
	}

	fseek(fileHandle, 0, SEEK_SET);

	recordsRead = fread(&ffsHeader, sizeof(EFI_FFS_FILE_HEADER), 1, fileHandle);

	if (!recordsRead)
	{
		fprintf(stderr, "Error: Failed to read FFS header\r\n");
		return 1;
	}

	if (ffsHeader.Attributes & FFS_ATTRIB_LARGE_FILE)
	{
		fprintf(stderr, "Error: FFS_ATTRIB_LARGE_FILE not supported\r\n");
		return 1;
	}
	
	payloadSize = ffsHeader.Size[2] << 16;
	payloadSize |= ffsHeader.Size[1] << 8;
	payloadSize |= ffsHeader.Size[0] & 0xFF;
	payloadSize -= sizeof(EFI_FFS_FILE_HEADER);
	
	payloadBuffer = (BYTE*)malloc(payloadSize);
	recordsRead = fread(payloadBuffer, payloadSize, 1, fileHandle);

	if (!recordsRead)
	{
		fprintf(stderr, "Error: Failed to read payload\r\n");
		return 1;
	}

	fclose(fileHandle);

	ffsHeader.IntegrityCheck.Checksum.File = 0;
	ffsHeader.IntegrityCheck.Checksum.Header = 0;
	oldHeaderState = ffsHeader.State;
	ffsHeader.State = 0;

	ffsHeader.IntegrityCheck.Checksum.Header = CalculateCheckSum8(
		(UINT8 *)&ffsHeader,
		sizeof(EFI_FFS_FILE_HEADER)
	);

	if (ffsHeader.Attributes & FFS_ATTRIB_CHECKSUM)
	{
		ffsHeader.IntegrityCheck.Checksum.File = CalculateCheckSum8(
			payloadBuffer,
			payloadSize
		);
	}
	else
	{
		ffsHeader.IntegrityCheck.Checksum.File = FFS_FIXED_CHECKSUM;
	}

	ffsHeader.State = oldHeaderState;

	openResult = fopen_s(&fileHandle, argv[1], "wb");

	if (openResult)
	{
		fprintf(stderr, "Error: Failed to open file for writing\r\n");
		return 1;
	}

	fwrite(&ffsHeader, sizeof(EFI_FFS_FILE_HEADER), 1, fileHandle);
	fwrite(payloadBuffer, payloadSize, 1, fileHandle);

	fclose(fileHandle);
	free(payloadBuffer);

	printf("Successfully updated checksums");

	return 0;
}
