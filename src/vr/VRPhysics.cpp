#include "VRPhysics.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace VRMusicStudio {

struct VRPhysics::Impl {
    // Bullet Physics
    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;
    
    // Rigid Bodies
    std::vector<RigidBody> rigidBodies;
    std::map<std::string, RigidBody*> rigidBodyMap;
    
    // Collision Shapes
    std::vector<CollisionShape> shapes;
    std::map<std::string, CollisionShape*> shapeMap;
    
    // Constraints
    std::map<std::string, btTypedConstraint*> constraints;
    
    // Collision Detection
    struct CollisionCallback : public btCollisionWorld::ContactResultCallback {
        std::vector<std::string> collidingBodies;
        
        virtual btScalar addSingleResult(btManifoldPoint& cp,
                                       const btCollisionObjectWrapper* colObj0Wrap,
                                       int partId0, int index0,
                                       const btCollisionObjectWrapper* colObj1Wrap,
                                       int partId1, int index1) {
            collidingBodies.push_back(
                static_cast<const char*>(colObj1Wrap->getCollisionObject()->getUserPointer())
            );
            return 0.0f;
        }
    };
    
    Impl() : collisionConfig(nullptr),
             dispatcher(nullptr),
             broadphase(nullptr),
             solver(nullptr),
             world(nullptr) {}
};

VRPhysics::VRPhysics() : pImpl(std::make_unique<Impl>()) {}

VRPhysics::~VRPhysics() {
    shutdown();
}

bool VRPhysics::initialize() {
    if (!initializeBullet()) {
        spdlog::error("Bullet Physics-Initialisierung fehlgeschlagen");
        return false;
    }
    
    spdlog::info("VR-Physik initialisiert");
    return true;
}

void VRPhysics::shutdown() {
    // Constraints freigeben
    for (const auto& [id, constraint] : pImpl->constraints) {
        pImpl->world->removeConstraint(constraint);
        delete constraint;
    }
    pImpl->constraints.clear();
    
    // Rigid Bodies freigeben
    for (const auto& body : pImpl->rigidBodies) {
        pImpl->world->removeRigidBody(body.body);
        delete body.body->getMotionState();
        delete body.body;
    }
    pImpl->rigidBodies.clear();
    pImpl->rigidBodyMap.clear();
    
    // Shapes freigeben
    for (const auto& shape : pImpl->shapes) {
        delete shape.shape;
    }
    pImpl->shapes.clear();
    pImpl->shapeMap.clear();
    
    // Bullet Physics freigeben
    if (pImpl->world) {
        delete pImpl->world;
    }
    if (pImpl->solver) {
        delete pImpl->solver;
    }
    if (pImpl->broadphase) {
        delete pImpl->broadphase;
    }
    if (pImpl->dispatcher) {
        delete pImpl->dispatcher;
    }
    if (pImpl->collisionConfig) {
        delete pImpl->collisionConfig;
    }
}

void VRPhysics::update(float deltaTime) {
    if (!pImpl->world) return;
    
    pImpl->world->stepSimulation(deltaTime);
    updateRigidBodies();
    processCollisions();
}

void VRPhysics::addRigidBody(const std::string& id, const std::string& shapeId,
                           const glm::vec3& position, const glm::quat& rotation,
                           float mass) {
    auto shapeIt = pImpl->shapeMap.find(shapeId);
    if (shapeIt == pImpl->shapeMap.end()) {
        spdlog::error("Shape nicht gefunden: {}", shapeId);
        return;
    }
    
    btCollisionShape* shape = shapeIt->second->shape;
    
    // Transform
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(toBulletVector(position));
    transform.setRotation(toBulletQuaternion(rotation));
    
    // Motion State
    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    
    // Inertia
    btVector3 inertia(0.0f, 0.0f, 0.0f);
    if (mass > 0.0f) {
        shape->calculateLocalInertia(mass, inertia);
    }
    
    // Rigid Body
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    body->setUserPointer(const_cast<char*>(id.c_str()));
    
    // In World hinzufügen
    pImpl->world->addRigidBody(body);
    
    // In Maps speichern
    RigidBody rigidBody;
    rigidBody.id = id;
    rigidBody.body = body;
    rigidBody.position = position;
    rigidBody.rotation = rotation;
    rigidBody.mass = mass;
    rigidBody.dynamic = mass > 0.0f;
    
    pImpl->rigidBodies.push_back(rigidBody);
    pImpl->rigidBodyMap[id] = &pImpl->rigidBodies.back();
}

void VRPhysics::removeRigidBody(const std::string& id) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        pImpl->world->removeRigidBody(body->body);
        delete body->body->getMotionState();
        delete body->body;
        
        pImpl->rigidBodies.erase(
            std::remove_if(pImpl->rigidBodies.begin(), pImpl->rigidBodies.end(),
                          [&id](const RigidBody& b) { return b.id == id; }),
            pImpl->rigidBodies.end()
        );
        pImpl->rigidBodyMap.erase(it);
    }
}

void VRPhysics::setRigidBodyPosition(const std::string& id, const glm::vec3& position) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        btTransform transform = body->body->getWorldTransform();
        transform.setOrigin(toBulletVector(position));
        body->body->setWorldTransform(transform);
        body->position = position;
    }
}

void VRPhysics::setRigidBodyRotation(const std::string& id, const glm::quat& rotation) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        btTransform transform = body->body->getWorldTransform();
        transform.setRotation(toBulletQuaternion(rotation));
        body->body->setWorldTransform(transform);
        body->rotation = rotation;
    }
}

void VRPhysics::setRigidBodyVelocity(const std::string& id, const glm::vec3& velocity) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->setLinearVelocity(toBulletVector(velocity));
    }
}

void VRPhysics::setRigidBodyAngularVelocity(const std::string& id, const glm::vec3& velocity) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->setAngularVelocity(toBulletVector(velocity));
    }
}

void VRPhysics::applyForce(const std::string& id, const glm::vec3& force) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->applyCentralForce(toBulletVector(force));
    }
}

void VRPhysics::applyTorque(const std::string& id, const glm::vec3& torque) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->applyTorque(toBulletVector(torque));
    }
}

void VRPhysics::applyImpulse(const std::string& id, const glm::vec3& impulse) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->applyCentralImpulse(toBulletVector(impulse));
    }
}

void VRPhysics::applyAngularImpulse(const std::string& id, const glm::vec3& impulse) {
    auto it = pImpl->rigidBodyMap.find(id);
    if (it != pImpl->rigidBodyMap.end()) {
        RigidBody* body = it->second;
        body->body->applyTorqueImpulse(toBulletVector(impulse));
    }
}

void VRPhysics::addBoxShape(const std::string& id, const glm::vec3& halfExtents) {
    btBoxShape* shape = new btBoxShape(toBulletVector(halfExtents));
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::addSphereShape(const std::string& id, float radius) {
    btSphereShape* shape = new btSphereShape(radius);
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::addCylinderShape(const std::string& id, const glm::vec3& halfExtents) {
    btCylinderShape* shape = new btCylinderShape(toBulletVector(halfExtents));
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::addCapsuleShape(const std::string& id, float radius, float height) {
    btCapsuleShape* shape = new btCapsuleShape(radius, height);
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::addConvexHullShape(const std::string& id,
                                 const std::vector<glm::vec3>& points) {
    btConvexHullShape* shape = new btConvexHullShape();
    
    for (const auto& point : points) {
        shape->addPoint(toBulletVector(point));
    }
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::addTriangleMeshShape(const std::string& id,
                                   const std::vector<glm::vec3>& vertices,
                                   const std::vector<int>& indices) {
    btTriangleMesh* mesh = new btTriangleMesh();
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        mesh->addTriangle(
            toBulletVector(vertices[indices[i]]),
            toBulletVector(vertices[indices[i + 1]]),
            toBulletVector(vertices[indices[i + 2]])
        );
    }
    
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);
    
    CollisionShape collisionShape;
    collisionShape.id = id;
    collisionShape.shape = shape;
    collisionShape.scale = glm::vec3(1.0f);
    
    pImpl->shapes.push_back(collisionShape);
    pImpl->shapeMap[id] = &pImpl->shapes.back();
}

void VRPhysics::removeShape(const std::string& id) {
    auto it = pImpl->shapeMap.find(id);
    if (it != pImpl->shapeMap.end()) {
        CollisionShape* shape = it->second;
        delete shape->shape;
        
        pImpl->shapes.erase(
            std::remove_if(pImpl->shapes.begin(), pImpl->shapes.end(),
                          [&id](const CollisionShape& s) { return s.id == id; }),
            pImpl->shapes.end()
        );
        pImpl->shapeMap.erase(it);
    }
}

void VRPhysics::setShapeScale(const std::string& id, const glm::vec3& scale) {
    auto it = pImpl->shapeMap.find(id);
    if (it != pImpl->shapeMap.end()) {
        CollisionShape* shape = it->second;
        shape->shape->setLocalScaling(toBulletVector(scale));
        shape->scale = scale;
    }
}

void VRPhysics::addPoint2PointConstraint(const std::string& id,
                                       const std::string& bodyA,
                                       const std::string& bodyB,
                                       const glm::vec3& pivotA,
                                       const glm::vec3& pivotB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        btPoint2PointConstraint* constraint = new btPoint2PointConstraint(
            *itA->second->body,
            *itB->second->body,
            toBulletVector(pivotA),
            toBulletVector(pivotB)
        );
        
        pImpl->world->addConstraint(constraint);
        pImpl->constraints[id] = constraint;
    }
}

void VRPhysics::addHingeConstraint(const std::string& id,
                                 const std::string& bodyA,
                                 const std::string& bodyB,
                                 const glm::vec3& pivotA,
                                 const glm::vec3& pivotB,
                                 const glm::vec3& axisA,
                                 const glm::vec3& axisB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        btHingeConstraint* constraint = new btHingeConstraint(
            *itA->second->body,
            *itB->second->body,
            toBulletVector(pivotA),
            toBulletVector(pivotB),
            toBulletVector(axisA),
            toBulletVector(axisB)
        );
        
        pImpl->world->addConstraint(constraint);
        pImpl->constraints[id] = constraint;
    }
}

void VRPhysics::addSliderConstraint(const std::string& id,
                                  const std::string& bodyA,
                                  const std::string& bodyB,
                                  const glm::mat4& frameA,
                                  const glm::mat4& frameB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        btTransform transformA, transformB;
        
        // GLM-Matrix zu Bullet-Transform
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                transformA.getBasis()[i][j] = frameA[i][j];
                transformB.getBasis()[i][j] = frameB[i][j];
            }
        }
        transformA.getOrigin() = btVector3(frameA[3][0], frameA[3][1], frameA[3][2]);
        transformB.getOrigin() = btVector3(frameB[3][0], frameB[3][1], frameB[3][2]);
        
        btSliderConstraint* constraint = new btSliderConstraint(
            *itA->second->body,
            *itB->second->body,
            transformA,
            transformB,
            true
        );
        
        pImpl->world->addConstraint(constraint);
        pImpl->constraints[id] = constraint;
    }
}

void VRPhysics::removeConstraint(const std::string& id) {
    auto it = pImpl->constraints.find(id);
    if (it != pImpl->constraints.end()) {
        pImpl->world->removeConstraint(it->second);
        delete it->second;
        pImpl->constraints.erase(it);
    }
}

bool VRPhysics::checkCollision(const std::string& bodyA, const std::string& bodyB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        int numManifolds = pImpl->world->getDispatcher()->getNumManifolds();
        
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = pImpl->world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
            const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
            
            if ((obA == itA->second->body && obB == itB->second->body) ||
                (obA == itB->second->body && obB == itA->second->body)) {
                return contactManifold->getNumContacts() > 0;
            }
        }
    }
    
    return false;
}

std::vector<std::string> VRPhysics::getCollidingBodies(const std::string& bodyId) {
    auto it = pImpl->rigidBodyMap.find(bodyId);
    if (it != pImpl->rigidBodyMap.end()) {
        Impl::CollisionCallback callback;
        pImpl->world->contactTest(it->second->body, callback);
        return callback.collidingBodies;
    }
    return std::vector<std::string>();
}

glm::vec3 VRPhysics::getContactPoint(const std::string& bodyA, const std::string& bodyB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        int numManifolds = pImpl->world->getDispatcher()->getNumManifolds();
        
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = pImpl->world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
            const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
            
            if ((obA == itA->second->body && obB == itB->second->body) ||
                (obA == itB->second->body && obB == itA->second->body)) {
                if (contactManifold->getNumContacts() > 0) {
                    return fromBulletVector(contactManifold->getContactPoint(0).getPositionWorldOnB());
                }
            }
        }
    }
    
    return glm::vec3(0.0f);
}

glm::vec3 VRPhysics::getContactNormal(const std::string& bodyA, const std::string& bodyB) {
    auto itA = pImpl->rigidBodyMap.find(bodyA);
    auto itB = pImpl->rigidBodyMap.find(bodyB);
    
    if (itA != pImpl->rigidBodyMap.end() && itB != pImpl->rigidBodyMap.end()) {
        int numManifolds = pImpl->world->getDispatcher()->getNumManifolds();
        
        for (int i = 0; i < numManifolds; i++) {
            btPersistentManifold* contactManifold = pImpl->world->getDispatcher()->getManifoldByIndexInternal(i);
            const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
            const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
            
            if ((obA == itA->second->body && obB == itB->second->body) ||
                (obA == itB->second->body && obB == itA->second->body)) {
                if (contactManifold->getNumContacts() > 0) {
                    return fromBulletVector(contactManifold->getContactPoint(0).m_normalWorldOnB);
                }
            }
        }
    }
    
    return glm::vec3(0.0f);
}

void VRPhysics::initializeBullet() {
    pImpl->collisionConfig = new btDefaultCollisionConfiguration();
    pImpl->dispatcher = new btCollisionDispatcher(pImpl->collisionConfig);
    pImpl->broadphase = new btDbvtBroadphase();
    pImpl->solver = new btSequentialImpulseConstraintSolver();
    
    pImpl->world = new btDiscreteDynamicsWorld(
        pImpl->dispatcher,
        pImpl->broadphase,
        pImpl->solver,
        pImpl->collisionConfig
    );
    
    pImpl->world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

void VRPhysics::updateRigidBodies() {
    for (auto& body : pImpl->rigidBodies) {
        btTransform transform;
        body.body->getMotionState()->getWorldTransform(transform);
        
        body.position = fromBulletVector(transform.getOrigin());
        body.rotation = fromBulletQuaternion(transform.getRotation());
    }
}

void VRPhysics::processCollisions() {
    // Hier können Kollisions-Events verarbeitet werden
}

btVector3 VRPhysics::toBulletVector(const glm::vec3& vec) {
    return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion VRPhysics::toBulletQuaternion(const glm::quat& quat) {
    return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

glm::vec3 VRPhysics::fromBulletVector(const btVector3& vec) {
    return glm::vec3(vec.x(), vec.y(), vec.z());
}

glm::quat VRPhysics::fromBulletQuaternion(const btQuaternion& quat) {
    return glm::quat(quat.w(), quat.x(), quat.y(), quat.z());
}

} // namespace VRMusicStudio 