#include "Character.h"

Character::Character(btConvexShape *shape) {
  m_ghostObject = new btPairCachingGhostObject();

}

Character::~Character() {

}

void Character::updateAction(btCollisionWorld *collisionWorld, btScalar deltaTimeStep) {

}

void Character::debugDraw(btIDebugDraw *debugDrawer) {

}

btPairCachingGhostObject *Character::getGhostObject() {
  return m_ghostObject;
}
