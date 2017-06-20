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

def do_mesh(me,modelMat,normalMat,useAdjacency):
  #todo adjacency

  #
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

    for i,x in enumerate(me.uv_textures):
      face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv1)
      face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv2)
      face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv3)

      if len(face.vertices)==4:
        face_uvs[i].append(me.tessface_uv_textures[i].data[faceInd].uv4)

    #
    face_pts=[]

    #
    for faceVertInd,vertInd in enumerate(face.vertices):
      key=''

      #
      pos=modelMat*me.vertices[vertInd].co


      nor=normalMat*(me.vertices[vertInd].normal if face.use_smooth else face.normal)

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


def do_shape(me,modelMat,useAdjacency):
  me.update(calc_tessface=True)

  shape_vertices = [] #array of points
  shape_vertex_indices = dict() #[k]=shape_vertex_ind
  shape_indices=[]

  for faceInd, face in enumerate(me.tessfaces):
    face_pts=[]

    for faceVertInd,vertInd in enumerate(face.vertices):
      pos=modelMat*me.vertices[vertInd].co
      k='%g %g %g'%(pos[0],pos[1],pos[2])

      if k not in shape_vertex_indices.keys():
        shape_vertex_indices[k]=len(shape_vertices)
        shape_vertices.append(pos)

      face_pts.append(shape_vertex_indices[k])

    for i in [[0,1,2],[0,2,3]] if len(face.vertices)==4 else [[0,1,2]]:
      for j in i:
        shape_indices.append(face_pts[j])

  # for a in shape_vertices:
  #   for b in a:
  #     print("%f %g"%(b,b))

  out_indices = [] if useAdjacency else shape_indices

  if useAdjacency:
    halfEdges={}
    #6 indices per triangle and 0,2,4=triangle and 1,3,5=adjacency

    for i in range(0,int(len(shape_indices)/3)):
      halfEdges["%i %i"%(shape_indices[i*3],shape_indices[i*3+1])]=shape_indices[i*3+2]
      halfEdges["%i %i"%(shape_indices[i*3+1],shape_indices[i*3+2])]=shape_indices[i*3]
      halfEdges["%i %i"%(shape_indices[i*3+2],shape_indices[i*3])]=shape_indices[i*3+1]


    for i in range(0,int(len(shape_indices)/3)):
      out=[shape_indices[i*3],
           halfEdges.get("%i %i"%(shape_indices[i*3+1],shape_indices[i*3]),-1),
           shape_indices[i*3+1],
           halfEdges.get("%i %i"%(shape_indices[i*3+2],shape_indices[i*3+1]),-1),
           shape_indices[i*3+2],
           halfEdges.get("%i %i"%(shape_indices[i*3],shape_indices[i*3+2]),-1)]

      out_indices.extend(out)

  return {
    "vertices" : shape_vertices,
    "indices" : out_indices
  }

def do_meshes(useCentered,useBase64,useSelected,
              useShape,useTransform,indicesType,
              useAdjacency):
  all=not (useSelected and bpy.context.selected_objects)
  objects=bpy.data.objects if all else bpy.context.selected_objects
  objects2=meshes=[ob for ob in objects if ob.type == "MESH"]
  # meshes_set=set([x.data for x in objects2])
  # calcd_meshes=dict([(x.name,do_mesh(x)) for x in meshes_set])
  # singleMesh=(not all) and len(bpy.context.selected_objects)==1


  triangleMode="triangles_adjacency" if useAdjacency and useShape else "triangles"

  fixModelMat=mathutils.Matrix.Rotation(-math.pi/2.0,4,'Y')*mathutils.Matrix.Rotation(-math.pi/2.0,4,'X')
  fixNormalMat=fixModelMat.to_3x3()

  out={
    "vertices" : {
      "positions" : {
        "size" : 3,
        "type" : "float",
        "data" : []
      },
    },
    "indices" : {
      "type" : indicesType,
      "data" : []
    },
    "draws" : {}
  }

  # inds_num=0

  if useShape:
    for ob in objects2:
      print(ob.name)
      worldMat=fixModelMat*ob.matrix_world if useTransform else fixModelMat
      shape=do_shape(ob.data,worldMat,useAdjacency)

      for pos in shape["vertices"]:
        out["vertices"]["positions"]["data"].extend(pos)

      out["indices"]["data"].extend(shape["indices"])
  else:
    uvLayers=set()
    colLayers=set()

    for ob in objects2:
      for x in ob.data.uv_textures:
        uvLayers.add(x.name)

      for x in ob.data.vertex_colors:
        colLayers.add(x.name)

    out["vertices"]["normals"]= {
      "size" : 3,
      "type" : "float",
      "data" : []
    }

    for uv in uvLayers:
      out["vertices"]["texcoords_"+uv]={"size" : 2, "type" : "float", "data" : []}

      out["vertices"]["tangents_"+uv]={"size" : 4, "type" : "float", "data" : []}

    for col in colLayers:
      out["vertices"]["colors_"+col]={"size" : 3, "type" : "float", "data" : []}

    indices_out=dict() #[mat][obj]
    vertsOffset=0

    for ob in objects2:
      print(ob.name)

      worldMat=fixModelMat*ob.matrix_world if useTransform else fixModelMat
      normalMat=fixNormalMat*ob.matrix_world.normalized().to_3x3() if useTransform else fixNormalMat
      myme=do_mesh(ob.data,worldMat,normalMat,useAdjacency)

      for pos in myme["positions"]:
        out["vertices"]["positions"]["data"].extend(pos)

      for nor in myme["normals"]:
        out["vertices"]["normals"]["data"].extend(nor)

      for uv in uvLayers:
        if uv not in myme["texcoords"].keys():
          for i in range(0,myme["vertices_num"]*2):
            out["vertices"]["texcoords_"+uv]["data"].append(0.0)

      for uv in uvLayers:
        if uv not in myme["tangents"].keys():
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

      # inds_num+=myme["indices_num"]


      vertsOffset+=len(myme["positions"])


    for ma,obInds in indices_out.items():
      outInds=out["indices"]["data"]
      first=len(outInds)
      first2=first
      count=0

      for ob,inds in obInds.items():
        count2=len(inds)

        if len(indices_out) != 1 or ma != "":
          out["draws"][ma+"_"+ob] = {"mode" : triangleMode, "first" : first2, "count" : count2}
        outInds.extend(inds)
        count+=count2
        first2+=count2


      if len(indices_out) != 1 or ma != "":
        out["draws"][ma] = {"mode" : triangleMode, "first" : first, "count" : count}

  inds_num=len(out["indices"]["data"])
  out["draws"]["default"] = {"mode" : triangleMode, "first" : 0, "count" : inds_num}

  if useCentered:
    avgPos=mathutils.Vector([0,0,0])
    v=out["vertices"]["positions"]["data"]

    for i in range(0,int(len(v)/3)):
      avgPos+=mathutils.Vector(v[i*3:i*3+3])

    avgPos=avgPos/(len(v)/3.0)

    for i in range(0,int(len(v)/3)):
      for j in range(0,3):
        v[i*3+j]-=avgPos[j]

  if useBase64:
    type=""
    type2=""

    if indicesType=="ushort":
      type="H"
      type2="h"
    elif indicesType=="uint":
      type="I"
      type2="i"

    b=b""

    if useAdjacency:
      for x in out["indices"]["data"]:
        if x==-1:
          b+=struct.pack("%s"%(type2),x)
        else:
          b+=struct.pack("%s"%(type),x)
    else:
      b=struct.pack("%i%s"%(inds_num,type),*out["indices"]["data"])

    out["indices"]["data"]=base64.b64encode(b).decode("ascii")

    for k,v in out["vertices"].items():
      b=struct.pack("%if"%(len(v["data"])),*v["data"])
      v["data"]=base64.b64encode(b).decode("ascii")

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

  return out

class MyExport(bpy.types.Operator, bpy_extras.io_utils.ExportHelper):
  bl_idname = "my_export.json";
  bl_label = "Export";
  bl_options = {'PRESET'};
  filename_ext = ".json";

  useCentered=bpy.props.BoolProperty(name="centered",default=False)
  useTransform=bpy.props.BoolProperty(name="transformed",default=True)
  useSelected=bpy.props.BoolProperty(name="selected only",default=False)
  useShape=bpy.props.BoolProperty(name="positions only",default=False)

  useAdjacency=bpy.props.BoolProperty(name="adjacency",default=False)

  useBase64=bpy.props.BoolProperty(name="base64",default=True)
  indicesTypeItems=[("ushort","ushort","ushort"),
                    ("uint","uint","uint")]
  indicesType=bpy.props.EnumProperty(items=indicesTypeItems,default="ushort",name="indices")
  indent=bpy.props.IntProperty(name="json indent",default=2,min=0,max=8,step=1)

  # drawModeItems=[("triangles","triangles","triangles"),
  #                # ("points","points","points"),
  #                # ("lines","lines","lines"),
  #                ("triangles_adjacency","triangles_adjacency","triangles_adjacency")]
  # drawMode=bpy.props.EnumProperty(items=drawModeItems,default="triangles",
  #                                    name="draw mode",description="")
  def execute(self, context):
    with open(self.filepath, 'w', encoding='utf-8') as fh:
      indentAmount=indent=None if self.indent==0 else self.indent
      mes=do_meshes(self.useCentered,self.useBase64,
                    self.useSelected,self.useShape,
                    self.useTransform,self.indicesType,
                    self.useAdjacency)
      # j=json.dumps(mes, # cls=ComplexEncoder, #not working...
                   # indentAmount)

      myJsonDump(fh,mes,indentAmount,0)

      # fh.write(j)
      # my_dump_json(fh,mes,indentAmount,0)

      # fh.write("{")
      # writeNewline(fh,indentAmount)

      # #vertices
      # writeIndents(fh,1,indentAmount)
      # fh.write('"vertices" : {')
      # writeNewline(fh,indentAmount)

      # for k,v in mes["vertices"].items():
      #   writeIndents(fh,2,indentAmount)
      #   fh.write('"%s" : {'%(k))
      #   writeNewline(fh,indentAmount)

      #   writeIndents(fh,2,indentAmount)
      #   fh.write("},")
      #   writeNewline(fh,indentAmount)

      # writeIndents(fh,1,indentAmount)
      # fh.write("},")
      # writeNewline(fh,indentAmount)

      # #indices
      # writeIndents(fh,1,indentAmount)
      # fh.write('"indices" : {')
      # writeNewline(fh,indentAmount)

      # writeIndents(fh,1,indentAmount)
      # fh.write("},")
      # writeNewline(fh,indentAmount)

      # #draws
      # writeIndents(fh,1,indentAmount)
      # fh.write('"draws" : {')
      # writeNewline(fh,indentAmount)

      # writeIndents(fh,1,indentAmount)
      # fh.write("},")
      # writeNewline(fh,indentAmount)

      # fh.write("}")
      # writeNewline(fh,indentAmount)



    print('Exported to "%s".'%self.filepath)
    return {'FINISHED'};

def menu_func(self, context):
  self.layout.operator(MyExport.bl_idname, text="My Geometry (.json)");

def register():
  bpy.utils.register_module(__name__);
  bpy.types.INFO_MT_file_export.append(menu_func);

def unregister():
  bpy.utils.unregister_module(__name__);
  bpy.types.INFO_MT_file_export.remove(menu_func);

if __name__ == "__main__":
  register()
