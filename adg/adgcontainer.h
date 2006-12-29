#ifndef __ADGCONTAINER_H__
#define __ADGCONTAINER_H__

#include <adg/adgentity.h>


G_BEGIN_DECLS

#define ADG_TYPE_CONTAINER             (adg_container_get_type ())
#define ADG_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_CONTAINER, AdgContainer))
#define ADG_CONTAINER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_CONTAINER, AdgContainerClass))
#define ADG_IS_CONTAINER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_CONTAINER))
#define ADG_IS_CONTAINER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_CONTAINER))
#define ADG_CONTAINER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_CONTAINER, AdgContainerClass))


typedef struct _AdgContainer       AdgContainer;
typedef struct _AdgContainerClass  AdgContainerClass;


struct _AdgContainer
{
  AdgEntity		 entity;

  /*< private >*/

  GSList                *children;

  AdgMatrix              matrix;
  AdgMatrix              ctm;
};

struct _AdgContainerClass
{
  AdgEntityClass         parent_class;

  /* Signals */

  void                  (*scale)                        (AdgContainer   *container,
                                                         AdgPair        *factor);
};


GType			adg_container_get_type          (void) G_GNUC_CONST;

const AdgMatrix *       adg_container_get_matrix        (AdgContainer   *container);
void                    adg_container_set_matrix        (AdgContainer   *container,
                                                         AdgMatrix      *matrix);

void                    adg_container_scale             (AdgContainer   *container,
                                                         AdgPair        *factor);
void                    adg_container_scale_explicit    (AdgContainer   *container,
                                                         double          sx,
                                                         double          sy);
void                    adg_container_translate         (AdgContainer   *container,
                                                         AdgPair        *device_offset,
                                                         AdgPair        *user_offset);
void                    adg_container_translate_explicit(AdgContainer   *container,
                                                         double          dx,
                                                         double          dy,
                                                         double          ux,
                                                         double          uy);

G_END_DECLS


#endif /* __ADGCONTAINER_H__ */
