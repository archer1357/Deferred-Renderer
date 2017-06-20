local module = {}
module.__index=module

vec3=module

function module.new(v)
  local t={0,0,0}

  if v then
    for i=1,4 do
      t[i]=v[i]
    end
  end

  setmetatable(t,module)
  return t
end

function module.__add(a,b)
  local t={}

  for i=1,3 do
    t[i]=a[i]+b[i]
  end

  setmetatable(t,module)
  return t
end

function module.__sub(a,b)
  local t={}

  for i=1,3 do
    t[i]=a[i]-b[i]
  end

  setmetatable(t,module)
  return t
end

function module.__mul(a,b)

  if type(b)=="number" then
    local t={}

    for i=1,3 do
      t[i]=a[i]*b
    end

    setmetatable(t,module)
    return t
  end

end

function module.__div(a,b)

  if type(b)=="number" then
    local t={}

    for i=1,3 do
      t[i]=a[i]/b
    end

    setmetatable(t,module)
    return t
  end
end

function module.dot(a,b)
  return a[1]*b[1]+a[2]*b[2]+a[3]*b[3]
end

function module.length(v)
  return math.sqrt(module.dot(v,v))
end

function module.normal(v)
  local l=module.length(v)
  local t={}

  for i=1,3 do
    t[i]=v[i]/l
  end
  
  t[4]=v[4]

  setmetatable(t,module)
  return t
end

function module.cross(a,b)
  local t={a[2]*b[3]-a[3]*b[2],a[3]*b[1]-a[1]*b[3],a[1]*b[2]-a[2]*b[1]}
  setmetatable(t,module)
  return t
end

-- function module.__newindex(t,k,v)
--   if k=="x" then
--      t[1]=v
--   elseif k=="y" then
--      t[2]=v
--   elseif k=="z" then
--      t[3]=v
--   elseif k=="w" then
--      t[4]=v
--   end

--   return v
-- end

-- function module.__index(t,k)
--   if k=="x" then
--     return t[1]
--   elseif k=="y" then
--     return t[2]
--   elseif k=="z" then
--     return t[3]
--   elseif k=="w" then
--     return t[4]
--   end
-- end
