
require "Scripts/Animation"
require "Scripts/Programs"
require "Scripts/Textures"
require "Scripts/Models"
require "Scripts/History"
require "Scripts/Entity"

drawings={
  depth={},
  bump={},
  emissive={},
  flat={}}

lights={
  pointShadow={},
  point={},
  spot={},
  spotShadow={},
  directional={},
  directionalShadow={}}

rootEntity=createEntity()
objects={}

do
  screenDraw=models.untitled.meshes.Screen.draw
  screenVao=models.untitled.meshes.Screen.vao
  mouseHistory=History.new(7,{x=0,y=0})

  if not camera then
    camera={yaw=0,pitch=0}
    camera.character=createCharacter(createCapsule(1,3),{position={0,20,0}})
    addCharacter(camera.character)
  end

  do
    local entity=createEntity(rootEntity)
    -- entity.parent=rootEntity
    entity.scale={33,33,33}
    entity.rigidBody=createRigidBody(createPlane({0,1,0},0),{mass=0})
    addRigidBody(entity.rigidBody)

    local mesh=models.untitled.meshes.Plane
    table.insert(drawings.flat,{entity=entity,vao=mesh.vao,draw=mesh.draw})
  end

  do
    local entity=createEntity(rootEntity)
    entity.scale={0.2,0.2,0.2}

    entity.keyframes.position={
      smooth=true,max=900,
      {0,{-11,5,-11}},
      {70,{11,5,-11}},
      {140,{11,5,11}},
      {210,{-11,5,11}},
      {280,{-14,5,-11}},
      {300,{-11,2,-11}},
      {320,{-11,5,-14}},
      {340,{-11,2,-11}},
      {360,{-14,5,-11}},
      {380,{-11,2,-11}},
      {400,{-11,5,-14}},
      {420,{-11,2,-11}},
      {440,{-14,5,-11}},
      {460,{-11,2,-11}},
      {480,{-11,5,-14}},
      {500,{-11,2,-11}},
      {520,{-11,5,-11}}}

    entity.keyframes.color={
      --max=400,
      {0,{1,1,1}},
      {70,{1,0.4,0.6}},
      {140,{0.4,1,0.4}},
      {210,{0.4,0.2,1}},
      {300,{0.7,0.7,0.3}},
      {400,{0.7,0.2,0.7}},
      {500,{0.1,0.7,0.3}},
      {600,{1,1,1}}}

    entity.lightAtten={0.1,0.1,0.01}

    -- entity.keyframes.lightAtten={
    --   {0,{1,0,0}},
    --   {100,{0.0,0.1,0.01}},
    --   {200,{1,0,0}}}

    local mesh=models.untitled.meshes.Cube
    table.insert(drawings.emissive,{entity=entity,vao=mesh.vao,draw=mesh.draw})

    table.insert(lights.point,{entity=entity})
  end

end

curFrame=0
frameRate=0.35

function step(stepTime)
  stepWorld(stepTime)
  curFrame=curFrame+frameRate
end

function render(interpTime)
  myglCheckError()

  local curInterpFrame=curFrame+frameRate*interpTime
  glViewport(0,0,clientWidth,clientHeight)
  updateDeferred()

  local znear=1
  local zfar=200
  local projMat=mat4.perspective(math.pi/4,clientWidth/clientHeight,znear,zfar)
  local invProjMat=mat4.inverse(projMat)
  local viewRotMat=mat4.multiply(
    mat4.rotateY(camera.yaw),
    mat4.rotateX(camera.pitch))
  local viewMat=mat4.multiply(
    mat4.translate{0,5,0},
    -- mat4.translate(camera.character:getPosition()),
    viewRotMat)
  viewRotMat=mat4.inverse(viewRotMat)
  viewMat=mat4.inverse(viewMat)
  local viewRotProjMat=mat4.multiply(projMat,viewRotMat)
  local viewProjMat=mat4.multiply(projMat,viewMat)

  rootEntity.viewMat=viewMat
  rootEntity.projMat=projMat
  rootEntity.frame=curFrame
  rootEntity.frame=curInterpFrame
  rootEntity.interpTime=interpTime
  rootEntity.stepTime=stepTime

  --states
  glEnable(GL_CULL_FACE)
  glCullFace(GL_BACK)
  glFrontFace(GL_CCW)

  glDisable(GL_BLEND)
  glDisable(GL_STENCIL_TEST)

  glEnable(GL_DEPTH_TEST)
  glDepthMask(GL_TRUE)
  glDepthFunc(GL_LEQUAL)

  --bind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferred.fbo);

  --clear
  glClearColor(1,0,0,1)
  glClearDepth(1)
  myglClear(GL_COLOR_BUFFER_BIT,GL_DEPTH_BUFFER_BIT)

  --states
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE)

  --draw depths
  do
    glUseProgram(programs.fNull.program)
    local lastVao=nil

    for i,v in ipairs(drawings.depth) do
      myglUniform("u_modelViewProjMat",v.entity.modelViewProjMat)

      if not lastVao or lastVao~=v.vao then
        glBindVertexArray(v.vao)
        lastVao=v.vao
      end

      v.draw()
    end
  end

  --states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

  --draw flats
  do
    local lastVao=nil
    glUseProgram(programs.gFlat.colored)

    for i,v in ipairs(drawings.flat) do
      myglUniform("u_modelViewProjMat",v.entity.modelViewProjMat)
      myglUniform("u_normalMat",v.entity.normalMat)
      myglUniform("u_col",v.entity.color)

      if not lastVao or lastVao~=v.vao then
        glBindVertexArray(v.vao)
        lastVao=v.vao
      end

      v.draw()
    end
  end

  --draw emissive
  do
    local lastVao=nil
    glUseProgram(programs.gFlat.emissive)

    for i,v in ipairs(drawings.emissive) do
      myglUniform("u_modelViewProjMat",v.entity.modelViewProjMat)
      myglUniform("u_col",v.entity.color)

      if not lastVao or lastVao~=v.vao then
        glBindVertexArray(v.vao)
        lastVao=v.vao
      end

      v.draw()
    end
  end

  --draw bump
  for i,v in ipairs(drawings.bump) do
  end

  --bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, 0)

  --bind textures
  myglBindTexture(GL_TEXTURE_2D, deferred.colorTex,0);
  glBindSampler(0,0);

  myglBindTexture(GL_TEXTURE_2D, deferred.normalTex,1);
  glBindSampler(1,0);

  myglBindTexture(GL_TEXTURE_2D, deferred.depthTex,2);
  glBindSampler(2,0);

  --states
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)

  --clear
  glClearDepth(1)
  myglClear(GL_DEPTH_BUFFER_BIT)

  --render deferred depth texture
  glUseProgram(programs.fDepthCopy.program);

  glBindVertexArray(screenVao)
  screenDraw()

  --states
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)
  glDepthMask(GL_FALSE)

  --clear
  glClearColor(0,1,0,1)
  glClearStencil(0)
  myglClear(GL_COLOR_BUFFER_BIT,GL_STENCIL_BUFFER_BIT)

  --draw sky
  myglBindTexture(GL_TEXTURE_CUBE_MAP,textureCubes.emerald_light, 3)

  glUseProgram(programs.fSkybox.bluish)
  myglUniform("u_viewRotProjMat",viewRotProjMat)

  glBindVertexArray(models.untitled.meshes.InvertedCube.vao)
  models.untitled.meshes.InvertedCube.draw()

  --states
  glEnable(GL_BLEND)

  --point lights
  do
    glBindVertexArray(screenVao)

    glUseProgram(programs.lLighting.point)
    myglUniform("u_invProjMat",invProjMat)

    for i,v in ipairs(lights.point) do
      myglUniform("u_lightPos",v.entity.viewPosition)
      myglUniform("u_lightAtten",v.entity.lightAtten)
      myglUniform("u_lightCol",v.entity.color)

      screenDraw()
    end
  end

  --point lights with shadows
  do
    for i,v in ipairs(lights.pointShadow) do
    end
  end

  --states
  glEnable(GL_CULL_FACE)
  glCullFace(GL_BACK)
  glFrontFace(GL_CCW)

  glDepthMask(GL_FALSE)
  glDisable(GL_DEPTH_TEST)
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

  --bind vao
  glBindVertexArray(screenVao)

  --draw ambient
  -- glUseProgram()
  -- screenDraw()

  --draw emissives
  glUseProgram(programs.lEmissive.program)
  screenDraw()

  --test colors
  -- glUseProgram(programs.lDeferredOutput.colors)
  -- screenDraw()

  --test depth
  -- glUseProgram(programs.lDeferredOutput.linearDepth)
  -- myglUniform("u_zNearFar",{znear,zfar})
  -- screenDraw()

  --test normals
  -- glUseProgram(programs.lDeferredOutput.normals)
  -- glUseProgram(programs.fNull.program)
  -- screenDraw()

  --test positions
  -- glUseProgram(programs.lDeferredOutput.positions)
  -- myglUniform("u_invProjMat",invProjMat)
  -- screenDraw()
end

function onRun()
  --delta time
  local deltaTime=time-lastTime

  --smooth mouse
  mouseHistory:insert{x=0,y=0}
  local smoothX=0
  local smoothY=0
  local smoothVar=1

  --local tblx={}
  --local tbly={}

  for i,v in mouseHistory:ipairs() do
    --tblx[i]=v.x
    --tbly[i]=v.y
    smoothX=smoothX+v.x*smoothVar
    smoothY=smoothY+v.y*smoothVar
    smoothVar=smoothVar*0.35
  end

  smoothX=smoothX*deltaTime
  smoothY=smoothY*deltaTime
  -- smoothX=0
  -- smoothY=0

  --mouse look
  if mouseLook then
    camera.pitch=camera.pitch-smoothY
    camera.yaw=camera.yaw-smoothX
    if camera.pitch > math.pi/2 then camera.pitch=math.pi/2 end
    if camera.pitch < -math.pi/2 then camera.pitch=-math.pi/2 end
  end

  --accumTime
  if not accumTime then
    accumTime=time --global
  end

  --step
  local stepTime=1/60
  local maxSteps=5
  local stepCount=0

  while accumTime+stepTime < time and stepCount < maxSteps do
    accumTime=accumTime+stepTime
    stepCount=stepCount+1
    step(stepTime)
  end

  --accumTime
  if stepCount==maxSteps then
    accumTime=time
  end

  --render
  local interpTime=time-accumTime
  render(interpTime)


end


function onFileModified(f)
  print(f .. " has been modified.")
  package.loaded.Programs = nil
  package.loaded.Textures = nil
  reload()
end

function onKey(key,action,mods)
  -- print('key',key,action,mods)

  if key==87 then --up
  elseif key==83 then --down
  elseif key==65 then --left
  elseif key==68 then --right
  elseif key==32 then --space
  end
end

function onMouseButton(button,action,mods)
  -- print(button,action,mods)

  -- if button==1 then
  mouseLook=action==1
  lockCursor(mouseLook)
  -- end
end

function onCursor(x,y,dx,dy)
  mouseHistory:insert{x=dx,y=dy}
end

function updateDeferred()
  --print(myglError())
  if not deferred then
    deferred={
      fbo=myglGenFramebuffer(),
      normalTex=myglGenTexture(),
      colorTex=myglGenTexture(),
      depthTex=myglGenTexture()}
  end

  if lastClientWidth == clientWidth and lastClientHeight == clientHeight then
    return
  end

  local w=clientWidth
  local h=clientHeight
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferred.fbo);

  --color
  myglBindTexture(GL_TEXTURE_2D, deferred.colorTex)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_FLOAT,0)
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,deferred.colorTex,0)

  --normal
  myglBindTexture(GL_TEXTURE_2D, deferred.normalTex)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA32F,w,h,0,GL_RGBA,GL_FLOAT,0)

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_2D,deferred.normalTex,0)

  --depth
  myglBindTexture(GL_TEXTURE_2D, deferred.depthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,w,h,0,GL_DEPTH_COMPONENT,GL_FLOAT,0)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL)

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D,deferred.depthTex,0)

  --
  myglDrawBuffers(GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1)

  myglCheckFrameBufferStatus(GL_FRAMEBUFFER)

  -- restore default FBO
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

end
