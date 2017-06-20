local module = {}
module.__index=module

mat4=module

function module.identity()
  local t={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}
  setmetatable(t,module)
  return t
end

function module.copy(m)
  local t={}
  for i=1,16 do t[i]=m[i] end
  setmetatable(t,module)
  return t
end
function module.transpose(m)
  local t={}
  setmetatable(t,module)

  for i=0,3 do
    for j=0,3 do
      t[i*4+j +1] = m[j*4+i +1]
      t[j*4+i +1] = m[i*4+j +1]
    end
  end

  return t
end

function module.inverse(m)
  local t={1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}
  setmetatable(t,module)

  local m2={}

  for i=1,16 do
    m2[i]=m[i]
  end

  for K=0,3 do
    local factor = m2[K*4+K +1]

    for q=0,3 do
      local x=K*4+q +1
      local y=K*4+q +1
      m2[x]=m2[x] / factor;
      t[y]=t[y] / factor;
    end

    for L=0,3 do
      if K~=L then
        local coefficient = m2[L*4+K +1];

        for q=0,3 do
          local x=L*4+q +1
          local y=K*4+q +1
          m2[x]=m2[x]-coefficient*m2[y];
          t[x]=t[x]-coefficient*t[y];
        end
      end
    end
  end

  return t
end

function module.toMat3(m)
  local t={}

  for i=0,2 do
    for j=0,2 do
      t[i*3+j +1]=m[i*4+j +1]
    end
  end

  return t
end

function module.__mul(a,b)
  local t={}

  if getmetatable(b) ~= module then
    for i=0,3 do
      t[i +1]=0

      for j=0,3 do
        t[i +1]=t[i +1]+a[i*4+j +1]*b[j +1];
      end
    end
  else
    setmetatable(t,module)

    for i=0,3 do
      for j=0,3 do
        local x=0

        for k=0,3 do
          x=x+a[i*4+k +1]*b[k*4+j +1];
        end

        t[i*4+j +1]=x
      end
    end
  end

  return t
end

function module.infinitePerspective(fov,aspect,znear)
  local h = 1 / math.tan( fov * 0.5 )
  local w= h / aspect
  local epsilon = 0.0001
  local t={w,0,0,0,
           0,h,0,0,
           0,0,1-epsilon,znear*(epsilon-1),
           0,0,1,0}
  --local yScale=1/math.tan(fov/2)
  --local xScale=yScale/aspect

 -- local t={xScale,0,0,0,
  --         0,yScale,0,0,
   --        0,0,-1,-2*znear,
   --        0,0,-1,0}

  setmetatable(t,module)
  return t
end

function module.perspective(fov,aspect,znear,zfar)
  local yScale=1/math.tan(fov/2)
  local xScale=yScale/aspect
  local nearmfar=znear-zfar

  local t={xScale,0,0,0,
           0,yScale,0,0,
           0,0,(zfar+znear)/nearmfar,2*zfar*znear/nearmfar,
           0,0,-1,0}

  setmetatable(t,module)
  return t
end

function module.translate(v)
  local t={1,0,0,v[1], 0,1,0,v[2], 0,0,1,v[3], 0,0,0,1}
  setmetatable(t,module)
  return t
end


function module.scale(v)
  local t={v[1],0,0,0, 0,v[2],0,0, 0,0,v[3],0, 0,0,0,1}
  setmetatable(t,module)
  return t
end


function module.rotateX(a)
  local c=math.cos(a)
  local s=math.sin(a)

  local t={1,0,0,0,
           0,c,-s,0,
           0,s,c,0,
           0,0,0,1}

  setmetatable(t,module)
  return t
end

function module.rotateY(a)
  local c=math.cos(a)
  local s=math.sin(a)

  local t={c,0,s,0,
           0,1,0,0,
           -s,0,c,0,
           0,0,0,1}

  setmetatable(t,module)
  return t
end

function module.rotateZ(a)
  local c=math.cos(a)
  local s=math.sin(a)

  local t={c,-s,0,0,
           s,c,0,0,
           0,0,1,0,
           0,0,0,1}

  setmetatable(t,module)
  return t
end
