/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2020  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


/**
 * SECTION:adg-dash
 * @Section_Id:AdgDash
 * @title: AdgDash
 * @short_description: Dash pattern for line stroking
 *
 * The #AdgDash boxed type wraps the values needed by cairo to
 * univoquely identify a dash pattern, an array of positive values. Each
 * value provides the length of alternate "on" and "off" portions of the
 * stroke. The offset specifies an offset into the pattern at which the
 * stroke begins.
 *
 * Each "on" segment will have caps applied as if the segment were a
 * separate sub-path. In particular, it is valid to use an "on" length of
 * 0 with %CAIRO_LINE_CAP_ROUND or %CAIRO_LINE_CAP_SQUARE in order to
 * distribute dots or squares along a path.
 *
 * Check the cairo_set_dash() documentation for further details:
 * http://www.cairographics.org/manual/cairo-cairo-t.html#cairo-set-dash
 *
 * Since: 1.0
 **/

/**
 * AdgDash:
 *
 * All fields are private and should not be used directly.
 * Use its public methods instead.
 *
 * Since: 1.0
 **/


#include "adg-internal.h"

#include "adg-dash.h"
#include "adg-dash-private.h"

#include <string.h>


GType
adg_dash_get_type(void)
{
    static GType dash_type = 0;

    if (G_UNLIKELY(dash_type == 0))
        dash_type = g_boxed_type_register_static("AdgDash",
                                                 (GBoxedCopyFunc) adg_dash_dup,
                                                 (GBoxedFreeFunc) adg_dash_destroy);

    return dash_type;
}

/**
 * adg_dash_dup:
 * @src: an #AdgDash instance
 *
 * Duplicates @src. The returned value must be freed with adg_dash_destroy()
 * when no longer needed.
 *
 * Returns: (transfer full): the duplicate of @src.
 *
 * Since: 1.0
 **/
AdgDash *
adg_dash_dup(const AdgDash *src)
{
    AdgDash *dash;

    g_return_val_if_fail(src != NULL, NULL);

    dash = g_memdup(src, sizeof(AdgDash));
    dash->dashes = g_memdup(src->dashes, sizeof(gdouble) * src->num_dashes);

    return dash;
}

/**
 * adg_dash_new:
 *
 * Creates a new empty dash pattern.
 *
 * Returns: (transfer full): the newly created dash pattern.
 *
 * Since: 1.0
 **/
AdgDash *
adg_dash_new(void)
{
    AdgDash *dash = g_new(AdgDash, 1);

    dash->dashes = NULL;
    dash->num_dashes = 0;
    dash->offset = 0;

    return dash;
}

/**
 * adg_dash_new_with_dashes:
 * @num_dashes: the number of dashes to set
 * @...: lengths (a list of double values) of each dash
 *
 * Creates a new dash pattern, presetting some dashes on it.
 *
 * Returns: (transfer full): the newly created dash pattern.
 *
 * Since: 1.0
 **/
AdgDash *
adg_dash_new_with_dashes(gint num_dashes, ...)
{
    AdgDash *dash;
    va_list var_args;

    va_start(var_args, num_dashes);
    dash = adg_dash_new_with_dashes_valist(num_dashes, var_args);
    va_end(var_args);

    return dash;
}

/**
 * adg_dash_new_with_dashes_valist:
 * @num_dashes: number of dashes to append
 * @var_args: a va_list containing @num_dashes list of #gdouble
 *
 * Variadic version of adg_dash_new_with_dashes().
 *
 * Returns: (transfer full): the newly created dash pattern.
 *
 * Since: 1.0
 **/
AdgDash *
adg_dash_new_with_dashes_valist(gint num_dashes, va_list var_args)
{
    AdgDash *dash = adg_dash_new();
    adg_dash_append_dashes_valist(dash, num_dashes, var_args);
    return dash;
}

/**
 * adg_dash_new_with_dashes_array: (rename-to adg_dash_new_with_dashes)
 * @num_dashes: number of dashes to append
 * @dashes: (array length=num_dashes): array of @num_dashes gdoubles
 *
 * Array version of adg_dash_new_with_dashes().
 *
 * Returns: (transfer full): the newly created dash pattern.
 *
 * Since: 1.0
 **/
AdgDash *
adg_dash_new_with_dashes_array(gint num_dashes, const gdouble *dashes)
{
    AdgDash *dash = adg_dash_new();
    adg_dash_append_dashes_array(dash, num_dashes, dashes);
    return dash;
}

/**
 * adg_dash_append_dash:
 * @dash: an #AdgDash instance
 * @length: the length value
 *
 * Appends to the @dash pattern a new dash of the specified @length value.
 *
 * Since: 1.0
 **/
void
adg_dash_append_dash(AdgDash *dash, gdouble length)
{
    g_return_if_fail(dash != NULL);

    ++ dash->num_dashes;
    dash->dashes = g_realloc(dash->dashes, sizeof(gdouble) * dash->num_dashes);
    dash->dashes[dash->num_dashes - 1] = length;
}

/**
 * adg_dash_append_dashes:
 * @dash: an #AdgDash instance
 * @num_dashes: number of dashes to append
 * @...: a @num_dashes list of #gdouble
 *
 * Appends to the current @dash pattern @num_dashes number of dashes.
 * The length of each dash must be specified as gdouble in the arguments.
 *
 * Since: 1.0
 **/
void
adg_dash_append_dashes(AdgDash *dash, gint num_dashes, ...)
{
    va_list var_args;

    va_start(var_args, num_dashes);
    adg_dash_append_dashes_valist(dash, num_dashes,  var_args);
    va_end(var_args);
}

/**
 * adg_dash_append_dashes_valist:
 * @dash: an #AdgDash instance
 * @num_dashes: number of dashes to append
 * @var_args: a va_list containing @num_dashes list of #gdouble
 *
 * Variadic version of adg_dash_append_dashes().
 *
 * Since: 1.0
 **/
void
adg_dash_append_dashes_valist(AdgDash *dash, gint num_dashes, va_list var_args)
{
    gdouble length;

    g_return_if_fail(dash != NULL);
    g_return_if_fail(num_dashes > 0);

    while (num_dashes --) {
        length = va_arg(var_args, gdouble);
        adg_dash_append_dash(dash, length);
    }
}

/**
 * adg_dash_append_dashes_array: (rename-to adg_dash_append_dashes)
 * @dash: an #AdgDash instance
 * @num_dashes: number of dashes to append
 * @dashes: (array length=num_dashes): array of @num_dashes gdoubles
 *
 * Array version of adg_dash_append_dashes().
 *
 * Since: 1.0
 **/
void
adg_dash_append_dashes_array(AdgDash *dash,
                             gint num_dashes, const gdouble *dashes)
{
    g_return_if_fail(dash != NULL);

    if (num_dashes > 0) {
        gint old_dashes = dash->num_dashes;
        dash->num_dashes += num_dashes;
        dash->dashes = g_realloc(dash->dashes, sizeof(gdouble) * dash->num_dashes);
        memcpy(dash->dashes + old_dashes, dashes, sizeof(gdouble) * num_dashes);
    }
}

/**
 * adg_dash_get_num_dashes:
 * @dash: an #AdgDash instance
 *
 * Gets the number of dashes stored inside this dash pattern.
 *
 * Returns: the number of dashes or -1 if @dash is invalid.
 *
 * Since: 1.0
 **/
gint
adg_dash_get_num_dashes(const AdgDash *dash)
{
    g_return_val_if_fail(dash != NULL, -1);
    return dash->num_dashes;
}

/**
 * adg_dash_get_dashes:
 * @dash: an #AdgDash instance
 *
 * Gets the array of gdoubles containing the length of each dash of the
 * pattern of @dash.
 *
 * Returns: the array of lengths or <constant>NULL</constant> on invalid @dash. The array is owned by @dash and must not be modified or freed.
 *
 * Since: 1.0
 **/
const gdouble *
adg_dash_get_dashes(const AdgDash *dash)
{
    g_return_val_if_fail(dash != NULL, NULL);
    return dash->dashes;
}

/**
 * adg_dash_clear_dashes:
 * @dash: an #AdgDash instance
 *
 * Resets the dashes of @dash, effectively clearing the pattern.
 *
 * Since: 1.0
 **/
void
adg_dash_clear_dashes(AdgDash *dash)
{
    g_return_if_fail(dash != NULL);

    g_free(dash->dashes);
    dash->dashes = NULL;
    dash->num_dashes = 0;
}

/**
 * adg_dash_set_offset:
 * @dash: an #AdgDash instance
 * @offset: the new offset value
 *
 * Sets the pattern offset of @dash to @offset.
 *
 * Since: 1.0
 **/
void
adg_dash_set_offset(AdgDash *dash, gdouble offset)
{
    g_return_if_fail(dash != NULL);

    dash->offset = offset;
}

/**
 * adg_dash_get_offset:
 * @dash: an #AdgDash instance
 *
 * Gets the offset of the pattern in @dash.
 *
 * Returns: the offset of @dash or 0 on invalid @dash.
 *
 * Since: 1.0
 **/
gdouble
adg_dash_get_offset(const AdgDash *dash)
{
    g_return_val_if_fail(dash != NULL, 0);
    return dash->offset;
}

/**
 * adg_dash_destroy:
 * @dash: an #AdgDash instance
 *
 * Destroys @dash, freeing every resource owned by it. After the destruction
 * @dash cannot be used anymore.
 *
 * Since: 1.0
 **/
void
adg_dash_destroy(AdgDash *dash)
{
    g_return_if_fail(dash != NULL);

    g_free(dash->dashes);
    g_free(dash);
}
