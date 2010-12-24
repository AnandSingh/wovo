#include "osal_types.h"
#include "osal_list.h"
//#include "scm_info.h"
#include "osal_code_page.h"


#define CODEPAGE (CP_ACP)


int osal_MultiByteToWideChar(char *pMultiByteStr, int cbMultiByte, wchar_t *pWideCharStr, int cchWideChar)
{
	unsigned short *pUnicodeTable=NULL;
	
	unsigned char FirstByte=pMultiByteStr[0];
	int Offset=0;
	int i;
	
	if( (0x20<=FirstByte)  )
	{
		int j=0;
		pUnicodeTable=(UINT16 *)CODEPAGE_ISOIEC_6937;
		if( cchWideChar )
		{
			for(i=Offset; i<cbMultiByte && j<cchWideChar; ++i)
			{
				switch( pUnicodeTable[(unsigned char)pMultiByteStr[i]] )
				{
				case 0x0000:
					break;
				case 0x0300:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C1[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0301:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C2[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0302:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C3[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0303:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C4[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0304:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C5[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0306:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C6[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0307:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C7[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0308:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_C8[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x030A:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_CA[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0327:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_CB[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x030B:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_CD[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x0328:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_CE[(unsigned char)pMultiByteStr[++i]];
					break;
				case 0x030C:
					pWideCharStr[j++]=CODEPAGE_ISOIEC_6937_CF[(unsigned char)pMultiByteStr[++i]];
					break;
				default:
					pWideCharStr[j++]=pUnicodeTable[(unsigned char)pMultiByteStr[i]];
					break;
				}
			}
		}
		else
		{
			for(i=Offset; i<cbMultiByte; ++i)
			{
				switch( pUnicodeTable[(unsigned char)pMultiByteStr[i]] )
				{
				case 0x0000:
					break;
				case 0x0300:
				case 0x0301:
				case 0x0302:
				case 0x0303:
				case 0x0304:
				case 0x0306:
				case 0x0307:
				case 0x0308:
				case 0x030A:
				case 0x0327:
				case 0x030B:
				case 0x0328:
				case 0x030C:
					++i;
				default:
					++j;
					break;
				}
			}
		}
		return j;
	}
	else if( (0x15<=FirstByte) && (0x20>FirstByte) ) // reserved 
	{
		return MultiByteToWideChar(CODEPAGE, 0, &pMultiByteStr[1], cbMultiByte-1, pWideCharStr, cchWideChar);
	}
	else if( (0x00<FirstByte) && (0x10>FirstByte) )
	{
		switch(FirstByte)
		{
		case 0x01:  // ISO/IEC 8859-5 (Latin/Cyrillic alphabet)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_5;
			break;

		case 0x02:  // ISO/IEC 8859-6 (Latin/Arabic alphabet)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_6;
			break;

		case 0x03:  // ISO/IEC 8859-7 (Latin/Greek alphabet)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_7;
			break;

		case 0x04:  // ISO/IEC 8859-8 (Latin/Hebrew alphabet)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_8;
			break;

		case 0x05:  // ISO/IEC 8859-9 (Latin alphabet No.5)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_9;
			break;

		case 0x06:  // ISO/IEC 8859-10 (Latin alphabet No.6)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_10;
			break;

		case 0x07:  // ISO/IEC 8859-11 (Latin/Thai)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_11;
			break;

		case 0x09:  // ISO/IEC 8859-13 (Latin alphabet No.7)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_13;
			break;

		case 0x0a:  // ISO/IEC 8859-14 (Latin alphabet No.8 (Celtic))
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_14;
			break;

		case 0x0b:  // ISO/IEC 8859-15 (Latin alphabet No.9)
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_15;
			break;

		case 0x08:  // ISO/IEC 8859-12 (reserved for Indian)
		case 0x0c:  // reserved
		case 0x0d:
		case 0x0e:
		case 0x0f:
			return MultiByteToWideChar(CODEPAGE, 0, &pMultiByteStr[1], cbMultiByte-1, pWideCharStr, cchWideChar);
		}

		Offset=1;
	}
	else if( 0x10==FirstByte ) // ISO Standard 8859 part 1~9
	{
		unsigned short Number=(((unsigned short)pMultiByteStr[1])<<8)|(unsigned short)pMultiByteStr[2];
		switch( Number ) 
		{
		case 0x01:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_1;
			break;
		case 0x02:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_2;
			break;
		case 0x03:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_3;
			break;
		case 0x04:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_4;
			break;
		case 0x05:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_5;
			break;
		case 0x06:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_6;
			break;
		case 0x07:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_7;
			break;
		case 0x09:
			pUnicodeTable=(UINT16 *)CODEPAGE_ISO8859_9;
			break;

		case 0x08:
		default:
			return MultiByteToWideChar(CODEPAGE, 0, &pMultiByteStr[3], cbMultiByte-3, pWideCharStr, cchWideChar);
		}

		Offset=3;
	}
	else if( (0x11==FirstByte) ||    // Basic Multilingual Plane of ISO/IEC 10646-1
		(0x12==FirstByte) ||    // Korean Character Set KSC5601-1987
		(0x13==FirstByte) ||    // Simplified Chinese Character Set GB-2312-1980
		(0x14==FirstByte)) // Big5 subset of ISO/IEC 10646-1 for use with Tranditional Chinese
	{    
		int j=0;
		if( cchWideChar )
		{
			for(i=1; i<cbMultiByte && j<cchWideChar; i+=2) // swap
			{
				pWideCharStr[j]=(((unsigned char)pMultiByteStr[i])<<8)|((unsigned char)pMultiByteStr[i+1]);
				if( 0xe08a==pWideCharStr[j] ) // NWP
					pWideCharStr[j]=0x20;
				++j;
			}
		}
		else
		{
			for(i=1; i<cbMultiByte; i+=2) // count
			{
				++j;
			}
		}
		return j;
	}
	else
	{
		return 0;
	}

	if( pUnicodeTable )
	{
		int j=0;
		if( cchWideChar )
		{
			for(i=Offset; i<cbMultiByte && j<cchWideChar; ++i) 
			{
				if( pUnicodeTable[(unsigned char)pMultiByteStr[i]] ){
					pWideCharStr[j]=pUnicodeTable[(unsigned char)pMultiByteStr[i]];
					++j;
				}
			}
		}
		else
		{
			for(i=Offset; i<cbMultiByte; ++i)
			{
				if( pUnicodeTable[(unsigned char)pMultiByteStr[i]] ){
					++j;
				}
			}
		}
		return j;
	}

	return 0;
}