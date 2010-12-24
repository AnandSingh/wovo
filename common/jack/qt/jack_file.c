#include <stdarg.h> 

#include "osal_file.h"
#include "osal_stdio.h"

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>

#include "osal_string.h"



/*---------------------------------------------------------------------------*/

EvFile* Evfopen(const char * filename, const char * mode)
{
	return (EvFile*)fopen(filename, mode);
}

/*---------------------------------------------------------------------------*/

size_t Evfread(void * buffer, size_t size, size_t count, EvFile * stream)
{
	return fread(buffer, size, count, (FILE*)stream);
}

/*---------------------------------------------------------------------------*/

size_t Evfwrite(const void * buffer, size_t size, size_t count, EvFile * stream)
{
	return fwrite(buffer, size, count, (FILE*)stream);
}

/*---------------------------------------------------------------------------*/

int Evfclose(EvFile * stream)
{
	return fclose((FILE*)stream);
}

/*---------------------------------------------------------------------------*/



int Evmkdir(const char * dirname)
{
	return _mkdir(dirname);
}

/*---------------------------------------------------------------------------*/
long int Evftell(EvFile *file)
{
	return ftell((FILE*)file);
}

/*---------------------------------------------------------------------------*/

int Evfseek (EvFile *stream, long int offset, int whence)
{
	long seek_type;

	if(whence==EV_FILE_SEEK_SET)
		seek_type = SEEK_SET;
	else if(whence==EV_FILE_SEEK_CURRENT)
		seek_type = SEEK_CUR;
	else if(whence==EV_FILE_SEEK_EOF)
		seek_type = SEEK_END;
	else
		return -1;

	return fseek((FILE*)stream, offset, whence);
}

/*---------------------------------------------------------------------------*/

int Evvfprintf(EvFile *stream, const char *format, va_list ptr)
{
	if(stream==EV_stderr || stream==EV_stdout)
		return Evvprintf(format, ptr);

	return vfprintf((FILE*)stream, format, ptr);
}

/*---------------------------------------------------------------------------*/

int Evfprintf(EvFile *stream, const char *format, ...)
{
	int nRes = -1;

	va_list ptr;
	va_start(ptr, format);

	nRes = Evvfprintf(stream, format, ptr);

	va_end(ptr);
	
	return nRes;
}

/*---------------------------------------------------------------------------*/

int	EvRemoveFile(const char * szPath)
{
	int nRes = -1;

	nRes = DeleteFile((LPCWSTR)szPath)?0:-1;
	return nRes;
}

/*---------------------------------------------------------------------------*/

int	EvRemoveDirectory(const char * szPath)
{
	int nRes = -1;

	nRes = RemoveDirectory((LPCWSTR)szPath)?0:-1;

	return nRes;
}

/*---------------------------------------------------------------------------*/
int Evfflush(EvFile *stream)
{
	if(stream == EV_stdout)
		return fflush(stdout);
	else if(stream == EV_stderr)
		return fflush(stderr);
	else
		return fflush((FILE*)stream);
}

/*---------------------------------------------------------------------------*/
int Evfputc(int c, EvFile *stream)
{
	if(stream == EV_stdout)
		return fputc(c, stdout);
	else if(stream == EV_stderr)
		return fputc(c, stderr);
	else
		return fputc(c, (FILE*)stream);
}

/*---------------------------------------------------------------------------*/
void Evclearerr(EvFile *stream)
{
	if(stream == EV_stdout)
		clearerr(stdout);
	else if(stream == EV_stderr)
		clearerr(stderr);
	else
		clearerr((FILE*)stream);
}
/*---------------------------------------------------------------------------*/
int Evferror(EvFile *stream)
{
	if(stream == EV_stdout)
		return ferror(stdout);
	else if(stream == EV_stderr)
		return ferror(stderr);
	else
		return ferror((FILE*)stream);
}
/*---------------------------------------------------------------------------*/
int Evfaccess(const char *filename, Evfile_access_type mode)
{
	int wmode = 0;



	if(mode & EV_FILE_ACCESS_READ)

		wmode |= 4; /* no defined constant, see the doc on msdn */

	if(mode & EV_FILE_ACCESS_WRITE)

		wmode |= 2; /* no defined constant, see the doc on msdn */



	return _access_s(filename, wmode);

}

EvFile* Evflock(const char *filename, Evfile_access_type mode)
{
	int wmode = 0;


	if(mode & EV_FILE_ACCESS_READ)
		wmode |= FILE_SHARE_READ;

	if(mode & EV_FILE_ACCESS_WRITE)
		wmode |= FILE_SHARE_WRITE;

	return (EvFile*)CreateFile((LPCWSTR)filename,
			GENERIC_WRITE | GENERIC_READ,
			wmode, /* Do not share. */
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL
			);

}

int Evfunlock(EvFile *file)
{
	return CloseHandle((HANDLE)file);
}


int EvGetCurrentDirectory(int buffer_size, char *buffer)
{

	if(GetCurrentDirectory(buffer_size, (LPWSTR)buffer) > 0)
		return 0;
	else
		return -1;
}