#pragma once 
#include "sha1.h"

struct GitInfo
{
	GitInfo();
	void free() { pRst(this); }

	struct TimeInfo { u16 person; 
		u16 zone; u32 time; };

	struct Commit {
		Commit* next; xstr message;
		Sha1 hash, tree;
		TimeInfo authur, committer;
		xArray<Commit*> parent;
		xArray<Commit*> child;
		
		Commit* Parent() { return parent.len ? *parent : 0; }

		Commit(Sha1& h) { ZINIT; hash=h; }
		bool operator==(const Commit& That) const {
			return hash == That.hash; }
	};


	// person list
	struct Person { xstr name, email; };
	xArray<Person> person;
	
	
	
	
	
	
	
	
	
	
	
	
	
	




	
	
	DEF_RETPAIR(commit_find_t, Commit*, item, bool, found);
	commit_find_t commit_find_(Sha1& hash, bool create);
	Commit* commit_find(Sha1& hash) { return commit_find_(hash, 0); }
	Commit& commit_create(Sha1& hash) { return *commit_find_(hash, 1); }
	
	
	
	
	
	int get_commits();
	int parse_commit(cch* str);
	
	int get_branches();
	int get_repo();
	
	
	// branch list
	int branch_default;
	struct Branch { Sha1 hash; xstr name;
		xstr remote, ahead; ~Branch(); 
		cch* isRemote();
	};

	xArray<Branch> branch;
	Commit* branch_commit(int i);
	
	static char* messageFix(char* str);
	
	
	
private:
	enum { HASH_SIZE=1024*1024 };
	Commit** hashTable;
	cch* parse_time(cch* str, TimeInfo& ti);
	cch* person_add(cch* str); u32 person_hint;
	Branch* branch_remote(cch* name);
	
	
	
};
