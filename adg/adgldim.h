#ifndef __ADGLDIM_H__
#define __ADGLDIM_H__

#include <adg/adgdim.h>


G_BEGIN_DECLS

#define ADG_TYPE_LDIM             (adg_ldim_get_type ())
#define ADG_LDIM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_LDIM, AdgLDim))
#define ADG_LDIM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_LDIM, AdgLDimClass))
#define ADG_IS_LDIM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_LDIM))
#define ADG_IS_LDIM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_LDIM))
#define ADG_LDIM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_LDIM, AdgLDimClass))


typedef struct _AdgLDim       AdgLDim;
typedef struct _AdgLDimClass  AdgLDimClass;

struct _AdgLDim
{
  AdgDim                 dim;

  /*< private >*/

  double                 direction;
};

struct _AdgLDimClass
{
  AdgDimClass            parent_class;
};


GType           adg_ldim_get_type               (void) G_GNUC_CONST;

AdgEntity *     adg_ldim_new                    (void);
AdgEntity *     adg_ldim_new_full               (const AdgPair  *ref1,
                                                 const AdgPair  *ref2,
                                                 double          direction,
                                                 const AdgPair  *pos);
AdgEntity *     adg_ldim_new_full_explicit      (double          ref1_x,
                                                 double          ref1_y,
                                                 double          ref2_x,
                                                 double          ref2_y,
                                                 double          direction,
                                                 double          pos_x,
                                                 double          pos_y);

void            adg_ldim_set_pos                (AdgLDim        *ldim,
                                                 const AdgPair  *pos);
void            adg_ldim_set_pos_explicit       (AdgLDim        *ldim,
                                                 double          pos_x,
                                                 double          pos_y);
double          adg_ldim_get_direction          (AdgLDim        *ldim);
void            adg_ldim_set_direction          (AdgLDim        *ldim,
                                                 double          direction);

G_END_DECLS


#endif /* __ADGLDIM_H__ */
