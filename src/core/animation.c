#include "animation.h"
#include "timing.h"

typedef struct translate
{
  DA *bro;
  DA *brm;
  DA *brostartms;
  DA *broendms;
  DA *brmstartms;
  DA *brmendms;
  DA *brotargetvec;
  DA *brmtargetvec;
  DA *brocurrentvec;
  DA *brmcurrentvec;
  DA *brop;
  DA *broduration;
  DA *brmduration;
} translate;

translate tranim;

void init_animations(void)
{
  tranim.bro = create_DA_HIGH_MEMORY(sizeof(br_object *));
  tranim.brm = create_DA_HIGH_MEMORY(sizeof(br_object_manager *));
  tranim.brostartms = create_DA_HIGH_MEMORY(sizeof(double));
  tranim.broendms = create_DA_HIGH_MEMORY(sizeof(double));
  tranim.brmstartms = create_DA_HIGH_MEMORY(sizeof(double));
  tranim.brmendms = create_DA_HIGH_MEMORY(sizeof(double));
  tranim.brotargetvec = create_DA_HIGH_MEMORY(sizeof(vec3));
  tranim.brmtargetvec = create_DA_HIGH_MEMORY(sizeof(vec3));
  tranim.brocurrentvec = create_DA_HIGH_MEMORY(sizeof(vec3));
  tranim.brmcurrentvec = create_DA_HIGH_MEMORY(sizeof(vec3));
  tranim.brop = create_DA_HIGH_MEMORY(sizeof(unsigned char));
  tranim.broduration = create_DA_HIGH_MEMORY(sizeof(double));
  tranim.brmduration = create_DA_HIGH_MEMORY(sizeof(double));
}

void add_animation_translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic, double durationms)
{
  remove_animation_translate_br_object(obj);
  double current = get_timems();
  double end = current + durationms;
  vec3 currentv = {0, 0, 0};
  pushback_DA(tranim.bro, &obj);
  pushback_DA(tranim.brostartms, &current);
  pushback_DA(tranim.broendms, &end);
  pushback_DA(tranim.brotargetvec, v);
  pushback_DA(tranim.brocurrentvec, currentv);
  pushback_DA(tranim.brop, &effect_physic);
  pushback_DA(tranim.broduration, &durationms);
}

void add_animation_translate_br_manager(br_object_manager *manager, vec3 v, double durationms)
{
  remove_animation_translate_br_manager(manager);
  double current = get_timems();
  double end = current + durationms;
  vec3 currentv = {0, 0, 0};
  pushback_DA(tranim.brm, &manager);
  pushback_DA(tranim.brmstartms, &current);
  pushback_DA(tranim.brmendms, &end);
  pushback_DA(tranim.brmtargetvec, v);
  pushback_DA(tranim.brmcurrentvec, currentv);
  pushback_DA(tranim.brmduration, &durationms);
}

void remove_animation_translate_br_object(br_object *obj)
{
  unsigned int index = get_index_DA(tranim.bro, &obj);
  remove_DA(tranim.bro, index);
  remove_DA(tranim.brostartms, index);
  remove_DA(tranim.broendms, index);
  remove_DA(tranim.brotargetvec, index);
  remove_DA(tranim.brocurrentvec, index);
  remove_DA(tranim.brop, index);
  remove_DA(tranim.broduration, index);
}

void remove_animation_translate_br_manager(br_object_manager *manager)
{
  unsigned int index = get_index_DA(tranim.brm, &manager);
  remove_DA(tranim.brm, index);
  remove_DA(tranim.brmstartms, index);
  remove_DA(tranim.brmendms, index);
  remove_DA(tranim.brmtargetvec, index);
  remove_DA(tranim.brmcurrentvec, index);
  remove_DA(tranim.brmduration, index);
}

void play_animations(void)
{
  double current = get_timems();
  vec3 v;
  unsigned int index = 0;
start:
  br_object **obj = get_data_DA(tranim.bro);
  double *startms = get_data_DA(tranim.brostartms);
  double *endms = get_data_DA(tranim.broendms);
  vec3 *vec = get_data_DA(tranim.brotargetvec);
  vec3 *veccurrent = get_data_DA(tranim.brocurrentvec);
  unsigned char *phy = get_data_DA(tranim.brop);
  double *duration = get_data_DA(tranim.broduration);
  for (unsigned int i = index; i < get_size_DA(tranim.bro); i++)
  {
    if (current >= endms[i])
    {
      glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
      translate_br_object(obj[i], veccurrent[i], phy[i]);
      translate_br_object(obj[i], vec[i], phy[i]);

      remove_DA(tranim.bro, i);
      remove_DA(tranim.brostartms, i);
      remove_DA(tranim.broendms, i);
      remove_DA(tranim.brotargetvec, i);
      remove_DA(tranim.brocurrentvec, i);
      remove_DA(tranim.brop, i);
      remove_DA(tranim.broduration, i);
      index = i;
      goto start;
    }
    glm_vec3_scale(vec[i], (float)((current - startms[i]) / duration[i]), v);
    glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
    translate_br_object(obj[i], veccurrent[i], phy[i]);
    glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
    glm_vec3_copy(v, veccurrent[i]);
    translate_br_object(obj[i], v, phy[i]);
  }
  index = 0;
start2:
  br_object_manager **mng = get_data_DA(tranim.brm);
  startms = get_data_DA(tranim.brmstartms);
  endms = get_data_DA(tranim.brmendms);
  vec = get_data_DA(tranim.brmtargetvec);
  veccurrent = get_data_DA(tranim.brmcurrentvec);
  duration = get_data_DA(tranim.brmduration);
  for (unsigned int i = index; i < get_size_DA(tranim.brm); i++)
  {
    if (current >= endms[i])
    {
      glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
      translate_br_object_all(mng[i], veccurrent[i]);
      translate_br_object_all(mng[i], vec[i]);

      remove_DA(tranim.brm, i);
      remove_DA(tranim.brmstartms, i);
      remove_DA(tranim.brmendms, i);
      remove_DA(tranim.brmtargetvec, i);
      remove_DA(tranim.brmcurrentvec, i);
      remove_DA(tranim.brmduration, i);
      index = i;
      goto start2;
    }
    glm_vec3_scale(vec[i], (float)((current - startms[i]) / duration[i]), v);
    glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
    translate_br_object_all(mng[i], veccurrent[i]);
    glm_vec3_scale(veccurrent[i], -1, veccurrent[i]);
    glm_vec3_copy(v, veccurrent[i]);
    translate_br_object_all(mng[i], v);
  }
}

void delete_animations(void)
{
  delete_DA(tranim.bro);
  delete_DA(tranim.brm);
  delete_DA(tranim.brostartms);
  delete_DA(tranim.broendms);
  delete_DA(tranim.brmstartms);
  delete_DA(tranim.brmendms);
  delete_DA(tranim.brotargetvec);
  delete_DA(tranim.brmtargetvec);
  delete_DA(tranim.brocurrentvec);
  delete_DA(tranim.brmcurrentvec);
  delete_DA(tranim.brop);
  delete_DA(tranim.broduration);
  delete_DA(tranim.brmduration);
}

unsigned char has_animation_br_object(br_object *obj)
{
  unsigned int index = get_index_DA(tranim.bro, &obj);
  if (index == UINT_MAX)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

unsigned char has_animation_br_manager(br_object_manager *manager)
{
  unsigned int index = get_index_DA(tranim.brm, &manager);
  if (index == UINT_MAX)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}