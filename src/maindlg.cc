#include <stdshit.h>
#include <win32hlp.h>
#include "resource.h"
#include "git-info.h"
#include "stuff.h"

const char progName[] = "df-git-ui";

GitInfo g_gitInfo;

void repo_reset(HWND hwnd)
{
	g_gitInfo.free();
	setDlgItemText(hwnd, IDC_PATH, "");
	dlgCombo_reset(hwnd, IDC_BRA);
	dlgCombo_reset(hwnd, IDC_CMT);
}

void select_commit(HWND hwnd)
{



}


void select_branch(HWND hwnd)
{
	GitInfo::Commit* prevCmt = Void(
		dlgCombo_getData(hwnd, IDC_CMT, 
		dlgCombo_getSel(hwnd, IDC_CMT)));
	int prevSel = 0;

	dlgCombo_reset(hwnd, IDC_CMT);
	int sel = dlgCombo_getSel(hwnd, IDC_BRA);
	auto* pos = g_gitInfo.branch_commit(sel);


	{ Bstr str; str.xreserve(80);
	while(pos) {
		str.slen = fmtTime(str, pos->committer.time);
		str.fmtcat("; %s", pos->message.data); 
		GitInfo::messageFix(str);
		int i = dlgCombo_addStr(hwnd, IDC_CMT, str, (LPARAM)pos);
		if(pos == prevCmt) prevSel = i;
		pos = pos->Parent();
	}}
	
	dlgCombo_setSel(hwnd, IDC_CMT, prevSel);
	select_commit(hwnd);
}

void repo_load(HWND hwnd, char* name)
{
	repo_reset(hwnd);
	if(!name) name = getCurrentDirectory();
	int ec = g_gitInfo.get_repo(name);
	if(ec) { contError(hwnd, "git query failed: %d\n", ec);
		return; }
		
	setDlgItemText(hwnd, IDC_PATH, g_gitInfo.dir);
	{ Bstr str;
	for(auto& x : g_gitInfo.branch) {
		str.strcpy(x.name.data);
		if(x.remote) str.fmtcat(" -> %s", x.remote.data);
		if(x.ahead) str.fmtcat(" : %s", x.ahead.data);
		dlgCombo_addStr(hwnd, IDC_BRA, str);
	}}
	
	dlgCombo_setSel(hwnd, IDC_BRA, 0);
	select_branch(hwnd);
}

void mainDlgInit(HWND hwnd)
{
	repo_load(hwnd, NULL);
}

BOOL CALLBACK mainDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	DLGMSG_SWITCH(
		ON_MESSAGE(WM_INITDIALOG, mainDlgInit(hwnd))
		ON_MESSAGE(WM_CLOSE, EndDialog(hwnd, 0))
		CASE_COMMAND(
			ON_CONTROL(CBN_SELCHANGE, IDC_BRA, select_branch(hwnd))
		
		
		,)
		
	  
		  /*ON_COMMAND(IDC_SPOIL, spoilChg(hwnd))
	    ON_COMMAND(IDC_COPY, clipCopy(hwnd))
	    ON_COMMAND(IDC_PASTE, clipPaste(hwnd))
	    ON_COMMAND(IDCANCEL, EndDialog(hwnd, 0))
			ON_COMMAND(IDC_GENERATE, format_call(hwnd))
			ON_CONTROL(CBN_SELCHANGE, IDC_CONVEN, format_call(hwnd))
			ON_CONTROL(EN_CHANGE, IDC_CALLEDT, parse_edit(hwnd))
			ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT1+100,
				edt_update(hwnd, LOWORD(wParam)))
			ON_CONTROL_RANGE(0, IDC_SPOIL_EAX, IDC_SPOIL_EBP, 
				format_call(hwnd));
			ON_COMMAND_RANGE(IDC_BUTTON1, IDC_BUTTON1+MAX_ARG, 
				arg_delelte(hwnd, index))*/
		//,)
	,)
}

int main()
{
	DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_DIALOG1), NULL, mainDlgProc);
}

/*

int main()
{
	GitInfo gi;

	int ec = gi.get_repo();
	printf("%d\n", ec);
	
	for(auto& x : gi.branch)
		printf("%s, %s\n", x.name.data, x.remote.data);
	


}
*/