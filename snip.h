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
#include <map>
#include <iomanip>
#include <complex>
#include <cmath>

// ast
struct Atom;
typedef std::shared_ptr<Atom> AtomPtr;
typedef double Real;
typedef AtomPtr (*Functor) (AtomPtr, AtomPtr);
inline thread_local std::vector<AtomPtr> eval_stack; // call stack
struct StackGuard {
    StackGuard(AtomPtr node) {
        eval_stack.push_back(node);
    }
    ~StackGuard() {
        eval_stack.pop_back();
    }
};
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
			out << e->value;
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
    if (eval_stack.size () > 1) {
        err << "\nstack trace:" << std::endl;
        for (auto it = eval_stack.rbegin(); it != eval_stack.rend(); ++it) {
            print(*it, err) << std::endl;
        }
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
std::string next (std::istream &in, unsigned& linenum) {
	std::stringstream accum;
	while (!in.eof ()) {
		char c = in.get ();
		switch (c) {
			case ';':
			do { c = in.get (); } while (c != '\n' && !in.eof ());
			++linenum;
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
				if (c == '\n') ++linenum;
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
					if (c == '\n') ++linenum;
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
AtomPtr read (std::istream& in, unsigned& linenum) {
	std::string token = next (in, linenum);
	if (!token.size ()) return make_atom();
	if (token == "(") {
		AtomPtr l = make_atom ();
		while (!in.eof ()) {
			AtomPtr n = read (in, linenum);
			if (n->lexeme == ")") break;
			else l->tail.push_back (n);
		}
		return l;
	} else if (token == "\'") {
		AtomPtr ll = make_atom();
		ll->tail.push_back (make_atom ("quote"));
		ll->tail.push_back (read (in, linenum));
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
	return false; // dummy
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
	StackGuard guard(node); 
	while (true) {
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
				continue; 
			} else {
				if (node->tail.size () == 4) {
					node = node->tail.at (3);
					continue; 
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
			continue; 
		}
		AtomPtr args = make_atom();
		for (unsigned i = 1; i < node->tail.size (); ++i) {
			args->tail.push_back (eval (node->tail.at (i), env));
		}
		if (func->type == LAMBDA) {
			AtomPtr vars = func->tail.at (0);
			AtomPtr body = func->tail.at (1);
			AtomPtr nenv = make_atom ();
			nenv->tail.push_back (func->tail.at (2)); // new environment with static binding

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
			continue; 
		}
		if (func->type == OP) {
			args_check (args, func->minargs);
			if (func->op == &fn_eval) {
				node = args->tail.at (0);
				continue; 
			}	
			if (func->op == &fn_apply) {
				AtomPtr l = type_check (args->tail.at (1), LIST);
				l->tail.push_front (args->tail.at (0));
				node = l;
				continue; 
			}			
			return func->op (args, env);
		}	
		error ("function expected", node);
	}
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
	unsigned linenum = 0;
	if (node->tail.size ()) {
		std::ifstream in (type_check (node->tail.at (0), STRING)->lexeme);
		if (!in.good ()) error ("cannot open input file", node);
		AtomPtr r = make_atom ();
		while (!in.eof ()) {
			AtomPtr l = read (in, linenum);
			if (!in.eof ()) r->tail.push_back (l);
		}
		return r;
	} else return read (std::cin, linenum);
}
AtomPtr load (const std::string&fname, std::istream& in, AtomPtr env) {
	AtomPtr r;
	unsigned linenum = 0;
	while (!in.eof ()) {
		try {
			AtomPtr l = read (in, linenum);
			if (!in.eof ()) r = eval (l, env);
		} catch (std::exception& e) {
			std::cerr << "[" << fname << ":" << linenum << "] " << e.what () << std::endl;
		} catch (...) {
			std::cerr << "unknown error detected" << std::endl;
		}
	}
	return r;
}
AtomPtr fn_load (AtomPtr node, AtomPtr env) {
	std::ifstream in (type_check (node->tail.at (0), STRING)->lexeme);
	if (!in.good ()) error ("cannot open input file", node);
	return load (node->tail.at (0)->lexeme, in, env);
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

AtomPtr fn_mean(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    if (list->tail.empty()) return make_atom(0.0);
    Real sum = 0;
    for (auto& e : list->tail) {
        sum += type_check(e, NUMBER)->value;
    }
    return make_atom(sum / list->tail.size());
}
AtomPtr fn_variance(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    if (list->tail.size() <= 1) return make_atom(0.0);
    Real m = type_check(fn_mean(node, env), NUMBER)->value;
    Real var = 0;
    for (auto& e : list->tail) {
        Real diff = type_check(e, NUMBER)->value - m;
        var += diff * diff;
    }
    return make_atom(var / (list->tail.size() - 1));
}
AtomPtr fn_stddev(AtomPtr node, AtomPtr env) {
    AtomPtr var = fn_variance(node, env);
    return make_atom(std::sqrt(type_check(var, NUMBER)->value));
}
AtomPtr fn_distance(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    if (a->tail.size() != b->tail.size()) error("vectors must have same size", node);
    Real sum = 0;
    for (size_t i = 0; i < a->tail.size(); ++i) {
        Real diff = type_check(a->tail[i], NUMBER)->value - type_check(b->tail[i], NUMBER)->value;
        sum += diff * diff;
    }
    return make_atom(std::sqrt(sum));
}
AtomPtr fn_linear_regression(AtomPtr node, AtomPtr env) {
    AtomPtr x_list = type_check(node->tail.at(0), LIST);
    AtomPtr y_list = type_check(node->tail.at(1), LIST);
    if (x_list->tail.size() != y_list->tail.size())
        error("linear-regression: x and y must have same length", node);
    size_t n = x_list->tail.size();
    bool is_1d = x_list->tail.size() > 0 && x_list->tail.at(0)->type == NUMBER;
    size_t dim = is_1d ? 1 : type_check(x_list->tail.at(0), LIST)->tail.size();
    size_t dim_b = dim + 1; // bias term
    // Build XtX matrix (dim_b × dim_b)
    std::vector<std::vector<Real>> XtX(dim_b, std::vector<Real>(dim_b, 0.0));
    // Build XtY vector (dim_b)
    std::vector<Real> XtY(dim_b, 0.0);
    for (size_t i = 0; i < n; ++i) {
        std::vector<Real> xi(dim_b, 1.0); // bias
        if (is_1d) {
            xi[1] = type_check(x_list->tail.at(i), NUMBER)->value;
        } else {
            AtomPtr row = type_check(x_list->tail.at(i), LIST);
            for (size_t j = 0; j < dim; ++j) {
                xi[j+1] = type_check(row->tail.at(j), NUMBER)->value;
            }
        }
        Real yi = type_check(y_list->tail.at(i), NUMBER)->value;
        for (size_t j = 0; j < dim_b; ++j) {
            for (size_t k = 0; k < dim_b; ++k) {
                XtX[j][k] += xi[j] * xi[k];
            }
            XtY[j] += xi[j] * yi;
        }
    }
    // Solve (XtX) w = XtY
    // Naive inversion (for very small matrices)
    std::vector<std::vector<Real>> inv(dim_b, std::vector<Real>(dim_b, 0.0));
    // First, compute determinant (only for dim_b <= 3 we'll support)
    if (dim_b == 2) {
        Real det = XtX[0][0]*XtX[1][1] - XtX[0][1]*XtX[1][0];
        if (std::abs(det) < 1e-8) error("singular matrix in regression", node);
        inv[0][0] =  XtX[1][1] / det;
        inv[0][1] = -XtX[0][1] / det;
        inv[1][0] = -XtX[1][0] / det;
        inv[1][1] =  XtX[0][0] / det;
    } else if (dim_b == 3) {
        Real a = XtX[0][0], b = XtX[0][1], c = XtX[0][2];
        Real d = XtX[1][0], e = XtX[1][1], f = XtX[1][2];
        Real g = XtX[2][0], h = XtX[2][1], i = XtX[2][2];
        Real det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
        if (std::abs(det) < 1e-8) error("singular matrix in regression", node);
        inv[0][0] = (e*i - f*h) / det;
        inv[0][1] = (c*h - b*i) / det;
        inv[0][2] = (b*f - c*e) / det;
        inv[1][0] = (f*g - d*i) / det;
        inv[1][1] = (a*i - c*g) / det;
        inv[1][2] = (c*d - a*f) / det;
        inv[2][0] = (d*h - e*g) / det;
        inv[2][1] = (b*g - a*h) / det;
        inv[2][2] = (a*e - b*d) / det;
    } else {
        error("linear regression currently supports up to 2 features", node);
    }
    // w = inv(XtX) * XtY
    std::vector<Real> w(dim_b, 0.0);
    for (size_t j = 0; j < dim_b; ++j) {
        for (size_t k = 0; k < dim_b; ++k) {
            w[j] += inv[j][k] * XtY[k];
        }
    }
    AtomPtr model = make_atom();
    for (auto v : w) {
        model->tail.push_back(make_atom(v));
    }
    return model;
}
AtomPtr fn_predict_linear(AtomPtr node, AtomPtr env) {
    AtomPtr model = type_check(node->tail.at(0), LIST);
    AtomPtr x = node->tail.at(1);
    Real y = type_check(model->tail.at(0), NUMBER)->value; // intercept
    if (x->type == NUMBER) {
        if (model->tail.size() != 2)
            error("predict-linear: model dimension mismatch for scalar input", node);
        y += type_check(model->tail.at(1), NUMBER)->value * type_check(x, NUMBER)->value;
    } else if (x->type == LIST) {
        if (model->tail.size() != x->tail.size() + 1)
            error("predict-linear: model dimension mismatch for list input", node);
        for (size_t i = 0; i < x->tail.size(); ++i) {
            y += type_check(model->tail.at(i + 1), NUMBER)->value * type_check(x->tail.at(i), NUMBER)->value;
        }
    } else {
        error("predict-linear: input must be number or list", node);
    }
    return make_atom(y);
}
AtomPtr fn_kmeans(AtomPtr node, AtomPtr env) {
    AtomPtr points = type_check(node->tail.at(0), LIST);
    AtomPtr k_atom = type_check(node->tail.at(1), NUMBER);
    int k = static_cast<int>(k_atom->value);
    if (k <= 0) error("k must be > 0", node);

    std::vector<Real> centers;
    for (unsigned i = 0; i < (unsigned) k && i < points->tail.size(); ++i) {
        centers.push_back(type_check(points->tail[i], NUMBER)->value);
    }

    bool changed = true;
    for (int iter = 0; iter < 10 && changed; ++iter) {
        std::vector<std::vector<Real>> clusters(k);
        for (auto& p : points->tail) {
            Real val = type_check(p, NUMBER)->value;
            int best = 0;
            Real best_dist = std::abs(val - centers[0]);
            for (int j = 1; j < k; ++j) {
                Real dist = std::abs(val - centers[j]);
                if (dist < best_dist) {
                    best = j;
                    best_dist = dist;
                }
            }
            clusters[best].push_back(val);
        }

        changed = false;
        for (int j = 0; j < k; ++j) {
            if (clusters[j].empty()) continue;
            Real sum = 0;
            for (Real v : clusters[j]) sum += v;
            Real new_center = sum / clusters[j].size();
            if (std::abs(new_center - centers[j]) > 1e-6) {
                centers[j] = new_center;
                changed = true;
            }
        }
    }

    AtomPtr result = make_atom();
    for (Real c : centers) result->tail.push_back(make_atom(c));
    return result;
}
AtomPtr fn_knn(AtomPtr node, AtomPtr env) {
    AtomPtr train_x = type_check(node->tail.at(0), LIST);
    AtomPtr train_y = type_check(node->tail.at(1), LIST);
    AtomPtr query = type_check(node->tail.at(2), LIST);
    AtomPtr k_atom = type_check(node->tail.at(3), NUMBER);
    int k = static_cast<int>(k_atom->value);
    if (train_x->tail.size() != train_y->tail.size()) error("train_x and train_y must match", node);
    std::vector<std::pair<Real, AtomPtr>> dists;
    for (size_t i = 0; i < train_x->tail.size(); ++i) {
        AtomPtr p = type_check(train_x->tail[i], LIST);
        if (p->tail.size() != query->tail.size()) error("dimension mismatch", node);
        Real dist = 0;
        for (size_t j = 0; j < p->tail.size(); ++j) {
            Real diff = type_check(p->tail[j], NUMBER)->value - type_check(query->tail[j], NUMBER)->value;
            dist += diff * diff;
        }
        dists.push_back({std::sqrt(dist), train_y->tail[i]});
    }
    std::sort(dists.begin(), dists.end(), [](auto& a, auto& b) { return a.first < b.first; });
    std::map<Real, int> votes;
    for (unsigned i = 0; i < (unsigned) k && i < dists.size(); ++i) {
        Real label = type_check(dists[i].second, NUMBER)->value;
        votes[label]++;
    }
    Real best_label = 0;
    int best_count = -1;
    for (auto& kv : votes) {
        if (kv.second > best_count) {
            best_label = kv.first;
            best_count = kv.second;
        }
    }
    return make_atom(best_label);
}
using Complex = std::complex<Real>;
void fft_compute(std::vector<Complex>& a, bool invert) {
    size_t n = a.size();
    if (n == 1) return;
    std::vector<Complex> a0(n / 2), a1(n / 2);
    for (size_t i = 0; 2 * i < n; ++i) {
        a0[i] = a[i*2];
        a1[i] = a[i*2+1];
    }
    fft_compute(a0, invert);
    fft_compute(a1, invert);
    Real ang = 2 * M_PI / n * (invert ? -1 : 1);
    Complex w(1), wn(cos(ang), sin(ang));
    for (size_t i = 0; 2 * i < n; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + n/2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + n/2] /= 2;
        }
        w *= wn;
    }
}
bool is_power_of_two(size_t n) {
    return (n > 0) && ((n & (n-1)) == 0);
}
AtomPtr fn_fft(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    std::vector<Complex> data;
    for (auto& elem : list->tail) {
        data.push_back(type_check(elem, NUMBER)->value);
    }
    if (!is_power_of_two(data.size())) {
        error("fft: input size must be a power of two", node);
    }
    fft_compute(data, false);
    AtomPtr out = make_atom();
    for (auto& c : data) {
        AtomPtr pair = make_atom();
        pair->tail.push_back(make_atom(c.real()));
        pair->tail.push_back(make_atom(c.imag()));
        out->tail.push_back(pair);
    }
    return out;
}
AtomPtr fn_ifft(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    std::vector<Complex> data;
    for (auto& elem : list->tail) {
        AtomPtr p = type_check(elem, LIST);
        Real re = type_check(p->tail.at(0), NUMBER)->value;
        Real im = type_check(p->tail.at(1), NUMBER)->value;
        data.push_back(Complex(re, im));
    }
    if (!is_power_of_two(data.size())) {
        error("fft: input size must be a power of two", node);
    }	
    fft_compute(data, true);
    AtomPtr out = make_atom();
    for (auto& c : data) {
        out->tail.push_back(make_atom(c.real()));
    }
    return out;
}
AtomPtr fn_pol2car(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    AtomPtr out = make_atom();
    for (auto& elem : list->tail) {
        AtomPtr pair = type_check(elem, LIST);
        Real r = type_check(pair->tail.at(0), NUMBER)->value;
        Real theta = type_check(pair->tail.at(1), NUMBER)->value;
        AtomPtr cartesian = make_atom();
        cartesian->tail.push_back(make_atom(r * std::cos(theta)));
        cartesian->tail.push_back(make_atom(r * std::sin(theta)));
        out->tail.push_back(cartesian);
    }
    return out;
}
AtomPtr fn_car2pol(AtomPtr node, AtomPtr env) {
    AtomPtr list = type_check(node->tail.at(0), LIST);
    AtomPtr out = make_atom();
    for (auto& elem : list->tail) {
        AtomPtr pair = type_check(elem, LIST);
        Real x = type_check(pair->tail.at(0), NUMBER)->value;
        Real y = type_check(pair->tail.at(1), NUMBER)->value;
        AtomPtr polar = make_atom();
        polar->tail.push_back(make_atom(std::sqrt(x*x + y*y)));
        polar->tail.push_back(make_atom(std::atan2(y, x)));
        out->tail.push_back(polar);
    }
    return out;
}
AtomPtr fn_conv(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    std::vector<Complex> A, B;
    for (auto& elem : a->tail) A.push_back(type_check(elem, NUMBER)->value);
    for (auto& elem : b->tail) B.push_back(type_check(elem, NUMBER)->value);
    size_t n = 1;
    while (n < A.size() + B.size()) n <<= 1;
    A.resize(n, 0.0);
    B.resize(n, 0.0);
    fft_compute(A, false);
    fft_compute(B, false);
    for (size_t i = 0; i < n; ++i) A[i] *= B[i];
    fft_compute(A, true);
    AtomPtr out = make_atom();
    for (auto& c : A) {
        out->tail.push_back(make_atom(c.real()));
    }
    return out;
}
AtomPtr fn_dot(AtomPtr node, AtomPtr env) {
    AtomPtr a = type_check(node->tail.at(0), LIST);
    AtomPtr b = type_check(node->tail.at(1), LIST);
    const auto& atail = a->tail;
    const auto& btail = b->tail;
    size_t n = atail.size();
    if (n != btail.size()) {
        error("dot: mismatched vector lengths", node);
    }
    Real sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
    size_t i = 0;
    // process blocks of 4
    for (; i + 3 < n; i += 4) {
        sum0 += type_check(atail[i+0], NUMBER)->value * type_check(btail[i+0], NUMBER)->value;
        sum1 += type_check(atail[i+1], NUMBER)->value * type_check(btail[i+1], NUMBER)->value;
        sum2 += type_check(atail[i+2], NUMBER)->value * type_check(btail[i+2], NUMBER)->value;
        sum3 += type_check(atail[i+3], NUMBER)->value * type_check(btail[i+3], NUMBER)->value;
    }
    for (; i < n; ++i) {
        sum0 += type_check(atail[i], NUMBER)->value * type_check(btail[i], NUMBER)->value;
    }

    return make_atom(sum0 + sum1 + sum2 + sum3);
}
AtomPtr fn_writewav(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    AtomPtr data = type_check(node->tail.at(1), LIST);
    uint16_t bits = 16;
    uint32_t samplerate = 44100;
    if (node->tail.size() >= 3) {
        bits = static_cast<uint16_t>(type_check(node->tail.at(2), NUMBER)->value);
        if (bits != 16 && bits != 32) error("bits must be 16 or 32", node);
    }
    if (node->tail.size() >= 4) {
        samplerate = static_cast<uint32_t>(type_check(node->tail.at(3), NUMBER)->value);
    }
    uint16_t channels = data->tail.size();
    if (channels == 0) error("empty channel list", node);
    uint32_t samples = data->tail.at(0)->tail.size();
    for (auto& chan : data->tail) {
        if (chan->tail.size() != samples)
            error("all channels must have same length", node);
    }
    uint32_t bytes_per_sample = bits / 8;
    uint32_t data_size = samples * channels * bytes_per_sample;
    uint32_t file_size = 36 + data_size;
    std::ofstream file(filename, std::ios::binary);
    if (!file) error("cannot create WAV file", node);
    file.write("RIFF", 4);
    file.write(reinterpret_cast<const char*>(&file_size), 4);
    file.write("WAVEfmt ", 8);
    uint32_t fmt_size = 16;
    uint16_t audio_fmt = 1;
    file.write(reinterpret_cast<const char*>(&fmt_size), 4);
    file.write(reinterpret_cast<const char*>(&audio_fmt), 2);
    file.write(reinterpret_cast<const char*>(&channels), 2);
    file.write(reinterpret_cast<const char*>(&samplerate), 4);
    uint32_t byterate = samplerate * channels * bits / 8;
    file.write(reinterpret_cast<const char*>(&byterate), 4);
    uint16_t blockalign = channels * bits / 8;
    file.write(reinterpret_cast<const char*>(&blockalign), 2);
    file.write(reinterpret_cast<const char*>(&bits), 2);
    file.write("data", 4);
    file.write(reinterpret_cast<const char*>(&data_size), 4);
    for (size_t i = 0; i < samples; ++i) {
        for (size_t ch = 0; ch < channels; ++ch) {
            Real v = type_check(data->tail.at(ch)->tail.at(i), NUMBER)->value;
            if (v > 1.0) v = 1.0;
            if (v < -1.0) v = -1.0;
            if (bits == 16) {
                int16_t sample = static_cast<int16_t>(v * 32767.0);
                file.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
            } else if (bits == 32) {
                int32_t sample = static_cast<int32_t>(v * 2147483647.0);
                file.write(reinterpret_cast<const char*>(&sample), sizeof(sample));
            }
        }
    }
    return make_atom();
}
AtomPtr fn_readwav(AtomPtr node, AtomPtr env) {
    std::string filename = type_check(node->tail.at(0), STRING)->lexeme;
    std::ifstream file(filename, std::ios::binary);
    if (!file) error("cannot open WAV file", node);
    char header[44];
    file.read(header, 44);
    if (std::string(header, header+4) != "RIFF" || std::string(header+8, header+12) != "WAVE")
        error("invalid WAV header", node);
    uint16_t channels = *reinterpret_cast<uint16_t*>(header + 22);
    // uint32_t samplerate = *reinterpret_cast<uint32_t*>(header + 24);
    uint16_t bits = *reinterpret_cast<uint16_t*>(header + 34);
    if (bits != 16 && bits != 32) error("only 16-bit or 32-bit PCM supported", node);
    uint32_t data_size = *reinterpret_cast<uint32_t*>(header + 40);
    size_t samples = data_size / (channels * (bits / 8));
    std::vector<std::vector<Real>> out(channels, std::vector<Real>(samples));
    for (size_t i = 0; i < samples; ++i) {
        for (size_t ch = 0; ch < channels; ++ch) {
            if (bits == 16) {
                int16_t sample;
                file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
                out[ch][i] = sample / 32768.0;
            } else if (bits == 32) {
                int32_t sample;
                file.read(reinterpret_cast<char*>(&sample), sizeof(sample));
                out[ch][i] = sample / 2147483648.0;
            }
        }
    }
    AtomPtr result = make_atom();
    for (auto& chan : out) {
        AtomPtr c = make_atom();
        for (auto v : chan) c->tail.push_back(make_atom(v));
        result->tail.push_back(c);
    }
    return result;
}
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
	add_op ("mean", &fn_mean, 1, env);
	add_op ("variance", &fn_variance, 1, env);
	add_op ("stddev", &fn_stddev, 1, env);
	add_op ("distance", &fn_distance, 2, env);
	add_op ("kmeans", &fn_kmeans, 2, env);
	add_op ("linear-regression", &fn_linear_regression, 2, env);
	add_op ("predict-linear", &fn_predict_linear, 2, env);
	add_op ("knn", &fn_knn, 4, env);	
	add_op ("fft", &fn_fft, 1, env);
	add_op ("ifft", &fn_ifft, 1, env);
	add_op ("conv", &fn_conv, 2, env);
	add_op ("dot", &fn_dot, 2, env);
	add_op ("pol2car", &fn_pol2car, 1, env);
	add_op ("car2pol", &fn_car2pol, 1, env);	
	add_op ("string", &fn_string, 2, env);
	add_op ("exec", &fn_exec, 1, env);
	add_op ("exit", &fn_exit, 0, env);
	return env;
}
void repl (std::istream& in, std::ostream& out, AtomPtr env) {
	unsigned linenum = 0;;
	while (true) {
		std::cout << ">> " << std::flush;
		try {
			print (eval (read (in, linenum), env), std::cout) << std::endl;
		} catch (std::exception& err) {
			std::cerr << "error: " << err.what () << std::endl;
		} catch (...) {
			std::cerr << "unknown error detected" << std::endl;
		}
	}
}

#endif // SNIP_H

// eof
