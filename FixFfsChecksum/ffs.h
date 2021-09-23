#pragma once

typedef GUID                      EFI_GUID;
typedef UINT8 EFI_FV_FILETYPE;
typedef UINT8 EFI_FFS_FILE_ATTRIBUTES;
typedef UINT8 EFI_FFS_FILE_STATE;

#define FFS_FIXED_CHECKSUM				0xAA

#define FFS_ATTRIB_LARGE_FILE			0x01

#define FFS_ATTRIB_TAIL_PRESENT			0x01
#define FFS_ATTRIB_RECOVERY				0x02
#define FFS_ATTRIB_FIXED				0x04
#define FFS_ATTRIB_DATA_ALIGNMENT		0x38
#define FFS_ATTRIB_CHECKSUM				0x40

#define EFI_FILE_HEADER_CONSTRUCTION	0x01
#define EFI_FILE_HEADER_VALID			0x02
#define EFI_FILE_DATA_VALID				0x04
#define EFI_FILE_MARKED_FOR_UPDATE		0x08
#define EFI_FILE_DELETED				0x10
#define EFI_FILE_HEADER_INVALID			0x20

#define EFI_FILE_ALL_STATE_BITS       (EFI_FILE_HEADER_CONSTRUCTION | \
                                 EFI_FILE_HEADER_VALID | \
                                 EFI_FILE_DATA_VALID | \
                                 EFI_FILE_MARKED_FOR_UPDATE | \
                                 EFI_FILE_DELETED | \
                                 EFI_FILE_HEADER_INVALID \
                                  )

///
/// Used to verify the integrity of the file.
///
typedef union {
	struct {
		///
		/// The IntegrityCheck.Checksum.Header field is an 8-bit checksum of the file
		/// header. The State and IntegrityCheck.Checksum.File fields are assumed
		/// to be zero and the checksum is calculated such that the entire header sums to zero.
		///
		UINT8   Header;
		///
		/// If the FFS_ATTRIB_CHECKSUM (see definition below) bit of the Attributes
		/// field is set to one, the IntegrityCheck.Checksum.File field is an 8-bit
		/// checksum of the file data.
		/// If the FFS_ATTRIB_CHECKSUM bit of the Attributes field is cleared to zero,
		/// the IntegrityCheck.Checksum.File field must be initialized with a value of
		/// 0xAA. The IntegrityCheck.Checksum.File field is valid any time the
		/// EFI_FILE_DATA_VALID bit is set in the State field.
		///
		UINT8   File;
	} Checksum;
	///
	/// This is the full 16 bits of the IntegrityCheck field.
	///
	UINT16    Checksum16;
} EFI_FFS_INTEGRITY_CHECK;

///
/// Each file begins with the header that describe the
/// contents and state of the files.
///
typedef struct {
	///
	/// This GUID is the file name. It is used to uniquely identify the file.
	///
	EFI_GUID                Name;
	///
	/// Used to verify the integrity of the file.
	///
	EFI_FFS_INTEGRITY_CHECK IntegrityCheck;
	///
	/// Identifies the type of file.
	///
	EFI_FV_FILETYPE         Type;
	///
	/// Declares various file attribute bits.
	///
	EFI_FFS_FILE_ATTRIBUTES Attributes;
	///
	/// The length of the file in bytes, including the FFS header.
	///
	UINT8                   Size[3];
	///
	/// Used to track the state of the file throughout the life of the file from creation to deletion.
	///
	EFI_FFS_FILE_STATE      State;
} EFI_FFS_FILE_HEADER;

typedef struct {
	///
	/// This GUID is the file name. It is used to uniquely identify the file. There may be only
	/// one instance of a file with the file name GUID of Name in any given firmware
	/// volume, except if the file type is EFI_FV_FILETYPE_FFS_PAD.
	///
	EFI_GUID                  Name;

	///
	/// Used to verify the integrity of the file.
	///
	EFI_FFS_INTEGRITY_CHECK   IntegrityCheck;

	///
	/// Identifies the type of file.
	///
	EFI_FV_FILETYPE           Type;

	///
	/// Declares various file attribute bits.
	///
	EFI_FFS_FILE_ATTRIBUTES   Attributes;

	///
	/// The length of the file in bytes, including the FFS header.
	/// The length of the file data is either (Size - sizeof(EFI_FFS_FILE_HEADER)). This calculation means a
	/// zero-length file has a Size of 24 bytes, which is sizeof(EFI_FFS_FILE_HEADER).
	/// Size is not required to be a multiple of 8 bytes. Given a file F, the next file header is
	/// located at the next 8-byte aligned firmware volume offset following the last byte of the file F.
	///
	UINT8                     Size[3];

	///
	/// Used to track the state of the file throughout the life of the file from creation to deletion.
	///
	EFI_FFS_FILE_STATE        State;

	///
	/// If FFS_ATTRIB_LARGE_FILE is set in Attributes, then ExtendedSize exists and Size must be set to zero.
	/// If FFS_ATTRIB_LARGE_FILE is not set then EFI_FFS_FILE_HEADER is used.
	///
	UINT64                    ExtendedSize;
} EFI_FFS_FILE_HEADER2;
