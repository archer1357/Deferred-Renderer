bl_info = {
  "name": "MyModel",
  "author": "me",
  "version": (1,0,0),
  "blender": (2,6,2),
  "location": "File > Export",
  "description": "Export my custom data format",
  "warning": "",
  "wiki_url": "",
  "tracker_url": "",
  "category" : "Import-Export"}

import bpy,bpy_extras,struct,os,mathutils,functools,base64,math,json,re

#=rotY(-pi/2)*rotX(-pi/2)
#=[0,0,-1, 0,1,0, 1,0,0]*[1,0,0, 0,0,1, 0,-1,0]
#=[0,1,0, 0,0,1, 1,0,0]

def get_xyz_index(i):
  if i==1:
    return 0
  elif i==2:
    return 1
  else:
    return 2
    
def get_euler_index(i):
  return i
  if i==1:
    return 0
  elif i==2:
    return 1
  else:
    return 2
  # zxy
  
def get_x(v):
  return v[1] or 0

def get_y(v):
  return v[2] or 0

def get_z(v):
  return v[0] or 0

def get_xyz(v):
  return (get_x(v),get_y(v),get_z(v))

def do_materials(file_format,fh):
  fh.write('materials={}')
  mymaterials=[ma for ma in bpy.data.materials if ma.users > 0]

  for ma in mymaterials:
    fh.write('\n\nmaterials["%s"] = {'%(ma.name))
    fh.write('\n\tname="%s",'%ma.name)
    fh.write('\n\tdiffuse={%g,%g,%g},'%tuple(ma.diffuse_color))
    fh.write('\n\talpha=%g,'%ma.alpha)
    fh.write('\n\temit=%g,'%ma.emit)
    fh.write('\n\tspecular={%g,%g,%g},'%tuple(ma.specular_color))
    fh.write('\n\tspecular_hardness=%g,'%ma.specular_hardness)
    fh.write('\n\tspecular_intensity=%g,'%ma.specular_intensity)
    fh.write('\n\tproperties={')

    for k,v in ma.items():
      if isinstance(v, str):
        fh.write('\n\t\t["%s"]="%s",'%(k,v))
      elif isinstance(v, (int, float, complex)):
        fh.write('\n\t\t["%s"]=%g,'%(k,v))

    fh.write('},')
    fh.write('\n\ttextures={')
    texslots=ma.texture_slots

    for i,texslot in enumerate(texslots):
      if texslot != None and texslot.texture.type=='IMAGE':
        fh.write('\n\t\t[%i]={'%(i+1))
        fh.write('\n\t\t\tname="%s",'%texslot.name)
        fn=re.sub("^//","",texslot.texture.image.filepath)
        fh.write('\n\t\t\tfile="%s",'%fn)
        uv=texslot.uv_layer if texslot.texture_coords == 'UV' else ""
        fh.write('\n\t\t\tuv="%s"'%uv)
        fh.write('},')

    fh.write('}')
    fh.write('}')

def calc_tangent_space(v1,v2,v3,uv1,uv2,uv3,n):
  edge1=v2-v1
  edge2=v3-v1
  uv_edge1=uv2-uv1
  uv_edge2=uv3-uv1
  cp=uv_edge1.x * uv_edge2.y - uv_edge1.y * uv_edge2.x

  if cp == 0.0:
    return mathutils.Vector((0,0,0,0))

  r = 1.0 / cp
  sdir=(uv_edge2.y*edge1 - uv_edge1.y*edge2)*r
  tdir=(uv_edge1.x*edge2 - uv_edge2.x*edge1)*r
  tg=(sdir - n * n.dot(sdir)).normalized()
  w=-1.0 if n.cross(sdir).dot(tdir) < 0 else 1.0

  return mathutils.Vector((tg.x,tg.y,tg.z,w))

def do_meshes(file_format,fh):
  fh.write('\n\nmeshes={}')

  mymeshes=[me for me in bpy.data.meshes if me.users > 0]

  for me in mymeshes:
    #tesselated specific section######################################
    me.update(calc_tessface=True)

    #
    uv_names = [x.name for x in me.uv_textures]
    col_names= [x.name for x in me.vertex_colors]
    my_verts_num=0
    my_positions=[]
    my_normals=[]
    my_colors=[[] for x in me.vertex_colors]
    my_texcoords=[[] for x in me.uv_textures]
    my_tangents=[[] for x in me.uv_textures]
    my_vert_inds=dict()
    my_inds=[[]] if len(me.materials)==0 else [[] for ma in me.materials]
    orig_vert_inds=[]

    #gen vert+index for each poly
    for faceInd, face in enumerate(me.tessfaces):
      #
      face_cols = [[] for x in me.vertex_colors]

      for i,x in enumerate(me.vertex_colors):
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color1)
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color2)
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color3)

        if len(face.vertices)==4:
          face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color4)

      #
      face_uvs = [[] for x in me.uv_textures]
      face_uv_images = [[] for x in me.uv_textures]

      for i,x in enumerate(me.uv_textures):
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv1)
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv2)
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv3)

        if len(face.vertices)==4:
          face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv4)

        # img=me.tessface_uv_textures[i].data[faceInd].image
        # face_uv_images.append(None if img == None else img.filepath)

      #
      face_pts=[]

      #
      for faceVertInd,vertInd in enumerate(face.vertices):

        key=''

        #
        pos=me.vertices[vertInd].co
        nor=me.vertices[vertInd].normal if face.use_smooth else face.normal

        key+=' %g %g %g'%(pos[0],pos[1],pos[2])
        key+=' %g %g %g'%(nor[0],nor[1],nor[2])

        #
        cols=[]

        for face_col in face_cols:
          col=face_col[faceVertInd]
          cols.append(col)
          key+=' %g %g %g'%(col[0],col[1],col[2])

        #
        uvs=[]

        for face_uv in face_uvs:
          uv=face_uv[faceVertInd]
          uvs.append(uv)
          key+=' %g %g'%(uv[0],uv[1])

        #
        adj_a=len(face.vertices)-1 if faceVertInd == 0 else faceVertInd-1
        adj_b=0 if faceVertInd == len(face.vertices)-1 else faceVertInd+1
        adj_vert_a = face.vertices[adj_a]
        adj_vert_b = face.vertices[adj_b]

        #
        tangs=[]

        for face_uv in face_uvs:
          p1=me.vertices[face.vertices[adj_a]].co
          p2=me.vertices[vertInd].co
          p3=me.vertices[face.vertices[adj_b]].co

          uv1=face_uv[adj_a]
          uv2=face_uv[faceVertInd]
          uv3=face_uv[adj_b]

          tang=calc_tangent_space(p1,p2,p3, uv1,uv2,uv3, nor)
          tangs.append(tang)
          key+=' %g %g %g'%(tang[0],tang[1],tang[2])

        #
        if key not in my_vert_inds.keys():
          orig_vert_inds.append(vertInd)

          my_vert_inds[key]=my_verts_num
          my_positions.append(pos)
          my_normals.append(nor)

          for i,vertcol in enumerate(me.vertex_colors):
            my_colors[i].append(cols[i])

          for i,uvtex in enumerate(me.uv_textures):
            my_texcoords[i].append(uvs[i])
            my_tangents[i].append(tangs[i])

          my_verts_num+=1

        face_pts.append(my_vert_inds[key])

      #gen vert indices for each poly
      for i in [[0,1,2],[0,2,3]] if len(face.vertices)==4 else [[0,1,2]]:
        for j in i:
          my_inds[face.material_index].append(face_pts[j])

    #vertex_groups : todo
    #ob.vertex_groups
    for vInd in orig_vert_inds:
      for g in me.vertices[vInd].groups:
        ''
        # print(g.group,g.weight)

    #shape vertices, indices
    shape_vertices = [] #array of points
    shape_vertex_indices = dict() #[k]=shape_vertex_ind
    shape_indices=[]

    for faceInd, face in enumerate(me.tessfaces):
      face_pts=[]

      for faceVertInd,vertInd in enumerate(face.vertices):
        pos=me.vertices[vertInd].co
        k='%g %g %g'%(pos[0],pos[1],pos[2])

        if k not in shape_vertex_indices.keys():
          shape_vertex_indices[k]=len(shape_vertices)
          shape_vertices.append(pos)

        face_pts.append(shape_vertex_indices[k])

      for i in [[0,1,2],[0,2,3]] if len(face.vertices)==4 else [[0,1,2]]:
        for j in i:
          shape_indices.append(face_pts[j])

    #shape radius
    shape_radius=0

    for v in shape_vertices:
      l=v.dot(v)

      if l > shape_radius:
        shape_radius=l

    shape_radius=math.sqrt(shape_radius)

    #shape x radius
    shape_x_radius=0

    for v in shape_vertices:
      l = v.y*v.y+v.z*v.z

      if l > shape_x_radius:
        shape_x_radius=l

    shape_x_radius=math.sqrt(shape_x_radius)

    #shape y radius
    shape_y_radius=0

    for v in shape_vertices:
      l = v.x*v.x+v.z*v.z

      if l > shape_y_radius:
        shape_y_radius=l

    shape_y_radius=math.sqrt(shape_y_radius)

    #shape z radius
    shape_z_radius=0

    for v in shape_vertices:
      l = v.x*v.x+v.y*v.y

      if l > shape_z_radius:
        shape_z_radius=l

    shape_z_radius=math.sqrt(shape_z_radius)

    #shape width
    shape_width = 0

    for v in shape_vertices:
      if abs(v.x) > shape_width:
        shape_width=v.x

    #shape height
    shape_height = 0

    for v in shape_vertices:
      if abs(v.y) > shape_height:
        shape_height=v.y

    #shape depth
    shape_depth = 0

    for v in shape_vertices:
      if abs(v.z) > shape_depth:
        shape_depth=v.z

    #shape max x
    shape_max_x = None

    for v in shape_vertices:
      if shape_max_x == None or v.x > shape_max_x:
        shape_max_x=v.x

    #shape min x
    shape_min_x = None

    for v in shape_vertices:
      if shape_min_x == None or v.x < shape_min_x:
        shape_min_x=v.x

    #shape max y
    shape_max_y = None

    for v in shape_vertices:
      if shape_max_y == None or v.y > shape_max_y:
        shape_max_y=v.y

    #shape min y
    shape_min_y = None

    for v in shape_vertices:
      if shape_min_y == None or v.y < shape_min_y:
        shape_min_y=v.y

    #shape max z
    shape_max_z = None

    for v in shape_vertices:
      if shape_max_z == None or v.z > shape_max_z:
        shape_max_z=v.z

    #shape min z
    shape_min_z = None

    for v in shape_vertices:
      if shape_min_z == None or v.z  < shape_min_z:
        shape_min_z=v.z

    #
    shape_size=(shape_width,shape_height,shape_depth)
    shape_min=(shape_min_x,shape_min_y,shape_min_z)
    shape_max=(shape_max_x,shape_max_y,shape_max_z)
    shape_radii=(shape_x_radius,shape_y_radius,shape_z_radius)

    ##################################################################
    #
    my_indices_num=sum([len(inds) for inds in my_inds])

    #store mesh header
    fh.write('\n\nmeshes["%s"]={'%(me.name))
    fh.write('\n\tname="%s",'%me.name)
    fh.write('\n\tvertices_num=%i,'%my_verts_num)
    fh.write('\n\tindices_num=%i,'%my_indices_num)

    #store subsets starts + nums
    fh.write('\n\tsubsets={')

    subset_cur_start=0

    for i,ma in enumerate(me.materials):
      subset_cur_num=len(my_inds[i])

      if subset_cur_num == 0:
        continue

      fh.write('\n\t\t{')
      fh.write('material=materials["%s"],'%ma.name)
      fh.write(' indices_start=%i,'%subset_cur_start)
      fh.write(' indices_num=%i'%subset_cur_num)
      fh.write('},')

      subset_cur_start+=subset_cur_num

    fh.write('},')

    #store positions
    out=b''

    for v in my_positions:
      out+=struct.pack('3f',*get_xyz(v))

    out_str=base64.b64encode(out).decode("ascii")
    fh.write('\n\tpositions=decode("%s"),'%out_str)

    #store normals
    out=b''

    for v in my_normals:
      out+=struct.pack('3f',*get_xyz(v))

    out_str=base64.b64encode(out).decode("ascii")
    fh.write('\n\tnormals=decode("%s"),'%out_str)

    #store cols
    fh.write('\n\tcolors={')

    for i,col_name in enumerate(col_names):
      out=b''

      for v in my_colors[i]:
        out+=struct.pack('3f',*v)

      out_str=base64.b64encode(out).decode("ascii")
      fh.write('\n\t\t["%s"]=decode("%s"),'%(col_name,out_str))

    fh.write('},')

    #store texcoords
    fh.write('\n\ttexcoords={')

    for i,uv_name in enumerate(uv_names):
      out=b''

      for v in my_texcoords[i]:
        # out+=struct.pack('2f',*v)
        out+=struct.pack('f',v[0])
        out+=struct.pack('f',-v[1])

      out_str=base64.b64encode(out).decode("ascii")
      fh.write('\n\t\t["%s"]=decode("%s"),'%(uv_name,out_str))

    fh.write('},')

    #store tangents
    fh.write('\n\ttangents={')

    for i,uv_name in enumerate(uv_names):
      out=b''

      for v in my_tangents[i]:
        out+=struct.pack('3f',*get_xyz(v))
        out+=struct.pack('f',v[3])

      out_str=base64.b64encode(out).decode("ascii")
      fh.write('\n\t\t["%s"]=decode("%s"),'%(uv_name,out_str))

    fh.write('},')

    #store inds
    out=b''

    for inds in my_inds:
      for ind in inds:
       out+=struct.pack('H',ind)

    out_str=base64.b64encode(out).decode("ascii")
    fh.write('\n\tindices=decode("%s"),'%out_str)

    #store shape_vertices
    out=b''

    for v in shape_vertices:
      out+=struct.pack('3f',*get_xyz(v))
      #out+=struct.pack('f',0)

    out_str=base64.b64encode(out).decode("ascii")
    fh.write('\n\tshape_vertices=decode("%s"),'%out_str)

    #store shape_indices
    out=b''

    for ind in shape_indices:
      out+=struct.pack('H',ind)

    out_str=base64.b64encode(out).decode("ascii")
    fh.write('\n\tshape_indices=decode("%s"),'%out_str)

    #
    fh.write('\n\tshape_vertices_num=%i,'%len(shape_vertices))
    fh.write('\n\tshape_indices_num=%i,'%len(shape_indices))
    fh.write('\n\tshape_size={%g,%g,%g},'%tuple(get_xyz(shape_size)))
    fh.write('\n\tshape_min={%g,%g,%g},'%tuple(get_xyz(shape_min)))
    fh.write('\n\tshape_max={%g,%g,%g},'%tuple(get_xyz(shape_max)))
    fh.write('\n\tshape_radius=%g,'%shape_radius)
    fh.write('\n\tshape_radii={%g,%g,%g}'%tuple(get_xyz(shape_radii)))

    #store mesh footer
    fh.write('}')


def do_armatures(file_format,fh):
  #todo

  fh.write('\n\narmatures={}')

  myarmatures=[arm for arm in bpy.data.armatures if arm.users > 0]

  for arm in myarmatures:
    fh.write('\n\narmatures["%s"]={'%(arm.name))

    #sort by parents?
    for bone in arm.bones:
      fh.write('\n\t["%s"]={}'%bone.name)
      #parent
      #head
      #tail
      #envelope

    fh.write('}')



def do_lights(file_format,fh):
  fh.write('\n\nlamps={}')

  mylamps=[la for la in bpy.data.lamps if la.users > 0
           and (la.type=='POINT' or la.type=='SPOT')]

  for la in mylamps:
    fh.write('\n\nlamps["%s"] = {'%(la.name))
    fh.write('\n\tname="%s",'%la.name)

    if la.type=='SPOT' or la.type=='POINT':
      fh.write('\n\tlinear_attenuation=%g,'%la.linear_attenuation)
      fh.write('\n\tquadratic_attenuation=%g,'%la.quadratic_attenuation)


    if la.type=='SPOT':
      fh.write('\n\tspot_blend=%g,'%la.spot_blend)
      fh.write('\n\tspot_size=%g,'%la.spot_size)
    elif la.type=='POINT':
      ''

    fh.write('\n\ttype="%s",'%str.lower(la.type))
    fh.write('\n\tcolor={%g,%g,%g},'%tuple(la.color))
    fh.write('\n\tdistance=%g,'%la.distance)
    fh.write('\n\tenergy=%g'%la.energy)

    fh.write('}')

def object_compare(x, y):
  if x.parent == None and y.parent != None:
    return -1

  if x.parent != None and y.parent == None:
    return 1

  if x.parent == None and y.parent == None:
    return 0

  return object_compare(x.parent,y.parent)

def do_objects(file_format,fh):
  fh.write('\n\nobjects={}')

  sorted_objects=sorted(bpy.data.objects,key=functools.cmp_to_key(object_compare))

  for ob in sorted_objects:
    fh.write('\n\nobjects["%s"]={'%(ob.name))
    fh.write('\n\tname="%s",'%ob.name)

    if ob.type == "MESH":
      fh.write('\n\tmesh=meshes["%s"],'%ob.data.name)
    elif ob.type == "ARMATURE":
      fh.write('\n\tarmature=armatures["%s"],'%ob.data.name)
    elif ob.type == "LAMP":
      if ob.data.type=='SPOT' or ob.data.type=='POINT':
        fh.write('\n\tlamp=lamps["%s"],'%ob.data.name)

    # for vg in ob.vertex_groups:
    #   print(vg.id_data)

    if ob.parent != None:
      fh.write('\n\tparent=objects["%s"],'%ob.parent.name)

    fh.write('\n\tposition={%g,%g,%g},'%tuple(get_xyz(ob.location)))
    #mathutils.Quaternion((0.5,-0.5,-0.5,-0.5))
    bla=get_xyz(ob.rotation_euler)
    fh.write('\n\trotation_euler={%g,%g,%g},'%tuple(bla))
    gaa=mathutils.Euler(bla).to_quaternion()
    fh.write('\n\trotation={%g,%g,%g,%g},'%(gaa.x,gaa.y,gaa.z,gaa.w))
    fh.write('\n\tscale={%g,%g,%g}'%tuple(get_xyz(ob.scale)))
    fh.write('}')


def do_actions(file_format,fh):
  fh.write('\n\nactions={}')

  for i,action in enumerate(bpy.data.actions):
    # fh.write('\n\nactions[%i]={'%(i+1))
    fh.write('\n\nactions["%s"]={'%action.name)
    fh.write('\n\tname="%s",'%action.name)
    fh.write('\n\ttype="%s",'%str.lower(action.id_root))
    fh.write('\n\trange={%g,%g},'%tuple(action.frame_range))

    #
    fh.write('\n\tgroups={')

    for group in action.groups:
      fh.write('\n\t\t["%s"]={'%group.name)
      fh.write('},')

    fh.write('},')

    #
    fh.write('\n\tpose_markers={')

    for pose_marker in action.pose_markers:
      fh.write('\n\t\t{')
      fh.write('},')

    fh.write('},')

    #
    fh.write('\n\tfcurves={')

    for fcurve in action.fcurves:
      fh.write('\n\t\t{')
      fh.write('\n\t\t\trange={%g,%g},'%tuple(fcurve.range()))
      fh.write('\n\t\t\textrapolation="%s",'%str.lower(fcurve.extrapolation))
      # print(fcurve.data_path)
      if fcurve.data_path=='location' or fcurve.data_path=='scale':
        fh.write('\n\t\t\tarray_index=%i,'%get_xyz_index(fcurve.array_index))
      elif fcurve.data_path=='rotation_euler':
        fh.write('\n\t\t\tarray_index=%i,'%get_euler_index(fcurve.array_index))
      else:
        fh.write('\n\t\t\tarray_index=%i,'%fcurve.array_index)

      fh.write('\n\t\t\tdata_path="%s",'%fcurve.data_path)
      fh.write('\n\t\t\tsampled_points={')

      for sampled_point in fcurve.sampled_points:
        fh.write('\n\t\t\t\t{},')

      fh.write('},')
      fh.write('\n\t\t\tmodifiers={')

      for modifier in fcurve.modifiers:
        fh.write('\n\t\t\t\t{},')

      fh.write('},')
      fh.write('\n\t\t\tkeyframe_points={')

      for keyframe in fcurve.keyframe_points:
        fh.write('\n\t\t\t\t{')
        fh.write('\n\t\t\t\t\tco={%g,%g},'%tuple(keyframe.co))
        fh.write('\n\t\t\t\t\thandle_left={%g,%g},'%tuple(keyframe.handle_left))
        fh.write('\n\t\t\t\t\thandle_right={%g,%g},'%tuple(keyframe.handle_right))
        fh.write('\n\t\t\t\t\thandle_left_type="%s",'%str.lower(keyframe.handle_left_type))
        fh.write('\n\t\t\t\t\thandle_right_type="%s",'%str.lower(keyframe.handle_right_type))
        fh.write('\n\t\t\t\t\ttype="%s",'%str.lower(keyframe.type))
        fh.write('\n\t\t\t\t\tinterpolation="%s",'%str.lower(keyframe.interpolation))
        fh.write('\n\t\t\t\t},')


      fh.write('}')
      fh.write('},')
      ''
    print()

    fh.write('}')
    fh.write('}')

def run_export(file_format,fn):
  with open(fn, 'w', encoding='utf-8') as fh:
    fh.write('if decode==nil then function decode(s) return s end end\n\n')

    do_materials(file_format,fh)
    do_meshes(file_format,fh)
    do_armatures(file_format,fh)
    do_lights(file_format,fh)
    do_objects(file_format,fh)
    do_actions(file_format,fh)

    print('Exported to "%s".'%fn)

class ExportMyFormatLua(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
  bl_idname = "export_my_format.lua";
  bl_label = "Export";
  bl_options = {'PRESET'};
  filename_ext = ".lua";

  def execute(self, context):
    run_export('lua',self.filepath)
    return {'FINISHED'};

def menu_func(self, context):
  self.layout.operator(ExportMyFormatLua.bl_idname, text="My Model Format(.lua)");

def register():
  bpy.utils.register_module(__name__);
  bpy.types.INFO_MT_file_export.append(menu_func);

def unregister():
  bpy.utils.unregister_module(__name__);
  bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
  register()
