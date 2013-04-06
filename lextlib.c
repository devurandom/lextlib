#include "lextlib.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <lauxlib.h>


int luaX_traceback (lua_State *L) {
	lua_getglobal(L, "debug");                                       // [-0,+1,-]
	if (!lua_istable(L, -1)) {                                       // [-0,+0,-]
		lua_pop(L, 1);                                                 // [-1,+0,-]
		return 1;
	}

	lua_getfield(L, -1, "traceback");                                // [-0,+1,-]
	if (!lua_isfunction(L, -1)) {                                    // [-0,+0,-]
		lua_pop(L, 2);                                                 // [-1,+0,-]
		return 1;
	}

	lua_pushvalue(L, 1); /* pass error message */                    // [-0,+1,-]
	lua_pushinteger(L, 2); /* skip this function and traceback */    // [-0,+1,-]
	lua_call(L, 2, 1); /* call debug.traceback */                    // [-3,+1,e]

	return 1;
}


int luaX_panic (lua_State *L) {
	int nresults = luaX_traceback(L);
	if (nresults != 1) {
		fprintf(stderr, "Unable to get backtrace: Unexpected number of results (%d)\n", nresults);
		return 0;
	}

	if (!lua_isstring(L, -1)) {
		fprintf(stderr, "Unable to get backtrace: Result not a string\n");
		return 0;
	}

	const char *err = lua_tostring(L, -1);
	if (err == NULL) {
		fprintf(stderr, "Unable to get backtrace: Cannot convert result to string\n");
		return 0;
	}

	fprintf(stderr, "PANIC: %s\n", err);

	return 0;
}


void luaX_showstack (lua_State *L) { //> [-0,+0,e]
	lua_pushcfunction(L, luaX_traceback);  /* push traceback function */
	lua_pushnil(L);

	if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
		fprintf(stderr, "Unable to get backtrace: Executing traceback function failed\n");
		return;
	}

	if (!lua_isstring(L, -1)) {
		fprintf(stderr, "Unable to get backtrace: Result not a string\n");
		return;
	}

	const char *err = lua_tostring(L, -1);
	if (err == NULL) {
		fprintf(stderr, "Unable to get backtrace: Cannot convert result to string\n");
		return;
	}

	lua_pop(L, 1);

	fprintf(stderr, "DEBUG: %s\n", err);
}


void luaX_error (lua_State *L, const char *fmt, ...) {
	va_list argp;

	va_start(argp, fmt);
	vfprintf(stderr, fmt, argp);
	va_end(argp);

	lua_close(L);

	exit (EXIT_FAILURE);
}


void luaX_preload(lua_State *L, const char *name, lua_CFunction init_function) {
	lua_getglobal(L, "package");                                         // [-0,+1,e]
	lua_getfield(L, -1, "preload");                                      // [-0,+1,e]
	lua_pushcfunction(L, init_function);                                 // [-0,+1,-]
	lua_setfield(L, -2, name); // package.preload[name] = init_function  // [-1,+0,e]
	lua_pop(L, 2); // pop (package.preload, package)                     // [-2,+0,-]
}


void luaX_restrict(lua_State *L) {
	lua_getglobal(L, "package");                                        // [-0,+1,e]
	lua_getfield(L, -1, "searchers");                                   // [-0,+1,e]
	lua_newtable(L); // push ($newtable)                                // [-0,+1,e]
	lua_rawgeti(L, -2, 1); // push (package.searchers[1])               // [-0,+1,-]
	lua_rawseti(L, -2, 1); // $newtable[1] = top, pop                   // [-1,+0,e]
	lua_setfield(L, -3, "searchers"); // package.searchers = top, pop   // [-1,+0,e]
	lua_pop(L, 2); // pop ($package.searchers, package)                 // [-2,+0,-]
}


const char *luaX_typename(lua_State *L, int narg) {
	if (luaL_getmetafield(L, narg, LUAX_STR_TYPENAME)) {
		const char *cname = lua_tostring(L, -1);
		lua_pop(L, 1); // FIXME might free the string
		return cname;
	}

	if (luaL_getmetafield(L, narg, LUAX_STR_CLASS)) {
		if (luaL_getmetafield(L, -1, LUAX_STR_TYPENAME)) {
			const char *cname = lua_tostring(L, -1);
			lua_pop(L, 2); // FIXME might free the string
			return cname;
		}

		lua_pop(L, 1);
	}

	return luaL_typename(L, narg);
}


int luaX_argerror (lua_State *L, int narg, const char *argname, const char *extramsg) {
	const char *msg = NULL;

	if (argname != NULL) {
		msg = lua_pushfstring(L, LUA_QS ": %s", argname, extramsg);
	}
	else {
#if LUA_VERSION_NUM >= 502
		msg = lua_pushstring(L, extramsg);
#else
		msg = lua_pushfstring(L, "%s", extramsg);
#endif
	}

	return luaL_argerror(L, narg, msg);
}


static int typeerror (lua_State *L, int narg, const char *argname, const char *tname) {
  const char *msg = lua_pushfstring(L, "%s expected, got %s", tname, luaX_typename(L, narg));

  return luaX_argerror(L, narg, argname, msg);
}


static int tag_error (lua_State *L, int narg, const char *argname, int tag) {
  return typeerror(L, narg, argname, lua_typename(L, tag));
}


void luaX_checktype (lua_State *L, int narg, const char *argname, int t) {
	if (lua_type(L, narg) != t)
		tag_error(L, narg, argname, t);
}


lua_Number luaX_checknumber (lua_State *L, int narg, const char *argname) {
	int isnum = false;
	lua_Number d = lua_tonumberx(L, narg, &isnum);

	if (!isnum) {
		tag_error(L, narg, argname, LUA_TNUMBER);
	}

	return d;
}


lua_Number luaX_optnumber (lua_State *L, int narg, const char *argname, lua_Number def) {
	return luaX_opt(L, luaX_checknumber, narg, argname, def);
}


lua_Integer luaX_checkinteger (lua_State *L, int narg, const char *argname) {
	int isnum = false;
	lua_Integer d = lua_tointegerx(L, narg, &isnum);

	if (!isnum) {
		tag_error(L, narg, argname, LUA_TNUMBER);
	}

	return d;
}


lua_Integer luaX_optinteger (lua_State *L, int narg, const char *argname, lua_Integer def) {
	return luaX_opt(L, luaX_checkinteger, narg, argname, def);
}


const char* luaX_checklstring (lua_State *L, int narg, const char *argname, size_t *len) {
	const char *s = lua_tolstring(L, narg, len);

	if (s == NULL) {
		tag_error(L, narg, argname, LUA_TSTRING);
	}

	return s;
}


const char* luaX_optlstring (lua_State *L, int narg, const char *argname, size_t *len, const char *def) {
	if (lua_isnoneornil(L, narg)) {
		if (len != NULL) {
			*len = def ? strlen(def) : 0;
		}

		return def;
	}

	return luaX_checklstring(L, narg, argname, len);
}


void* luaX_checkudata (lua_State *L, int narg, const char *argname, const char *tname) {
	void *d = luaL_testudata(L, narg, tname);

	if (d == NULL) {
		typeerror(L, narg, argname, tname);
	}

	return d;
}


void* luaX_optudata (lua_State *L, int narg, const char *argname, const char *tname, void* def) {
	if (lua_isnoneornil(L, narg)) {
		return def;
	}

	return luaX_checkudata(L, narg, argname, tname);
}


bool luaX_isclass (lua_State *L, int narg, const char *cname) {
	narg = lua_absindex(L, narg);

	lua_getmetatable(L, narg);
	luaL_getmetatable(L, cname);

	if (lua_rawequal(L, -1, -2)) {
		lua_pop(L, 2);
		return true;
	}

	if (!luaL_getmetafield(L, narg, LUAX_STR_CLASS)) {
		lua_pop(L, 2);
		return false;
	}

	if (lua_rawequal(L, -1, -2)) {
		lua_pop(L, 3);
		return true;
	}

	lua_pop(L, 3);

	return false;
}


void* luaX_testclass (lua_State *L, int narg, const char *cname) {
	void *p = lua_touserdata(L, narg);
	if (p == NULL) {
		return NULL;
	}

	if (luaX_isclass(L, narg, cname)) {
		return p;
	}

	return NULL;
}

void* luaX_checkclass (lua_State *L, int narg, const char *cname, const char *argname) {
	void *p = luaX_testclass(L, narg, cname);
	if (p == NULL) {
		typeerror(L, narg, argname, cname);
	}

	return p;
}


const char* luaX_status2str (int status) {
	switch (status) {
		case LUA_OK:
			return "no error";
		case LUA_YIELD:
			return "yield";
		case LUA_ERRRUN:
			return "runtime error";
		case LUA_ERRSYNTAX:
			return "syntax error";
		case LUA_ERRMEM:
			return "memory allocation error";
		case LUA_ERRGCMM:
			return "garbage collector error";
		case LUA_ERRERR:
			return "message handler error";
		default:
			return "unknown error";
	}
}
