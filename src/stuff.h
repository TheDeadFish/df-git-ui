#pragma once

#define DEC_CHAR(ch, ...) ({ u32 x = ch-'0'; \
	if(x >= 10) { __VA_ARGS__; }  x; })
	
#define FOR_FIH(s,h,r,i, ...) { auto* p = s.begin(); \
	for(size_t i = h; i < s.size(); i++) { auto& r = p[i]; __VA_ARGS__; } \
	for(size_t i = 0; i < h; i++) { auto& r = p[i]; __VA_ARGS__; }}
	
TMPL2(T, U) SHITCALL 
void add_uniqueP(T& xa, U* cmt)
{
	for(auto* x : xa) 
		if(*x == *cmt) return;
	xa.push_back(cmt);
}

SHITCALL DWORD winPopen(HANDLE* hReadPipe, cch* dir, cch* cmd);

struct FileStrRead 
{
	FileStrRead(int size=65536);	
	DWORD popen(cch* dir, cch* cmd) {
		return winPopen(&hFile, dir, cmd); }
	
	~FileStrRead() { CloseHandle(hFile); }
	char* get();
	char* getLine();
	int error();
	
private:
	HANDLE hFile; 
	char *base, *limit;
	char *pos, *end;
	DWORD errCode;

	char* get_(char ch);
	void next();
};


TMPL(T)
static T* skip_space(T* str) {
	if(str) while(*str == ' ') str++;
	return str; }

SHITCALL cstr getCurrentDirectory(void);

SHITCALL int fmtTime(char* str, u32 time);

