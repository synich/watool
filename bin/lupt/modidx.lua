local function conv2lua(src, dst)
  local fd = io.open(src, "r")
  local od = io.open(dst, "w")
  local function one_li()
    return fd:read()
  end
  local function out_f(tok)
    od:write(tok);od:write("\n")
  end
  -- conv condition
  local function conv_li(one)
    if one:match("[%w_]+%[[-+*/_%w]+%]") then -- a[1] -> a[0]
      one = one:gsub("([%w_]+)%[([-+*/_%w]+)%]", function(v,i)
        return v.."[("..i..")+1]" end)
    end
    if one:match("[%w_]+<[-%d:]+>") then -- a<1> ->a:sub(1)
      one = one:gsub("([%w_]+)<([-%d:]+)>", function(v,r)
          local pos,s,e = r:find(":")
          if pos then
            if 1==pos then s=0;e=tonumber(r:sub(2))
            elseif #r==pos then s=tonumber(r:sub(1,pos-1));e=-1
            else s=tonumber(r:sub(1,pos-1));e=tonumber(r:sub(pos+1))
            end
            if s>=0 then s=s+1 end;if e<0 then e=e-1 end
          else -- single access
            s=tonumber(r)+1;e=s
          end
          return v.."\\sub("..s..","..e..")"
        end)
    end
    return one
  end
  -- convert src
  local fin = false
  repeat
    local li = one_li()
    if li == nil then fin = true
    else
      local to = conv_li(li)
      out_f(to)
    end
  until fin
  fd:close()
  od:close()
end

local function main()
  local mod_y = arg[1]:gsub(".[%w]+$", ".m%0")
  local run_l = arg[1]:gsub(".[%w]+$", ".lua")
  conv2lua(arg[1], mod_y)
  if 0==os.execute("yue -c --target=5.1 -o "..run_l.." "..mod_y) then
    local cmd,j = "pb lua "..run_l, 2
    for j = 2,#arg do cmd = cmd.." "..arg[j] end
    os.execute(cmd)
  end
end

main()
