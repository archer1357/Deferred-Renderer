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

import bpy,bmesh,bpy_extras,struct,os,mathutils,functools,base64,math,json,re

def writeIndents(f,indent,depth):
  for i in range(0,depth*indent):
    f.write(" ")

def writeNewline(f,indent):
  if indent!=0:
    f.write("\n")

def myJsonDump(f,data,indent,depth):
  if isinstance(data, dict):
    f.write('{')
    writeNewline(f,indent)

    c=0
    end=len(data.items())-1

    for k,v in data.items():
      writeIndents(f,indent,depth+1)
      f.write('"%s" : '%(k))
      myJsonDump(f,v,indent,depth+1)

      if c!= end:
        f.write(",")

      c+=1
      writeNewline(f,indent+1)

    writeIndents(f,indent,depth)
    f.write('}')
  elif isinstance(data, list):
    f.write('[')
    end=len(data)-1

    for c,v in enumerate(data):

      myJsonDump(f,v,indent,depth)

      if c!= end:
        f.write(", ")

      #f.tell()
      # if c!= end and (c+1)%20==0:
      #   writeNewline(f,indent+1)
      #   writeIndents(f,indent,depth)
      #   f.write("    ")


    f.write(']')
  elif isinstance(data, str):
    f.write('"%s"'%data)
  else:
    f.write("%g"%(data))


def calc_tangent_space(vec1,vec2,vec3,tex1,tex2,uv3,nor):
  e1=vec2-vec1
  e2=vec3-vec1
  uv1=tex2-tex1
  uv2=uv3-tex1
  cp=uv1.x * uv2.y - uv1.y * uv2.x

  if cp == 0.0:
    return mathutils.Vector((0,0,0,0))

  r = 1.0 / cp
  sdir=(uv2.y*e1 - uv1.y*e2)*r
  tdir=(uv1.x*e2 - uv2.x*e1)*r
  tg=(sdir - nor * nor.dot(sdir)).normalized()
  w=-1.0 if nor.cross(sdir).dot(tdir) < 0.0 else 1.0

  return mathutils.Vector((tg.x,tg.y,tg.z,w))


#t = normalize(t - n * dot(n, t));
#if (dot(cross(n, t), b) < 0.0) t = t * -1.0

def do_mesh(me,modelMat,normalMat,useShape):
  me.update(calc_tessface=True)

  my_verts_num=0
  my_inds_num=0

  my_positions=[]
  my_normals=[]
  my_colors=dict([(x.name,[]) for x in me.vertex_colors])
  my_texcoords=dict([(x.name,[]) for x in me.uv_textures])
  my_tangents=dict([(x.name,[]) for x in me.uv_textures])

  my_vert_inds=dict()
  my_indices=dict([(ma.name,[]) for ma in me.materials] if me.materials else [("",[])])
  orig_vert_inds=[]

  #gen vert+index for each poly
  for faceInd, face in enumerate(me.tessfaces):
    maName=me.materials[face.material_index].name if me.materials else ""
    face_cols = [[] for x in me.vertex_colors]
    face_pts=[]

    #
    if not useShape:
      for i,x in enumerate(me.vertex_colors):
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color1)
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color2)
        face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color3)

        if len(face.vertices)==4:
          face_cols[i].append(me.tessface_vertex_colors[i].data[faceInd].color4)

      #
      face_uvs = [[] for x in me.uv_textures]

      for i,x in enumerate(me.uv_textures):
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv1)
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv2)
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv3)

        if len(face.vertices)==4:
          face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv4)

    #
    for faceVertInd,vertInd in enumerate(face.vertices):
      key=''
      cols=[]
      uvs=[]


      #
      pos=modelMat*me.vertices[vertInd].co

      if not useShape:
        nor=me.vertices[vertInd].normal if face.use_smooth else face.normal
        nor=normalMat*nor
        nor.normalize()

      key+=' %g %g %g'%(pos[0],pos[1],pos[2])

      if not useShape:
        key+=' %g %g %g'%(nor[0],nor[1],nor[2])

      #
      if not useShape:
        for face_col in face_cols:
          col=face_col[faceVertInd]
          cols.append(col)
          key+=' %g %g %g'%(col[0],col[1],col[2])

        #
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
          p1=modelMat*me.vertices[adj_vert_a].co
          p2=modelMat*me.vertices[vertInd].co
          p3=modelMat*me.vertices[adj_vert_b].co

          uv1=face_uv[adj_a]
          uv2=face_uv[faceVertInd]
          uv3=face_uv[adj_b]

          tang=calc_tangent_space(p1,p2,p3, uv1,uv2,uv3, nor)
          tangs.append(tang)
          key+=' %g %g %g %g'%(tang[0],tang[1],tang[2],tang[3])

      #
      if key not in my_vert_inds.keys():
        orig_vert_inds.append(vertInd)

        my_vert_inds[key]=my_verts_num
        my_positions.append(pos)

        if not useShape:
          my_normals.append(nor)

          for i,vertcol in enumerate(me.vertex_colors):
            my_colors[vertcol.name].append(cols[i])

          for i,uvtex in enumerate(me.uv_textures):
            my_texcoords[uvtex.name].append(uvs[i])

            my_tangents[uvtex.name].append(tangs[i])

        my_verts_num+=1

      face_pts.append(my_vert_inds[key])

    #gen vert indices for each poly
    for i in [[0,1,2],[0,2,3]] if len(face.vertices)==4 else [[0,1,2]]:
      for j in i:
        my_indices[maName].append(face_pts[j])
        my_inds_num+=1

  #
  return {
    "positions" : my_positions,
    "normals" : my_normals,
    "texcoords" : my_texcoords,
    "tangents" : my_tangents,
    "colors" :  my_colors,
    "indices" : my_indices,
    "vertices_num" : my_verts_num,
    "indices_num" : my_inds_num
  }

def do_meshes(useSelected,
              useShape,useTransform,
              indicesType,useAdjacency):
  all=not (useSelected and bpy.context.selected_objects)
  objects=bpy.data.objects if all else bpy.context.selected_objects
  objects2=meshes=[ob for ob in objects if ob.type == "MESH"]
  triangleMode="triangles_adjacency" if useAdjacency else "triangles"
  fixModelMat=mathutils.Matrix.Rotation(-math.pi/2.0,4,'Y')*mathutils.Matrix.Rotation(-math.pi/2.0,4,'X')
  fixNormalMat=fixModelMat.to_3x3()

  out={
    "vertices" : {
      "positions" : {
        "size" : 4 if useAdjacency else 3,
        "type" : "float",
        "data" : [0.0,0.0,0.0,0.0] if useAdjacency else []
      },
    },
    "indices" : {
      "type" : indicesType,
      "data" : []
    },
    "draws" : {}
  }

  uvLayers=set()
  colLayers=set()

  if not useShape:
    for ob in objects2:
      for x in ob.data.uv_textures:
        uvLayers.add(x.name)

      for x in ob.data.vertex_colors:
        colLayers.add(x.name)

  if not useShape:
    out["vertices"]["normals"]= {
      "size" : 3,
      "type" : "float",
      "data" : [0.0,0.0,0.0] if useAdjacency else []
    }

  if not useShape:
    for uv in uvLayers:
      out["vertices"]["texcoords_"+uv]={"size" : 2, "type" : "float", "data" : [0.0,0.0] if useAdjacency else []}

      # out["vertices"]["tangents_"+uv]={"size" : 3, "type" : "float", "data" : []}
      out["vertices"]["tangents_"+uv]={"size" : 4, "type" : "float", "data" : [0.0,0,0.0,0.0] if useAdjacency else []}

    for col in colLayers:
      out["vertices"]["colors_"+col]={"size" : 3, "type" : "float", "data" : [0.0,0.0,0.0] if useAdjacency else []}

  indices_out=dict() #[mat][obj]
  vertsOffset=1 if useAdjacency else 0

  for ob in objects2:
    print(ob.name)

    worldMat=fixModelMat*ob.matrix_world if useTransform else fixModelMat
    normalMat=fixNormalMat*ob.matrix_world.normalized().to_3x3() if useTransform else fixNormalMat
    myme=do_mesh(ob.data,worldMat,normalMat,useShape)

    for pos in myme["positions"]:
      out["vertices"]["positions"]["data"].extend(pos)

      if useAdjacency:
        out["vertices"]["positions"]["data"].append(1.0)

    if not useShape:
      for nor in myme["normals"]:
        out["vertices"]["normals"]["data"].extend(nor)

      for uv in uvLayers:
        if uv not in myme["texcoords"].keys():
          for i in range(0,myme["vertices_num"]*2):
            out["vertices"]["texcoords_"+uv]["data"].append(0.0)

      for uv in uvLayers:
        if uv not in myme["tangents"].keys():
          #for i in range(0,myme["vertices_num"]*3):
          for i in range(0,myme["vertices_num"]*4):
            out["vertices"]["tangents_"+uv]["data"].append(0.0)

      for c in colLayers:
        if c not in myme["colors"].keys():
          for i in range(0,myme["vertices_num"]*3):
            out["vertices"]["colors_"+c]["data"].append(1.0)

      for uv,texs in myme["texcoords"].items():
        for tex in texs:
          out["vertices"]["texcoords_"+uv]["data"].extend(tex)

      for uv,tgs in myme["tangents"].items():
        for tg in tgs:
          out["vertices"]["tangents_"+uv]["data"].extend(tg)

      for c,cols in myme["colors"].items():
        for col in cols:
          out["vertices"]["colors_"+c]["data"].extend(col)

    for ma,inds in myme["indices"].items():
      if ma not in indices_out:
        indices_out[ma]=dict()

      if ob.name not in indices_out[ma]:
        indices_out[ma][ob.name]=[]

      indices_out[ma][ob.name].extend([x+vertsOffset for x in inds])

    vertsOffset+=len(myme["positions"])

  #for adjacency
  halfEdges={}

  if useAdjacency:
    for ma,obInds in indices_out.items():
      for ob,inds in obInds.items():
        for i in range(0,int(len(inds)/3)):
          halfEdges["%i %i"%(inds[i*3],inds[i*3+1])]=inds[i*3+2]
          halfEdges["%i %i"%(inds[i*3+1],inds[i*3+2])]=inds[i*3]
          halfEdges["%i %i"%(inds[i*3+2],inds[i*3])]=inds[i*3+1]

  #
  for ma,obInds in indices_out.items():
    outInds=out["indices"]["data"]
    first=len(outInds)
    first2=first
    count=0

    for ob,inds in obInds.items():
      count2=len(inds)

      if len(indices_out) != 1 or ma != "":
        out["draws"][ma+"_"+ob] = {"mode" : triangleMode, "first" : first2, "count" : count2}

      #

      if useAdjacency:
        inds2=[]

        for i in range(0,int(len(inds)/3)):
          aaa=[inds[i*3],
               halfEdges.get("%i %i"%(inds[i*3+1],inds[i*3]),-1),
               inds[i*3+1],
               halfEdges.get("%i %i"%(inds[i*3+2],inds[i*3+1]),-1),
               inds[i*3+2],
               halfEdges.get("%i %i"%(inds[i*3],inds[i*3+2]),-1)]

          inds2.extend(aaa)
        holes_count=0
        for i in inds2:
          if i==-1:
            holes_count+=1
        print("holes %i of %i"%(holes_count,len(inds2)))

        outInds.extend([0 if i==-1 else i for i in inds2])
      else:
        outInds.extend(inds)


      count+=count2
      first2+=count2


    if len(indices_out) != 1 or ma != "":
      out["draws"][ma] = {"mode" : triangleMode, "first" : first, "count" : count}

  #
  inds_num=len(out["indices"]["data"])
  out["draws"]["default"] = {"mode" : triangleMode, "first" : 0, "count" : inds_num}

  return out

def meshes_toBytes(mes):
  indicesType=mes["indices"]["type"]
  inds_num=len(mes["indices"]["data"])

  type=""

  if indicesType=="ushort":
    type="H"
  elif indicesType=="uint":
    type="I"

  b=struct.pack("%i%s"%(inds_num,type),*mes["indices"]["data"])
  mes["indices"]["data"]=b

  for k,v in mes["vertices"].items():
    b=struct.pack("%if"%(len(v["data"])),*v["data"])
    v["data"]=b

def meshes_toBase64(mes):
  meshes_toBytes(mes)

  b=base64.b64encode(mes["indices"]["data"]).decode("ascii")
  mes["indices"]["data"]=b

  for k,v in mes["vertices"].items():
    b=base64.b64encode(v["data"]).decode("ascii")
    v["data"]=b


class MyExportJson(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
  bl_idname = "my_export.json";
  bl_label = "Export";
  bl_options = {'PRESET'};
  filename_ext = ".json";

  useTransform=bpy.props.BoolProperty(name="transformed",default=True)
  useSelected=bpy.props.BoolProperty(name="selected only",default=False)
  useShape=bpy.props.BoolProperty(name="positions only",default=False)

  useAdjacency=bpy.props.BoolProperty(name="adjacency",default=False)
  useBase64=bpy.props.BoolProperty(name="base64",default=True)
  indicesTypeItems=[("ushort","ushort","ushort"),
                    ("uint","uint","uint")]
  indicesType=bpy.props.EnumProperty(items=indicesTypeItems,default="ushort",name="indices")
  indent=bpy.props.IntProperty(name="json indent",default=2,min=0,max=8,step=1)

  def execute(self, context):
    with open(self.filepath, 'w', encoding='utf-8') as fh:
      indentAmount=indent=None if self.indent==0 else self.indent
      mes=do_meshes(self.useSelected,self.useShape,
                    self.useTransform,self.indicesType,
                    self.useAdjacency)

      if self.useBase64:
        meshes_toBase64(mes)
      # j=json.dumps(mes, # cls=ComplexEncoder, #not working...
                   # indentAmount)

      myJsonDump(fh,mes,indentAmount,0)




    print('Exported to "%s".'%self.filepath)
    return {'FINISHED'};

class MyExportGeom(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
  bl_idname = "my_export.geom";
  bl_label = "Export";
  bl_options = {'PRESET'};
  filename_ext = ".geom";

  useTransform=bpy.props.BoolProperty(name="transformed",default=True)
  useSelected=bpy.props.BoolProperty(name="selected only",default=False)
  useShape=bpy.props.BoolProperty(name="positions only",default=False)

  useAdjacency=bpy.props.BoolProperty(name="adjacency",default=False)
  indicesTypeItems=[("ushort","ushort","ushort"),
                    ("uint","uint","uint")]
  indicesType=bpy.props.EnumProperty(items=indicesTypeItems,default="ushort",name="indices")

  def execute(self, context):
    with open(self.filepath, 'wb') as fh:
      mes=do_meshes(self.useSelected,self.useShape,
                    self.useTransform,self.indicesType,
                    self.useAdjacency)

      meshes_toBytes(mes)

      for name,v in mes["vertices"].items():
        fh.write(b'v') #vertices
        fh.write(struct.pack('I', len(name))) #nameSize
        fh.write(bytes(name,'ascii')) #name

        if v["type"]=="float":
          fh.write(struct.pack('I', 5126)) #type GL_FLOAT
        else:
          fh.write(struct.pack('I', 0)) #error

        fh.write(struct.pack('c', bytes([v["size"]]))) #size
        fh.write(struct.pack('I', len(v["data"]))) #dataSize
        fh.write(v["data"])

        data=v["data"]

      if "indices" in mes.keys():
        v=mes["indices"]
        fh.write(b'i') #indices

        if v["type"]=="uint":
          fh.write(struct.pack('I', 5125)) #GL_UNSIGNED_INT
        elif v["type"]=="ushort":
          fh.write(struct.pack('I', 5123)) #GL_UNSIGNED_SHORT
        else:
          fh.write(struct.pack('I', 0)) #error

        fh.write(struct.pack('I', len(v["data"]))) #dataSize
        fh.write(v["data"])

      for name,v in mes["draws"].items():
        fh.write(b'd') #draw
        fh.write(struct.pack('I', len(name))) #nameSize
        fh.write(bytes(name,'ascii')) #name

        if v["mode"]=="triangles":
          fh.write(struct.pack('I', 4)) #type GL_TRIANGLES
        elif v["mode"]=="triangles_adjacency":
          fh.write(struct.pack('I', 12)) #type GL_TRIANGLES_ADJACENCY
        else:
          fh.write(struct.pack('I', 0)) #error

        fh.write(struct.pack('I', v["first"])) #first
        fh.write(struct.pack('I', v["count"])) #count

    print('Exported to "%s".'%self.filepath)
    return {'FINISHED'};
def menu_func(self, context):
  self.layout.operator(MyExportJson.bl_idname, text="My Geometry (.json)");
  self.layout.operator(MyExportGeom.bl_idname, text="My Geometry (.geom)");

def register():
  bpy.utils.register_module(__name__);
  bpy.types.INFO_MT_file_export.append(menu_func);

def unregister():
  bpy.utils.unregister_module(__name__);
  bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
  register()



# bm = bmesh.new()
# print(bm.loops.layers.uv)
# # lastOb=None

# for ob in objects2:
#   me=ob.data
# #   ""
#   print(ob.name)
  # print(ob.data.polygons)
  # for face in ob.data.polygons:
  #   for faceVertInd,vertInd in enumerate(face.vertices):
  #     print(faceVertInd,vertInd)
#   lastOb=ob

#   #
#   # vertFrom=len(bm.verts)
#   # bm.from_mesh(ob.data)
#   # bmesh.ops.triangulate(bm, faces=bm.faces)
#   # vertTo=len(bm.verts)
#   # print(vertFrom,vertTo)

#   for vert in bm.verts:
#     bm.verts.new(vert)

#   #
#   # bmesh.ops.transform(bm,matrix=ob.matrix_world,verts=bm.verts[vertFrom:vertTo])

# print([x.normal for x in bm.verts[:]])
# bm.to_mesh(lastOb.data)

# # print(bm.loops.layers.uv.active)
# # print([x for x in bm.loops.layers.uv])
# # for face in bm.faces:
# #   for loop in face.loops:
# #     print([(k,v) for k,v in loop.items()])
# #     # print(loop["UVMap"])
# bm.free()
# del bm

# drawModeItems=[("triangles","triangles","triangles"),
#                # ("points","points","points"),
#                # ("lines","lines","lines"),
#                ("triangles_adjacency","triangles_adjacency","triangles_adjacency")]
# drawMode=bpy.props.EnumProperty(items=drawModeItems,default="triangles",
#                                    name="draw mode",description="")
