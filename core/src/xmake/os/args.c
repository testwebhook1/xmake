/*!A cross-platform build utility based on Lua
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2015-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        args.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "os.args"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_os_args_append(tb_string_ref_t result, tb_char_t const* cstr, tb_size_t size)
{
    // check
    tb_assert_and_check_return(size < TB_PATH_MAXN);

    // wrap and escape characters
    tb_char_t ch;
    tb_size_t n = 0;
    tb_char_t const* p = cstr;
    tb_bool_t wrap_quote = tb_false;
    tb_char_t buff[TB_PATH_MAXN];
    tb_size_t m = tb_arrayn(buff);
    while ((ch = *p) && n < m)
    {
        if (ch == '\"') 
        {
            if (n < m) buff[n++] = '\\';
            wrap_quote = tb_true;
        }
        else if (ch == ' ' || ch == '(' || ch == ')') wrap_quote = tb_true;
        if (n < m) buff[n++] = ch;
        p++;
    }
    tb_assert_and_check_return(n < m);
    buff[n] = '\0';

    // wrap "" and escape '\\' if exists escape characters and spaces?
    if (wrap_quote) 
    {
        tb_string_chrcat(result, '\"');
        tb_size_t i = 0;
        tb_char_t ch;
        for (i = 0; i < n; i++)
        {
            ch = buff[i];
            if (ch == '\\') // escape the '\\' characters in ""
                tb_string_chrcat(result, '\\');
            tb_string_chrcat(result, ch);
        }
        tb_string_chrcat(result, '\"');
    }
    else if (n) tb_string_cstrncat(result, buff, n);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_int_t xm_os_args(lua_State* lua)
{
    // check
    tb_assert_and_check_return_val(lua, 0);

    // init result
    tb_string_t result;
    tb_string_init(&result);

    // make string from arguments list
    if (lua_istable(lua, 1))
    {
        tb_size_t i = 0;
        tb_size_t n = lua_objlen(lua, 1);
        for (i = 1; i <= n; i++)
        {
            // add space 
            if (i != 1) tb_string_chrcat(&result, ' ');

            // add argument
            lua_pushnumber(lua, (tb_int_t)i);
            lua_rawget(lua, 1);
            size_t size = 0;
            tb_char_t const* cstr = luaL_checklstring(lua, -1, &size);
            if (cstr && size)
                tb_os_args_append(&result, cstr, size);
            lua_pop(lua, 1);
        }
    }
    else
    {
        size_t size = 0;
        tb_char_t const* cstr = luaL_checklstring(lua, 1, &size);
        if (cstr && size)
            tb_os_args_append(&result, cstr, size);
    }

    // return result
    tb_size_t size = tb_string_size(&result);
    if (size) lua_pushlstring(lua, tb_string_cstr(&result), size);
    else lua_pushnil(lua);
    tb_string_exit(&result);
    return 1;
}