
#include <BulletDynamics/Dynamics/btActionInterface.h>
#include <BulletCollision/BroadphaseCollision/btCollisionAlgorithm.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <LinearMath/btVector3.h>

class Character : public btActionInterface {
private:
  btPairCachingGhostObject *m_ghostObject;
public:
  Character(btConvexShape *shape);
  ~Character();
  void updateAction(btCollisionWorld *collisionWorld, btScalar deltaTimeStep);
  void debugDraw(btIDebugDraw *debugDrawer);
  btPairCachingGhostObject *getGhostObject();
};
