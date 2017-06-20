
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

require "data/Mat4"
require "data/Animation"

useGeometryShadows=true


if not frame then frame=0 end
entities={}
lightAttenuation1={0.4,0.1,0.01}
-- lightAttenuation1={0.9,0.1,0.01}
lightStrength1=0.1
h=3
entities.light3={
  pointlight=render.pointlight{
    attenuation=lightAttenuation1,
    strength=lightStrength1,
    shadow=true
},
  mesh=render.mesh{
    --geometryVaoDraw={"PointLight.json","Mesh.json","default"},
        geometry="data/old/PointLight.json",
    vao="data/geometry/Mesh.json",
    draw="default",
    emissive=1,
    unlit=true},
  positionKfs={
  {0, 0,h,4},
  {4, 0,h,-3},
  {8, 0,h,4},

			   },
  scale={0.05,0.05,0.05}}

  entities.light4={
  pointlight=render.pointlight{
    attenuation=lightAttenuation1,
    strength=lightStrength1,
    shadow=true
},
  mesh=render.mesh{
    --geometryVaoDraw={"PointLight.json","Mesh.json","default"},
        geometry="data/old/PointLight.json",
    vao="data/geometry/Mesh.json",
    draw="default",
    emissive=1,
    unlit=true},
	color={0.5,0.8,1},
  positionKfs={
  {0, 4,h,-4},
               {5, 4,h,4},
               {10, -4,h,4},
               {15, -4,h,-4},
               {20, 0,h,-4},
               {25, 0,h,4},
               {30, -4,h,4},
               {35, -4,h,0},
               {40, 4,h,0},
			   {45, 4,h,-4},
			   },
  scale={0.05,0.05,0.05}}



-- entities.roomFloor={
--   mesh=render.mesh{
--     shininess=0.01,
--     geometryVaoDraw={"room.json","Mesh.json","Floor"},
--     colorTex="4351-diffuse.dds",
--     normalTex="4351-normal.dds",
--     heightTex="4351-bump.dds",
--     material="Parallax",
--    -- material="Color",

-- 	}}

-- entities.roomStone={
--   mesh=render.mesh{
--     shininess=0.01,
--     geometryVaoDraw={"room.json","Mesh.json","Stone"},
--     colorTex="stone-diffuse.dds",
--     normalTex="stone-normal.dds",
--     heightTex="stone-bump.dds",
--     material="Parallax",
--     --    material="Color",
--     bumpBias=-0.03,
--     bumpScale=0.06
--     }}

-- entities.roomGrey={
--   mesh=render.mesh{
--     color={0.5,0.5,0.5},
--     shininess=0.01,
--     geometryVaoDraw={"room.json","Mesh.json","Grey"}}}

entities.roomFloor={
  mesh=render.mesh{
    shininess=0.01,
   -- geometryVaoDraw={"room2.json","Mesh.json","Floor"},
     geometry="data/old/room2.json",
     -- geometry="data2/geometry/room2.json",
    vao="data/geometry/Mesh.json",
    draw="Floor",
    colorTex="data/texture/6133-diffuse.dds",
    normalTex="data/texture/6133-normal.dds",
    heightTex="data/texture/6133-bump.dds",

    --colorTex="6133-diffuse.dds",
    --normalTex="6133-normal.dds",
   -- heightTex="6133-bump.dds",

    -- colorTex="4351-diffuse.dds",
    -- normalTex="4351-normal.dds",
    -- heightTex="4351-bump.dds",
    material="Parallax",

    bumpBias=-0.02,
    bumpScale=0.03

	}}

entities.roomStone={
  mesh=render.mesh{
    shininess=0.1,
--~     geometryVaoDraw={"room2.json","Mesh.json","Stone"},
--~     colorTex="stone-diffuse.dds",
--~     normalTex="stone-normal.dds",
--~     heightTex="stone-bump.dds",
    geometry="data/old/room2.json",
    -- geometry="data2/geometry/room2.json",
    vao="data/geometry/Mesh.json",
    draw="Stone",
    colorTex="data/texture/stone-diffuse.dds",
    normalTex="data/texture/stone-normal.dds",
    heightTex="data/texture/stone-bump.dds",
    material="Parallax",
    --    material="Color",
    bumpBias=-0.03,
    bumpScale=0.04
    }}

entities.roomGrey={
  mesh=render.mesh{
    color={0.5,0.5,0.5},
    shininess=0.1,
 geometry="data/old/room2.json",
     -- geometry="data2/geometry/room2.json",
    vao="data/geometry/Mesh.json",
    draw="Grey",
    -- geometryVaoDraw={"room2.json","Mesh.json","Grey"}
}}

entities.roomShadow={shadow=render.shadow{
                       geometry="data/old/room2_shadow.json"

                       -- geometry="data2/geometry/room2_shadow.json"
                                         }}

 entities.cube1={
   mesh=render.mesh{
     shininess=0.001,
     geometry="data/geometry/Teapot.json",
    vao="data/geometry/Mesh.json",
    draw="default",
    -- geometryVaoDraw={"cube.json","Mesh.json","default"},
     colorTex="data/texture/6133-diffuse.dds",
     normalTex="data/texture/6133-normal.dds",
     heightTex="data/texture/6133-bump.dds",
     specularTex="data/texture/6133-specstrength.dds",
   --  material="Parallax",
     material="Color",
     bumpBias=-0.035,
     bumpScale=0.045},
   shadow=render.shadow{ geometry="data/geometry/Teapot_shadow.json"
    -- geometry_cpu="cubeshape.json",
    -- geometry_gpu="cube_shadow.json"
    },
   position={2,0.5,0},
   scale={0.3,0.3,0.3},
 }

-- entities.SPHERE1={
--   mesh=render.mesh{
--   reflective=1.0,
--     shininess=0.001,
--     geometryVaoDraw={"sphere.json","Mesh.json","default"},
--     material="Color"},
--   shadow=render.shadow{
--     geometry_cpu="sphere_shape.json",
--     geometry_gpu="sphere_shadow.json"},
--   position={0,6,0},
--   scale={2,2,2},
-- }

-- for x=-1,1 do
--   for y=-1,1 do
--     if x~=0 or y~=0 then

--     local s=3
--     entities["pillar" .. x .. y]={
--       mesh=render.mesh{
--         shininess=0.1,
--         geometryVaoDraw={"pillar.json","Mesh.json","Stone"},
--         colorTex="stone-diffuse.dds",
--         normalTex="stone-normal.dds",
--         heightTex="stone-bump.dds",
--         material="Parallax",
--     --material="Color",
--         bumpBias=-0.01,
--         bumpScale=0.01},
--       position={x*s,0,y*s}}

--     entities["pillartop" .. x .. y]={
--       mesh=render.mesh{
--         shininess=0.01,
--         geometryVaoDraw={"pillar.json","Mesh.json","Grey"},
--         material="Color"},
--       position={x*s,0,y*s}}

--       entities["pillar_shadow" .. x .. y]={
--         shadow=render.shadow{
--           geometry_cpu="pillarshape.json",
--           geometry_gpu="pillar_shadow.json"
--                             },
--         position={x*s,0,y*s}
--       }

--     end
--   end
-- end

 --render.cubeEnvTex="CubeTest.dds"
render.cubeEnvTex="blue_light.cube"
-- render.cubeEnvTex="SwedishRoyalCastle.dds"
-- render.cubeEnvTex="NissiBeach2.dds"
--render.cubeEnvTex="FishPond.dds"

function step(stepTime)
  frame=frame+stepTime
end

--table.insert(t,v)
cameraHeight=19
cameraPosKfs={{0, 0,cameraHeight,3},{10, 5,cameraHeight,3},{20, 0,cameraHeight,3}}
cameraYawKfs={{0, 0}}
cameraPitchKfs={{0, -0.3}}
spin=6.7
spinrate=0.1
function run(stepTime,interpTime)
if dospin then spin=spin-stepTime*spinrate end
  local interpFrame=frame+stepTime*interpTime
  local projMat=mat4.perspective(math.pi/4,clientWidth/clientHeight,1,1000)
  local invProjMat=projMat:inverse()
local shadowProjMat=mat4.infinitePerspective(math.pi/4,clientWidth/clientHeight,1)

  local cameraPos=keyframer(cameraPosKfs,interpFrame,true)
  local cameraYaw=0.4 --keyframer(cameraYawKfs,interpFrame,true)[1]
if dospin then cameraYaw=cameraYaw+spinrate*interpTime end
  local cameraPitch=-1.2 --keyframer(cameraPitchKfs,interpFrame,true)[1]

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

  render.invViewMat=viewMat:inverse();
  render.invProjMat=invProjMat
  render.projMat=projMat
  render.viewRotProjMat=projMat*viewRotMat
  --*mat4.rotateY(5.1)
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
  local invModelMat=modelMat:inverse()
  local modelViewProjMat=projMat*modelViewMat

  if entity.mesh then

    entity.mesh.modelViewProjMat=modelViewProjMat
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
    entity.shadow.invModelMat=invModelMat
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

function onKey(key, action,mods)
  print(key, action,mods)

  if key==68 then
    if action==1 then
      render.shadowDebug=not render.shadowDebug
      print("Shadow debug",render.shadowDebug)
    end
  elseif key==83 then
    if action==1 then
      render.geometryShadows=not render.geometryShadows
      print("Geometry shadows",render.geometryShadows)
    end
  end
end

function onMouseButton(button,action,mods)
  --print(button,action,mods)

  -- lockCursor(button~=0 or action~=1)

end

function onCursor(x,y)
end

function onScroll(x,y)
end

function onChar(c)
  --print(c)
end
