
/* Generated data (by glib-mkenums) */

#include "adg.h"


/* enumerations from "adgentity.h" */
GType
adg_entity_flags_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GFlagsValue values[] =
        {
          { ADG_UPTODATE, "ADG_UPTODATE", "uptodate" },
          { 0, NULL, NULL }
        };
      etype = g_flags_register_static ("AdgEntityFlags", values);
    }
  return etype;
}

/* enumerations from "adgstyle.h" */
GType
adg_line_style_id_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GEnumValue values[] =
        {
          { ADG_LINE_STYLE_DRAW, "ADG_LINE_STYLE_DRAW", "draw" },
          { ADG_LINE_STYLE_CENTER, "ADG_LINE_STYLE_CENTER", "center" },
          { ADG_LINE_STYLE_HIDDEN, "ADG_LINE_STYLE_HIDDEN", "hidden" },
          { ADG_LINE_STYLE_XATCH, "ADG_LINE_STYLE_XATCH", "xatch" },
          { ADG_LINE_STYLE_DIM, "ADG_LINE_STYLE_DIM", "dim" },
          { ADG_LINE_STYLE_LAST, "ADG_LINE_STYLE_LAST", "last" },
          { 0, NULL, NULL }
        };
      etype = g_enum_register_static ("AdgLineStyleId", values);
    }
  return etype;
}
GType
adg_font_style_id_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GEnumValue values[] =
        {
          { ADG_FONT_STYLE_TEXT, "ADG_FONT_STYLE_TEXT", "text" },
          { ADG_FONT_STYLE_DIMLABEL, "ADG_FONT_STYLE_DIMLABEL", "dimlabel" },
          { ADG_FONT_STYLE_DIMTOLERANCE, "ADG_FONT_STYLE_DIMTOLERANCE", "dimtolerance" },
          { ADG_FONT_STYLE_DIMNOTE, "ADG_FONT_STYLE_DIMNOTE", "dimnote" },
          { ADG_FONT_STYLE_LAST, "ADG_FONT_STYLE_LAST", "last" },
          { 0, NULL, NULL }
        };
      etype = g_enum_register_static ("AdgFontStyleId", values);
    }
  return etype;
}
GType
adg_arrow_style_id_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GEnumValue values[] =
        {
          { ADG_ARROW_STYLE_ARROW, "ADG_ARROW_STYLE_ARROW", "arrow" },
          { ADG_ARROW_STYLE_TRIANGLE, "ADG_ARROW_STYLE_TRIANGLE", "triangle" },
          { ADG_ARROW_STYLE_DOT, "ADG_ARROW_STYLE_DOT", "dot" },
          { ADG_ARROW_STYLE_CIRCLE, "ADG_ARROW_STYLE_CIRCLE", "circle" },
          { ADG_ARROW_STYLE_SQUARE, "ADG_ARROW_STYLE_SQUARE", "square" },
          { ADG_ARROW_STYLE_TICK, "ADG_ARROW_STYLE_TICK", "tick" },
          { ADG_ARROW_STYLE_LAST, "ADG_ARROW_STYLE_LAST", "last" },
          { 0, NULL, NULL }
        };
      etype = g_enum_register_static ("AdgArrowStyleId", values);
    }
  return etype;
}
GType
adg_path_point_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GEnumValue values[] =
        {
          { ADG_PATH_POINT_START, "ADG_PATH_POINT_START", "start" },
          { ADG_PATH_POINT_MIDDLE, "ADG_PATH_POINT_MIDDLE", "middle" },
          { ADG_PATH_POINT_END, "ADG_PATH_POINT_END", "end" },
          { 0, NULL, NULL }
        };
      etype = g_enum_register_static ("AdgPathPoint", values);
    }
  return etype;
}
GType
adg_dim_style_id_get_type (void)
{
  static GType etype = 0;
  if G_UNLIKELY (etype == 0)
    {
      static const GEnumValue values[] =
        {
          { ADG_DIM_STYLE_ISO, "ADG_DIM_STYLE_ISO", "iso" },
          { ADG_DIM_STYLE_LAST, "ADG_DIM_STYLE_LAST", "last" },
          { 0, NULL, NULL }
        };
      etype = g_enum_register_static ("AdgDimStyleId", values);
    }
  return etype;
}

/* Generated data ends here */

