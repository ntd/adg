/* -*- mode: C; c-basic-offset: 4; indent-tabs-mode: nil; -*- */
#include "hippo-canvas-type-builtins.h"
#include "hippo-canvas-internal.h"
#include "hippo-canvas-style.h"
#include "hippo-canvas-text.h"
#include "hippo-canvas-box.h"
#include <gdk/gdk.h>
#include <pango/pangocairo.h>
#include <stdlib.h>
#include <string.h>

static void      hippo_canvas_text_init                (HippoCanvasText       *text);
static void      hippo_canvas_text_class_init          (HippoCanvasTextClass  *klass);
static void      hippo_canvas_text_iface_init          (HippoCanvasItemIface   *item_class);
static void      hippo_canvas_text_finalize            (GObject                *object);

static void hippo_canvas_text_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec);
static void hippo_canvas_text_get_property (GObject      *object,
                                            guint         prop_id,
                                            GValue       *value,
                                            GParamSpec   *pspec);


/* Canvas item methods */
static gboolean hippo_canvas_text_button_press_event (HippoCanvasItem    *item,
                                                      HippoEvent         *event);
static void     hippo_canvas_text_set_context        (HippoCanvasItem    *item,
                                                      HippoCanvasContext *context);
static char*    hippo_canvas_text_get_tooltip        (HippoCanvasItem    *item,
                                                      int                 x,
                                                      int                 y,
                                                      HippoRectangle     *for_area);

/* Box methods */
static void hippo_canvas_text_paint_below_children       (HippoCanvasBox *box,
                                                          cairo_t        *cr,
                                                          GdkRegion      *damaged_region);
static void hippo_canvas_text_get_content_width_request  (HippoCanvasBox *box,
                                                          int            *min_width_p,
                                                          int            *natural_width_p);
static void hippo_canvas_text_get_content_height_request (HippoCanvasBox *box,
                                                          int             for_width,
                                                          int            *min_height_p,
                                                          int            *natural_height_p);

enum {
    NO_SIGNALS_YET,
    LAST_SIGNAL
};

/* static int signals[LAST_SIGNAL]; */

enum {
    PROP_0,
    PROP_TEXT,
    PROP_MARKUP,
    PROP_ATTRIBUTES,
    PROP_FONT_SCALE,
    PROP_SIZE_MODE
};

G_DEFINE_TYPE_WITH_CODE(HippoCanvasText, hippo_canvas_text, HIPPO_TYPE_CANVAS_BOX,
                        G_IMPLEMENT_INTERFACE(HIPPO_TYPE_CANVAS_ITEM, hippo_canvas_text_iface_init));

static void
hippo_canvas_text_init(HippoCanvasText *text)
{
    text->font_scale = 1.0;
    text->size_mode = HIPPO_CANVAS_SIZE_FULL_WIDTH;
}

static HippoCanvasItemIface *item_parent_class;

static void
hippo_canvas_text_iface_init(HippoCanvasItemIface *item_class)
{
    item_parent_class = g_type_interface_peek_parent(item_class);

    item_class->button_press_event = hippo_canvas_text_button_press_event;

    item_class->set_context = hippo_canvas_text_set_context;
    item_class->get_tooltip = hippo_canvas_text_get_tooltip;
}

static void
hippo_canvas_text_class_init(HippoCanvasTextClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    HippoCanvasBoxClass *box_class = HIPPO_CANVAS_BOX_CLASS(klass);

    object_class->set_property = hippo_canvas_text_set_property;
    object_class->get_property = hippo_canvas_text_get_property;

    object_class->finalize = hippo_canvas_text_finalize;

    box_class->paint_below_children = hippo_canvas_text_paint_below_children;
    box_class->get_content_width_request = hippo_canvas_text_get_content_width_request;
    box_class->get_content_height_request = hippo_canvas_text_get_content_height_request;

    /**
     * HippoCanvasText:text
     *
     * The text to display.
     */
    g_object_class_install_property(object_class,
                                    PROP_TEXT,
                                    g_param_spec_string("text",
                                                        _("Text"),
                                                        _("Text to display"),
                                                        NULL,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HippoCanvasText:markup
     *
     * The text to display in Pango markup format, allowing you to do bold and other text styles.
     * This is a shortcut way to set the 'text' and 'attributes' properties.
     */
    g_object_class_install_property(object_class,
                                    PROP_MARKUP,
                                    g_param_spec_string("markup",
                                                        _("Markup"),
                                                        _("Marked-up text to display"),
                                                        NULL,
                                                        G_PARAM_WRITABLE));


    /**
     * HippoCanvasText:attributes
     *
     * The Pango attributes of the text; usually it is more convenient to use the 'markup' property
     * than to do the attributes by hand.
     */
    g_object_class_install_property(object_class,
                                    PROP_ATTRIBUTES,
                                    g_param_spec_boxed ("attributes",
                                                        _("Attributes"),
                                                        _("A list of style attributes to apply to the text"),
                                                        PANGO_TYPE_ATTR_LIST,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HippoCanvasText:font-scale
     *
     * A scale factor for the font; Pango exports constants for common
     * factors, such as #PANGO_SCALE_LARGE, #PANGO_SCALE_X_SMALL and
     * so forth. If you want to set an absolute font size, use the
     * 'font' or 'font-desc' properties which are introduced in the
     * HippoCanvasBox base class.
     */
    g_object_class_install_property(object_class,
                                    PROP_FONT_SCALE,
                                    g_param_spec_double("font-scale",
                                                        _("Font scale"),
                                                        _("Scale factor for fonts"),
                                                        0.0,
                                                        100.0,
                                                        1.0,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HippoCanvasText:size-mode
     *
     * Three "size modes" are available. In #HIPPO_CANVAS_SIZE_FULL_WIDTH mode, the
     * text item will give the width of its text as its minimum size. In #HIPPO_CANVAS_SIZE_WRAP_WORD,
     * the text item will wrap to fit available space - its minimum width will be pretty small, but as
     * the width decreases its height will increase. In #HIPPO_CANVAS_SIZE_ELLIPSIZE_END mode, the
     * width of the text will be the natural width of the item, but the minimum width will be small.
     * If the item gets less than its natural width, it will ellipsize the text in order to fit
     * in available space.
     *
     * If none of that made sense, just decide whether you want to
     * always display all the text without wrapping, want to be able
     * to wrap as the available space shrinks, or want to ellipsize
     * the text as the available space shrinks.
     */
    g_object_class_install_property(object_class,
                                    PROP_SIZE_MODE,
                                    g_param_spec_enum("size-mode",
                                                      _("Size mode"),
                                                      _("Mode for size request and allocation"),
                                                      HIPPO_TYPE_CANVAS_SIZE_MODE,
                                                      HIPPO_CANVAS_SIZE_FULL_WIDTH,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));
}

static void
hippo_canvas_text_finalize(GObject *object)
{
    HippoCanvasText *text = HIPPO_CANVAS_TEXT(object);

    g_free(text->text);
    text->text = NULL;

    if (text->attributes) {
        pango_attr_list_unref(text->attributes);
        text->attributes = NULL;
    }

    G_OBJECT_CLASS(hippo_canvas_text_parent_class)->finalize(object);
}

HippoCanvasItem*
hippo_canvas_text_new(void)
{
    HippoCanvasText *text = g_object_new(HIPPO_TYPE_CANVAS_TEXT, NULL);



    return HIPPO_CANVAS_ITEM(text);
}


static void
hippo_canvas_text_set_property(GObject         *object,
                               guint            prop_id,
                               const GValue    *value,
                               GParamSpec      *pspec)
{
    HippoCanvasText *text;

    text = HIPPO_CANVAS_TEXT(object);

    switch (prop_id) {
    case PROP_TEXT:
        {
            const char *new_text;
            new_text = g_value_get_string(value);
            if (!(new_text == text->text ||
                  (new_text && text->text && strcmp(new_text, text->text) == 0))) {
                g_free(text->text);
                text->text = g_strdup(new_text);
                hippo_canvas_item_emit_request_changed(HIPPO_CANVAS_ITEM(text));
                hippo_canvas_item_emit_paint_needed(HIPPO_CANVAS_ITEM(text), 0, 0, -1, -1);
            }
        }
        break;
    case PROP_ATTRIBUTES:
        {
            PangoAttrList *attrs = g_value_get_boxed(value);
            if (attrs)
                pango_attr_list_ref(attrs);
            if (text->attributes)
                pango_attr_list_unref(text->attributes);
            text->attributes = attrs;
            hippo_canvas_item_emit_request_changed(HIPPO_CANVAS_ITEM(text));
            hippo_canvas_item_emit_paint_needed(HIPPO_CANVAS_ITEM(text), 0, 0, -1, -1);
        }
        break;
    case PROP_MARKUP:
        {
            char *text;
            PangoAttrList *attrs;
            GError *error = NULL;

            if (!pango_parse_markup(g_value_get_string(value),
                                    -1,
                                    0,
                                    &attrs,
                                    &text,
                                    NULL,
                                    &error)) {
                g_error("Failed to set markup: %s", error->message);
                return;
            }
            g_object_set(object, "text", text, "attributes", attrs, NULL);
            pango_attr_list_unref(attrs);
            g_free(text);
        }
        break;
    case PROP_FONT_SCALE:
        text->font_scale = g_value_get_double(value);
        hippo_canvas_item_emit_request_changed(HIPPO_CANVAS_ITEM(text));
        hippo_canvas_item_emit_paint_needed(HIPPO_CANVAS_ITEM(text), 0, 0, -1, -1);
        break;
    case PROP_SIZE_MODE:
        text->size_mode = g_value_get_enum(value);
        hippo_canvas_item_emit_request_changed(HIPPO_CANVAS_ITEM(text));
        hippo_canvas_item_emit_paint_needed(HIPPO_CANVAS_ITEM(text), 0, 0, -1, -1);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hippo_canvas_text_get_property(GObject         *object,
                               guint            prop_id,
                               GValue          *value,
                               GParamSpec      *pspec)
{
    HippoCanvasText *text;

    text = HIPPO_CANVAS_TEXT (object);

    switch (prop_id) {
    case PROP_TEXT:
        g_value_set_string(value, text->text);
        break;
    case PROP_ATTRIBUTES:
        g_value_set_boxed(value, text->attributes);
        break;
    case PROP_FONT_SCALE:
        g_value_set_double(value, text->font_scale);
        break;
    case PROP_SIZE_MODE:
        g_value_set_enum(value, text->size_mode);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hippo_canvas_text_set_context(HippoCanvasItem    *item,
                              HippoCanvasContext *context)
{
    HippoCanvasBox *box = HIPPO_CANVAS_BOX(item);
    gboolean changed;

    changed = context != box->context;

    item_parent_class->set_context(item, context);

    /* we can't create a layout until we have a context,
     * so we have to queue a size change when the context
     * is set.
     */
    if (changed) {
        hippo_canvas_item_emit_request_changed(HIPPO_CANVAS_ITEM(item));
        hippo_canvas_item_emit_paint_needed(HIPPO_CANVAS_ITEM(item), 0, 0, -1, -1);
    }
}


static char *
hippo_canvas_text_get_tooltip (HippoCanvasItem *item,
                               int              x,
                               int              y,
                               HippoRectangle  *for_area)
{
    HippoCanvasText *text = HIPPO_CANVAS_TEXT(item);
    HippoCanvasBox *box = HIPPO_CANVAS_BOX(item);


    if (text->is_ellipsized && text->text) {
        for_area->x = 0;
        for_area->y = 0;
        for_area->width = box->allocated_width;
        for_area->height = box->allocated_height;

        return g_strdup(text->text);
    } else
        return item_parent_class->get_tooltip(item, x, y, for_area);
}

static char*
remove_newlines(const char *text)
{
    char *s;
    char *p;

    s = g_strdup(text);

    for (p = s; *p != '\0'; ++p) {
        if (*p == '\n' || *p == '\r')
            *p = ' ';
    }

    return s;
}

static PangoLayout*
create_layout(HippoCanvasText *text,
              int              allocation_width)
{
    HippoCanvasBox *box = HIPPO_CANVAS_BOX(text);
    PangoLayout *layout;
    HippoCanvasStyle *style = hippo_canvas_context_get_style(HIPPO_CANVAS_CONTEXT(text));

    g_return_val_if_fail(box->context != NULL, NULL);

    layout = hippo_canvas_context_create_layout(box->context);

    if (box->font_desc) {
        PangoFontDescription *merged = pango_font_description_copy(hippo_canvas_style_get_font(style));
        pango_font_description_merge(merged, box->font_desc, TRUE);
        pango_layout_set_font_description(layout, merged);
        pango_font_description_free(merged);
    } else {
        pango_layout_set_font_description(layout, hippo_canvas_style_get_font(style));
    }

    {
        PangoAttrList *attrs;
        HippoTextDecoration decoration = hippo_canvas_style_get_text_decoration(style);

        if (text->attributes)
            attrs = pango_attr_list_copy(text->attributes);
        else
            attrs = pango_attr_list_new();


        if (ABS(1.0 - text->font_scale) > .000001) {
            PangoAttribute *attr = pango_attr_scale_new(text->font_scale);
            attr->start_index = 0;
            attr->end_index = G_MAXUINT;
            pango_attr_list_insert(attrs, attr);
        }

        if ((decoration & HIPPO_TEXT_DECORATION_UNDERLINE) != 0) {
            PangoAttribute *attr = pango_attr_underline_new(TRUE);
            attr->start_index = 0;
            attr->end_index = G_MAXUINT;
            pango_attr_list_insert(attrs, attr);
        }

        if ((decoration & HIPPO_TEXT_DECORATION_LINE_THROUGH) != 0) {
            PangoAttribute *attr = pango_attr_strikethrough_new(TRUE);
            attr->start_index = 0;
            attr->end_index = G_MAXUINT;
            pango_attr_list_insert(attrs, attr);
        }

        pango_layout_set_attributes(layout, attrs);
        pango_attr_list_unref(attrs);
    }

    if (text->text != NULL) {
        pango_layout_set_text(layout, text->text, -1);
    }

    if (allocation_width >= 0) {
        int layout_width, layout_height;
        pango_layout_get_size(layout, &layout_width, &layout_height);
        layout_width /= PANGO_SCALE;
        layout_height /= PANGO_SCALE;

        /* Force layout smaller if required, but we don't want to make
         * the layout _wider_ because it breaks alignment, so only do
         * this if required.
         */
        if (layout_width > allocation_width) {
            pango_layout_set_width(layout, allocation_width * PANGO_SCALE);

            /* If we set ellipsize, then it overrides wrapping. If we get
             * too-small allocation for HIPPO_CANVAS_SIZE_FULL_WIDTH, then
             * we want to ellipsize instead of wrapping.
             */
            if (text->size_mode == HIPPO_CANVAS_SIZE_WRAP_WORD) {
                pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_NONE);
            } else {
                pango_layout_set_ellipsize(layout, PANGO_ELLIPSIZE_END);
            }

            /* For now if we say ellipsize end, we always just want one line.
             * Maybe this should be an orthogonal property?
             */
            if (text->size_mode == HIPPO_CANVAS_SIZE_ELLIPSIZE_END) {
                pango_layout_set_single_paragraph_mode(layout, TRUE);

                /* Pango's line separator character in this case is ugly, so we
                 * fix it. Not a very efficient approach, but oh well.
                 */
                if (text->text != NULL) {
                    char *new_text = remove_newlines(text->text);
                    /* avoid making the layout recompute everything
                     * if we didn't have newlines anyhow
                     */
                    if (strcmp(text->text, new_text) != 0) {
                        pango_layout_set_text(layout, new_text, -1);
                    }
                    g_free(new_text);
                }
            }
        }
    }

    return layout;
}

static gboolean
layout_is_ellipsized(PangoLayout *layout)
{
    /* pango_layout_is_ellipsized() is a new function in Pango-1.16; we
     * emulate it here by trying to look for the ellipsis run
     */
    PangoLogAttr *log_attrs;
    int n_attrs;
    PangoLayoutIter *iter;
    gboolean result = FALSE;

    /* Short circuit when we aren't ellipsizing at all */
    if (pango_layout_get_ellipsize(layout) == PANGO_ELLIPSIZE_NONE)
        return FALSE;

    pango_layout_get_log_attrs(layout, &log_attrs, &n_attrs);

    iter = pango_layout_get_iter(layout);
    do {
        PangoGlyphItem *run;
        int n_glyphs;
        int start_index;
        int n_graphemes;
        int i;

        run = pango_layout_iter_get_run(iter);
        if (!run)
            continue;

        n_glyphs = run->glyphs->num_glyphs;
        start_index = pango_layout_iter_get_index(iter);

        /* Check the number of clusters in the run ... if it is greater
         * than 1, then it isn't an ellipsis
         */
        if (run->glyphs->log_clusters[0] != run->glyphs->log_clusters[n_glyphs - 1])
            continue;

        /* Now check the number of graphemes in the run ... if it is less
         * than 3, it's probably an isolated 'fi' ligature or something
         * like that rather than an ellipsis.
         */
        n_graphemes = 0;
        for (i = 0; i < run->item->num_chars && i + start_index < n_attrs; i++)
            if (log_attrs[i + start_index].is_cursor_position)
                n_graphemes++;

        if (n_graphemes < 3)
            continue;

        /* OK, at this point it is probably an ellipsis; it's possible that
         * the text consists of just the letters 'ffi' and the font has a ligature
         * for that or something, but it's not too likely.
         */
        result = TRUE;
        break;

    } while (pango_layout_iter_next_run(iter));

    pango_layout_iter_free(iter);

    g_free(log_attrs);

    return result;
}

static void
hippo_canvas_text_paint_below_children(HippoCanvasBox  *box,
                                       cairo_t         *cr,
                                       GdkRegion       *damaged_region)
{
    HippoCanvasText *text = HIPPO_CANVAS_TEXT(box);
    guint32 color_rgba;

    if (box->color_set) {
        color_rgba = box->color_rgba;
    } else {
        HippoCanvasStyle *style = hippo_canvas_context_get_style(HIPPO_CANVAS_CONTEXT(text));
        color_rgba = hippo_canvas_style_get_foreground_color(style);
    }

    /* It would seem more natural to compute whether we are ellipsized or
     * not when we are allocated to some width, but we don't have a layout
     * at that point. We could do it in get_content_height_request(), but
     * the parent could theoretically give us more width than it asked us
     * about (and there are also some quirks in HippoCanvasBox where it
     * will call get_content_height_request() with a width if 0 at odd times),
     * so doing it here is more reliable. We use is_ellipsized only for
     * computing whether to show a tooltip, and we make the assumption that
     * if the user hasn't seen the text item drawn, they won't need a
     * tooltip for it.
     */
    text->is_ellipsized = FALSE;

    if ((color_rgba & 0xff) != 0 && text->text != NULL) {
        PangoLayout *layout;
        int layout_width, layout_height;
        int x, y, w, h;
        int allocation_width, allocation_height;

        int space_left = box->border_left + box->padding_left;
        int space_right = box->border_right + box->padding_right;


        hippo_canvas_item_get_allocation(HIPPO_CANVAS_ITEM(box),
                                         &allocation_width, &allocation_height);

        layout = create_layout(text, allocation_width - space_left - space_right);
        pango_layout_get_size(layout, &layout_width, &layout_height);
        layout_width /= PANGO_SCALE;
        layout_height /= PANGO_SCALE;

        text->is_ellipsized = layout_is_ellipsized(layout);

        hippo_canvas_box_align(box,
                               layout_width, layout_height,
                               &x, &y, &w, &h);

        /* we can't really "fill" so we fall back to center if we seem to be
         * in fill mode
         */
        if (w > layout_width) {
            x += (w - layout_width) / 2;
        }
        if (h > layout_height) {
            y += (h - layout_height) / 2;
        }

        /* Clipping is needed since the layout size could exceed our
         * allocation if we got a too-small allocation.
         * FIXME It would be better to ellipsize or something instead, though.
         */
        cairo_save(cr);
        cairo_rectangle(cr, 0, 0, allocation_width, allocation_height);
        cairo_clip(cr);

        cairo_move_to (cr, x, y);
        hippo_cairo_set_source_rgba32(cr, color_rgba);
        pango_cairo_show_layout(cr, layout);
        cairo_restore(cr);

        g_object_unref(layout);
    }
}

static void
hippo_canvas_text_get_content_width_request(HippoCanvasBox *box,
                                            int            *min_width_p,
                                            int            *natural_width_p)
{
    HippoCanvasText *text = HIPPO_CANVAS_TEXT(box);
    int children_min_width, children_natural_width;
    int layout_width;

    HIPPO_CANVAS_BOX_CLASS(hippo_canvas_text_parent_class)->get_content_width_request(box,
                                                                                      &children_min_width,
                                                                                      &children_natural_width);
    if (box->context != NULL) {
        PangoLayout *layout = create_layout(text, -1);
        pango_layout_get_size(layout, &layout_width, NULL);
        layout_width /= PANGO_SCALE;
        g_object_unref(layout);
    } else {
        layout_width = 0;
    }

    if (min_width_p) {
        if (text->size_mode == HIPPO_CANVAS_SIZE_FULL_WIDTH)
            *min_width_p = MAX(children_min_width, layout_width);
        else
            *min_width_p = children_min_width;
    }

    if (natural_width_p) {
        *natural_width_p = MAX(children_natural_width, layout_width);
    }
}

static void
hippo_canvas_text_get_content_height_request(HippoCanvasBox  *box,
                                             int              for_width,
                                             int             *min_height_p,
                                             int             *natural_height_p)
{
    HippoCanvasText *text = HIPPO_CANVAS_TEXT(box);
    int children_min_height, children_natural_height;
    PangoLayout *layout;
    int layout_height;

    HIPPO_CANVAS_BOX_CLASS(hippo_canvas_text_parent_class)->get_content_height_request(box,
                                                                                       for_width,
                                                                                       &children_min_height,
                                                                                       &children_natural_height);

    if (for_width > 0) {
        if (box->context != NULL) {
            layout = create_layout(text, for_width);
            pango_layout_get_size(layout, NULL, &layout_height);

            layout_height /= PANGO_SCALE;
            g_object_unref(layout);
        } else {
            layout_height = 0;
        }
    } else {
        layout_height = 0;
    }

    if (min_height_p)
        *min_height_p = MAX(layout_height, children_min_height);
    if (natural_height_p)
        *natural_height_p = MAX(layout_height, children_natural_height);
}

static gboolean
hippo_canvas_text_button_press_event (HippoCanvasItem *item,
                                      HippoEvent      *event)
{
    /* HippoCanvasText *text = HIPPO_CANVAS_TEXT(item); */

    /* see if a child wants it */
    return item_parent_class->button_press_event(item, event);
}
