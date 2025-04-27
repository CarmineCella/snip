// snip.cpp
//
#include <iostream>
#include "snip.h"
#include "scilib.h"

using namespace std;

int main (int argc, char* argv[]) {
	AtomPtr env = make_env ();
	add_scilib (env);
	
	if (argc == 1) {
		cout << "[snip, v. 0.1]" << endl << endl;
		cout << "scheme nano-interpreter project" << endl;
		cout << "(c) 2025 by Carmine-Emanuele Cella" << endl << endl;
	
		repl (cin, cout, env);
	} else {
		AtomPtr r;
		for (int i = 1; i < argc; ++i) {
			ifstream in (argv[1]);
			if (!in.good ()) cout << "warning: cannot open " << argv[i] << endl;
			load (argv[1], in, env);
		}
	}
	return 0;
}

// eof

