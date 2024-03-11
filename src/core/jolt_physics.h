#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct bodyid bodyid;

  typedef struct playerid playerid;

  void init_jolt(float *gravity);

  void deinit_jolt(void);

  void run_jolt(float cDeltaTime);

  // heightmap must be square because of jolt >:(
  bodyid *create_hm_jolt(float *heightmappoints, float *offset, float *scale, unsigned int length,
                         float friction, float restitution, float gravityfactor);

  bodyid *create_hm_voxel_jolt(int **hm, int dimensionx, int dimensionz, int startx, int startz, int widthx, int widthz,
                               float friction, float restitution);

  void delete_body_jolt(bodyid *id);

  void optimize_jolt(void);

  // type 0,1,2 static,kinematic,dynamic
  bodyid *create_box_jolt(float *boxsize, float *center, float friction, float restitution, float gravityfactor,
                          unsigned char noangular, unsigned int type);

  void set_linear_velocity_jolt(bodyid *id, float *velocity);

  void set_angular_velocity_jolt(bodyid *id, float *velocity);

  void set_position_jolt(bodyid *id, float *position);

  void set_rotation_jolt(bodyid *id, float *rotation);

  void add_force_jolt(bodyid *id, float *force);

  void get_linear_velocity_jolt(bodyid *id, float *velocity);

  void get_angular_velocity_jolt(bodyid *id, float *velocity);

  void get_position_jolt(bodyid *id, float *position);

  void get_rotation_jolt(bodyid *id, float *rotation);

  unsigned int get_body_count_jolt(void);

  unsigned int get_active_body_count_jolt(void);

  void get_gravity_jolt(float *vec);

  void set_gravity_jolt(float *vec);

  playerid *create_player_jolt(float height, float radius, float maxslopeangle, float maxstrength, float mass, float *pos);

  void delete_player_jolt(playerid *x);

  void set_linear_velocity_player_jolt(playerid *id, float *velocity);

  void set_position_player_jolt(playerid *id, float *position);

  void set_rotation_player_jolt(playerid *id, float *rotation);

  void get_linear_velocity_player_jolt(playerid *id, float *velocity);

  void get_position_player_jolt(playerid *id, float *position);

  void get_rotation_player_jolt(playerid *id, float *rotation);

  unsigned char is_supported_jolt(playerid *id);

  void update_player_jolt(playerid *id, float deltatime, unsigned char enablesticktofloor, unsigned char enablewalkstairs,
                          float *input_velocity, float gravity_factor);

#ifdef __cplusplus
}
#endif