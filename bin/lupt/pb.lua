--ver260211
function fmt(str, ...)
  local hole, lst = "{}", {...}
  local res = str:gsub(hole, function()
    if #lst == 0 then return hole end
    return table.remove(lst, 1)
  end)
  return res
end

function fmtf(s,...) return string.format(s,...) end

function var_dump(t)
  if "table"==type(t) then
    local k,v;  for k,v in pairs(t) do print(k,v) end
  else print(t) end
end

function dprint(...)
  if os.getenv("PB_DEBUG") then print(...)end
end

function _wc(fname, txt) -- write text to file
  local fout = io.open(fname, "w"); fout:write(txt); fout:close()
end
function _rc(fname)
  local fout = io.open(fname, "r");local txt=fout:read("*a"); fout:close(); return txt
end

string.replace = string.gsub
string.slice = string.sub
os.system = os.execute

function table.shift(t) if #t==0 then return nil end return table.remove(t, 1) end
function table.unshift(t, ...) local v = {...} for i = #v, 1, -1 do table.insert(t, 1, v[i]) end return #t end
function table.pop(t) if #t==0 then return nil end return table.remove(t) end
function table.push(t, ...) local v={...} for _, n in ipairs(v) do table.insert(t, n) end return #t end

function string.split(s, delim)
  local t = {}
  local p_start, p_next = 1, 1
  local s_sec, p_eq = "", 1
  repeat
    p_next = s:find(delim, p_start, true)
    if p_next then
      s_sec = s:sub(p_start, p_next-1)
      p_start = p_next + 1
    else
      s_sec = s:sub(p_start)
    end
    table.insert(t, s_sec)
  until not p_next
  return t
end

function string.indexOf(str, srch, pos)
  pos = pos or 1
  local startIdx = string.find(str, srch, pos, true)
  return startIdx and startIdx or -1
end

function string.trim(str)
  local s = str:gsub("^%s+", "")
  s = s:gsub("%s+$", "")  -- drop 2nd number
  return s
end

function string.search(str, pat)
  local s, e = string.find(str, pat, 1, false)
  if s then return s, e else return -1 end
end

function os.popen(cmd)
  local function fkopen(cmd)
    os.execute(cmd.." >fk.txt")
    local fd = io.open("fk.txt")
    local t = fd:read("*a")
    fd:close()
    return t
  end
  local function popen(cmd)
    local fd = io.popen(cmd)
    local t = fd:read("*a")
    fd:close()
    return t
  end
  cmd = cmd:gsub("`", "\\`") -- avoid ` work in shell
  local ok, r = pcall(popen, cmd)
  if not ok then
    r = fkopen(cmd)
  end
  return string.trim(r)
end

function ts(v)
  if type(v)=="number" then v=tostring(v) end
  local ret = "accept 6/8/10/13 digit, but input len is "..#v
  if #v ==8 then
    local a={year=tonumber(v:sub(1,4)), month=tonumber(v:sub(5,6)), day=tonumber(v:sub(7,8)), hour=0}
    ret = os.time(a)
  elseif #v ==6 then
    local a={year=2000+tonumber(v:sub(1,2)), month=tonumber(v:sub(3,4)), day=tonumber(v:sub(5,6)), hour=0}
    ret = os.time(a)
  elseif #v==10 then
    ret = os.date("%Y-%m-%d %H:%M:%S", tonumber(v))
  elseif #v==13 then
    ret = os.date("%Y-%m-%d %H:%M:%S", math.floor(tonumber(v)/1000))
  end
  return ret
end

function tie(t)
  return setmetatable(t, {__index=table,
  __tostring=function(t)local k=next(t) return "tbl-arr:"..#t..",1st_k:"..(k and k or "nil")end})
end

function map(f, lst)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #lst do
		local x = lst[_index_0]
		_accum_0[_len_0] = f(x)
		_len_0 = _len_0 + 1
	end
	return _accum_0
end

function filter(f, lst)
	local _accum_0 = { }
	local _len_0 = 1
	for _index_0 = 1, #lst do
		local x = lst[_index_0]
		if f(x) then
			_accum_0[_len_0] = x
			_len_0 = _len_0 + 1
		end
	end
	return _accum_0
end

function reduce(f, lst, init)
	local acc = init
	for idx = 1, #lst do
	  local x = lst[idx]
      if idx==1 and not init then
        acc = lst[idx]
      else
		acc = f(acc, x)
      end
	end
	return acc
end

table.join = table.concat
if table.unpack then _G.unpack = table.unpack end
table.map=function(t, f) return map(f, t) end
table.filter=function(t,f) return filter(f, t) end
table.reduce=function(t,f,i) return reduce(f,t,i) end

function range(a, b)
	if not b then
		b, a = a, 0
	end
	local _accum_0 = { }
	local _len_0 = 1
	for x = a, b - 1 do
		_accum_0[_len_0] = x
		_len_0 = _len_0 + 1
	end
	return _accum_0
end

-- set.lua like JS
set = {}
set.__index = set
function set.new(items)
    local self = setmetatable({}, set)
    self._items = {}
    if items then
        for _, v in ipairs(items) do self:add(v) end
    end
    return self
end
function set:add(value) self._items[value] = true; return self end
function set:delete(value) self._items[value] = nil; return self end
function set:has(value) return self._items[value] ~= nil end
function set:clear() self._items = {}; return self end
function set:pairs() return pairs(self._items) end
function set:values()
  local keys = {}
  for k in pairs(self._items) do table.insert(keys, k) end
  return keys
end
function set:__tostring()
  local vals = {}
  for k in pairs(self._items) do table.insert(vals, tostring(k)) end
  return "set{" .. table.concat(vals, ", ") .. "}"
end

--
-- json.lua
--
-- Copyright (c) 2020 rxi
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy of
-- this software and associated documentation files (the "Software"), to deal in
-- the Software without restriction, including without limitation the rights to
-- use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
-- of the Software, and to permit persons to whom the Software is furnished to do
-- so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.
--

JSON = { _version = "0.1.2" }

-------------------------------------------------------------------------------
-- Encode
-------------------------------------------------------------------------------

local encode

local escape_char_map = {
  [ "\\" ] = "\\",
  [ "\"" ] = "\"",
  [ "\b" ] = "b",
  [ "\f" ] = "f",
  [ "\n" ] = "n",
  [ "\r" ] = "r",
  [ "\t" ] = "t",
}

local escape_char_map_inv = { [ "/" ] = "/" }
for k, v in pairs(escape_char_map) do
  escape_char_map_inv[v] = k
end


local function escape_char(c)
  return "\\" .. (escape_char_map[c] or string.format("u%04x", c:byte()))
end


local function encode_nil(val)
  return "null"
end


local function encode_table(val, stack)
  local res = {}
  stack = stack or {}

  -- Circular reference?
  if stack[val] then error("circular reference") end

  stack[val] = true

  if rawget(val, 1) ~= nil or next(val) == nil then
    -- Treat as array -- check keys are valid and it is not sparse
    local n = 0
    for k in pairs(val) do
      if type(k) ~= "number" then
        error("invalid table: mixed or invalid key types")
      end
      n = n + 1
    end
    if n ~= #val then
      error("invalid table: sparse array")
    end
    -- Encode
    for i, v in ipairs(val) do
      table.insert(res, encode(v, stack))
    end
    stack[val] = nil
    return "[" .. table.concat(res, ",") .. "]"

  else
    -- Treat as an object
    for k, v in pairs(val) do
      if type(k) ~= "string" then
        error("invalid table: mixed or invalid key types")
      end
      table.insert(res, encode(k, stack) .. ":" .. encode(v, stack))
    end
    stack[val] = nil
    return "{" .. table.concat(res, ",") .. "}"
  end
end


local function encode_string(val)
  return '"' .. val:gsub('[%z\1-\31\\"]', escape_char) .. '"'
end


local function encode_number(val)
  -- Check for NaN, -inf and inf
  if val ~= val or val <= -math.huge or val >= math.huge then
    error("unexpected number value '" .. tostring(val) .. "'")
  end
  return string.format("%.14g", val)
end


local type_func_map = {
  [ "nil"     ] = encode_nil,
  [ "table"   ] = encode_table,
  [ "string"  ] = encode_string,
  [ "number"  ] = encode_number,
  [ "boolean" ] = tostring,
}


encode = function(val, stack)
  local t = type(val)
  local f = type_func_map[t]
  if f then
    return f(val, stack)
  end
  error("unexpected type '" .. t .. "'")
end


function JSON.stringify(val)
  return ( encode(val) )
end


-------------------------------------------------------------------------------
-- Decode
-------------------------------------------------------------------------------

local parse

local function create_set(...)
  local res = {}
  for i = 1, select("#", ...) do
    res[ select(i, ...) ] = true
  end
  return res
end

local space_chars   = create_set(" ", "\t", "\r", "\n")
local delim_chars   = create_set(" ", "\t", "\r", "\n", "]", "}", ",")
local escape_chars  = create_set("\\", "/", '"', "b", "f", "n", "r", "t", "u")
local literals      = create_set("true", "false", "null")

local literal_map = {
  [ "true"  ] = true,
  [ "false" ] = false,
  [ "null"  ] = nil,
}


local function next_char(str, idx, set, negate)
  for i = idx, #str do
    if set[str:sub(i, i)] ~= negate then
      return i
    end
  end
  return #str + 1
end


local function decode_error(str, idx, msg)
  local line_count = 1
  local col_count = 1
  for i = 1, idx - 1 do
    col_count = col_count + 1
    if str:sub(i, i) == "\n" then
      line_count = line_count + 1
      col_count = 1
    end
  end
  error( string.format("%s at line %d col %d", msg, line_count, col_count) )
end


local function codepoint_to_utf8(n)
  -- http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=iws-appendixa
  local f = math.floor
  if n <= 0x7f then
    return string.char(n)
  elseif n <= 0x7ff then
    return string.char(f(n / 64) + 192, n % 64 + 128)
  elseif n <= 0xffff then
    return string.char(f(n / 4096) + 224, f(n % 4096 / 64) + 128, n % 64 + 128)
  elseif n <= 0x10ffff then
    return string.char(f(n / 262144) + 240, f(n % 262144 / 4096) + 128,
                       f(n % 4096 / 64) + 128, n % 64 + 128)
  end
  error( string.format("invalid unicode codepoint '%x'", n) )
end


local function parse_unicode_escape(s)
  local n1 = tonumber( s:sub(1, 4),  16 )
  local n2 = tonumber( s:sub(7, 10), 16 )
   -- Surrogate pair?
  if n2 then
    return codepoint_to_utf8((n1 - 0xd800) * 0x400 + (n2 - 0xdc00) + 0x10000)
  else
    return codepoint_to_utf8(n1)
  end
end


local function parse_string(str, i)
  local res = ""
  local j = i + 1
  local k = j

  while j <= #str do
    local x = str:byte(j)

    if x < 32 then
      decode_error(str, j, "control character in string")

    elseif x == 92 then -- `\`: Escape
      res = res .. str:sub(k, j - 1)
      j = j + 1
      local c = str:sub(j, j)
      if c == "u" then
        local hex = str:match("^[dD][89aAbB]%x%x\\u%x%x%x%x", j + 1)
                 or str:match("^%x%x%x%x", j + 1)
                 or decode_error(str, j - 1, "invalid unicode escape in string")
        res = res .. parse_unicode_escape(hex)
        j = j + #hex
      else
        if not escape_chars[c] then
          decode_error(str, j - 1, "invalid escape char '" .. c .. "' in string")
        end
        res = res .. escape_char_map_inv[c]
      end
      k = j + 1

    elseif x == 34 then -- `"`: End of string
      res = res .. str:sub(k, j - 1)
      return res, j + 1
    end

    j = j + 1
  end

  decode_error(str, i, "expected closing quote for string")
end


local function parse_number(str, i)
  local x = next_char(str, i, delim_chars)
  local s = str:sub(i, x - 1)
  local n = tonumber(s)
  if not n then
    decode_error(str, i, "invalid number '" .. s .. "'")
  end
  return n, x
end


local function parse_literal(str, i)
  local x = next_char(str, i, delim_chars)
  local word = str:sub(i, x - 1)
  if not literals[word] then
    decode_error(str, i, "invalid literal '" .. word .. "'")
  end
  return literal_map[word], x
end


local function parse_array(str, i)
  local res = {}
  local n = 1
  i = i + 1
  while 1 do
    local x
    i = next_char(str, i, space_chars, true)
    -- Empty / end of array?
    if str:sub(i, i) == "]" then
      i = i + 1
      break
    end
    -- Read token
    x, i = parse(str, i)
    res[n] = x
    n = n + 1
    -- Next token
    i = next_char(str, i, space_chars, true)
    local chr = str:sub(i, i)
    i = i + 1
    if chr == "]" then break end
    if chr ~= "," then decode_error(str, i, "expected ']' or ','") end
  end
  return res, i
end


local function parse_object(str, i)
  local res = {}
  i = i + 1
  while 1 do
    local key, val
    i = next_char(str, i, space_chars, true)
    -- Empty / end of object?
    if str:sub(i, i) == "}" then
      i = i + 1
      break
    end
    -- Read key
    if str:sub(i, i) ~= '"' then
      decode_error(str, i, "expected string for key")
    end
    key, i = parse(str, i)
    -- Read ':' delimiter
    i = next_char(str, i, space_chars, true)
    if str:sub(i, i) ~= ":" then
      decode_error(str, i, "expected ':' after key")
    end
    i = next_char(str, i + 1, space_chars, true)
    -- Read value
    val, i = parse(str, i)
    -- Set
    res[key] = val
    -- Next token
    i = next_char(str, i, space_chars, true)
    local chr = str:sub(i, i)
    i = i + 1
    if chr == "}" then break end
    if chr ~= "," then decode_error(str, i, "expected '}' or ','") end
  end
  return res, i
end


local char_func_map = {
  [ '"' ] = parse_string,
  [ "0" ] = parse_number,
  [ "1" ] = parse_number,
  [ "2" ] = parse_number,
  [ "3" ] = parse_number,
  [ "4" ] = parse_number,
  [ "5" ] = parse_number,
  [ "6" ] = parse_number,
  [ "7" ] = parse_number,
  [ "8" ] = parse_number,
  [ "9" ] = parse_number,
  [ "-" ] = parse_number,
  [ "t" ] = parse_literal,
  [ "f" ] = parse_literal,
  [ "n" ] = parse_literal,
  [ "[" ] = parse_array,
  [ "{" ] = parse_object,
}


parse = function(str, idx)
  local chr = str:sub(idx, idx)
  local f = char_func_map[chr]
  if f then
    return f(str, idx)
  end
  decode_error(str, idx, "unexpected character '" .. chr .. "'")
end


function JSON.parse(str)
  if type(str) ~= "string" then
    error("expected argument of type string, got " .. type(str))
  end
  local res, idx = parse(str, next_char(str, 1, space_chars, true))
  idx = next_char(str, idx, space_chars, true)
  if idx <= #str then
    decode_error(str, idx, "trailing garbage")
  end
  return res
end

