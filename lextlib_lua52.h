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

#	define lua_absindex(L, i) (((i) > 0 || (i) < LUA_REGISTRYINDEX) ? (i) : lua_gettop(L)+(i)+1)

/* WARNING: Something very different, but it might get your job done */
#	define lua_getuservalue lua_getfenv
#	define lua_setuservalue lua_setfenv

/* WARNING: Probably slower than Lua 5.2's implementation */
#	define lua_compare(L,i1,i2,op) luaX52_lua_compare(L,(i1),(i2),(op))

#	define lua_tonumberx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tonumber(L,(i))) : (*(b)=0, 0))
#	define lua_tointegerx(L,i,b) (lua_isnumber(L,(i)) ? (*(b)=1, lua_tointeger(L,(i))) : (*(b)=0, 0))

#	define luaL_newlib(L,l) (lua_newtable(L), luaL_register(L,NULL,l))

# define luaL_requiref(L,l,f,g) luaX52_luaL_requiref(L,(l),(f),(g))

/* WARNING: Does not work if you need common upvalues */
#	define luaL_setfuncs(L,l,n) (assert(n==0), luaL_register(L,NULL,l))

#	define luaL_setmetatable(L,t) (luaL_getmetatable(L,t), lua_setmetatable(L,-2))

/* WARNING: Probably slower than Lua 5.2's implementation */
# define luaL_testudata(L,i,t) luaX52_luaL_testudata(L,(i),(t))

static inline int luaX52_lua_compare(lua_State *L, int index1, int index2, int op) {
	switch (op) {
	case LUA_OPEQ:
		return lua_equal(L, index1, index2);
	case LUA_OPLT:
		return lua_lessthan(L, index1, index2);
	case LUA_OPLE:
		return lua_lessthan(L, index1, index2) || lua_equal(L, index1, index2);
	default:
		return luaL_error(L, "Call to lua_compare with unsupported operator %d", op);
	}
}

static inline void luaX52_luaL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb) {
	int l_package_loaded = -1;
	lua_getglobal(L, "package");
	if (!lua_isnil(L, -1)) {
		lua_getfield(L, -1, "loaded");
		l_package_loaded = lua_gettop(L);
	}
	
	lua_pushcfunction(L, openf);
	lua_pushstring(L, modname);
	lua_call(L, 1, 1);

	if (l_package_loaded >= 0 && !lua_isnil(L, l_package_loaded)) {
		lua_pushvalue(L, -1);
		lua_setfield(L, l_package_loaded, modname);
	}
	
	if (glb) {
		lua_pushvalue(L, -1);
		lua_setglobal(L, modname);
	}
}

static inline void *luaX52_luaL_testudata(lua_State *L, int arg, const char *tname) {
	lua_getmetatable(L, arg);
	if (!lua_isnil(L, -1)) {
		luaL_getmetatable(L, tname);
		if (!lua_isnil(L, -1)) {
			if (lua_equal(L, -1, -2)) {
				lua_pop(L, 2);
				return lua_touserdata(L, arg);
			}
		}

		lua_pop(L, 2);
	}
	else {
		lua_pop(L, 1);
	}
	
	return NULL;
}

#endif


#endif
