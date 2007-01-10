#ifndef __ADGADIM_H__
#define __ADGADIM_H__

#include <adg/adgdim.h>


G_BEGIN_DECLS

#define ADG_TYPE_ADIM             (adg_adim_get_type ())
#define ADG_ADIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_ADIM, AdgADim))
#define ADG_ADIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_ADIM, AdgADimClass))
#define ADG_IS_ADIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_ADIM))
#define ADG_IS_ADIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_ADIM))
#define ADG_ADIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_ADIM, AdgADimClass))


typedef struct _AdgADim       AdgADim;
typedef struct _AdgADimClass  AdgADimClass;

struct _AdgADim
{
  AdgDim                 dim;

  /*< private >*/
};

struct _AdgADimClass
{
  AdgDimClass            parent_class;
};


GType           adg_adim_get_type               (void) G_GNUC_CONST;

AdgEntity *     adg_adim_new                    (void);
AdgEntity *     adg_adim_new_full               (const AdgPair  *ref1,
                                                 const AdgPair  *ref2,
                                                 double          direction,
                                                 const AdgPair  *pos);
AdgEntity *     adg_adim_new_full_explicit      (double          ref1_x,
                                                 double          ref1_y,
                                                 double          ref2_x,
                                                 double          ref2_y,
                                                 double          direction,
                                                 double          pos_x,
                                                 double          pos_y);

void            adg_adim_set_pos                (AdgADim        *adim,
                                                 const AdgPair  *pos);
void            adg_adim_set_pos_explicit       (AdgADim        *adim,
                                                 double          pos_x,
                                                 double          pos_y);
double          adg_adim_get_direction          (AdgADim        *adim);
void            adg_adim_set_direction          (AdgADim        *adim,
                                                 double          direction);

G_END_DECLS


#endif /* __ADGADIM_H__ */
