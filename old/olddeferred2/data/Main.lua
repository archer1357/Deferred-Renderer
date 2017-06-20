
function onFileModified(f)
  if f=="Mat4.lua" then
    package.loaded.Mat4 = nil
    return true
  elseif f=="Animation.lua" then
    package.loaded.Animation = nil
    return true
  elseif f=="Main.lua" then
    return true
  end

  return false
end

require "Mat4"
require "Animation"

useGeometryShadows=true


frame=0
entities={}

lightAttenuation1={0.9,0.01,0.001}
lightStrength1=0.001

entities.light1={
  pointlight=render.pointlight{
    attenuation={0.9,0.1,0.01},
    strength=lightStrength1,
    shadow=true
                              },
  mesh=render.mesh{
    emissive=1,
    unlit=true,
    geometryVaoDraw={"PointLight.geom.json","Mesh.vao.json","default"}},
color={1,1,1},
  -- colorKfs={{0, 1,0.7,0.7},
  --           {1, 0.9,0.6,0.1},
  --           {2, 1,0.3,1},
  --           {3, 0.3,0.7,1},
  --           {4, 1,0.7,0.7}},
  positionKfs={{0, -3,3,-6},
               {3, 3,3,-6},
               {6, 3,3,6},
               {9, -3,3,6},
               {12, -3,3,-6}},
  scale={0.15,0.15,0.15}}

entities.light2={
  pointlight=render.pointlight{
    attenuation=lightAttenuation1,
    strength=lightStrength1,
    shadow=true
},
  mesh=render.mesh{
    geometryVaoDraw={"PointLight.geom.json","Mesh.vao.json","default"},
    emissive=1,
    unlit=true},
  colorKfs={{0, 1,0.5,0.2},
            {5, 1,0.5,0.6},
            {10, 1,0.7,0.4},
            {15, 0.9,0.6,0.8},
            {20, 1,0.5,0.2}},
  positionKfs={{0, -6,2,-6},
               {8, 6,2,-6},
               {16, 6,2,6},
               {24, -6,2,6},
               {32, -6,2,-6}},
  scale={0.15,0.15,0.15}}

entities.light3={
  pointlight=render.pointlight{
    attenuation=lightAttenuation1,
    strength=lightStrength1,
    shadow=true
},
  mesh=render.mesh{
    geometryVaoDraw={"PointLight.geom.json","Mesh.vao.json","default"},
    emissive=1,
    unlit=true},
  positionKfs={{0, 9,1,-9},
               {8, 9,1,9},
               {16, -9,1,9},
               {24, -9,1,-9},
               {32, 9,1,-9}},
  scale={0.15,0.15,0.15}}

entities.light4={
  pointlight=render.pointlight{
    attenuation=lightAttenuation1,
    strength=lightStrength1,
    shadow=true
                              },
  mesh=render.mesh{
    geometryVaoDraw={"PointLight.geom.json","Mesh.vao.json","default"},
    emissive=1,
    unlit=true},
  scale={0.15,0.15,0.15},
  color={0.6,0.6,0.9},
  positionKfs={{0, -7,2,9},
               {10, -7,2,-7},
               {20, 7,2,-7},
               {30, 7,2,9},
               {40, -7,2,9}}}

entities.roomFloor={
  mesh=render.mesh{
    shininess=0.01,
    geometryVaoDraw={"room.json","Mesh.vao.json","Floor"},
    colorTex="4351-diffuse.dds",
    normalTex="4351-normal.dds",
    heightTex="4351-bump.dds",
    material="Parallax"}}

entities.roomStone={
  mesh=render.mesh{
    shininess=0.01,
    geometryVaoDraw={"room.json","Mesh.vao.json","Stone"},
    colorTex="stone-diffuse.dds",
    normalTex="stone-normal.dds",
    heightTex="stone-bump.dds",
    material="Parallax",
--~     bumpBias=-0.01,
--~     bumpScale=0.01
    }}

entities.roomGrey={
  mesh=render.mesh{
    color={0.5,0.5,0.5},
    shininess=0.01,
    geometryVaoDraw={"room.json","Mesh.vao.json","Grey"}}}

entities.cube1={
  mesh=render.mesh{
    shininess=0.001,
    geometryVaoDraw={"cube.json","Mesh.vao.json","default"},
    colorTex="6133-diffuse.dds",
    normalTex="6133-normal.dds",
    heightTex="6133-bump.dds",
    specularTex="6133-specstrength.dds",
    material="Parallax",
    bumpBias=-0.035,
    bumpScale=0.045},
  shadow=render.shadow{
    geometry_cpu="cubeshape.json",
    geometry_gpu="cube_shadow.json"},
  position={0,1,0},
  scale={1,1,1},
   -- rotationKfs={{0, 0,0,0.1}, {100, 0,97,0.1}, {133, 0,0,0.1}}
}

for x=-2,1 do
  for y=-2,1 do
    if x~=0 or y~=0 then

    local s=4
    entities["pillar" .. x .. y]={
      mesh=render.mesh{
        shininess=0.1,
        geometryVaoDraw={"pillar.json","Mesh.vao.json","Stone"},
        colorTex="stone-diffuse.dds",
        normalTex="stone-normal.dds",
        heightTex="stone-bump.dds",
        material="Parallax",
        bumpBias=-0.01,
        bumpScale=0.01},
      position={x*s,0,y*s}}

    entities["pillartop" .. x .. y]={
      mesh=render.mesh{
        shininess=0.01,
        geometryVaoDraw={"pillar.json","Mesh.vao.json","Grey"},
        material="Color"},
      position={x*s,0,y*s}}

      entities["pillar_shadow" .. x .. y]={
        shadow=render.shadow{
          geometry_cpu="pillarshape.json",
          geometry_gpu="pillar_shadow.json"
                            },
        position={x*s,0,y*s}
      }

    end
  end
end
function step(stepTime)
  frame=frame+stepTime
end

--table.insert(t,v)

cameraPosKfs={{-2, -6,7,-22},}
cameraYawKfs={{0, 3.4}}
cameraPitchKfs={{0, -0.3}}
spin=6.7
spinrate=0.5
function run(stepTime,interpTime)
if dospin then spin=spin-stepTime*spinrate end
  local interpFrame=frame+stepTime*interpTime
  local projMat=mat4.perspective(math.pi/4,clientWidth/clientHeight,1,1000)
  local invProjMat=projMat:inverse()
local shadowProjMat=mat4.infinitePerspective(math.pi/4,clientWidth/clientHeight,1)

  local cameraPos={0,10,25} --keyframer(cameraPosKfs,interpFrame,true)
  local cameraYaw=spin --keyframer(cameraYawKfs,interpFrame,true)[1]
if dospin then cameraYaw=cameraYaw+spinrate*interpTime end
  local cameraPitch=-0.37 --keyframer(cameraPitchKfs,interpFrame,true)[1]

  local viewMat=(mat4.rotateY(cameraYaw)*mat4.translate(cameraPos)*mat4.rotateX(cameraPitch)):inverse()
  --
  local viewRotMat=mat4.identity()
  for i,v in ipairs(viewMat) do
    viewRotMat[i]=viewMat[i]
  end
  viewRotMat[4]=0
  viewRotMat[8]=0
  viewRotMat[12]=0

  local viewRotProjMat=projMat*viewRotMat

  render.invProjMat=invProjMat
  render.projMat=projMat
  render.viewRotProjMat=projMat*viewRotMat*mat4.rotateY(5.1)
  --render.shadowProjMat=shadowProjMat
  render.shadowProjMat=projMat

  for k,v in pairs(entities) do
    updateEntity(v,projMat,viewMat,interpFrame)
  end

end


function updateEntity(entity,projMat,viewMat,interpFrame)
  local modelMat=mat4.identity()

  if entity.positionKfs then
    entity.position=keyframer(entity.positionKfs,interpFrame,true)
  end

  if entity.rotationKfs then
    entity.rotation=keyframer(entity.rotationKfs,interpFrame,true)
  end

  if entity.scaleKfs then
    entity.scale=keyframer(entity.scaleKfs,interpFrame,true)
  end

  if entity.position then
    modelMat=mat4.translate(entity.position)
  end

  if entity.rotation then
    modelMat=modelMat*mat4.rotateX(entity.rotation[1])
    modelMat=modelMat*mat4.rotateY(entity.rotation[2])
    modelMat=modelMat*mat4.rotateZ(entity.rotation[3])
  end

  if entity.scale then
    modelMat=modelMat*mat4.scale(entity.scale)
  end

  if entity.colorKfs then
    entity.color=keyframer(entity.colorKfs,interpFrame,false)
  end

  if entity.emissiveKfs then
    entity.emissive=keyframer(entity.emissiveKfs,interpFrame,false)[1]
  end

  if entity.reflectiveKfs then
    entity.reflective=keyframer(entity.reflectiveKfs,interpFrame,false)[1]
  end

  if entity.shininessKfs then
    entity.shininess=keyframer(entity.shininessKfs,interpFrame,false)[1]
  end

  if entity.attenuationKfs then
    entity.attenuation=keyframer(entity.attenuationKfs,interpFrame,false)
  end

  if entity.strengthKfs then
    entity.strength=keyframer(entity.strengthKfs,interpFrame,false)[1]
  end

  if entity.spotExponentKfs then
    entity.spotExponent=keyframer(entity.spotExponentKfs,interpFrame,false)[1]
  end

  if entity.spotCutoffKfs then
    entity.spotCutoff=keyframer(entity.spotCutoffKfs,interpFrame,false)[1]
  end

  local modelViewMat=viewMat*modelMat
  local normalMat=modelViewMat:inverse():transpose():toMat3()

  if entity.mesh then
    local modelViewProjMat=projMat*modelViewMat

    entity.mesh.modelViewProjMat=modelViewProjMat
    entity.mesh.normalMat=normalMat
    entity.mesh.modelViewMat=modelViewMat

    if entity.color then
      entity.mesh.color=entity.color
    end

    if entity.emissive then
      entity.mesh.emissive=entity.emissive
    end

    if entity.reflective then
      entity.mesh.emissive=entity.emissive
    end

    if entity.shininess then
      entity.mesh.shininess=entity.shininess
    end
  end

  if entity.shadow then
    entity.shadow.modelViewMat=modelViewMat
    entity.shadow.modelMat=modelMat
  end

  if entity.pointlight then
    entity.pointlight.viewPos=modelViewMat*{0,0,0,1}
    entity.pointlight.pos=modelMat*{0,0,0,1}

    if entity.color then
      entity.pointlight.color=entity.color
    end

    if entity.attenuation then
      entity.pointlight.attenuation=entity.attenuation
    end

    if entity.strength then
      entity.pointlight.strength=entity.strength
    end
  end

  if entity.spotlight then
    entity.spotlight.viewPos=modelView*{0,0,0,1}
    entity.spotlight.viewDir=modelView:inverse():transpose()*{0,0,-1,0}

    if entity.color then
      entity.spotlight.color=entity.color
    end

    if entity.attenuation then
      entity.spotlight.attenuation=entity.attenuation
    end

    if entity.strength then
      entity.spotlight.strength=entity.strength
    end

    if entity.spotExponent then
      entity.spotlight.spotExponent=entity.spotExponent
    end

    if entity.spotCutoff then
      entity.spotlight.spotCutoff=entity.spotCutoff
    end
  end
end

function onKey(key, scancode,action,mods)
  print(key, scancode,action,mods)

  if key==82 and action==1 then
    dospin= not dospin
  elseif key==83 and action==1 then
    useGeometryShadows=not useGeometryShadows
    enableGeometryShadows(useGeometryShadows)
	
	if useGeometryShadows then
	print("Geometry shadows on")
	else
	print("Geometry shadows off")
	end
  elseif key==68 and action==1 then
    useShadowDebug=not useShadowDebug
    enableShadowDebug(useShadowDebug)
  end
end

function onMouseButton(button,action,mods)
  --print(button,action,mods)

  lockCursor(button~=0 or action~=1)

end

function onCursor(x,y)
end

function onScroll(x,y)
end

function onChar(c)
  --print(c)
end
