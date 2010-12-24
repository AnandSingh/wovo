/* ----------------------------------------- Header File Inclusion */
#include <windows.h>
#include "osal_types.h"
#include "osal_file.h"









/* ----------------------------------------- Functions */

int osal_saveLog(char * pBuffer)
{
   EvFile* hOurOutputDebugFile;
   dword_t dwBufferLength;
//   dword_t dwNumberOfBytesWritten;

   hOurOutputDebugFile = Evfopen(".\scm_trace_log.txt", /*EV_FILE_ACCESS_READWRITE*/"a+");

  // Evfseek (hOurOutputDebugFile, 0, EV_FILE_SEEK_CURRENT);
 //  SetFilePointer( hOurOutputDebugFile, 0, NULL, FILE_END );

   if ( (!pBuffer) | (hOurOutputDebugFile == NULL) )
   {
      return 0;
   }

   dwBufferLength = (dword_t)strlen(pBuffer);

   if ( !Evfwrite(pBuffer,1,dwBufferLength,hOurOutputDebugFile))
   {
	     return 0;
   }
   Evfclose( hOurOutputDebugFile );

   return 1;
}


void osal_printLogFile( char *format, ... ) 
{
     char TempBuffer[1024];
     va_list ap;
     va_start( ap, format );
     TempBuffer[0] = 0;
     vsprintf( TempBuffer, format, ap );
     osal_saveLog( TempBuffer );
     va_end(ap);

}

void osal_print( char *format, ... )
{
   char TempBuffer[4096];
   int Size = 512;
   //WCHAR buffer[512];
   char buffer[512];
   char *wUnicode = buffer;
   va_list ap;
   va_start( ap, format );
   TempBuffer[0] = 0;
   _cprintf( TempBuffer, format, ap );

 
   //MultiByteToWideChar(CP_ACP, 0, TempBuffer, -1, wUnicode, 512);

   //NKDbgPrintfW(wUnicode);

   va_end(ap);
}
