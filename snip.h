// snip.h
//

#ifndef SNIP_H
#define SNIP_H

#include <iostream>
#include <fstream>
#include <deque>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <iomanip>
#include <cmath>

// ast
struct Atom;
typedef std::shared_ptr<Atom> AtomPtr;
typedef double Real;
typedef AtomPtr (*Functor) (AtomPtr, AtomPtr);
#define make_atom(a)(std::make_shared<Atom> (a))
enum AtomType {LIST, SYMBOL, STRING, NUMBER, LAMBDA, OP};
const char* ATOM_NAMES[] = {"list", "symbol", "std::string", "number", "lambda", "op"};
bool is_string (const std::string& l);
void error (const std::string& msg, AtomPtr n);
struct Atom {
	Atom () { type = LIST; }
	Atom (std::string lex) {
		lexeme = lex;
		if (is_string (lexeme)) {
			type = STRING;
			lexeme = lex.substr (1, lex.size () - 1);
		} else {
			type = SYMBOL;
			lexeme = lex;
		}
	}
	Atom (Real val) {
		type = NUMBER;
		value = val;
	}
	Atom (AtomPtr ll) {
		if (ll->tail.size () < 3) error ("malformed lambda function", make_atom());
		type = LAMBDA;
		tail.push_back (ll->tail.at (0)); // vars
		tail.push_back (ll->tail.at (1)); // body
		tail.push_back (ll->tail.at (2)); // env
	}
	Atom (Functor f) {
		type = OP;
		op = f;
	}
	AtomType type;
	std::string lexeme;
	Real value;
	Functor op;
	unsigned minargs;
	std::deque <AtomPtr> tail;
};
bool is_nil (AtomPtr e) {
	return (e == nullptr || (e->type == LIST && e->tail.size () == 0));
}

// helpers
bool is_string (const std::string& l) {
	if (l.size () > 1 && l.at (0) == '\"') return true;
	return false;
}
bool is_number (std::string token) {
	return std::regex_match(token, std::regex (("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?")));
}
std::ostream& print (AtomPtr e, std::ostream& out, bool write = false) {
	if (e != nullptr) { // to have () printed for nil
		switch (e->type) {
		case LIST:
			out << "(";
			for (unsigned i = 0; i < e->tail.size (); ++i) {
				print (e->tail.at (i), out, write);
				if (i != e->tail.size () - 1) out << " ";
			}
			out << ")";
		break;
		case SYMBOL:
			out << e->lexeme;
		break;
		case STRING:
			if (write) out << "\"" << e->lexeme << "\"";
			else out << e->lexeme;
		break;
		case NUMBER:
			out << std::fixed << e->value;
		break;
		case LAMBDA:
			out << "(lambda ";
			print (e->tail.at (0), out, write) << " "; // vars
			print (e->tail.at (1), out, write) << ")"; // body
 		break;
		case OP:
			if (write) out << e->lexeme;
			else out << "<op @ " << (std::hex) << &e->op << ">";
		break;
		}
	}
	return out;
}
void error (const std::string& msg, AtomPtr n) {
	std::stringstream err;
	err << msg;
	if (!is_nil (n)) {
		err << " -> ";
		print (n, err);
	}
	throw std::runtime_error (err.str ());
}
AtomPtr args_check (AtomPtr node, unsigned args) {
	std::stringstream err;
	err << "insufficient number of arguments (required " << args << ", got " << node->tail.size () << ")";
	if (node->tail.size () < args) error (err.str (), node);
	return node;
}
AtomPtr type_check (AtomPtr node, AtomType t) {
	std::stringstream err;
	err << "invalid type (required " << ATOM_NAMES[t] << ", got " << ATOM_NAMES[node->type] << ")";
	if (node->type != t) error (err.str (), node);
	return node;
}

// lexing, parsing, evaluation
std::string next (std::istream &in) {
	std::stringstream accum;
	while (!in.eof ()) {
		char c = in.get ();
		switch (c) {
			case ';':
			do { c = in.get (); } while (c != '\n' && !in.eof ());
			break;
			case '(': case ')': case '\'':
				if (accum.str ().size ()) {
					in.putback (c);
					return accum.str ();
				} else {
					accum << c;
					return accum.str ();
				}
			break;
			case '\t': case '\n': case '\r': case ' ':
				if (accum.str ().size ()) return accum.str ();
				else continue;
			break;
			case '\"':
			if (accum.str ().size ()) {
				in.putback(c);
				return accum.str ();
			} else {
				accum << c;
				while (!in.eof ()) {
					in.get (c);
					if (c == '\"') break;
					else if (c == '\\') {
						c = in.get ();
						switch (c) {
							case 'n': accum <<'\n'; break;
							case 'r': accum <<'\r'; break;
							case 't': accum <<'\t'; break;
							case '\"': accum << "\""; c = 0; break;
						}
					} else accum << c;
				}
				return accum.str ();
			}
			break; 
			default:
				if (c > 0) accum << c;
			break;
		}
	}
	return accum.str ();
}
AtomPtr read (std::istream& in) {
	std::string token = next (in);
	if (!token.size ()) return make_atom();
	if (token == "(") {
		AtomPtr l = make_atom ();
		while (!in.eof ()) {
			AtomPtr n = read (in);
			if (n->lexeme == ")") break;
			else l->tail.push_back (n);
		}
		return l;
	} else if (token == "\'") {
		AtomPtr ll = make_atom();
		ll->tail.push_back (make_atom ("quote"));
		ll->tail.push_back (read (in));
		return ll;
	} else if (is_number (token)) {
		return make_atom(atof (token.c_str ()));
	}
	else {
		return make_atom (token);
	}
}
bool atom_eq (AtomPtr a, AtomPtr b) {
	if (is_nil (a) && !is_nil (b)) return false;
	if (!is_nil (a) && is_nil (b)) return false;
	if (is_nil (a) && is_nil (b)) return true;
	if (a->type != b->type) return false;
	switch (a->type) {
		case LIST:
			if (a->tail.size () != b->tail.size ()) return false;
			for (unsigned i = 0; i < a->tail.size (); ++i) {
				if (! atom_eq (a->tail.at (i), b->tail.at (i))) return false;
			}
			return true;
		break;
		case SYMBOL: case STRING:
			return a->lexeme == b->lexeme;
		break;
		case NUMBER:
		return a->value == b->value;
		break;
		case LAMBDA:
			if (a->tail.at (0) != b->tail.at (0)) return false;
			if (a->tail.at (1) != b->tail.at (1)) return false;
			return true; // env not checked in comparison
		break;
		case OP:
			return a->op == b->op;
		break;
	}
}
AtomPtr assoc (AtomPtr node, AtomPtr env) {
	for (unsigned i = 1; i < env->tail.size (); ++i) {
		AtomPtr vv = env->tail.at (i);
		if (atom_eq (node, vv->tail.at (0))) return vv->tail.at(1);
	}
	if (!is_nil (env->tail.at (0))) return assoc (node, env->tail.at (0));
	error ("unbound identifier", node);
	return make_atom (); // dummy
}
AtomPtr extend (AtomPtr node, AtomPtr val, AtomPtr env, bool recurse = false) {
	for (unsigned i = 1; i < env->tail.size (); ++i) {
		AtomPtr vv = env->tail.at (i);
		if (atom_eq (node, vv->tail.at (0))) {
			vv->tail.at(1) = val;
			return val;
		}
	}
	if (recurse) { // set
		if (!is_nil (env->tail.at (0))) return extend (node, val, env->tail.at (0), recurse);
		error ("unbound identifier", node);
	} else {
		AtomPtr vv = make_atom();
		vv->tail.push_back (node);
		vv->tail.push_back (val);
		env->tail.push_back (vv);
		return val;
	}
	
	return make_atom(); // dummy
}
AtomPtr fn_quote (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_def (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_set (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_lambda (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_if (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_while (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_begin (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_apply (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr fn_eval (AtomPtr, AtomPtr) { return nullptr; } // dummy
AtomPtr eval (AtomPtr node, AtomPtr env) {
tail_call:
	if (is_nil (node)) return make_atom ();
	if (node->type == SYMBOL && node->lexeme.size ()) return assoc (node, env);
	if (node->type != LIST) return node;

	AtomPtr func = eval (node->tail.at (0), env);
	if (func->op == &fn_quote) {
		args_check (node, 2);
		return node->tail.at (1);
	}
	if (func->op == &fn_def) {
		args_check (node, 3);
		return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), env);
	}
	if (func->op == &fn_set) {
		args_check (node, 3);
		return extend (type_check (node->tail.at (1), SYMBOL), eval (node->tail.at (2), env), env, true);
	}
	if (func->op == &fn_lambda) {
		args_check (node, 3);
		AtomPtr ll = make_atom();
		ll->tail.push_back (type_check (node->tail.at (1), LIST)); // vars
		ll->tail.push_back (type_check (node->tail.at (2), LIST)); // body
		ll->tail.push_back (env); // env (lexical scope)
		return make_atom(ll); // lambda
	}
	if (func->op == &fn_if) {
		args_check (node, 3);
		if (type_check (eval (node->tail.at (1), env), NUMBER)->value) {
			node = node->tail.at (2);
			goto tail_call;
		} else {
			if (node->tail.size () == 4) {
				node = node->tail.at (3);
				goto tail_call;
			} else return make_atom ();
		}
	}
	if (func->op == &fn_while) {
		args_check (node, 3);
		AtomPtr r = make_atom ();
		while (type_check (eval (node->tail.at (1), env), NUMBER)->value) {
			r = eval (node->tail.at (2), env);
		}
		return r;
	}	
	if (func->op == &fn_begin) {
		args_check (node, 2);
		for (unsigned i = 0; i < node->tail.size () - 1; ++i) {
			eval (node->tail.at (i), env);
		} 
		node = node->tail.at (node->tail.size () - 1);
		goto tail_call;
	}
	AtomPtr args = make_atom();
	for (unsigned i = 1; i < node->tail.size (); ++i) {
		args->tail.push_back (eval (node->tail.at (i), env));
	}
	if (func->type == LAMBDA) {
		AtomPtr vars = func->tail.at (0);
		AtomPtr body = func->tail.at (1);
		AtomPtr nenv = make_atom ();
		nenv->tail.push_back (env); //func->tail.at (2)); // new environment with static binding

		if (vars->tail.size () < args->tail.size ()) error ("too many arguments in lambda", node);
		unsigned minargs = (vars->tail.size () > args->tail.size () ? args->tail.size () : vars->tail.size ());
		for (unsigned i = 0; i < minargs; ++i) {
			extend (vars->tail.at (i), args->tail.at (i), nenv);
		}

		if (vars->tail.size () > args->tail.size ()) {		
			AtomPtr vars_cut = make_atom ();
			for (unsigned i = 0; i < minargs; ++i) {
				vars_cut->tail.push_back (vars->tail.at (i));
			}	
			AtomPtr new_lambda = make_atom (); 
			new_lambda->tail.push_back (vars_cut);
			new_lambda->tail.push_back (body);
			new_lambda->tail.push_back (nenv);
			return make_atom (new_lambda); // return lambda with bounded vars
		}
		env = nenv;
		node = body;
		goto tail_call;
	}
	if (func->type == OP) {
		args_check (args, func->minargs);
		if (func->op == &fn_eval) {
			node = args->tail.at (0);
			goto tail_call;
		}	
		if (func->op == &fn_apply) {
			AtomPtr l = type_check (args->tail.at (1), LIST);
			l->tail.push_front (args->tail.at (0));
			node = l;
			goto tail_call;
		}			
		return func->op (args, env);
	}	
	error ("function expected", node);
	return node; // dummy
}

// functors
AtomPtr fn_env (AtomPtr node, AtomPtr env) {
	if (node->tail.size () && type_check(node->tail.at(0), SYMBOL)->lexeme == "full") return env;
	AtomPtr l = make_atom();
	for (unsigned i = 1; i < env->tail.size (); ++i) l->tail.push_back (env->tail.at (i)->tail.at (0));
	return l;
}
AtomPtr fn_list (AtomPtr node, AtomPtr env) {
	return node;
}
AtomPtr fn_cons(AtomPtr node, AtomPtr env) {
	AtomPtr result = make_atom ();
    result->tail.push_back(node->tail.at (0));
    if (node->tail.at (1)->type == LIST) {
        result->tail.insert(result->tail.end(), node->tail.at (1)->tail.begin(), node->tail.at (1)->tail.end());
    } else result->tail.push_back(node->tail.at (1));
    return result;
}
AtomPtr fn_car (AtomPtr node, AtomPtr env) {
	if (!node->tail.at (0)->tail.size ()) return make_atom();
	return node->tail.at (0)->tail.at (0);
}
AtomPtr fn_cdr (AtomPtr node, AtomPtr env) {
	if (!node->tail.at (0)->tail.size ()) return make_atom();
	AtomPtr cdr = make_atom ();
	for (unsigned i = 1; i < node->tail.at (0)->tail.size (); ++i) {
		cdr->tail.push_back (node->tail.at (0)->tail.at (i));
	}
	return cdr;
}
AtomPtr fn_eq (AtomPtr node, AtomPtr env) {
	return make_atom ((Real) atom_eq (node->tail.at (0), node->tail.at (1)));
}
AtomPtr fn_type (AtomPtr node, AtomPtr env) {
	return make_atom (ATOM_NAMES[node->tail.at (0)->type]);
}
template <bool WRITE>
AtomPtr fn_print (AtomPtr node, AtomPtr env) {
	std::ostream* out = &std::cout;
	if (WRITE) {
		out = new std::ofstream (type_check (node->tail.at (0), STRING)->lexeme);
		if (!out->good ()) error ("cannot create output file", node);
	}
	for (unsigned int i = WRITE; i < node->tail.size (); ++i) {
		print (node->tail.at (i), *out, WRITE);
	}
	if (WRITE) ((std::ofstream*) out)->close (); // downcast
	return make_atom ("");
}
AtomPtr fn_read (AtomPtr node, AtomPtr env) {
	if (node->tail.size ()) {
		std::ifstream in (type_check (node->tail.at (0), STRING)->lexeme);
		if (!in.good ()) error ("cannot open input file", node);
		AtomPtr r = make_atom ();
		while (!in.eof ()) {
			AtomPtr l = read (in);
			if (!in.eof ()) r->tail.push_back (l);
		}
		return r;
	} else return read (std::cin);
}
AtomPtr load (std::istream& in, AtomPtr env) {
	AtomPtr r;
	while (!in.eof ()) {
		try {
			AtomPtr l = read (in);
			if (!in.eof ()) r = eval (l, env);
		} catch (std::exception& e) {
			std::cerr << "error: " << e.what () << std::endl;
		} catch (...) {
			std::cerr << "unknown error detected" << std::endl;
		}
	}
	return r;
}
AtomPtr fn_load (AtomPtr node, AtomPtr env) {
	std::ifstream in (type_check (node->tail.at (0), STRING)->lexeme);
	if (!in.good ()) error ("cannot open input file", node);
	return load (in, env);
}
#define MAKE_BINOP(op,name, unit) \
AtomPtr name (AtomPtr node, AtomPtr env) { \
	Real v = 0; \
	if (node->tail.size () == 1) v = unit op type_check (node->tail.at (0), NUMBER)->value; \
	else v = type_check (node->tail.at (0), NUMBER)->value; \
	for (unsigned i = 1; i < node->tail.size (); ++i) { \
		v = v op type_check (node->tail.at (i), NUMBER)->value; \
	} \
	return make_atom (v); \
} \

MAKE_BINOP (+, fn_add, 0);
MAKE_BINOP (-, fn_sub, 0);
MAKE_BINOP (*, fn_mul, 1);
MAKE_BINOP (/, fn_div, 1);
#define MAKE_CMPOP(op,name) \
AtomPtr name (AtomPtr node, AtomPtr env) { \
	bool r = true; \
	for (unsigned i = 0; i < node->tail.size () - 1; ++i) { \
		r = type_check(node->tail.at (i), NUMBER)->value op type_check (node->tail.at (i + 1), NUMBER)->value; \
		if (r == false) break; \
	} \
	return make_atom (r); \
} \

MAKE_CMPOP (<, fn_less);
MAKE_CMPOP (<=, fn_le);
MAKE_CMPOP (>, fn_gt);
MAKE_CMPOP (>=, fn_ge);
#define MAKE_SINGOP(op,name) \
AtomPtr name (AtomPtr node, AtomPtr env) { \
	AtomPtr l = make_atom (); \
	for (unsigned i = 0; i < node->tail.size (); ++i) l->tail.push_back (make_atom (op ((Real) type_check (node->tail.at (i), NUMBER)->value))); \
	if (l->tail.size () == 1) return  l->tail.at (0); \
	else return l; \
} \

MAKE_SINGOP (std::sin, fn_sin);
MAKE_SINGOP (std::cos, fn_cos);
MAKE_SINGOP (std::tan, fn_tan);
MAKE_SINGOP (std::exp, fn_exp);
MAKE_SINGOP (std::log, fn_log);
MAKE_SINGOP (std::log10, fn_log10);
MAKE_SINGOP (std::sqrt, fn_sqrt);
MAKE_SINGOP (std::abs, fn_abs);
MAKE_SINGOP (std::floor, fn_floor);

void replace (std::string &s, std::string from, std::string to) {
	int idx = 0;
	size_t next;
	while ((next = s.find (from, idx)) != std::string::npos) {
		s.replace (next, from.size (), to);
		idx = next + to.size ();
	} 
}
std::vector<std::string> split (const std::string& in, char separator) {
	std::istringstream iss(in);
	std::string s;
	std::vector<std::string> tokens;
	while (std::getline(iss, s, separator)) {
		tokens.push_back (s);
	}
	return tokens;
}
AtomPtr fn_string (AtomPtr node, AtomPtr env) {
	std::string cmd = type_check (node->tail.at (0), SYMBOL)->lexeme;
	AtomPtr l = make_atom();
	std::regex r;
	if (cmd == "length") { // argnum checked by default
		return make_atom(type_check (node->tail.at(1), STRING)->lexeme.size ());
	} else if (cmd == "find") {
		args_check (node, 3);
		unsigned long pos = type_check (node->tail.at(1), STRING)->lexeme.find (
			type_check (node->tail.at(2), STRING)->lexeme);
		if (pos == std::string::npos) return make_atom (-1);
		else return make_atom (pos);		
	} else if (cmd == "range") {
		args_check (node, 4);
		std::string tmp = type_check (node->tail.at(1), STRING)->lexeme.substr(
			type_check (node->tail.at(2), NUMBER)->value, 
			type_check (node->tail.at(3), NUMBER)->value);
		return make_atom ((std::string) "\"" + tmp);
	} else if (cmd == "replace") {
		args_check (node, 4);
		std::string tmp = type_check (node->tail.at(1), STRING)->lexeme;
		replace (tmp,
			type_check (node->tail.at(2), STRING)->lexeme, 
			type_check (node->tail.at(3), STRING)->lexeme);
		return make_atom((std::string) "\"" + tmp);
	} else if (cmd == "split") {
		args_check (node, 3);
		std::string tmp = type_check (node->tail.at(1), STRING)->lexeme;
		char sep =  type_check (node->tail.at(2), STRING)->lexeme[0];
		std::vector<std::string> tokens = split (tmp, sep);
		AtomPtr l = make_atom ();
		for (unsigned i = 0; i < tokens.size (); ++i) l->tail.push_back (make_atom ((std::string) "\"" + tokens[i]));
		return l;
	}else if (cmd == "regex") {
		args_check (node, 3);
		std::string str = type_check (node->tail.at(1), STRING)->lexeme;
		std::regex r (type_check (node->tail.at(2), STRING)->lexeme);
		std::smatch m; 
		std::regex_search(str, m, r);
		AtomPtr l = make_atom();
		for(auto v: m) {
			l->tail.push_back (make_atom ((std::string) "\"" + v.str()));
		}
		return l;		
	} 
	return l;
}
AtomPtr fn_exec (AtomPtr node, AtomPtr env) {
	return make_atom (system (type_check (node->tail.at (0), STRING)->lexeme.c_str ()));
}
AtomPtr fn_exit (AtomPtr node, AtomPtr env) {
	std::cout << std::endl;
	exit (0);
	return make_atom ();
}

// interface
void add_op (const std::string& lexeme, Functor f, int minargs, AtomPtr env) {
	AtomPtr op = make_atom(f);
	op->lexeme = lexeme;
	op->minargs = minargs;
	extend (make_atom(lexeme), op, env);
}
AtomPtr make_env () {
	AtomPtr env = make_atom ();
	env->tail.push_back (make_atom ()); // nil parent
	add_op ("quote", &fn_quote, -1, env); // -1 are checked in the handling function
	add_op ("define", &fn_def, -1, env);
	add_op ("set!", &fn_set, -1, env);
	add_op ("lambda", &fn_lambda, -1, env);
	add_op ("if", &fn_if, -1, env);
	add_op ("while", &fn_while, -1, env);
	add_op ("begin", &fn_begin, -1, env);
	add_op ("eval", &fn_eval, 1, env);
	add_op ("apply", &fn_apply, 2, env);
	add_op ("env", &fn_env, 0, env);
	add_op ("list", &fn_list, 0, env);
	add_op ("cons", &fn_cons, 2, env);
	add_op ("car", &fn_car, 1, env);
	add_op ("cdr", &fn_cdr, 1, env);
	add_op ("eq?", &fn_eq, 2, env);
	add_op ("type", &fn_type, 1, env);
	add_op ("print", &fn_print<false>, 1, env);
	add_op ("save", &fn_print<true>, 2, env);
	add_op ("read", &fn_read, 0, env);
	add_op ("load", &fn_load, 0, env);
	add_op ("+", &fn_add, 1, env);
	add_op ("-", &fn_sub, 1, env);
	add_op ("*", &fn_mul, 1, env);
	add_op ("/", &fn_div, 1, env);
	add_op ("<", &fn_less, 2, env);
	add_op ("<=", &fn_le, 2, env);
	add_op (">", &fn_gt, 2, env);
	add_op (">=", &fn_ge, 2, env);
	add_op ("sin", &fn_sin, 1, env);
	add_op ("cos", &fn_cos, 1, env);
	add_op ("tan", &fn_tan, 1, env);
	add_op ("exp", &fn_exp, 1, env);
	add_op ("log", &fn_log, 1, env);
	add_op ("log10", &fn_log10, 1, env);
	add_op ("sqrt", &fn_sqrt, 1, env);
	add_op ("abs", &fn_abs, 1, env);
	add_op ("floor", &fn_floor, 1, env);
	add_op ("std::string", &fn_string, 2, env);
	add_op ("exec", &fn_exec, 1, env);
	add_op ("exit", &fn_exit, 0, env);
	return env;
}
void repl (std::istream& in, std::ostream& out, AtomPtr env) {
	while (true) {
		std::cout << ">> ";
		try {
			print (eval (read (in), env), std::cout) << std::endl;
		} catch (std::exception& err) {
			std::cerr << "error: " << err.what () << std::endl;
		} catch (...) {
			std::cerr << "unknown error detected" << std::endl;
		}
	}
}

#endif // SNIP_H

// eof

