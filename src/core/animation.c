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
  DA *brovec;
  DA *brmvec;
  DA *brop;
  DA *broduration;
  DA *brmduration;
} translate;

translate tranim;

void init_animations(void)
{
  tranim.bro = create_DA(sizeof(br_object *));
  tranim.brm = create_DA(sizeof(br_object_manager *));
  tranim.brostartms = create_DA(sizeof(double));
  tranim.broendms = create_DA(sizeof(double));
  tranim.brmstartms = create_DA(sizeof(double));
  tranim.brmendms = create_DA(sizeof(double));
  tranim.brovec = create_DA(sizeof(vec3));
  tranim.brmvec = create_DA(sizeof(vec3));
  tranim.brop = create_DA(sizeof(unsigned char));
  tranim.broduration = create_DA(sizeof(double));
  tranim.brmduration = create_DA(sizeof(double));
}

void add_animation_translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic, double durationms)
{
  double current = get_timems();
  double end = current + durationms;
  pushback_DA(tranim.bro, &obj);
  pushback_DA(tranim.brostartms, &current);
  pushback_DA(tranim.broendms, &end);
  pushback_DA(tranim.brovec, &v);
  pushback_DA(tranim.brop, &effect_physic);
  pushback_DA(tranim.broduration, &durationms);
}

void add_animation_translate_br_manager(br_object_manager *manager, vec3 v, double durationms)
{
  double current = get_timems();
  double end = current + durationms;
  pushback_DA(tranim.brm, &manager);
  pushback_DA(tranim.brmstartms, &current);
  pushback_DA(tranim.brmendms, &end);
  pushback_DA(tranim.brmvec, &v);
  pushback_DA(tranim.brmduration, &durationms);
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
  vec3 *vec = get_data_DA(tranim.brovec);
  unsigned char *phy = get_data_DA(tranim.brop);
  double *duration = get_data_DA(tranim.broduration);
  double tmpcur = current;
  for (unsigned int i = index; i < get_size_DA(tranim.bro); i++)
  {
    if (current > endms[i])
    {
      tmpcur = endms[i];
    }
    else
    {
      tmpcur = current;
    }
    if (tmpcur - startms[i] <= 0)
    {
      remove_DA(tranim.bro, i);
      remove_DA(tranim.brostartms, i);
      remove_DA(tranim.broendms, i);
      remove_DA(tranim.brovec, i);
      remove_DA(tranim.brop, i);
      remove_DA(tranim.broduration, i);
      index = i;
      goto start;
    }
    glm_vec3_scale(vec[i], (tmpcur - startms[i]) / duration[i], v);
    translate_br_object(obj[i], v, phy[i]);
    startms[i] = current;
  }
  index = 0;
start2:
  br_object_manager **mng = get_data_DA(tranim.brm);
  startms = get_data_DA(tranim.brmstartms);
  endms = get_data_DA(tranim.brmendms);
  vec = get_data_DA(tranim.brmvec);
  duration = get_data_DA(tranim.brmduration);
  for (unsigned int i = index; i < get_size_DA(tranim.brm); i++)
  {
    if (current > endms[i])
    {
      tmpcur = endms[i];
    }
    else
    {
      tmpcur = current;
    }
    if (tmpcur - startms[i] <= 0)
    {
      remove_DA(tranim.brm, i);
      remove_DA(tranim.brmstartms, i);
      remove_DA(tranim.brmendms, i);
      remove_DA(tranim.brmvec, i);
      remove_DA(tranim.brmduration, i);
      index = i;
      goto start2;
    }
    glm_vec3_scale(vec[i], (tmpcur - startms[i]) / duration[i], v);
    translate_br_object_all(mng[i], v);
    startms[i] = current;
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
  delete_DA(tranim.brovec);
  delete_DA(tranim.brmvec);
  delete_DA(tranim.brop);
  delete_DA(tranim.broduration);
  delete_DA(tranim.brmduration);
}