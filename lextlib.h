#ifndef LEXTLIB_H
#define LEXTLIB_H


#include <stdio.h>
#include <stdbool.h>

#include <lua.h>

#include "lextlib_global.h"
#include "lextlib_lua52.h"


#define luaX_setconst(L, idx, val) \
	(lua_pushinteger((L), (val)), lua_setfield((L), (idx) > 0 ? (idx) : (idx)-1, #val))

#define luaX_opt(L, func, narg, argname, def) \
	(lua_isnoneornil((L), (narg)) ? (def) : func((L), (narg), (argname)))

#define luaX_stkdbg(L) { \
		int i, top = lua_gettop(L); \
		fprintf(stderr, "L%d -> %d items\n", __LINE__, top); \
		for (i = top; i > 0; i--) { \
			printf(" [%d] %s = %s\n", i, lua_typename(L, lua_type(L, i)), (lua_isstring(L, i) ? lua_tostring(L, i) : "?")); \
		} \
	}

#define luaX_passerr(L, func) { \
		int ret = (func)(L); \
		if (lua_isnil((L), -ret)) { \
			return ret; \
		} \
		else { \
			lua_pop((L), ret); \
		} \
	}


#define LUAX_STR_TYPENAME "luax_typename"
#define LUAX_STR_CLASS "luax_class"


extern const char* luaX_status2str (int status);

extern int luaX_traceback (lua_State *L);
extern int luaX_panic (lua_State *L);

extern void luaX_showstack (lua_State *L);
extern void luaX_error (lua_State *L, const char *fmt, ...) LUAX_DECL_NORETURN;

extern void luaX_preload (lua_State *L, const char *name, lua_CFunction function);
extern void luaX_restrict (lua_State *L);

extern const char *luaX_typename(lua_State *L, int narg);

extern void luaX_checktype (lua_State *L, int narg, const char *argname, int t);
extern int luaX_argerror (lua_State *L, int narg, const char *argname, const char *extramsg);

extern lua_Number luaX_checknumber (lua_State *L, int narg, const char *argname);
extern lua_Number luaX_optnumber (lua_State *L, int narg, const char *argname, lua_Number def);

extern lua_Integer luaX_checkinteger (lua_State *L, int narg, const char *argname);
extern lua_Integer luaX_optinteger (lua_State *L, int narg, const char *argname, lua_Integer def);

extern const char* luaX_checklstring (lua_State *L, int narg, const char *argname, size_t *len);
extern const char* luaX_optlstring (lua_State *L, int narg, const char *argname, size_t *len, const char *def);

extern void* luaX_checkudata (lua_State *L, int narg, const char *tname, const char *argname);
extern void* luaX_optudata (lua_State *L, int narg, const char *tname, const char *argname, void *def);

extern bool luaX_isclass (lua_State *L, int narg, const char *cname);
extern void* luaX_testclass (lua_State *L, int narg, const char *cname);
extern void* luaX_checkclass (lua_State *L, int narg, const char *cname, const char *argname);
extern void* luaX_optclass (lua_State *L, int narg, const char *cname, const char *argname, void *def);


static inline const char* luaX_checkstring (lua_State *L, int narg, const char *argname) {
	return luaX_checklstring(L, narg, argname, NULL);
}


static inline const char* luaX_optstring (lua_State *L, int narg, const char *argname, const char *def) {
	return luaX_optlstring(L, narg, argname, NULL, def);
}

#endif
