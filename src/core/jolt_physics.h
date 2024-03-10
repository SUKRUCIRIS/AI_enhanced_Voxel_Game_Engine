#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct bodyid bodyid;

  void init_jolt(float *gravity);

  void deinit_jolt(void);

  void run_jolt(float cDeltaTime);

  // heightmap must be square because of jolt >:(
  bodyid *create_hm_jolt(float *heightmappoints, float *offset, float *scale, unsigned int length,
                         float friction, float restitution, float gravityfactor);

  void delete_body_jolt(bodyid *id);

  void optimize_jolt(void);

  // type 0,1,2 static,kinematic,dynamic
  bodyid *create_box_jolt(float *boxsize, float *center, float friction, float restitution, float gravityfactor, unsigned char noangular, unsigned int type);

  void set_linear_velocity_jolt(bodyid *id, float *velocity);

  void set_angular_velocity_jolt(bodyid *id, float *velocity);

  void set_position_jolt(bodyid *id, float *position);

  void set_rotation_jolt(bodyid *id, float *rotation);

  void add_force_jolt(bodyid *id, float *force);

  void get_linear_velocity_jolt(bodyid *id, float *velocity);

  void get_angular_velocity_jolt(bodyid *id, float *velocity);

  void get_position_jolt(bodyid *id, float *position);

  void get_rotation_jolt(bodyid *id, float *rotation);

#ifdef __cplusplus
}
#endif