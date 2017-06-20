local function min(a,b)
  if a <= b then
    return a
  end

  return b
end

local function max(a,b)
  if a >= b then
    return a
  end

  return b
end

local function saturate(x)
  return max(0, min(1,x))
end

local function smoothStep(a,b,x)
  local t = saturate((x-a)/(b-a));
  return t*t*(3-2*t);
end

local function lerp(f0,f1,u)
  return (1-u)*f0+u*f1;
end

function keyframer(kfs, frame, smooth)
  local kfsNum=#kfs

  if kfsNum == 0 then
    return
  end

  local frame2=math.fmod(frame,kfs[kfsNum][1])

  if kfsNum==1 or frame2 <= kfs[1][1] then
    local t={}

    for i=2,#kfs[1] do
      t[i-1]=kfs[1][i]
    end

    return t
  end

  for i=1,kfsNum-1 do
    local pos1=kfs[i][1]
    local pos2=kfs[i+1][1]

    if frame2>=pos1 and frame2<=pos2 then
      local ii=(frame2-pos1)/(pos2-pos1)
      local out={}

      for j=2,#kfs[1] do
        local val1=kfs[i][j]
        local val2=kfs[i+1][j]

        if smooth then
          out[j-1]=lerp(val1,val2,smoothStep(0,1,ii));
        else
          out[j-1]=lerp(val1,val2,ii);
        end
      end

      return out
    end
  end
end
