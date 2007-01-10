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

G_END_DECLS


#endif /* __ADGADIM_H__ */
