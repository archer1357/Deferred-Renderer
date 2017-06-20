function myglUniform(t)
for k,v in pairs(t) do
myglUniform2(k,v)
end
end
require "Shader"
require "Texture"
require "VertexArray"
require "Model"

geometry_programs={
  normal=gNormalProg,
  bump=gBumpProg,
  -- bump=gRelief2Prog,
  relief=gReliefProg,
  relief2=gRelief2Prog,
  __index=function(t,k)
    return gGreyProg
  end}

setmetatable(geometry_programs,geometry_programs)

function init()
  local model=load_model("untitled.lua")

  local axisVao=VertexArray(
    {VertexBuffer(floats{0,0,0,1,0,0, 0,0,0,0,1,0, 0,0,0,0,0,1}),0,3,GL_FLOAT},
    {VertexBuffer(floats{1,0,0, 1,0,0, 0,1,0, 0,1,0, 0,0,1, 0,0,1}),1,3,GL_FLOAT})

  screenVao=VertexArray(
    {VertexBuffer(floats{-1,-1,0, 1,-1,0, -1,1,0, 1,1,0}),0,3,GL_FLOAT})



  roomDraw=getModelMesh(model.meshes.Room2)
  teapotDraw=getModelMesh(model.meshes.Teapot)
  torusDraw=getModelMesh(model.meshes.Torus)
  sphereDraw=getModelMesh(model.meshes.Icosphere)
  cubeDraw=getModelMesh(model.meshes.Cube)

  lightAction1=getModelAction(model.actions["LampAction"])
  teapotAction=getModelAction(model.actions["TeapotAction.002"])
  sphereAction=getModelAction(model.actions["IcosphereAction"])
  torusAction=getModelAction(model.actions["TorusAction.002"])
  cubeAction=getModelAction(model.actions["CubeAction.003"])



  teapot={
    shadowVao=myglGenVertexArray(),
    shadowPosBuf=myglGenBuffer(),
    shadowIndBuf=myglGenBuffer(),
    draw=function(self) teapotDraw(geometry_programs,textures,self.uniforms) end,
    shapeIndsNum=model.meshes.Teapot.shape_indices_num,
    shapeVerts=model.meshes.Teapot.shape_vertices,
    shapeInds=model.meshes.Teapot.shape_indices}

  sphere={
    shadowVao=myglGenVertexArray(),
    shadowPosBuf=myglGenBuffer(),
    shadowIndBuf=myglGenBuffer(),
    draw=function(self) sphereDraw(geometry_programs,textures,self.uniforms) end,
    shapeIndsNum=model.meshes.Icosphere.shape_indices_num,
    shapeVerts=model.meshes.Icosphere.shape_vertices,
    shapeInds=model.meshes.Icosphere.shape_indices}

  torus={
    shadowVao=myglGenVertexArray(),
    shadowPosBuf=myglGenBuffer(),
    shadowIndBuf=myglGenBuffer(),
    draw=function(self) torusDraw(geometry_programs,textures,self.uniforms) end,
    shapeIndsNum=model.meshes.Torus.shape_indices_num,
    shapeVerts=model.meshes.Torus.shape_vertices,
    shapeInds=model.meshes.Torus.shape_indices}

  cube={
    shadowVao=myglGenVertexArray(),
    shadowPosBuf=myglGenBuffer(),
    shadowIndBuf=myglGenBuffer(),
    draw=function(self) cubeDraw(geometry_programs,textures,self.uniforms) end,
    shapeIndsNum=model.meshes.Cube.shape_indices_num,
    shapeVerts=model.meshes.Cube.shape_vertices,
    shapeInds=model.meshes.Cube.shape_indices}

  room={
    shadowVao=myglGenVertexArray(),
    shadowPosBuf=myglGenBuffer(),
    shadowIndBuf=myglGenBuffer(),
    draw=function(self) roomDraw(geometry_programs,textures,self.uniforms) end,
    shapeIndsNum=model.meshes.Room2.shape_indices_num,
    shapeVerts=model.meshes.Room2.shape_vertices,
    shapeInds=model.meshes.Room2.shape_indices}

  function drawAxis()
    glBindVertexArray(axisVao)
    glDrawArrays(GL_LINES,0,6)
  end
end

if not keyframe then keyframe=110 end
keyframeRate=0.3
function step(st)
  keyframe=keyframe+keyframeRate
end

function render_shadows(light,objects,test)
  for i,object in ipairs(objects) do
    local func=nil

    if test then
      func=getShadowVolumeSilhouette
      object.shadowDrawMode=GL_LINES
    else
      func=getShadowVolume
      object.shadowDrawMode=GL_TRIANGLES
    end

    local aaa,bbb=func(object.modelMat,object.shapeVerts,object.shapeInds,light.pos,nil,nil)

    glBindVertexArray(object.shadowVao)
    glBindBuffer(GL_ARRAY_BUFFER,object.shadowPosBuf)
    glBufferData(GL_ARRAY_BUFFER,0,aaa,GL_DYNAMIC_DRAW)
    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(0)


    object.shadowVertsNum=bbb
  end
end

function render(st,it)
  glViewport(0,0,client_width,client_height)

  local interpKeyframe=keyframe+keyframeRate*it

  -- print(interpKeyframe)
  local zNearFar={1,100}
  projMat=mat4.perspective(math.pi/4,client_width/client_height,
                           zNearFar[1],zNearFar[2])
  local viewMat=mat4.inverse(
    mat4.multiply(
      mat4.translate{0,15,0},
      mat4.rotateY(1.5),
      mat4.rotateY(time/7),
      mat4.translate{0,0,22},
      mat4.rotateX(-0.6),
      mat4.identity()))

  -- local viewMat=mat4.inverse(
  --   mat4.multiply(
  --     mat4.translate{0,22,0},
  --     -- mat4.rotateY(1.6),
  --     mat4.rotateY(0.19),
  --     mat4.translate{-4,0,13},
  --     mat4.rotateX(-0.9),
  --     mat4.identity()))


  -- local viewMat=mat4.inverse(mat4.multiply(mat4.rotateY(time/2)))
  -- local viewMat=mat4.identity()
  local viewProjMat=mat4.multiply(projMat,viewMat)
  local invProjMat=mat4.inverse(projMat)

  local light1={}
  -- light1.pos=mat4.multiply(lightAction1(interpKeyframe), {0,0,0,1})
  light1.pos={math.cos(time)*2-4,math.sin(time)*1+2,-5,1}
  light1.modelViewProjMat=mat4.multiply(viewProjMat,mat4.translate(light1.pos))
  light1.viewPos=mat4.multiply(viewMat,light1.pos)
  light1.uniforms={
    u_lightPos=light1.viewPos,
    u_invProjMat=invProjMat,
    u_lightAtten={0.6,0.01,0.01}}

  teapot.modelMat=teapotAction(interpKeyframe)
  teapot.modelViewMat=mat4.multiply(viewMat,teapot.modelMat)
  teapot.modelViewProjMat=mat4.multiply(projMat,teapot.modelViewMat)
  teapot.uniforms={
    u_projMat=projMat,
    u_modelViewProjMat=teapot.modelViewProjMat,
    u_modelViewMat=teapot.modelViewMat,
    u_normalMat=mat4.normal3(teapot.modelViewMat)
  }

  sphere.modelMat=sphereAction(interpKeyframe)
  sphere.modelViewMat=mat4.multiply(viewMat,sphere.modelMat)
  sphere.modelViewProjMat=mat4.multiply(projMat,sphere.modelViewMat)
  sphere.uniforms={
    u_projMat=projMat,
    u_modelViewMat=sphere.modelViewMat,
    u_modelViewProjMat=sphere.modelViewProjMat,
    u_normalMat=mat4.normal3(sphere.modelViewMat)
  }

  torus.modelMat=torusAction(interpKeyframe)
  torus.modelViewMat=mat4.multiply(viewMat,torus.modelMat)
  torus.modelViewProjMat=mat4.multiply(projMat,torus.modelViewMat)
  torus.uniforms={
    u_projMat=projMat,
    u_modelViewProjMat=torus.modelViewProjMat,
    u_modelViewMat=torus.modelViewMat,
    u_normalMat=mat4.normal3(torus.modelViewMat)
  }

  cube.modelMat=cubeAction(interpKeyframe)
  cube.modelViewMat=mat4.multiply(viewMat,cube.modelMat)
  cube.modelViewProjMat=mat4.multiply(projMat,cube.modelViewMat)
  cube.uniforms={
    u_projMat=projMat,
    u_scale=0.35,
    u_zNearFar=zNearFar,
    u_modelViewMat=cube.modelViewMat,
    u_modelViewProjMat=cube.modelViewProjMat,
    u_normalMat=mat4.normal3(cube.modelViewMat)
  }

  room.modelMat=mat4.identity()
  room.modelViewMat=mat4.multiply(viewMat,room.modelMat)
  room.modelViewProjMat=mat4.multiply(projMat,room.modelViewMat)
  room.uniforms={
-- u_texScale={2,3},
    u_projMat=projMat,
    u_scale=0.045,u_bias=-0.035,
    u_scale2=0.02,
    -- u_scale=0.04,u_bias=-0.03,
    u_zNearFar=zNearFar,
    u_modelViewMat=room.modelViewMat,
    u_modelViewProjMat=room.modelViewProjMat,
    u_normalMat=mat4.normal3(room.modelViewMat)}

  local skybox={}
  skybox.uniforms={
    -- u_modelViewProjMat=mat4.multiply(projMat,mat4.translate{0,0,-7.0},mat4.rotateY(time)),
    -- u_modelViewProjMat=mat4.multiply(mat4.rotateY(time)),
    u_projMat=projMat,
    u_viewMat=viewMat
    -- u_viewMat=mat4.inverse(mat4.rotateEuler{time/4,time/8,time/5})
  }

--
  -- objects={}
  objects={sphere,teapot,torus,cube}

  -- geometry pass begin
  glEnable(GL_CULL_FACE)
  glCullFace(GL_BACK)
  glFrontFace(GL_CCW)

  glDisable(GL_BLEND)

  glDisable(GL_STENCIL_TEST)

  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

  glEnable(GL_DEPTH_TEST)
  glDepthMask(GL_TRUE)

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferred.fbo);

  glClearColor(0,0,0,1)
  glClear(bit32.bor(GL_COLOR_BUFFER_BIT,GL_DEPTH_BUFFER_BIT))

  --room
  room:draw()

  --sphere
  sphere:draw()

  --teapot
  teapot:draw()

  --torus
  torus:draw()

  --cube
  cube:draw()

  --geometry pass end

  glBindFramebuffer(GL_FRAMEBUFFER, 0)

  glEnable(GL_CULL_FACE)

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)

  glEnable(GL_DEPTH_TEST)
  glDepthMask(GL_TRUE)

  glActiveTexture(0);
  glBindTexture(GL_TEXTURE_2D, deferred.normal_texture);
  -- glBindSampler(0,0);

  glActiveTexture(1);
  glBindTexture(GL_TEXTURE_2D, deferred.color_texture);
  -- glBindSampler(1,0);

  glActiveTexture(2);
  glBindTexture(GL_TEXTURE_2D, deferred.depth_texture);
  -- glBindSampler(2,0);

  glClear(GL_DEPTH_BUFFER_BIT)

  glBindVertexArray(screenVao)
  glUseProgram(depthCopyProg);
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);


  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)

  glClearColor(0,0,0,1)
  glClearStencil(0)
  glClear(bit32.bor(GL_COLOR_BUFFER_BIT,GL_STENCIL_BUFFER_BIT))



  --lights
  glEnable(GL_CULL_FACE)
  glCullFace(GL_BACK)
  glFrontFace(GL_CCW)

  glEnable(GL_BLEND)
  glBlendEquation(GL_FUNC_ADD)
  glBlendFunc(GL_ONE, GL_ONE)

--~   glEnable(GL_STENCIL_TEST)
--~   glStencilFunc(GL_EQUAL, 0, 0xFF)
--~   glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

--~   glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

--~   glDisable(GL_DEPTH_TEST)
--~   glDepthMask(GL_FALSE)

--~   glBindVertexArray(screenVao)

  --light1 shadow volumes
  -- glDisable(GL_CULL_FACE)
  glDisable(GL_BLEND)

  glEnable(GL_STENCIL_TEST)
  --glStencilFunc(GL_ALWAYS, 0, 0xff)
  --glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP)
  --glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP)

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE)

  glEnable(GL_DEPTH_TEST)
  glDepthMask(GL_FALSE)

  glClearStencil(0)
  glClear(GL_STENCIL_BUFFER_BIT)
  -- glDrawBuffer(GL_NONE);

  render_shadows(light1,objects,false)

  glUseProgram(fShadowProg)
  myglUniform({u_lightPos=light1.viewPos})

  -- glDepthFunc(GL_LEQUAL)

  -- glEnable(GL_POLYGON_OFFSET_FILL)
  -- glPolygonOffset(0.1,21)
  -- glPolygonOffset(1,100)

  glCullFace(GL_FRONT);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);

  for k,object in pairs(objects) do
    myglUniform(object.uniforms)
    glBindVertexArray(object.shadowVao)
    glDrawArrays(object.shadowDrawMode,0,object.shadowVertsNum)
  end

  glCullFace(GL_BACK);
  glStencilFunc(GL_ALWAYS, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

  for k,object in pairs(objects) do
    myglUniform(object.uniforms)
    glBindVertexArray(object.shadowVao)
    glDrawArrays(object.shadowDrawMode,0,object.shadowVertsNum)
  end



  -- glCullFace(GL_BACK);

--~   glDisable(GL_POLYGON_OFFSET_FILL)
--~   glPolygonOffset(0,0)


  -- glDepthFunc(GL_LESS)
  -- render_shadows(light1,objects,false)

  --
  -- glDrawBuffer(GL_BACK);
  glEnable(GL_CULL_FACE)
  glEnable(GL_BLEND);

  glEnable(GL_STENCIL_TEST)
  glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilFunc(GL_EQUAL, 0x0, 0xFF);

  -- glStencilFunc(GL_EQUAL, 0, 0xFF)
  -- glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

  glDisable(GL_DEPTH_TEST)
  glDepthMask(GL_FALSE)

  --light1

  glBindVertexArray(screenVao)
  glUseProgram(lPointProg)
  myglUniform(light1.uniforms)
  glDrawArrays(GL_TRIANGLE_STRIP,0,4)

  glClearStencil(0)
  glClear(GL_STENCIL_BUFFER_BIT)

  --
  glBindVertexArray(screenVao)
  glUseProgram(lAmbientProg)

  -- glDrawArrays(GL_TRIANGLE_STRIP,0,4)

  --test
  do
    -- glDisable(GL_BLEND)
    local prog=lNormalsProg
    -- local prog=lPositionsProg
    glUseProgram(prog)
    myglUniform({u_invProjMat=invProjMat,u_zNearFar=zNearFar})
    -- light_pass()
    -- glDrawArrays(GL_TRIANGLE_STRIP,0,4)
  end


  --forward
  glEnable(GL_CULL_FACE)
  glCullFace(GL_BACK)
  glFrontFace(GL_CCW)

  glDisable(GL_BLEND)

  glEnable(GL_STENCIL_TEST)
  glStencilFunc(GL_EQUAL, 0, 0xFF)
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP)

  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE)

  glEnable(GL_DEPTH_TEST)
  glDepthMask(GL_TRUE)

  --light1 representation
  glUseProgram(nullColProg)
  myglUniform({u_modelViewProjMat=light1.modelViewProjMat})
  -- forward_pass()
  drawAxis()

  --draw shadow volumes
  render_shadows(light1,objects,true)

  glUseProgram(fShadowProg)
  myglUniform({u_lightPos=light1.viewPos})

  glDepthFunc(GL_LEQUAL)
  -- glEnable(GL_BLEND)
  for k,object in pairs(objects) do
    myglUniform(object.uniforms)
    glBindVertexArray(object.shadowVao)
    glDrawArrays(object.shadowDrawMode,0,object.shadowVertsNum)
  end

  -- glDisable(GL_BLEND)
  glDepthFunc(GL_LESS)

  --
  -- glUseProgram(lStencilOutFiniteProg)
  -- stencil_out_pass()
  -- light_pass()
  -- glDrawArrays(GL_TRIANGLE_STRIP,0,4)

  --skybox
  -- glUseProgram(fSkyBoxProg)
  -- myglUniform( skybox.uniforms)

  -- glActiveTexture(3)
  -- glBindTexture(GL_TEXTURE_CUBE_MAP,skyTex)

  -- skyDraw(nil,nil,nil)

  --


end

function on_file_change(f)
  print(string.format("%s updated",f))
  -- for k,v in pairs(package.loaded) do
  --   print(k,v)
  -- end
  package.loaded.Shader = nil
  package.loaded.Model = nil
  -- package.loaded.Texture = nil
  reload()
end

function uninit()
end

function on_key(key,scancode,action,mods)
end

function on_mouse_button(button,action,mods)
  lock_cursor(action==1)
end

function on_mouse_move(x,y)
end

function on_client_size(w,h)
end

function on_cursor_pos(x,y)
end

function on_cursor_enter(entered)
end

function on_scroll(x,y)
end