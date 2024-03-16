#include "jolt_physics.h"

#include "../../third_party/jolt/Jolt/Jolt.h"
#include "../../third_party/jolt/Jolt/RegisterTypes.h"
#include "../../third_party/jolt/Jolt/Core/Factory.h"
#include "../../third_party/jolt/Jolt/Core/TempAllocator.h"
#include "../../third_party/jolt/Jolt/Core/JobSystemThreadPool.h"
#include "../../third_party/jolt/Jolt/Physics/PhysicsSettings.h"
#include "../../third_party/jolt/Jolt/Physics/PhysicsSystem.h"
#include "../../third_party/jolt/Jolt/Physics/Collision/Shape/BoxShape.h"
#include "../../third_party/jolt/Jolt/Physics/Collision/Shape/SphereShape.h"
#include "../../third_party/jolt/Jolt/Physics/Body/BodyCreationSettings.h"
#include "../../third_party/jolt/Jolt/Physics/Body/BodyActivationListener.h"
#include "../../third_party/jolt/Jolt/Physics/Collision/Shape/HeightFieldShape.h"
#include "../../third_party/jolt/Jolt/Physics/Character/CharacterVirtual.h"
#include "../../third_party/jolt/Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include "../../third_party/jolt/Jolt/Physics/Collision/Shape/StaticCompoundShape.h"

// Disable common warnings triggered by Jolt, you can use JPH_SUPPRESS_WARNING_PUSH / JPH_SUPPRESS_WARNING_POP to store and restore the warning state
JPH_SUPPRESS_WARNINGS

// All Jolt symbols are in the JPH namespace
using namespace JPH;

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// We're also using STL classes in this example
using namespace std;

extern "C" struct bodyid
{
  BodyID x;
};

extern "C" struct playerid
{
  CharacterVirtual *x;
};

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

// Layer that objects can be in, determines which other objects it can collide with
// Typically you at least want to have 1 layer for moving bodies and 1 layer for static bodies, but you can have more
// layers if you want. E.g. you could have a layer for high detail collision (which is not used by the physics simulation
// but only if you do collision testing).
namespace Layers
{
  static constexpr ObjectLayer NON_MOVING = 0;
  static constexpr ObjectLayer MOVING = 1;
  static constexpr ObjectLayer NUM_LAYERS = 2;
};

// Each broadphase layer results in a separate bounding volume tree in the broad phase. You at least want to have
// a layer for non-moving and moving objects to avoid having to update a tree full of static objects every frame.
// You can have a 1-on-1 mapping between object layers and broadphase layers (like in this case) but if you have
// many object layers you'll be creating many broad phase trees, which is not efficient. If you want to fine tune
// your broadphase layers define JPH_TRACK_BROADPHASE_STATS and look at the stats reported on the TTY.
namespace BroadPhaseLayers
{
  static constexpr BroadPhaseLayer NON_MOVING(0);
  static constexpr BroadPhaseLayer MOVING(1);
  static constexpr uint NUM_LAYERS(2);
};

// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
  BPLayerInterfaceImpl()
  {
    // Create a mapping table from object to broad phase layer
    mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
    mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
  }

  virtual uint GetNumBroadPhaseLayers() const override
  {
    return BroadPhaseLayers::NUM_LAYERS;
  }

  virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
  {
    JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
    return mObjectToBroadPhase[inLayer];
  }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
  virtual const char *GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
  {
    switch ((BroadPhaseLayer::Type)inLayer)
    {
    case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
      return "NON_MOVING";
    case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
      return "MOVING";
    default:
      JPH_ASSERT(false);
      return "INVALID";
    }
  }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
  BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

/// Class that determines if an object layer can collide with a broadphase layer
class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
  virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
  {
    switch (inLayer1)
    {
    case Layers::NON_MOVING:
      return inLayer2 == BroadPhaseLayers::MOVING;
    case Layers::MOVING:
      return true;
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

/// Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
  virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
  {
    switch (inObject1)
    {
    case Layers::NON_MOVING:
      return inObject2 == Layers::MOVING; // Non moving only collides with moving
    case Layers::MOVING:
      return true; // Moving collides with everything
    default:
      JPH_ASSERT(false);
      return false;
    }
  }
};

PhysicsSystem physics_system;

const float cOriginDeltaTime = 1.0f / 60.0f;

// We need a temp allocator for temporary allocations during the physics update. We're
// pre-allocating 10 MB to avoid having to do allocations during the physics update.
// B.t.w. 10 MB is way too much for this example but it is a typical value you can use.
// If you don't want to pre-allocate you can also use TempAllocatorMalloc to fall back to
// malloc / free.
TempAllocatorImpl *temp_allocator = 0;

// We need a job system that will execute physics jobs on multiple threads. Typically
// you would implement the JobSystem interface yourself and let Jolt Physics run on top
// of your own job scheduler. JobSystemThreadPool is an example implementation.
JobSystemThreadPool *job_system = 0;

// Create mapping table from object layer to broadphase layer
// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
BPLayerInterfaceImpl broad_phase_layer_interface;

// Create class that filters object vs broadphase layers
// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

// Create class that filters object vs object layers
// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
ObjectLayerPairFilterImpl object_vs_object_layer_filter;

extern "C" void init_jolt(float *gravity)
{
  // Register allocation hook
  RegisterDefaultAllocator();

  // Create a factory
  Factory::sInstance = new Factory();

  // Register all Jolt physics types
  RegisterTypes();

  temp_allocator = new TempAllocatorImpl(512 * 1024 * 1024);

  job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

  // This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
  // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
  const uint cMaxBodies = 65536;

  // This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
  const uint cNumBodyMutexes = 0;

  // This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
  // body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
  // too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
  // Note: This value is low because this is a simple test. For a real project use something in the order of 65536.
  const uint cMaxBodyPairs = 65536;

  // This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
  // number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
  // Note: This value is low because this is a simple test. For a real project use something in the order of 10240.
  const uint cMaxContactConstraints = 10240;

  // Now we can create the actual physics system.
  physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface,
                      object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

  // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
  // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
  // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
  physics_system.OptimizeBroadPhase();
  physics_system.SetGravity(Vec3(gravity[0], gravity[1], gravity[2]));
}

extern "C" void deinit_jolt(void)
{
  delete temp_allocator;
  delete job_system;
  temp_allocator = 0;
  job_system = 0;
  // Unregisters all types with the factory and cleans up the default material
  UnregisterTypes();

  // Destroy the factory
  delete Factory::sInstance;
  Factory::sInstance = nullptr;
}

extern "C" void run_jolt(float cDeltaTime)
{
  if (cDeltaTime < cOriginDeltaTime)
  {
    cDeltaTime = cOriginDeltaTime;
  }
  physics_system.Update(cDeltaTime, (int)(cDeltaTime / cOriginDeltaTime), temp_allocator, job_system);
}

extern "C" bodyid *create_hm_jolt(float *heightmappoints, float *offset, float *scale, unsigned int length,
                                  float friction, float restitution, float gravityfactor)
{
  Vec3 xoffset(offset[0], offset[1], offset[2]);
  Vec3 xscale(scale[0], scale[1], scale[2]);
  HeightFieldShapeSettings settings(heightmappoints, xoffset, xscale, (uint32)length);
  settings.mBlockSize = 8;
  settings.mBitsPerSample = 8;
  ShapeSettings::ShapeResult floor_shape_result = settings.Create();
  ShapeRefC floor_shape = floor_shape_result.Get();
  BodyCreationSettings floor_settings(floor_shape, RVec3::sZero(), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
  floor_settings.mFriction = friction;
  floor_settings.mRestitution = restitution;
  floor_settings.mGravityFactor = gravityfactor;
  Body *floor = physics_system.GetBodyInterface().CreateBody(floor_settings);
  physics_system.GetBodyInterface().AddBody(floor->GetID(), EActivation::Activate);
  bodyid *res = new bodyid;
  res->x = floor->GetID();
  return res;
}

extern "C" void delete_body_jolt(bodyid *id)
{
  physics_system.GetBodyInterface().RemoveBody(id->x);
  physics_system.GetBodyInterface().DestroyBody(id->x);
  delete id;
}

extern "C" void optimize_jolt(void)
{
  physics_system.OptimizeBroadPhase();
}

extern "C" bodyid *create_box_jolt(float *boxsize, float *center, float friction, float restitution, float gravityfactor, unsigned char noangular, unsigned int type)
{
  BoxShapeSettings floor_shape_settings(Vec3(boxsize[0] / 2, boxsize[1] / 2, boxsize[2] / 2));
  ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
  ShapeRefC floor_shape = floor_shape_result.Get();
  BodyCreationSettings *floor_settings = 0;
  if (type == 0)
  {
    floor_settings = new BodyCreationSettings(floor_shape, RVec3(center[0], center[1], center[2]), Quat::sIdentity(),
                                              EMotionType::Static, Layers::NON_MOVING);
  }
  else if (type == 1)
  {
    floor_settings = new BodyCreationSettings(floor_shape, RVec3(center[0], center[1], center[2]), Quat::sIdentity(),
                                              EMotionType::Kinematic, Layers::MOVING);
  }
  else if (type == 2)
  {
    floor_settings = new BodyCreationSettings(floor_shape, RVec3(center[0], center[1], center[2]), Quat::sIdentity(),
                                              EMotionType::Dynamic, Layers::MOVING);
  }
  floor_settings->mFriction = friction;
  floor_settings->mRestitution = restitution;
  floor_settings->mGravityFactor = gravityfactor;
  floor_settings->mEnhancedInternalEdgeRemoval = true;
  if (noangular == 1)
  {
    floor_settings->mMaxAngularVelocity = 0;
    floor_settings->mAngularDamping = 1;
  }
  Body *floor = physics_system.GetBodyInterface().CreateBody(*floor_settings);
  delete floor_settings;
  physics_system.GetBodyInterface().AddBody(floor->GetID(), EActivation::Activate);
  bodyid *res = new bodyid;
  res->x = floor->GetID();
  return res;
}

extern "C" void set_linear_velocity_jolt(bodyid *id, float *velocity)
{
  physics_system.GetBodyInterface().SetLinearVelocity(id->x, Vec3(velocity[0], velocity[1], velocity[2]));
}

extern "C" void set_angular_velocity_jolt(bodyid *id, float *velocity)
{
  physics_system.GetBodyInterface().SetAngularVelocity(id->x, Vec3(velocity[0], velocity[1], velocity[2]));
}

extern "C" void set_position_jolt(bodyid *id, float *position)
{
  physics_system.GetBodyInterface().SetPosition(id->x, Vec3(position[0], position[1], position[2]), EActivation::Activate);
}

extern "C" void set_rotation_jolt(bodyid *id, float *rotation)
{
  physics_system.GetBodyInterface().SetRotation(id->x, Quat(rotation[0], rotation[1], rotation[2], rotation[3]), EActivation::Activate);
}

extern "C" void add_force_jolt(bodyid *id, float *force)
{
  physics_system.GetBodyInterface().AddForce(id->x, Vec3(force[0], force[1], force[2]));
}

extern "C" void get_linear_velocity_jolt(bodyid *id, float *velocity)
{
  velocity[0] = physics_system.GetBodyInterface().GetLinearVelocity(id->x)[0];
  velocity[1] = physics_system.GetBodyInterface().GetLinearVelocity(id->x)[1];
  velocity[2] = physics_system.GetBodyInterface().GetLinearVelocity(id->x)[2];
}

extern "C" void get_angular_velocity_jolt(bodyid *id, float *velocity)
{
  velocity[0] = physics_system.GetBodyInterface().GetAngularVelocity(id->x)[0];
  velocity[1] = physics_system.GetBodyInterface().GetAngularVelocity(id->x)[1];
  velocity[2] = physics_system.GetBodyInterface().GetAngularVelocity(id->x)[2];
}

extern "C" void get_position_jolt(bodyid *id, float *position)
{
  position[0] = physics_system.GetBodyInterface().GetPosition(id->x)[0];
  position[1] = physics_system.GetBodyInterface().GetPosition(id->x)[1];
  position[2] = physics_system.GetBodyInterface().GetPosition(id->x)[2];
}

extern "C" void get_rotation_jolt(bodyid *id, float *rotation)
{
  rotation[0] = physics_system.GetBodyInterface().GetRotation(id->x).GetXYZW()[0];
  rotation[1] = physics_system.GetBodyInterface().GetRotation(id->x).GetXYZW()[1];
  rotation[2] = physics_system.GetBodyInterface().GetRotation(id->x).GetXYZW()[2];
  rotation[3] = physics_system.GetBodyInterface().GetRotation(id->x).GetXYZW()[3];
}

bodyid *create_hm_voxel_jolt(int **hm, int dimensionx, int dimensionz, int startx, int startz, int widthx, int widthz,
                             float friction, float restitution)
{
  Ref<StaticCompoundShapeSettings> compound_shape = new StaticCompoundShapeSettings;
  unsigned char **done = (unsigned char **)calloc(dimensionz, sizeof(unsigned char *));
  for (int i = 0; i < dimensionz; i++)
  {
    done[i] = (unsigned char *)calloc(dimensionx, sizeof(unsigned char));
  }
  for (int i2 = startz; i2 < startz + widthz; i2++)
  {
    for (int i = startx; i < startx + widthx; i++)
    {
      if (i < dimensionx && i2 < dimensionz)
      {
        if (done[i][i2] == 0)
        {
          int loop = 0;
          unsigned char x = 0;
          if (i < dimensionx - 1 && hm[i + 1][i2] == hm[i][i2] && done[i + 1][i2] == 0)
          {
            x = 1;
          }
          else if (i2 < dimensionz - 1 && hm[i][i2 + 1] == hm[i][i2] && done[i][i2 + 1] == 0)
          {
            x = 0;
          }
          else
          {
            x = 2;
          }
          if (x == 2)
          {
            compound_shape->AddShape(Vec3((float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))),
                                     Quat::sIdentity(), new BoxShape(Vec3(0.5f, 0.5f, 0.5f)));
            done[i][i2] = 1;
          }
          else if (x == 0)
          {
            while (i2 + loop < dimensionz - 1 && hm[i][i2 + loop] == hm[i][i2] && done[i][i2 + loop] == 0)
            {
              done[i][i2 + loop] = 1;
              loop++;
            }
            compound_shape->AddShape(Vec3((float)(i - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 + (loop - 1) / 2.0f - (int)(dimensionz / 2))),
                                     Quat::sIdentity(), new BoxShape(Vec3(0.5f, 0.5f, (float)loop / 2.0f)));
          }
          else if (x == 1)
          {
            while (i + loop < dimensionx - 1 && hm[i + loop][i2] == hm[i][i2] && done[i + loop][i2] == 0)
            {
              done[i + loop][i2] = 1;
              loop++;
            }
            compound_shape->AddShape(Vec3((float)(i + (loop - 1) / 2.0f - (int)(dimensionx / 2)), (float)hm[i][i2], (float)(i2 - (int)(dimensionz / 2))),
                                     Quat::sIdentity(), new BoxShape(Vec3((float)loop / 2.0f, 0.5f, 0.5f)));
          }
        }

        if (hm[i][i2] != 0)
        {
          for (int i3 = hm[i][i2] - 1; i3 >= 0; i3--)
          {
            if ((!(i2 > 0) || hm[i][i2 - 1] >= i3) &&
                (!(i2 < dimensionz - 1) || hm[i][i2 + 1] >= i3) &&
                (!(i > 0) || hm[i - 1][i2] >= i3) &&
                (!(i < dimensionx - 1) || hm[i + 1][i2] >= i3))
            {
              break;
            }
            compound_shape->AddShape(Vec3((float)(i - (int)(dimensionx / 2)), (float)i3, (float)(i2 - (int)(dimensionz / 2))),
                                     Quat::sIdentity(), new BoxShape(Vec3(0.5f, 0.5f, 0.5f)));
          }
        }
      }
    }
  }
  for (int i = 0; i < dimensionz; i++)
  {
    free(done[i]);
  }
  free(done);
  BodyCreationSettings floor_settings = BodyCreationSettings(compound_shape, Vec3::sZero(), Quat::sIdentity(),
                                                             EMotionType::Static, Layers::NON_MOVING);
  floor_settings.mFriction = friction;
  floor_settings.mRestitution = restitution;
  floor_settings.mEnhancedInternalEdgeRemoval = true;
  Body *floor = physics_system.GetBodyInterface().CreateBody(floor_settings);
  physics_system.GetBodyInterface().AddBody(floor->GetID(), EActivation::Activate);
  bodyid *res = new bodyid;
  res->x = floor->GetID();
  return res;
}

unsigned int get_body_count_jolt(void)
{
  return physics_system.GetNumBodies();
}

unsigned int get_active_body_count_jolt(void)
{
  return physics_system.GetNumActiveBodies(EBodyType::RigidBody);
}

void get_gravity_jolt(float *vec)
{
  Vec3 x = physics_system.GetGravity();
  vec[0] = x[0];
  vec[1] = x[1];
  vec[2] = x[2];
}

void set_gravity_jolt(float *vec)
{
  physics_system.SetGravity(Vec3(vec[0], vec[1], vec[2]));
}

playerid *create_player_jolt(float height, float radius, float maxslopeangle, float maxstrength, float mass, float *pos)
{
  CharacterVirtualSettings settings = CharacterVirtualSettings();
  settings.mMaxSlopeAngle = DegreesToRadians(maxslopeangle);
  settings.mMaxStrength = maxstrength;
  settings.mMass = mass;
  JPH::CapsuleShape *shape = new CapsuleShape(0.5f * height, radius);
  settings.mShape = RotatedTranslatedShapeSettings(Vec3(0, 0.5f * height + radius, 0), Quat::sIdentity(), shape).Create().Get();
  settings.mBackFaceMode = EBackFaceMode::CollideWithBackFaces;
  settings.mCharacterPadding = 0.02f;
  settings.mPenetrationRecoverySpeed = 1.0f;
  settings.mPredictiveContactDistance = 0.1f;
  settings.mSupportingVolume = Plane(Vec3::sAxisY(), -radius);
  CharacterVirtual *x = new CharacterVirtual(&settings, RVec3(pos[0], pos[1], pos[2]), Quat::sIdentity(), &physics_system);
  x->SetUp(Vec3(0, 1, 0));
  playerid *res = new playerid;
  res->x = x;
  return res;
}

void delete_player_jolt(playerid *x)
{
  delete x->x;
  delete x;
}

void set_linear_velocity_player_jolt(playerid *id, float *velocity)
{
  id->x->SetLinearVelocity(Vec3(velocity[0], velocity[1], velocity[2]));
}

void set_position_player_jolt(playerid *id, float *position)
{
  id->x->SetPosition(RVec3(position[0], position[1], position[2]));
}

void set_rotation_player_jolt(playerid *id, float *rotation)
{
  id->x->SetRotation(Quat(rotation[0], rotation[1], rotation[2], rotation[3]));
}

void get_linear_velocity_player_jolt(playerid *id, float *velocity)
{
  velocity[0] = id->x->GetLinearVelocity()[0];
  velocity[1] = id->x->GetLinearVelocity()[1];
  velocity[2] = id->x->GetLinearVelocity()[2];
}

void get_position_player_jolt(playerid *id, float *position)
{
  position[0] = id->x->GetPosition()[0];
  position[1] = id->x->GetPosition()[1];
  position[2] = id->x->GetPosition()[2];
}

void get_rotation_player_jolt(playerid *id, float *rotation)
{
  rotation[0] = id->x->GetRotation().GetXYZW()[0];
  rotation[1] = id->x->GetRotation().GetXYZW()[1];
  rotation[2] = id->x->GetRotation().GetXYZW()[2];
  rotation[3] = id->x->GetRotation().GetXYZW()[3];
}

unsigned char is_supported_jolt(playerid *id)
{
  return id->x->IsSupported();
}

void update_player_jolt(playerid *id, float deltatime, unsigned char enablesticktofloor, unsigned char enablewalkstairs,
                        float *input_velocity, float gravity_factor)
{
  Vec3 velocity = Vec3::sZero();
  id->x->UpdateGroundVelocity();
  bool moving_towards_ground = (input_velocity[1] - id->x->GetGroundVelocity().GetY()) < 0.1f;
  if (id->x->GetGroundState() == CharacterVirtual::EGroundState::OnGround && moving_towards_ground)
  {
    velocity = id->x->GetGroundVelocity() + Vec3(input_velocity[0], input_velocity[1], input_velocity[2]) +
               deltatime * physics_system.GetGravity() * gravity_factor;
  }
  else
  {
    velocity = Vec3(input_velocity[0], input_velocity[1], input_velocity[2]) +
               deltatime * physics_system.GetGravity() * gravity_factor;
  }
  id->x->SetLinearVelocity(velocity);
  CharacterVirtual::ExtendedUpdateSettings update_settings;
  if (!enablesticktofloor)
  {
    update_settings.mStickToFloorStepDown = Vec3::sZero();
  }
  else
  {
    update_settings.mStickToFloorStepDown = -id->x->GetUp() * update_settings.mStickToFloorStepDown.Length();
  }
  if (!enablewalkstairs)
  {
    update_settings.mWalkStairsStepUp = Vec3::sZero();
  }
  else
  {
    update_settings.mWalkStairsStepUp = id->x->GetUp() * update_settings.mWalkStairsStepUp.Length();
  }
  id->x->ExtendedUpdate(deltatime, -id->x->GetUp() * physics_system.GetGravity().Length() * gravity_factor, update_settings,
                        physics_system.GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
                        physics_system.GetDefaultLayerFilter(Layers::MOVING), {}, {}, *temp_allocator);
}