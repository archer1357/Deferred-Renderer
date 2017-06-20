
require "data/Mat4"
require "data/Animation"
require "data/Vec3"
require "data/History"

frame = frame or 0
camera= camera or {
  forward=false,
  backward=false,
  leftward=false,
  rightward=false,
  yawing=0,
  pitching=0,
position={7.2509432592619,-0.19249274004198,2.39618249687},yaw=0.98971819014925,pitch=-0.18118903333307,
-- yaw=0,pitch=0,position={0,0,0},
  -- yaw=6.542592, pitch=0.820522, position=vec3.new {-5.7399013894179,-6.0630242469917,4.2864349433006}, --{1.5,3,20},
 -- yaw=1.3,pitch=-0.7,position={12,5,11},
  velocity=vec3.new{0,0,0},
  speed=1}

entities={}

entities.roomGrey={
  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Grey",

    colorTex="data/texture/wood.dds",
    reliefTex="data/texture/four_NM_height.dds",
    bumpBias=-0.02,
    bumpScale=0.025,
    shininess=0.07,


}
}

entities.roomFloor={
  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Floor",
 colorTex="data/texture/rocks.dds",
 reliefTex="data/texture/rocks_NM_height.dds",

    bumpBias=-0.035,
    bumpScale=0.065,
    shininess=0.07,
  reflective=0.1,
}
}

entities.roomVault={
  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Vault",
 colorTex="data/texture/stones.dds",
     reliefTex="data/texture/stones_NM_height.dds",

    bumpBias=-0.008,
    bumpScale=0.01,
    shininess=0.1,
  }
}
entities.roomRidge={
  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Ridge",

     --colorTex="data/texture/bricks2_color.dds",
     --reliefTex="data/texture/bricks2_parallax.dds",

    bumpBias=-0.025,
    bumpScale=0.03,
    shininess=0.1,
    --reflective=0.8
    }
}
entities.roomCorner={
  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Corner"}
}



entities.roomSide={

  mesh=render.mesh{
    geometry="data/geometry/my2.json",
    vao="data/geometry/Mesh.json",
    draw="Side",
    colorTex="data/texture/stones.dds",
    reliefTex="data/texture/stones_NM_height.dds",

    bumpBias=-0.008,
    bumpScale=0.01,
    parallaxInvertHeight=false,
    shininess=0.1,
  }
}
 entities.roomShadow={
   shadow=render.shadow{robust=false,geometry="data/geometry/my2_shadow.json"}
 }

entities.teapot={
  position={10,-10,0},
  rotation={0,2,0},
  scale={0.5,0.5,0.5},
  mesh=render.mesh{
    geometry="data/geometry/Teapot.json",
    vao="data/geometry/Mesh.json",
    draw="default",
    reflective=0.8},
  shadow=render.shadow{ geometry="data/geometry/Teapot_shadow.json"  }
}
lightAtten={0.5,0.05,0.02}
lightCol4={1.0,0.6,0.8}
lightCol2={0.6,1.0,0.8}
lightCol3={1.0,0.8,0.6}
lightCol1={1,1,1}

if true then
dirlight1=render.directionallight{
  color={0.9,0.7,0.6},
  strength=0.1,
  -- shadow=false
  shadow=true
  }
end

if true then
--~  entities.light1={
--~    position={0,2,0},
--~    rotation={0,2,0},
--~    scale={0.1,0.1,0.1},
--~    mesh=render.mesh{geometry="data/geometry/Sphere.json",
--~                     vao="data/geometry/Mesh.json",
--~                     draw="default",
--~                     color=lightCol1,
--~                     emissive=true},
--~    pointlight=render.pointlight{
--~      color=lightCol1,
--~      attenuation=lightAtten,
--~      strength=0.1,
--~      shadow=true}
--~   }

--~  entities.light2={
--~    position={0,2,0},
--~    rotation={0,2,0},
--~    scale={0.1,0.1,0.1},
--~    mesh=render.mesh{geometry="data/geometry/Sphere.json",
--~                     vao="data/geometry/Mesh.json",
--~                     draw="default",
--~                     color=lightCol2,
--~                     emissive=true},
--~    pointlight=render.pointlight{
--~      attenuation=lightAtten,
--~      strength=0.1,
--~      color=lightCol2,
--~      shadow=true}
--~  }

 entities.light3={
   position={0,2,0},
   rotation={0,2,0},
   scale={0.1,0.1,0.1},
   mesh=render.mesh{geometry="data/geometry/Sphere.json",
                    vao="data/geometry/Mesh.json",
                    draw="default",
                    color=lightCol3,
                    emissive=true},
   pointlight=render.pointlight{
     attenuation=lightAtten,
     strength=0.1,
     color=lightCol3,
     shadow=true}
 }

--~  entities.light4={
--~    position={0,2,0},
--~    rotation={0,2,0},
--~    scale={0.1,0.1,0.1},
--~    mesh=render.mesh{geometry="data/geometry/Sphere.json",
--~                     vao="data/geometry/Mesh.json",
--~                     draw="default",
--~                     color=lightCol4,
--~                     emissive=true},
--~    pointlight=render.pointlight{
--~      attenuation=lightAtten,
--~      strength=0.1,
--~      color=lightCol4,
--~      shadow=true}
--~  }
end

function step(stepTime)
  frame=frame+stepTime

  local rotMat=mat4.rotateY(camera.yaw)*mat4.rotateX(camera.pitch)
  local xaxis=vec3.new{rotMat[1],rotMat[5],rotMat[9]}
  local yaxis=vec3.new{rotMat[2],rotMat[6],rotMat[10]}
  local zaxis=vec3.new{rotMat[3],rotMat[7],rotMat[11]}

  if camera.forward then
    camera.position=camera.position-zaxis*stepTime*5
  end

  if camera.backward then
    camera.position=camera.position+zaxis*stepTime*5
  end

  if camera.leftward then
    camera.position=camera.position-xaxis*stepTime*5
  end

  if camera.rightward then
    camera.position=camera.position+xaxis*stepTime*5
  end
end
blay1=12-6
blay2=12-6
blax=12-6
blay=-7
blaaa={{0, -blax,blay,-blay1},
       {15, blax,blay,-blay1},
       {30, blax,blay, blay2},
       {45, -blax,blay, blay2},
       {60, -blax,blay,-blay1},
}

-- blaaa={{0, 1,0,0},
       -- {4, 4,-3,0},
       -- {8, 1,0, 0},
       -- {12, 4,-3, 0},
       -- {16, 1,0,0},
--}
function run(deltaTime,stepTime,interpTime)
  local interpFrame=frame+stepTime*interpTime

  if entities.light1 then entities.light1.position=keyframer(blaaa,interpFrame,true) end
  if entities.light2 then entities.light2.position=keyframer(blaaa,interpFrame+15,true) end
  if entities.light3 then entities.light3.position=keyframer(blaaa,interpFrame+30,true) end
  if entities.light4 then entities.light4.position=keyframer(blaaa,interpFrame+45,true) end
  -- print(deltaTime)

 

  if mouseLook then
    --camera.pitch=camera.pitch-smoothY
    --camera.yaw=camera.yaw-smoothX
    camera.pitch=camera.pitch-render.smoothMouseY*deltaTime*0.2
    camera.yaw=camera.yaw-render.smoothMouseX*deltaTime*0.2
    
    --print(render.smoothMouseX,render.smoothMouseY)
  end
    if camera.pitch > math.pi/2.1 then camera.pitch=math.pi/2.1 end
    if camera.pitch < -math.pi/2.1 then camera.pitch=-math.pi/2.1 end
 
  local viewMat=(mat4.translate(camera.position)*
             mat4.rotateY(camera.yaw)*
             mat4.rotateX(camera.pitch)):inverse()
 
render.viewMat=viewMat
  if dirlight1 then
    local a=-interpFrame*0.005
    local dir=vec3.new({math.cos(a),-0.7,math.sin(a), 0}):normal()
    dirlight1.dir=dir
  end

  for k,v in pairs(entities) do

    local modelMat=mat4.identity()

    if v.position then
      modelMat=modelMat*mat4.translate(v.position)
    end

    if v.rotation then
      modelMat=modelMat*mat4.rotateX(v.rotation[1])
      modelMat=modelMat*mat4.rotateY(v.rotation[2])
      modelMat=modelMat*mat4.rotateZ(v.rotation[3])
    end

    if v.scale then
      modelMat=modelMat*mat4.scale(v.scale)
    end


    if v.mesh then
      v.mesh.modelMat=modelMat
    end

    if v.shadow then
      v.shadow.modelMat=modelMat
    end

    if v.pointlight then
      v.pointlight.pos=modelMat*{0,0,0,1}
    end

    if v.spotlight then
    end

  end


end

function onKey(key, action,mods)
  if key==49 and action==1 then
    render.shadowDebug=not render.shadowDebug
    print("Shadow debug",render.shadowDebug)
  elseif key==50 and action==1 then
    render.geometryShadows=not render.geometryShadows
    print("Geometry shadows",render.geometryShadows)
  elseif key==65 then
    camera.leftward=action~=0
  elseif key==83 then
    camera.backward=action~=0
  elseif key==68 then
    camera.rightward=action~=0
  elseif key==87 then
    camera.forward=action~=0

  elseif key==80 and action==1 then
    print(camera.position[1],camera.position[2],camera.position[3],camera.yaw,camera.pitch)
  else
    --print(key, action,mods)
  end

end

function onMouseButton(button,action,mods)
  -- print(button,action,mods)
  if button ==0 then
    mouseLook=action~=0
    render.lockCursor=action~=0
  end

end

function onCursor(x,y,dx,dy)

  -- print(dx,dy)
end

function onScroll(x,y)
end

function onChar(c)
  --print(c)
end
