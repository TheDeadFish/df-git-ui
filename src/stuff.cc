#include <stdshit.h>
#include "stuff.h"
#include <time.h>

SHITCALL DWORD winPopen(HANDLE* hReadPipe, cch* dir, cch* cmd)
{
	// create the pipe	
	HANDLE hWritePipe;
	SECURITY_ATTRIBUTES sa = {sizeof(sa), 0, TRUE}; 
	if(!CreatePipe(hReadPipe, &hWritePipe, &sa, 0))
		return GetLastError();
		
	// create the process
	STARTUPINFOA si = {sizeof(si)};
	si.hStdOutput = hWritePipe;
	si.dwFlags = STARTF_USESTDHANDLES;
	PROCESS_INFORMATION pi;
	BOOL result = createProcess(NULL, cmd, NULL, 
		NULL, TRUE, 0, NULL, dir, &si, &pi);
		
	// cleanup & error handling
	CloseHandle(hWritePipe);
	if(!result) { CloseHandle(*hReadPipe);
		return GetLastError(); }
	CloseHandle(pi.hProcess); return 0;
}

FileStrRead::FileStrRead(int size)
{
	hFile = NULL;
	base = xmalloc(size); limit = base+size;
	pos = base; end = base; errCode = 0;
}

int FileStrRead::error()
{
	IFRET(errCode);
	if(end-pos) return -1;
	return 0;
}

void FileStrRead::next()
{
	// shuffle remainder
	DWORD size = end-pos;
	memcpy(base, pos, size);
	pos = base; end = pos+size;
	
	// read from file
	for(;size = limit-end; end += size) {
		if(!ReadFile(hFile, end, size, &size, 0)) {
			errCode = GetLastError();
			if(errCode == ERROR_HANDLE_EOF) errCode = 0;
			if(errCode == ERROR_BROKEN_PIPE) errCode = 0;
			break;
		}
	}
}

char* FileStrRead::get_(char x)
{
	char* ptr = this->pos;
	while(ptr < end) { char ch = *ptr; ptr++;
		if(ch==x) { ptr[-1] = 0; 
			return release(this->pos, ptr); }
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


int fmtTime(char* str, u32 time)
{
	time_t pt = time;
	return strftime(str, 80, 
		"%Y-%m-%d %H:%M", localtime(&pt));
}
