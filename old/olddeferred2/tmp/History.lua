History={}

History.__index=History

function History.__newindex(t,k,b)
end

function History.new(size,v)
  local h={size=size,ind=1}

  for i=1,size do
    h[i]=v
  end

  setmetatable(h,History)
  return h
end

function History.ipairs(h)
  return h.iter, h, 0
end

function History.iter(h,i)
  i=i+1
  local ii=h.ind-i

  if ii<=0 then
    ii=h.size+ii
  end

  if i>h.size then
    return nil
  end

  return i,h[ii]
end

function History.ipairs2(h)
  return h.iter2, h, 0
end

function History.iter2(h,i)
  local ii=h.ind+i
  i=i+1

  if ii>h.size then
    ii=ii-h.size
  end

  if i>h.size then
    return nil
  end

  return i,h[ii]
end

function History.insert(h,v)
  h[h.ind]=v
  h.ind=h.ind+1

  if h.ind > h.size then
    h.ind=1
  end
end

function History.last(h)
  return h[h.ind]
end