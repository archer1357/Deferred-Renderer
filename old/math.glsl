//aras-p.info/texts/CompactNormalStorage.html
  
vec3 decodeNorSphereMap(vec2 enc) {
    vec2 fenc = enc*4.0-2.0;
    float f = dot(fenc,fenc);
    float g = sqrt(1.0-f/4.0);
    return vec3(fenc*g,1.0-f/2.0);
}

vec2 encodeNorSphereMap(vec3 n) {
    return n.xy/sqrt(8.0*n.z+8.0) + 0.5;
}

//
float smoothStep(float a,float b,float x) {
    float t = clamp((x-a)/(b-a),0.0,1.0);
    return t*t*(3.0-2.0*t);
}

float lerp(float f0,float f1,float u) {
    return (1.0-u)*f0+u*f1;
}

//

float posToDepth(vec3 pos,float znear,float zfar) {
    float d=1.0/(zfar - znear);
  
    //ortho
    //float p10=2.0, p11=znear-zfar; 
  
    //perspective
    float p10=zfar+znear, p11=2.0*zfar*znear; 

    //
    float clipZ=(p10*pos.z+p11)*d;
    float clipW=-pos.z;
    float ndcZ = clipZ/clipW;
  
    //
    float nearRange=0.0;
    float farRange=1.0;
  
    return (((farRange-nearRange)*ndcZ)+nearRange+farRange)*0.5;
}

vec3 depthToPos(mat4 invProjMat,vec2 screen,float depth) {
    vec4 H=vec4(vec3(screen,depth)*2.0-1.0,1.0);//screen.xy between -1 and 1
    vec4 D=invProjMat*H;
    return D.xyz/D.w;
}

float linearDepth(float depth,float znear,float zfar) {
    return (2.0*znear)/(zfar+znear-depth*(zfar-znear));
}

mat4 frustum(float left,float right,float bottom,float top,float zNear,float zFar) {
    //warning: in row major
    mat4 m;
	
    m[0]=(2.0*zNear)/(right-left); //x
    m[2]=(right+left)/(right-left); //A
    m[5]=(2.0*zNear)/(top-bottom); //y
    m[6]=(top+bottom)/(top-bottom); //B
    m[10]=-(zFar+zNear)/(zFar-zNear); //C
    m[11]=-(2.0*zFar*zNear)/(zFar-zNear); //D
    m[14]=-1.0;
	
    m[1]=m[3]=m[4]=m[7]=m[8]=m[9]=m[12]=m[13]=m[15]=0.0;
	
    return m;
}

mat4 perspective_fovx(float fovx,float aspect,float znear,float zfar) {
    float right=tan(fovx/2.0)*znear;
    float top=right/aspect;
    return frustum(-right,right,-top,top,znear,zfar);
}

mat4 mat4_perspective_fovy(float fovy,float aspect,float znear,float zfar) {
    float top=tan(fovy/2.0)*znear;
    float right=top*aspect;
    return frustum(-right,right,-top,top,znear,zfar);
}

mat4 ortho(float left,float right,float bottom,float top,float nearVal,float farVal) {
    //warning: in row major
    mat4 m;
	
    m[0]=2.0/(right-left);
    m[3]=-(right+left)/(right-left); //tx
    m[5]=2.0/(top-bottom);
    m[7]=-(top+bottom)/(top-bottom); //ty
    m[10]=-2.0/(nearVal-farVal);
    m[11]=-(farVal+nearVal)/(farVal-nearVal); //tz
    m[15]=1.0;
	
    m[1]=m[2]=m[4]=m[6]=m[8]=m[9]=m[12]=m[13]=m[14]=0.0;
	
    return m;
}

mat4 ortho2d(float left,float right,float bottom,float top) {
    return ortho(left,right,bottom,top,-1.0,1.0);
}

//from http://gamedev.stackexchange.com/questions/68612

vec4 calcTangent(vec3 pt0,vec3 pt1,vec3 pt2,vec2 uv0,vec2 uv1,vec2 uv2,vec3 nor) {
    vec3 e0=pt1-pt0;
    vec3 e1=pt2-pt0;
    vec3 e0_uv=uv1-uv0;
    vec3 e1_uv=uv2-uv0;

    float cp=e0_uv.x*e1_uv.y-e0_uv.y*e1_uv.x;

    if(cp == 0.0) {
        return vec4(0.0);
    }
  
    float r=1.0/cp;
  
    vec3 sdir=(e0*e1_uv.y-e1*e0_uv.y)*r
    vec3 tdir=(e1*e0_uv.x-e0*e1_uv.x)*r
  
    vec3 tg=normalize(sdir-nor*dot(nor,sdir));
    float w=(dot(cross(nor,sdir),tdir)<0.0)?-1.0:1.0;
  
    //bt=w*cross(nor,tg)
    //dot(cross(T,B),N)?

    return vec4(tg,w);
}

void tangent_usage() {
    n=normalize(u_normalMat*a_nor);
    tg=normalize(u_normalMat*a_tan.xyz);
    btg=normalize(a_tan.w*cross(n,tg)); 

    mat3 invTbnMat=mat3(tg,btg,nor);
    mat3 tbnMat=transpose(invTbnMat);
	
    pos=u_modelViewMat*vec4(a_pos,1.0);
    v_posTs=tbnMat*(pos.xyz/pos.w);
    //viewTS=normalize(tbnMat*-rd).xy;
	//viewTS=normalize(tbnMat*(ro-pt)).xy;
  
    nor=normalize(invTbnMat*normalize(2.0*norHgt.rgb-1.0));
}
