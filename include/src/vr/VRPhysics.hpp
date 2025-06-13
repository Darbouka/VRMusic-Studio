#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bullet/btBulletDynamicsCommon.h>

namespace VRMusicStudio {

class VRPhysics {
public:
    struct RigidBody {
        std::string id;
        btRigidBody* body;
        glm::vec3 position;
        glm::quat rotation;
        float mass;
        bool dynamic;
    };

    struct CollisionShape {
        std::string id;
        btCollisionShape* shape;
        glm::vec3 scale;
    };

    VRPhysics();
    ~VRPhysics();

    bool initialize();
    void shutdown();
    void update(float deltaTime);

    // Rigid Bodies
    void addRigidBody(const std::string& id, const std::string& shapeId,
                     const glm::vec3& position, const glm::quat& rotation,
                     float mass = 0.0f);
    void removeRigidBody(const std::string& id);
    void setRigidBodyPosition(const std::string& id, const glm::vec3& position);
    void setRigidBodyRotation(const std::string& id, const glm::quat& rotation);
    void setRigidBodyVelocity(const std::string& id, const glm::vec3& velocity);
    void setRigidBodyAngularVelocity(const std::string& id, const glm::vec3& velocity);
    void applyForce(const std::string& id, const glm::vec3& force);
    void applyTorque(const std::string& id, const glm::vec3& torque);
    void applyImpulse(const std::string& id, const glm::vec3& impulse);
    void applyAngularImpulse(const std::string& id, const glm::vec3& impulse);

    // Collision Shapes
    void addBoxShape(const std::string& id, const glm::vec3& halfExtents);
    void addSphereShape(const std::string& id, float radius);
    void addCylinderShape(const std::string& id, const glm::vec3& halfExtents);
    void addCapsuleShape(const std::string& id, float radius, float height);
    void addConvexHullShape(const std::string& id,
                           const std::vector<glm::vec3>& points);
    void addTriangleMeshShape(const std::string& id,
                             const std::vector<glm::vec3>& vertices,
                             const std::vector<int>& indices);
    void removeShape(const std::string& id);
    void setShapeScale(const std::string& id, const glm::vec3& scale);

    // Constraints
    void addPoint2PointConstraint(const std::string& id,
                                 const std::string& bodyA,
                                 const std::string& bodyB,
                                 const glm::vec3& pivotA,
                                 const glm::vec3& pivotB);
    void addHingeConstraint(const std::string& id,
                           const std::string& bodyA,
                           const std::string& bodyB,
                           const glm::vec3& pivotA,
                           const glm::vec3& pivotB,
                           const glm::vec3& axisA,
                           const glm::vec3& axisB);
    void addSliderConstraint(const std::string& id,
                            const std::string& bodyA,
                            const std::string& bodyB,
                            const glm::mat4& frameA,
                            const glm::mat4& frameB);
    void removeConstraint(const std::string& id);

    // Collision Detection
    bool checkCollision(const std::string& bodyA, const std::string& bodyB);
    std::vector<std::string> getCollidingBodies(const std::string& bodyId);
    glm::vec3 getContactPoint(const std::string& bodyA, const std::string& bodyB);
    glm::vec3 getContactNormal(const std::string& bodyA, const std::string& bodyB);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void initializeBullet();
    void updateRigidBodies();
    void processCollisions();
    btVector3 toBulletVector(const glm::vec3& vec);
    btQuaternion toBulletQuaternion(const glm::quat& quat);
    glm::vec3 fromBulletVector(const btVector3& vec);
    glm::quat fromBulletQuaternion(const btQuaternion& quat);
};

} // namespace VRMusicStudio 