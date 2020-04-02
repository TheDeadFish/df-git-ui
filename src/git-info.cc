#include <stdshit.h>
#include "git-info.h"
#include "stuff.h"

cch* GitInfo::parse_time(cch* str, TimeInfo& ti)
{
	// get person info
	if(!(str = person_add(str))) return str;
	if(person_hint > 0xFFFF) return 0;
	ti.person = person_hint;
		
	// commit timestamp
	char* end;
	ti.time = strtoul(str, &end, 10);
	if(str == end) return end; str = end;

	// commit timezone
	if(!(str[0] && str[1])) return 0;
	ti.zone = DEC_CHAR(str[2], return 0)*600;
	ti.zone += DEC_CHAR(str[3], return 0)*60;
	ti.zone += DEC_CHAR(str[4], return 0)*10;
	ti.zone += DEC_CHAR(str[5], return 0);
	if(str[1] == '-') ti.zone = -ti.zone;
	return str+6;
}


int GitInfo::parse_commit(cch* str)
{
	#define PARSE_HASH(h) { \
		str = h.parse(str); if(!str) return -2; }
	#define PARSE_TIME(t) { \
		str = parse_time(str, t); if(!str) return -2; }		
	#define STR_CHK(x, ...) \
		ei(cch* xx = strScmp(str, x)) { str = xx; __VA_ARGS__; }
		
	Sha1 hash; PARSE_HASH(hash);
	Commit& cmt = commit_create(hash);
	
	while(1) {
		if(0) {}
		STR_CHK("\ntree ", PARSE_HASH(cmt.tree))
		STR_CHK("\nparent ", PARSE_HASH(hash);
			Commit& parent = commit_create(hash);
			add_uniqueP(cmt.parent, &parent);
			add_uniqueP(parent.child, &cmt))
		STR_CHK("\nauthor ", PARSE_TIME(cmt.authur))
		STR_CHK("\ncommitter ",	PARSE_TIME(cmt.committer))
		STR_CHK("\ngpgsig ", 
			str = strstr(str, "-----END PGP SIGNATURE-----");
			if(!str) return 2; str += 27; )
		STR_CHK("\n\n", str = skip_space(str);
			cmt.message.xcopy(str); return 0)
		else { return -3; }
	}
}

int GitInfo::get_commits(void)
{
	FileStrRead fp;
	IFRET(fp.popen(dir, "git rev-list --all --header"));
	while(cch* str = fp.get()) {
		IFRET(parse_commit(str)); }
	return fp.error();
}


GitInfo::GitInfo()
{
	ZINIT;
	hashTable = xMalloc(HASH_SIZE);
}

GitInfo::commit_find_t GitInfo::commit_find_(Sha1& hash, bool create)
{
	Commit** pos = &hashTable[hash.hash[0]%(HASH_SIZE-1)];
	for(; *pos; pos = &(*pos)->next) {
		if((*pos)->hash == hash) return {*pos, true}; }
	if(create) *pos = new Commit(hash);
	return {*pos, false};
}



cch* GitInfo::person_add(cch* str)
{
	struct PP { cstr cs1, cs2; 
		int cmp(GitInfo::Person& x) { return 
			cs1.cmp(x.name) ?: cs2.cmp(x.email); }
	};
	
	// parse person string
	cch* end1 = strchr(str, '<'); if(!end1) return end1;	
	cch* end2 = strchr(end1, '>'); if(!end2) return end2;
	PP pp = {{str, end1-1}, {end1+1, end2}};
	
	// lookup existing person
	FOR_FIH(person, person_hint, x, i,
		if(!pp.cmp(x)) {person_hint = i; return end2+1; }
	);

	// create new person
	person_hint = person.len;
	auto& x = person.xnxalloc();
	x.name.init(pp.cs1.xdup());
	x.email.init(pp.cs2.xdup());
	return end2+1;
}

int GitInfo::get_branches()
{
	FileStrRead fp;
	IFRET(fp.popen(dir, "git branch -a -vv --no-abbrev"));
	
	branch.Clear();
	while(char* str = fp.getLine()) 
	{
		// split branch info
		if(str[0] == 0) return -2;
		cch* name = strtok(str+1, " ");
		cch* hash = strtok(NULL, " ");
		if(!name || !hash) return -2;
		if(hash[0] == '-') continue;
		
		// parse tracking
		char* ahead = NULL;
		char* remote = strtok(NULL, "]");
		if(remote) { 
			if(*remote != '[') remote = 0;
			else { remote = strtok(remote, "[:");
				ahead = skip_space(strtok(ahead, "[:"));
			}
		}

		// create branch item
		if(str[0] == '*') branch_default = branch.len;
		auto& x = branch.xnxalloc();
		x.name.init(xstrdup(name));
		x.remote.init(xstrdup(remote));
		x.ahead.init(xstrdup(ahead));
		if(!x.hash.parse(hash)) return -2;
	}
	
	// merge remotes
	FOR_FI(branch, r, i,
		Branch* remote = branch_remote(r.remote);
		if(remote && (r.hash == remote->hash))
			branch.Remove(remote);
	);

	return 0;
}

int GitInfo::get_repo(char* dir)
{
	pRst(this);
	this->dir.init(dir);
	IFRET(get_branches());
	IFRET(get_commits());
	return 0;
}

GitInfo::Branch* GitInfo::branch_remote(cch* name)
{
	if(!name) return NULL;
	for(auto& x : branch) {
		cch* remote = x.isRemote();
		if(remote && !strcmp(remote, name))
			return &x; }
	return NULL;
}

GitInfo::Branch::~Branch() {}
cch* GitInfo::Branch::isRemote() {
	return strScmp(name, "remotes/"); }

GitInfo::Commit* GitInfo::branch_commit(int i) {
	return commit_find(branch[i].hash); }
	
char* GitInfo::messageFix(char* str)
{
	char* wrPos = str;
NEXT_LINE:
	str = skip_space(str);
	for(; ; wrPos++) {
		char ch = *str; str++;
		if(!(*wrPos = ch)) break;
		if(ch == '\n') {
			if(*str) { 
				RW(wrPos) = ' ;'; 
				wrPos += 2; }
			goto NEXT_LINE;
		}
	}

	return wrPos;
}
