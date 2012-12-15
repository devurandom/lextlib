#ifndef LEXTLIB_LUA52_H
#define LEXTLIB_LUA52_H


#include <assert.h>

#include <lua.h>
#include <lauxlib.h>


#if LUA_VERSION_NUM < 502
/* Error codes for lua_load: */
#	define LUA_OK 0
/* WARNING: This error does not exist in Lua 5.1 */
#	define LUA_ERRGCMM (-1)

/* Comparison types for lua_compare: */
#	define LUA_OPEQ 0
#	define LUA_OPLT 1
#	define LUA_OPLE 2

/* WARNING: Not entirely correct, but should work anyway */
#	define lua_rawlen lua_objlen

/* WARNING: Something very different, but it might get your job done */
#	define lua_getuservalue lua_getfenv
#	define lua_setuservalue lua_setfenv

#	define lua_tonumberx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tonumber(L,(i))) : (*(b)=0, 0))
#	define lua_tointegerx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tointeger(L,(i))) : (*(b)=0, 0))

#	define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))
/* WARNING: Does not work if you need common upvalues */
#	define luaL_setfuncs(L,l,n) (assert(n==0), luaL_register(L,NULL,l))

#	define luaL_setmetatable(L,t) (luaL_getmetatable(L,t), lua_setmetatable(L,-2))

/* WARNING: Probably slower than Lua 5.2's implementation */
#	define luaL_testudata(L,i,t) ( \
	lua_getmetatable(L,(i)), \
	!lua_isnil(L,-1) ? ( \
		luaL_getmetatable(L,t), \
		!lua_isnil(L,-1) ? ( \
			lua_equal(L,-1,-2) ? ( \
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

/* WARNING: Probably quite a bit slower than Lua 5.2's implementation */
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
				luaL_error(L, "Call to lua_compare with unsupported operator %d", op) \
			) \
		) \
	) \
)
#endif


#endif
