/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#ifndef __HIPPO_CANVAS_TEXT_H__
#define __HIPPO_CANVAS_TEXT_H__

/* A text paragraph canvas item. */

#include "hippo-canvas-box.h"
#include <cairo.h>
#include <pango/pango.h>

G_BEGIN_DECLS

typedef enum {
    HIPPO_CANVAS_SIZE_FULL_WIDTH,
    HIPPO_CANVAS_SIZE_WRAP_WORD,
    HIPPO_CANVAS_SIZE_ELLIPSIZE_END
} HippoCanvasSizeMode;

typedef struct _HippoCanvasText      HippoCanvasText;
typedef struct _HippoCanvasTextClass HippoCanvasTextClass;

#define HIPPO_TYPE_CANVAS_TEXT              (hippo_canvas_text_get_type ())
#define HIPPO_CANVAS_TEXT(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), HIPPO_TYPE_CANVAS_TEXT, HippoCanvasText))
#define HIPPO_CANVAS_TEXT_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), HIPPO_TYPE_CANVAS_TEXT, HippoCanvasTextClass))
#define HIPPO_IS_CANVAS_TEXT(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), HIPPO_TYPE_CANVAS_TEXT))
#define HIPPO_IS_CANVAS_TEXT_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), HIPPO_TYPE_CANVAS_TEXT))
#define HIPPO_CANVAS_TEXT_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), HIPPO_TYPE_CANVAS_TEXT, HippoCanvasTextClass))

struct _HippoCanvasText {
    HippoCanvasBox box;
    char *text;
    PangoAttrList *attributes;
    double font_scale;
    guint size_mode : 3;
    guint is_ellipsized : 1;
};

struct _HippoCanvasTextClass {
    HippoCanvasBoxClass parent_class;

};

GType            hippo_canvas_text_get_type               (void) G_GNUC_CONST;

HippoCanvasItem* hippo_canvas_text_new    (void);


G_END_DECLS

#endif /* __HIPPO_CANVAS_TEXT_H__ */
