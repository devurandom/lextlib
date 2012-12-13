#ifndef LEXTLIB_H
#define LEXTLIB_H

#include <lua.h>

#include "lextlib_global.h"

#if LUA_VERSION_NUM < 502
#	define LUA_OK 0
#	define LUA_ERRGCMM 5
#	define LUA_OPEQ 0
#	define LUA_OPLT 1
#	define LUA_OPLE 2
/* lua_rawlen: Not entirely correct, but should work anyway */
#	define lua_rawlen lua_objlen
/* lua_...uservalue: Something very different, but it should get the job done */
#	define lua_getuservalue lua_getfenv
#	define lua_setuservalue lua_setfenv
#	define lua_tonumberx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tonumber(L,(i))) : (*(b)=0, 0))
#	define lua_tointegerx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tointeger(L,(i))) : (*(b)=0, 0))
#	define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
#	define luaL_setfuncs(L,l,n) (assert(n==0), luaL_register(L,NULL,l))
#	define luaL_setmetatable(L,t) ( \
	luaL_getmetatable(L,t), \
	!lua_isnil(L,-1) ? 0 : ( \
		lua_newtable(L), \
		lua_pushvalue(L,-1), \
		lua_setfield(L,LUA_REGISTRYINDEX,t), \
		1 \
	) \
)
#	define luaL_testudata(L,i,t) ( \
	lua_getmetatable(L,(i)), \
	!lua_isnil(L,-1) ? ( \
		luaL_getmetatable(L,t), \
		!lua_isnil(L,-1) ? ( \
			lua_compare(L,-1,-2,LUA_OPEQ) ? ( \
				lua_pop(L,2), \
				lua_touserdata(L,(i)) \
			) : ( \
				lua_pop(L,2), \
				NULL \
			) \
		) : ( \
			lua_pop(L,2), \
			NULL \
		) \
	) : ( \
		lua_pop(L,1), \
		NULL \
	) \
)
#	define lua_compare(L,i1,i2,op) ( \
	(op) == LUA_OPEQ ? ( \
		lua_equal(L,(i1),(i2)) \
	) : ( \
		(op) == LUA_OPLT ? ( \
			lua_lessthan(L,(i1),(i2)) \
		) : ( \
			(op) == LUA_OPLE ? ( \
				lua_lessthan(L,(i1),(i2)) || lua_equal(L,(i1),(i2)) \
			) : ( \
				lua_error(L, "Call to lua_compare with unsupported operator %d", op) \
			) \
		) \
	) \
)
#endif

#define luaX_setconst(L, idx, val) \
	(lua_pushinteger((L), (val)), lua_setfield((L), (idx) > 0 ? (idx) : (idx)-1, #val))

#define luaX_opt(L, func, narg, argname, def) \
	(lua_isnoneornil((L), (narg)) ? (def) : func((L), (narg), (argname)))

#define luaX_stkdbg(L) { \
		printf("L%d -> %d items\n", __LINE__, lua_gettop(L)); \
		printf(" [-1] %s\n", lua_typename(L, lua_type(L, -1))); \
		printf(" [-2] %s\n", lua_typename(L, lua_type(L, -2))); \
		printf(" [-3] %s\n", lua_typename(L, lua_type(L, -3))); \
		printf(" [-4] %s\n", lua_typename(L, lua_type(L, -4))); \
		printf(" [-5] %s\n", lua_typename(L, lua_type(L, -5))); \
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

extern void* luaX_testclass (lua_State *L, int narg, const char *cname, const char *argname);
extern void* luaX_checkclass (lua_State *L, int narg, const char *cname, const char *argname);
extern void* luaX_optclass (lua_State *L, int narg, const char *cname, const char *argname, void *def);


static inline const char* luaX_checkstring (lua_State *L, int narg, const char *argname) {
	return luaX_checklstring(L, narg, argname, NULL);
}


static inline const char* luaX_optstring (lua_State *L, int narg, const char *argname, const char *def) {
	return luaX_optlstring(L, narg, argname, NULL, def);
}

#endif
