struct s {
	int x, y;
};

void meth(struct *s, int x, int y);
int cmeth(lua_State *L) {
	struct s *_s;
	luaXt_pull(L, 1, s, &_s); // safe, since typechecks were done by wrapper
	int x = lua_tointeger(L, 2);
	int y = lua_tointeger(L, 3);
	meth(_s, x, y);
	return 0;
}

luaXt_declfield(s, x, int);
luaXt_declfield(s, y, int);

luaXt_declmethod(s, void, meth, 2, int, x, int, y); // FIXME What is this supposed to produce?
luaXt_declcmethod(s, cmeth, 2, int, x, int, y);

struct s *_s;
luaXt_push(L, s, _s);

// ---

void fun(int x);
int cfun(lua_State *L) {
	int x = lua_tointeger(L, 1); // safe, since typechecks were done by wrapper
	fun(x);
	return 0;
}

luaXt_declfunction(void, fun, 1, int, x);
luaXt_declcfunction(cfun, 1, int, x);

luaXt_pushcfunction(L, cfun);
lua_setglobal(L, "fun");

// ---

typedef struct luaXt_Type luaXt_Type;
typedef struct luaXt_Field luaXt_Field;
typedef struct luaXt_CMethod luaXt_CMethod;
typedef struct luaXt_Argument luaXt_Argument;
typedef struct luaXt_CFunction luaXt_CFunction;

struct luaXt_Type {
	const char *name;
	bool (*is)(lua_State*, int narg); // FIXME call it "test" instead?
	void (*push)(lua_State*, void*);
	void (*pull)(lua_State*, int narg, void*);
	int type; // LUA_TTABLE, LUA_TLIGHTUSERDATA or LUA_TUSERDATA
	union {
		struct {
			int nfields;
			luaXt_Field fields[];
		} table;
		struct {
			int nmethods;
			luaXt_CMethod methods[];
		} udata;
	};
};

struct luaXt_Field {
	const char *name;
	luaXt_Type *type;
};

struct luaXt_CMethod {
	luaXt_Type *self;
	const char *name;
	lua_CFunction function;
};

#ifdef 0
struct luaXt_CMethod {
	luaXt_Type *self;
	luaXt_CFunction f;
};

#include "macromagic.h"

#define luaXt_declcfunction(function, ...) \
	luaXt_declcfunction_impl(function, STRINGIFY function, NUM_ARGS(__VA_ARGS__)/2, FOREACH(STRINGIFY, (__VA_ARGS__)))

#define luaXt_pushcfunction(L, function) \
	luaXt_pushcfunction_impl(L, STRINGIFY function)

struct luaXt_Argument {
	const char *name;
	luaXt_Type *type;
};

struct luaXt_CFunction {
	const char *name;
	lua_CFunction function;
	int nargs;
	luaXt_Argument args[];
};


void luaXt_declcfunction_impl(lua_CFunction function, const char *fname, int nargs, ...) {
	va_list ap;

	luaXt_CFunction *f = malloc(sizeof(luaXt_CFunction) + nargs*sizeof(luaXt_Argument));
	*f = (luaXt_CFunction){
		function,
		strdup(fname),
		nargs
	};

	va_start(ap, nargs);

	for (int i = 0; i < nargs; i++) {
		const char *arg_tname = va_arg(ap, const char*);
		const char *arg_name = va_arg(ap, const char*);

		luaXt_Type *targ = find_type(arg_tname);
		if (targ == NULL) {
			bail();
		}

		f->args[i] = (luaXt_Argument){strdup(arg_name), targ};
	}

	va_end(ap);

	insert_cfunction(f); // map[fname] = f
}

void luaXt_pushcfunction_impl(lua_State *L, const char *fname) {
	luaXt_CFunction *f = find_cfunction(fname); // f = map[fname]

	lua_pushlightuserdata(L, f);
	lua_pushcclosure(L, wrap_cfunction, 1);
}

int wrap_cfunction(lua_State *L) {
	luaXt_CFunction *f = lua_touserdata(L, lua_upvalueindex(1));

	for (int i = 0; i < f->nargs; i++) {
		luaXt_Argument *arg = &f->args[i];

		if (!arg->type->is(L, i)) {
			return typeerror(L, i, f, arg);
		}
	}

	return f->function(L);
}

int wrap_cmethod(lua_State *L) {
	luaXt_CMethod *m = lua_touserdata(L, lua_upvalueindex(1));

	void *self = luaX_testclass(L, 1, m->self->name, "self"); // FIXME Why not use checkclass instead?
	if (self == NULL) {
		return typeerror(L, 0, &m->f, m->self); // FIXME what is this exactly?
	}

	for (int i = 0; i < m->f.nargs; i++) {
		luaXt_Argument *arg = &m->f.args[i];

		if (!arg->type->is(L, i+1)) { // FIXME Why not use check functions instead?
			return typeerror(L, i, &m->f, arg);
		}
	}

	return m->f.function(L);
}

// --

Creating a function that will call through to Lua:

luaXt_declfunction(void, fun, 1, int, x);

#define luaXt_declfunction(rtype, function, ...) \
	luaXt_declfunction_impl(STRINGIFY rtype, function, STRINGIFY function, NUM_ARGS(__VA_ARGS__), FOREACH(STRINGIFY, (__VA_ARGS__)))

	// FIXME: Idea using the macro technique:
	FOREACH_PAIR(PUSHARG, (__VA_ARGS__));

#define PUSHARG(arg_tname, arg_name) PUSHARG_##arg_tname(arg_name)
#define PUSHARG_int(arg_name) lua_pushinteger(L, arg_name)


void luaXt_declfunction_impl(const char *rtype, lua_CFunction function, const char *fname, int nargs, ...) {

}

#endif
