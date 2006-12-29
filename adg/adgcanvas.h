#ifndef __ADGCANVAS_H__
#define __ADGCANVAS_H__

#include <adg/adgcontainer.h>
#include <adg/adgstyle.h>


G_BEGIN_DECLS

#define ADG_TYPE_CANVAS             (adg_canvas_get_type ())
#define ADG_CANVAS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), ADG_TYPE_CANVAS, AdgCanvas))
#define ADG_CANVAS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), ADG_TYPE_CANVAS, AdgCanvasClass))
#define ADG_IS_CANVAS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ADG_TYPE_CANVAS))
#define ADG_IS_CANVAS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), ADG_TYPE_CANVAS))
#define ADG_CANVAS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), ADG_TYPE_CANVAS, AdgCanvasClass))

/* AdgCanvas declared in adgentity.h */
typedef struct _AdgCanvasClass  AdgCanvasClass;


struct _AdgCanvas
{
  AdgContainer           container;

  /*< private >*/

  /* Fallback styles */
  AdgLineStyle          *line_style;
  AdgFontStyle          *font_style;
  AdgArrowStyle         *arrow_style;
  AdgDimStyle           *dim_style;
};

struct _AdgCanvasClass
{
  AdgContainerClass      parent_class;
};


GType                    adg_canvas_get_type            (void) G_GNUC_CONST;

AdgCanvas *              adg_canvas_new                 (void);

G_END_DECLS


#endif /* __ADGCANVAS_H__ */

