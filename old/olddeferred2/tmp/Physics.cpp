#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <lua.hpp>
#include <iostream>
#include "MyLua.h"

#include "Character.h"

namespace PhysicsModule {
  btDiscreteDynamicsWorld *world;

}

using namespace PhysicsModule;

int physics_step(lua_State *L) {
  int collisionObjects_index=lua_upvalueindex(1);
  // for(lua_pushnil(L); lua_next(L, -2); lua_pop(L, 1))
  btScalar stepTime=(btScalar)luaL_checknumber(L,1);

  world->stepSimulation(stepTime,0,0);

  for(int i=0;i<world->getDispatcher()->getNumManifolds();i++) {
    btPersistentManifold *contactManifold;
    contactManifold=world->getDispatcher()->getManifoldByIndexInternal(i);
    const btCollisionObject *obA=contactManifold->getBody0();
    const btCollisionObject *obB=contactManifold->getBody1();
    int numContacts = contactManifold->getNumContacts();

    for(int j=0;j<numContacts;j++) {
      btManifoldPoint &pt = contactManifold->getContactPoint(j);

      if(pt.getDistance()<0.0) {
        const btVector3 &pt1 = pt.getPositionWorldOnA();
        const btVector3 &pt2 = pt.getPositionWorldOnB();
        const btVector3 &normalOn2 = pt.m_normalWorldOnB;

        lua_pushlightuserdata(L,(void*)obA);
        lua_gettable(L,collisionObjects_index);
        int object1_index=lua_gettop(L);

        lua_pushlightuserdata(L,(void*)obA);
        lua_gettable(L,collisionObjects_index);
        int object2_index=lua_gettop(L);

        //todo
        lua_pop(L,2);
      }
    }
  }


  //
  return 0;
}

int physics_character_delete(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));
  btPairCachingGhostObject *ghost=character->getGhostObject();

  world->removeCollisionObject(ghost);
  world->removeAction(character);

  delete character;
  delete ghost;

  return 0;
}

int physics_character_getPosition(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  btVector3 pos=character->getGhostObject()->getWorldTransform().getOrigin();
  mylua_numberArrayToTable(L,3,false,&pos[0]);
  return 1;
}

int physics_character_setPosition(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  btVector3 pos;
  mylua_checkTableToStaticNumberArray(L,2,3,false,false,&pos[0]);
  character->warp(pos);
  return 0;
}

int physics_character_getLinearVelocity(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  //todo
  return 0;
}

int physics_character_setLinearVelocity(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  //todo

  // btVector3 vel;
  // mylua_checkTableToStaticNumberArray(L,2,3,false,false,&vel[0]);
  // c->setWalkDirection(vel);

  return 0;
}

int physics_character_jump(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  character->jump();
  return 0;
}

int physics_character_canJump(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  lua_pushboolean(L,character->canJump());
  return 1;
}

int physics_character_onGround(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));

  lua_pushboolean(L,character->onGround());
  return 1;
}

//btCollisionObject *getOverlappingObject (int index)
//int 	getNumOverlappingObjects ()
//void rayTest (const btVector3 &rayFromWorld, const btVector3 &rayToWorld, btCollisionWorld::RayResultCallback &resultCallback)
//void convexSweepTest (const class btConvexShape *castShape, const btTransform &convexFromWorld, const btTransform &convexToWorld, btCollisionWorld::ConvexResultCallback &resultCallback, btScalar allowedCcdPenetration=0.f)


int physics_rigidBody_delete(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));
  world->removeCollisionObject(rigidBody);
  delete rigidBody->getMotionState();
  delete rigidBody;
  return 0;
}

int physics_rigidBody_getPosition(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 pos=rigidBody->getWorldTransform().getOrigin();
  mylua_numberArrayToTable(L,3,false,&pos[0]);
  return 1;
}

int physics_rigidBody_setPosition(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 pos;
  mylua_checkTableToStaticNumberArray(L,2,3,false,false,&pos[0]);
  rigidBody->getWorldTransform().setOrigin(pos);
  return 0;
}

int physics_rigidBody_getRotation(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btQuaternion rot=rigidBody->getWorldTransform().getRotation();
  mylua_numberArrayToTable(L,4,false,&rot[0]);
  return 1;
}

int physics_rigidBody_setRotation(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btQuaternion rot;
  mylua_checkTableToStaticNumberArray(L,2,4,false,false,&rot[0]);
  rigidBody->getWorldTransform().setRotation(rot);
  return 0;
}

int physics_rigidBody_getLinearVelocity(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 vel=rigidBody->getLinearVelocity();
  mylua_numberArrayToTable(L,3,false,&vel[0]);
  return 1;
}

int physics_rigidBody_setLinearVelocity(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 vel;
  mylua_checkTableToStaticNumberArray(L,2,3,false,false,&vel[0]);
  rigidBody->setLinearVelocity(vel);
  return 0;
}

int physics_rigidBody_getAngularVelocity(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 vel=rigidBody->getAngularVelocity();
  mylua_numberArrayToTable(L,3,false,&vel[0]);
  return 1;
}

int physics_rigidBody_setAngularVelocity(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  btVector3 vel;
  mylua_checkTableToStaticNumberArray(L,2,3,false,false,&vel[0]);
  rigidBody->setAngularVelocity(vel);
  return 0;
}

int physics_rigidBody_isActive(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  lua_pushboolean(L,rigidBody->isActive());
  return 1;
}

int physics_rigidBody_activate(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  rigidBody->activate();
  return 0;
}

int physics_rigidBody_wantsSleeping(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  lua_pushboolean(L,rigidBody->wantsSleeping());
  return 1;
}

int physics_rigidBody_isInWorld(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));

  lua_pushboolean(L,rigidBody->isInWorld());
  return 1;
}

//void applyTorque (const btVector3 &torque)
//void applyForce (const btVector3 &force, const btVector3 &rel_pos)
//void applyCentralImpulse (const btVector3 &impulse)
//void applyTorqueImpulse (const btVector3 &torque)
//void applyImpulse (const btVector3 &impulse, const btVector3 &rel_pos)
//btVector3 getVelocityInLocalPoint (const btVector3 &rel_pos) const
//void applyCentralForce (const btVector3 &force)


int physics_shape_delete(lua_State *L) {
  btCollisionShape *shape;
  shape=*((btCollisionShape**)lua_touserdata(L,1));

  btStridingMeshInterface *mesh=0;
  //  btBvhTriangleMeshShape *triangleShape;

  //if(triangleShape=dynamic_cast<btBvhTriangleMeshShape*>(shape)) {
  //  mesh=triangleShape->getMeshInterface();
  // }

  delete shape;
  //delete mesh;
  return 0;
}

int physics_shape_setScale(lua_State *L) {
  btCollisionShape *shape=*((btCollisionShape**)lua_touserdata(L,1));

  btVector3 scale;
  mylua_checkTableToStaticNumberArray(L,2,3,false,false,&scale[0]);
  shape->setLocalScaling(scale);
  return 0;
}

int physics_shape_getScale(lua_State *L) {
  btCollisionShape *shape=*((btCollisionShape**)lua_touserdata(L,1));

  btVector3 scale=shape->getLocalScaling();
  mylua_numberArrayToTable(L,2,false,&scale[0]);
  return 1;
}

int physics_createCharacter(lua_State *L) {
  int character_mt_index=lua_upvalueindex(1);
  int collisionObjects_index=lua_upvalueindex(2);
  int shapeDependencies_index=lua_upvalueindex(3);
  int shape_index=1;
  int values_index=2;

  btConvexShape *shape=*(btConvexShape**)lua_touserdata(L,shape_index);

  btScalar stepHeight=0.5;//luaL_checknumber(L,2);
  btScalar jumpSpeed=7;
  btScalar fallSpeed=20;
  btScalar gravity=16;
  btScalar maxSlope=btRadians(45);
  btScalar contactProcessingThreshold=(btScalar)0.025;
  btScalar maxJumpHeight=10;
  btVector3 position=btVector3(0,0,0);
  btVector3 linVel=btVector3(0,0,0);

  if(lua_istable(L,values_index)) {
    //stepHeight
    lua_getfield(L,values_index,"stepHeight");

    if(lua_isnumber(L,-1)) {
      stepHeight=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //jumpSpeed
    lua_getfield(L,values_index,"jumpSpeed");

    if(lua_isnumber(L,-1)) {
      jumpSpeed=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //fallSpeed
    lua_getfield(L,values_index,"fallSpeed");

    if(lua_isnumber(L,-1)) {
      fallSpeed=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //gravity
    lua_getfield(L,values_index,"gravity");

    if(lua_isnumber(L,-1)) {
      gravity=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //maxSlope
    lua_getfield(L,values_index,"maxSlope");

    if(lua_isnumber(L,-1)) {
      maxSlope=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //contactProcessingThreshold
    lua_getfield(L,values_index,"contactProcessingThreshold");

    if(lua_isnumber(L,-1)) {
      contactProcessingThreshold=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //maxJumpHeight
    lua_getfield(L,values_index,"maxJumpHeight");

    if(lua_isnumber(L,-1)) {
      maxJumpHeight=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //position
    lua_getfield(L,values_index,"position");
    mylua_tableToStaticNumberArray(L,-1,3,false,false,&position[0]);
    lua_pop(L,1);

    //linearVelocity
    lua_getfield(L,values_index,"linearVelocity");
    mylua_tableToStaticNumberArray(L,-1,3,false,false,&linVel[0]);
    lua_pop(L,1);
  }

  //
  btTransform startTransform;
  startTransform.setIdentity();

  //ghost
  btPairCachingGhostObject *ghost = new btPairCachingGhostObject();

  ghost->setWorldTransform(startTransform);
  ghost->setCollisionShape(shape);
  ghost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
  ghost->setContactProcessingThreshold(contactProcessingThreshold);

  //character
  btKinematicCharacterController *character;
  character=new btKinematicCharacterController(ghost,shape,stepHeight);

  character->setJumpSpeed(jumpSpeed);
  character->setFallSpeed(fallSpeed);
  character->setGravity(gravity);
  character->setMaxSlope(maxSlope);
  character->setMaxJumpHeight(maxJumpHeight);
  character->warp(position);

  //
  btKinematicCharacterController **userdata;
  userdata=(btKinematicCharacterController**)lua_newuserdata(L,sizeof(void*));
  *userdata=character;
  int character_index=lua_gettop(L);

  lua_pushvalue(L,character_mt_index);
  lua_setmetatable(L,character_index);

  lua_pushlightuserdata(L,ghost);
  lua_pushvalue(L,character_index);
  lua_settable(L,collisionObjects_index);

  lua_pushvalue(L,character_index);
  lua_pushvalue(L,shape_index);
  lua_settable(L,shapeDependencies_index);

  return 1;
}

int physics_createRigidBody(lua_State *L) {
  int rigidBody_mt_index=lua_upvalueindex(1);
  int collisionObjects_index=lua_upvalueindex(2);
  int shapeDependencies_index=lua_upvalueindex(3);
  int shape_index=1;
  int values_index=2;
  btCollisionShape *shape=*(btCollisionShape**)lua_touserdata(L,shape_index);
  btScalar mass=0;//luaL_checknumber(L,2);

  if(lua_istable(L,values_index)) {
    //mass
    lua_getfield(L,values_index,"mass");

    if(lua_isnumber(L,-1)) {
      mass=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);
  }

  btTransform startTransform;
  startTransform.setIdentity();
  btVector3 localInertia(0,0,0);

  if(mass) {
    shape->calculateLocalInertia(mass,localInertia);
  }

  btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,motionState,shape,localInertia);
  btRigidBody *rigidBody = new btRigidBody(rbInfo);

  btScalar linDamping=rigidBody->getLinearDamping();
  btScalar angDamping=rigidBody->getAngularDamping();
  btScalar restituion=rigidBody->getRestitution();
  btScalar friction=rigidBody->getFriction();
  btScalar rollingFriction=rigidBody->getRollingFriction();
  btScalar deactivationTime=rigidBody->getDeactivationTime();
  btVector3 position=btVector3(0,0,0);
  btQuaternion rotation=btQuaternion(0,0,0,1);
  btVector3 linVel=btVector3(0,0,0);
  btVector3 angVel=btVector3(0,0,0);

  if(lua_istable(L,values_index)) {
    //linearDamping
    lua_getfield(L,values_index,"linearDamping");

    if(lua_isnumber(L,-1)) {
      linDamping=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //angularDamping
    lua_getfield(L,values_index,"angularDamping");

    if(lua_isnumber(L,-1)) {
      angDamping=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //friction
    lua_getfield(L,values_index,"friction");

    if(lua_isnumber(L,-1)) {
      friction=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //rollingFriction
    lua_getfield(L,values_index,"rollingFriction");

    if(lua_isnumber(L,-1)) {
      rollingFriction=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //deactivationTime
    lua_getfield(L,values_index,"deactivationTime");

    if(lua_isnumber(L,-1)) {
      deactivationTime=(btScalar)lua_tonumber(L,-1);
    }

    lua_pop(L,1);

    //position
    lua_getfield(L,values_index,"position");
    mylua_tableToStaticNumberArray(L,-1,3,false,false,&position[0]);
    lua_pop(L,1);

    //rotation
    lua_getfield(L,values_index,"rotation");
    mylua_tableToStaticNumberArray(L,-1,4,false,false,&rotation[0]);
    lua_pop(L,1);

    //linearVelocity
    lua_getfield(L,values_index,"linearVelocity");
    mylua_tableToStaticNumberArray(L,-1,3,false,false,&linVel[0]);
    lua_pop(L,1);

    //angularVelocity
    lua_getfield(L,values_index,"linearVelocity");
    mylua_tableToStaticNumberArray(L,-1,3,false,false,&angVel[0]);
    lua_pop(L,1);
  }

  rigidBody->setDamping(linDamping,angDamping);
  rigidBody->setRestitution(restituion);
  rigidBody->setFriction(friction);
  rigidBody->setRollingFriction(rollingFriction);
  rigidBody->setDeactivationTime(deactivationTime);
  rigidBody->getWorldTransform().setOrigin(position);
  rigidBody->getWorldTransform().setRotation(rotation);
  rigidBody->setLinearVelocity(linVel);
  rigidBody->setAngularVelocity(angVel);

  btRigidBody **userdata;
  userdata=(btRigidBody**)lua_newuserdata(L,sizeof(void*));
  *userdata=rigidBody;
  int rigidBody_index=lua_gettop(L);

  lua_pushvalue(L,rigidBody_mt_index);
  lua_setmetatable(L,rigidBody_index);

  lua_pushlightuserdata(L,rigidBody);
  lua_pushvalue(L,rigidBody_index);
  lua_settable(L,collisionObjects_index);

  lua_pushvalue(L,rigidBody_index);
  lua_pushvalue(L,shape_index);
  lua_settable(L,shapeDependencies_index);

  return 1;
}

int physics_createPlane(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  btVector3 n;
  mylua_checkTableToStaticNumberArray(L,1,3,false,false,&n[0]);
  btScalar c=(btScalar)luaL_checknumber(L,2);
  btStaticPlaneShape *shape=new btStaticPlaneShape(n,c);

  btStaticPlaneShape **userdata;
  userdata=(btStaticPlaneShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}

int physics_createBox(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  btVector3 h;
  mylua_checkTableToStaticNumberArray(L,1,3,false,false,&h[0]);
  btBoxShape *shape=new btBoxShape(h);

  btBoxShape **userdata;
  userdata=(btBoxShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}

int physics_createSphere(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  btScalar r=(btScalar)luaL_checknumber(L,1);
  btSphereShape *shape=new btSphereShape(r);

  btSphereShape **userdata;
  userdata=(btSphereShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}


int physics_createCylinder(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  btVector3 h;
  mylua_checkTableToStaticNumberArray(L,1,3,false,false,&h[0]);
  btCylinderShape *shape=new btCylinderShape(h);
  // btCylinderShapeX
  // btCylinderShapeZ

  btCylinderShape **userdata;
  userdata=(btCylinderShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}

int physics_createCapsule(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  btScalar r=(btScalar)luaL_checknumber(L,1);
  btScalar h=(btScalar)luaL_checknumber(L,2);
  btCapsuleShape *shape=new btCapsuleShape(r,h);

  btCapsuleShape **userdata;
  userdata=(btCapsuleShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}

int physics_createConvexHull(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  int numPoints=1;
  btScalar *points=new btScalar[numPoints*3];

  //todo set numPoints
  //todo set points

  int stride=sizeof(btVector3);
  btConvexHullShape *shape=new btConvexHullShape(points, numPoints, stride);

  btConvexHullShape **userdata;
  userdata=(btConvexHullShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);
  return 1;
}

int physics_createTriangleMesh(lua_State *L) {
  int shape_mt_index=lua_upvalueindex(1);

  int numTris=1;
  int *inds=new int[numTris*3];
  int numVerts=1;
  btScalar *verts=new btScalar[numVerts];

  //todo set numVerts
  //todo set verts
  //todo set num tris
  //todo set inds

  //
  int indStride=sizeof(int)*3;
  int vertStride=sizeof(btScalar)*3;

  //
  btTriangleIndexVertexArray *triangles;
  triangles = new btTriangleIndexVertexArray(numTris,inds,
                                             indStride,numVerts,
                                             verts,vertStride);

  btBvhTriangleMeshShape *shape;
  shape=new btBvhTriangleMeshShape(triangles,false,true);

  btBvhTriangleMeshShape **userdata;
  userdata=(btBvhTriangleMeshShape**)lua_newuserdata(L,sizeof(void*));
  *userdata=shape;
  int shape_index=lua_gettop(L);

  lua_pushvalue(L,shape_mt_index);
  lua_setmetatable(L,shape_index);

  return 1;
}

int physics_addRigidBody(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));
  world->addRigidBody(rigidBody);

  lua_pushvalue(L,1);
  return 1;
}

int physics_removeRigidBody(lua_State *L) {
  btRigidBody *rigidBody=*((btRigidBody**)lua_touserdata(L,1));
  world->removeRigidBody(rigidBody);
  return 0;
}

int physics_addCharacter(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));
  btPairCachingGhostObject *ghost=character->getGhostObject();

  short int colFilterGroup = btBroadphaseProxy::CharacterFilter;
  short int colFilterMask =
    btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter;

  world->addCollisionObject(ghost,colFilterGroup,colFilterMask);
  world->addAction(character);

  lua_pushvalue(L,1);
  return 1;
}

int physics_removeCharacter(lua_State *L) {
  btKinematicCharacterController *character;
  character=*((btKinematicCharacterController**)lua_touserdata(L,1));
  btPairCachingGhostObject *ghost=character->getGhostObject();

  world->removeCollisionObject(ghost);
  world->removeAction(character);
  return 0;
}

void init_physics_module(lua_State *L) {
  btCollisionDispatcher *dispatcher;
  btConstraintSolver *constraintSolver;
  btDefaultCollisionConfiguration *colConfig;

  colConfig = new btDefaultCollisionConfiguration();
  dispatcher = new btCollisionDispatcher(colConfig);
  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  btAxisSweep3* sweepBP = new btAxisSweep3(worldMin,worldMax);
  btOverlappingPairCache *overlappingPairCache=sweepBP->getOverlappingPairCache();
  constraintSolver = new btSequentialImpulseConstraintSolver();
  world = new btDiscreteDynamicsWorld(dispatcher,sweepBP,constraintSolver,colConfig);
  world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;
  overlappingPairCache->setInternalGhostPairCallback(new btGhostPairCallback());
  world->setGravity(btVector3(0,-10,0));

  //
  //shapeDependencies
  lua_newtable(L);
  int shapeDependencies_index=lua_gettop(L);

  //shapeDependencies.__mode = weak_keys
  lua_pushstring(L,"__k");
  lua_setfield(L,shapeDependencies_index,"__mode");

  //shapeDependencies.mt=shapeDependencies
  lua_pushvalue(L,shapeDependencies_index);
  lua_setmetatable(L,shapeDependencies_index);

  //collisionObjects
  lua_newtable(L);
  int collisionObjects_index=lua_gettop(L);

  //collisionObjects.__mode = weak_keys
  lua_pushstring(L,"__v");
  lua_setfield(L,collisionObjects_index,"__mode");

  //collisionObjects.mt=collisionObjects
  lua_pushvalue(L,collisionObjects_index);
  lua_setmetatable(L,collisionObjects_index);

  //character_mt
  lua_newtable(L);
  int character_mt_index=lua_gettop(L);

  //character_mt.__index
  lua_pushvalue(L,character_mt_index);
  lua_setfield(L,character_mt_index,"__index");

  //character_mt.__gc
  lua_pushcfunction(L,physics_character_delete);
  lua_setfield(L,character_mt_index,"__gc");

  //
  //rigidBody_mt
  lua_newtable(L);
  int rigidBody_mt_index=lua_gettop(L);

  //rigidBody_mt.__index
  lua_pushvalue(L,rigidBody_mt_index);
  lua_setfield(L,rigidBody_mt_index,"__index");

  //rigidBody_mt.__gc
  lua_pushcfunction(L,physics_rigidBody_delete);
  lua_setfield(L,rigidBody_mt_index,"__gc");

  //
  //shape_mt
  lua_newtable(L);
  int shape_mt_index=lua_gettop(L);

  //shape_mt.__index
  lua_pushvalue(L,shape_mt_index);
  lua_setfield(L,shape_mt_index,"__index");

  //shape_mt.__gc
  lua_pushcfunction(L,physics_shape_delete);
  lua_setfield(L,shape_mt_index,"__gc");

  //character_mt.getPosition
  lua_pushcfunction(L,physics_character_getPosition);
  lua_setfield(L,character_mt_index,"getPosition");

  //character_mt.setPosition
  lua_pushcfunction(L,physics_character_setPosition);
  lua_setfield(L,character_mt_index,"setPosition");

  //character_mt.getLinearVelocity
  lua_pushcfunction(L,physics_character_getLinearVelocity);
  lua_setfield(L,character_mt_index,"getLinearVelocity");

  //character_mt.setLinearVelocity
  lua_pushcfunction(L,physics_character_setLinearVelocity);
  lua_setfield(L,character_mt_index,"setLinearVelocity");

  //character_mt.jump
  lua_pushcfunction(L,physics_character_jump);
  lua_setfield(L,character_mt_index,"jump");

  //character_mt.canJump
  lua_pushcfunction(L,physics_character_canJump);
  lua_setfield(L,character_mt_index,"canJump");

  //character_mt.onGround
  lua_pushcfunction(L,physics_character_onGround);
  lua_setfield(L,character_mt_index,"onGround");

  //rigidbody_mt.getPosition
  lua_pushcfunction(L,physics_rigidBody_getPosition);
  lua_setfield(L,rigidBody_mt_index,"getPosition");

  //rigidbody_mt.setPosition
  lua_pushcfunction(L,physics_rigidBody_setPosition);
  lua_setfield(L,rigidBody_mt_index,"setPosition");

  //rigidbody_mt.getRotation
  lua_pushcfunction(L,physics_rigidBody_getRotation);
  lua_setfield(L,rigidBody_mt_index,"getRotation");

  //rigidbody_mt.setRotation
  lua_pushcfunction(L,physics_rigidBody_setRotation);
  lua_setfield(L,rigidBody_mt_index,"setRotation");

  //rigidBody_mt.getLinearVelocity
  lua_pushcfunction(L,physics_rigidBody_getLinearVelocity);
  lua_setfield(L,rigidBody_mt_index,"getLinearVelocity");

  //rigidBody_mt.setLinearVelocity
  lua_pushcfunction(L,physics_rigidBody_setLinearVelocity);
  lua_setfield(L,rigidBody_mt_index,"setLinearVelocity");

  //rigidBody_mt.getAngularVelocity
  lua_pushcfunction(L,physics_rigidBody_getAngularVelocity);
  lua_setfield(L,rigidBody_mt_index,"getAngularVelocity");

  //rigidBody_mt.setAngularVelocity
  lua_pushcfunction(L,physics_rigidBody_setAngularVelocity);
  lua_setfield(L,rigidBody_mt_index,"setAngularVelocity");

  //rigidBody_mt.isActive
  lua_pushcfunction(L,physics_rigidBody_isActive);
  lua_setfield(L,rigidBody_mt_index,"isActive");

  //rigidBody_mt.activate
  lua_pushcfunction(L,physics_rigidBody_activate);
  lua_setfield(L,rigidBody_mt_index,"activate");

  //rigidBody_mt.wantsSleeping
  lua_pushcfunction(L,physics_rigidBody_wantsSleeping);
  lua_setfield(L,rigidBody_mt_index,"wantsSleeping");

  //rigidBody_mt.isInWorld
  lua_pushcfunction(L,physics_rigidBody_isInWorld);
  lua_setfield(L,rigidBody_mt_index,"isInWorld");

  //shape_mt.setScale
  lua_pushcfunction(L,physics_shape_setScale);
  lua_setfield(L,shape_mt_index,"setScale");

  //shape_mt.getScale
  lua_pushcfunction(L,physics_shape_getScale);
  lua_setfield(L,shape_mt_index,"getScale");

  //physics
  // lua_newtable(L);
  // int physics_index=lua_gettop(L);

  //physics.createCharacter
  lua_pushvalue(L,character_mt_index);
  lua_pushvalue(L,collisionObjects_index);
  lua_pushvalue(L,shapeDependencies_index);
  lua_pushcclosure(L,physics_createCharacter,3);
  // lua_setfield(L,physics_index,"createCharacter");
  lua_setglobal(L,"createCharacter");

  //physics.createRigidBody
  lua_pushvalue(L,rigidBody_mt_index);
  lua_pushvalue(L,collisionObjects_index);
  lua_pushvalue(L,shapeDependencies_index);
  lua_pushcclosure(L,physics_createRigidBody,3);
  // lua_setfield(L,physics_index,"createRigidBody");
  lua_setglobal(L,"createRigidBody");

  //physics.createPlane
  lua_pushvalue(L,shape_mt_index);
  lua_pushcclosure(L,physics_createPlane,1);
  // lua_setfield(L,physics_index,"createPlane");
  lua_setglobal(L,"createPlane");

  //physics.createBox
  lua_pushvalue(L,shape_mt_index);
  lua_pushcclosure(L,physics_createBox,1);
  // lua_setfield(L,physics_index,"createBox");
  lua_setglobal(L,"createBox");

  //physics.createSphere
  lua_pushvalue(L,shape_mt_index);
  lua_pushcclosure(L,physics_createSphere,1);
  // lua_setfield(L,physics_index,"createSphere");
  lua_setglobal(L,"createSphere");

  //physics.createCylinder
  lua_pushvalue(L,shape_mt_index);
  lua_pushcclosure(L,physics_createCylinder,1);
  // lua_setfield(L,physics_index,"createCylinder");
  lua_setglobal(L,"createCylinder");

  //physics.createCapsule
  lua_pushvalue(L,shape_mt_index);
  lua_pushcclosure(L,physics_createCapsule,1);
  // lua_setfield(L,physics_index,"createCapsule");
  lua_setglobal(L,"createCapsule");

  //physics.step
  lua_pushvalue(L,collisionObjects_index);
  lua_pushcclosure(L,physics_step,1);
  // lua_setfield(L,physics_index,"step");
  lua_setglobal(L,"stepWorld");

  //physics.addRigidBody
  lua_pushcfunction(L,physics_addRigidBody);
  // lua_setfield(L,physics_index,"addRigidBody");
  lua_setglobal(L,"addRigidBody");

  //physics.removeRigidBody
  lua_pushcfunction(L,physics_removeRigidBody);
  // lua_setfield(L,physics_index,"removeRigidBody");
  lua_setglobal(L,"removeRigidBody");

  //physics.addCharacter
  lua_pushcfunction(L,physics_addCharacter);
  // lua_setfield(L,physics_index,"addCharacter");
  lua_setglobal(L,"addCharacter");

  //physics.removeCharacter
  lua_pushcfunction(L,physics_removeCharacter);
  // lua_setfield(L,physics_index,"removeCharacter");
  lua_setglobal(L,"removeCharacter");

  //
  // lua_setglobal(L,"Physics");

  lua_pop(L,5);
}
