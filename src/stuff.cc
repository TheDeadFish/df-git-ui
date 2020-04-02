#include <stdshit.h>
#include "stuff.h"

FileStrRead::FileStrRead(FILE* fp_, int size) : fp(fp_)
{
	this->fp = fp;
	if(setvbuf(fp, 0, _IOFBF, size))
		errorAlloc();		
}

int FileStrRead::error()
{
	IFRET(ferror(fp));
	return feof(fp) ? 0 : -1;
}

void FileStrRead::next()
{
	FILE* fp = this->fp;
	fp->_base = memcpyX(fp->_base, fp->_ptr, fp->_cnt);
	int cnt = fp->_cnt; fp->_bufsiz -= cnt;
	int ch = _filbuf(fp); fp->_base -= cnt; fp->_bufsiz += cnt;
	if(ch >= 0) cnt++; fp->_ptr -= cnt; fp->_cnt += cnt;
}

char* FileStrRead::get_(char x)
{
	int cnt = fp->_cnt; char* ptr = fp->_ptr;
	while(--cnt >= 0) { char ch = *ptr; ptr++;
		if(ch==x) { ptr[-1] = 0; fp->_cnt = cnt;
			return release(fp->_ptr, ptr); }
	} return NULL;
}

char* FileStrRead::get()
{
	IFRET(get_(0));
	next(); 
	return get_(0);
}

char* FileStrRead::getLine()
{
	IFRET(get_('\n'));
	next(); 
	return get_('\n');	
}

SHITCALL cstr getCurrentDirectory(void)
{
	WCHAR buff[MAX_PATH]; buff[0] = 0;
	GetCurrentDirectory(MAX_PATH, buff);
	return utf816_dup(buff);
}
