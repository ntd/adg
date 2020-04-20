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

#include "demo.h"
#include <adg.h>
#include <string.h>
#include <math.h>

#define SQRT3   1.732050808
#define CHAMFER 0.3


/* Whether render the boxes to highlight the extents of every entity */
static gboolean show_extents = FALSE;


typedef struct _DemoPart DemoPart;

struct _DemoPart {
    /* Dimensions */
    gdouble              A;
    gdouble              B;
    gdouble              C;
    gdouble              DHOLE;
    gdouble              LHOLE;
    gdouble              D1;
    gdouble              D2;
    gdouble              D3;
    gdouble              D4;
    gdouble              D5;
    gdouble              D6;
    gdouble              D7;
    gdouble              RD34;
    gdouble              RD56;
    gdouble              LD2;
    gdouble              LD3;
    gdouble              LD5;
    gdouble              LD6;
    gdouble              LD7;
    gboolean             GROOVE;
    gdouble              ZGROOVE;
    gdouble              DGROOVE;
    gdouble              LGROOVE;

    /* Metadata */
    gchar               *TITLE;
    gchar               *DRAWING;
    gchar               *AUTHOR;
    gchar               *DATE;

    /* User interface widgets */
    AdgGtkArea          *area;
    GHashTable          *widgets;
    GtkButton           *apply;
    GtkButton           *reset;

    /* Data models */
    AdgPath             *body;
    AdgPath             *hole;
    AdgPath             *axis;

    /* Special entities */
    AdgTitleBlock       *title_block;
    AdgEdges            *edges;
};


static void
_adg_version(void)
{
    g_print("adg-demo " PACKAGE_VERSION "\n");
    exit(0);
}

static void
parse_args(gint *p_argc, gchar **p_argv[])
{
    GOptionEntry entries[] = {
        {"version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
         (gpointer) _adg_version, _("Display version information"), NULL},
        {"show-extents", 'E', 0, G_OPTION_ARG_NONE,
         &show_extents, _("Show the boundary boxes of every entity"), NULL},
        {NULL}
    };
    GError *error;

    error = NULL;
    gtk_init_with_args(p_argc, p_argv, _("- ADG demonstration program"),
                       entries, GETTEXT_PACKAGE, &error);
    if (error != NULL) {
        g_error("%s", error->message);
        g_assert_not_reached();
    }
}

/**
 * _adg_error:
 * @message: a custom error message
 * @parent_window: the parent window or %NULL
 *
 * Convenient function that presents an error dialog and waits the user
 * to close this modal dialog.
 **/
void
_adg_error(const gchar *message, GtkWindow *parent_window)
{
    GtkWidget *dialog = gtk_message_dialog_new(parent_window,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), _("Error from adg-demo"));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void
_adg_path_add_groove(AdgPath *path, const DemoPart *part)
{
    AdgModel *model;
    CpmlPair pair;

    model = ADG_MODEL(path);

    pair.x = part->ZGROOVE;
    pair.y = part->D1 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "DGROOVEI_X", &pair);

    pair.y = part->D3 / 2;
    adg_model_set_named_pair(model, "DGROOVEY_POS", &pair);

    pair.y = part->DGROOVE / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "DGROOVEI_Y", &pair);

    pair.x += part->LGROOVE;
    adg_path_line_to(path, &pair);

    pair.y = part->D3 / 2;
    adg_model_set_named_pair(model, "DGROOVEX_POS", &pair);

    pair.y = part->D1 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "DGROOVEF_X", &pair);
}


static void
_adg_part_define_title_block(DemoPart *part)
{
    g_object_set(part->title_block,
                 "title", part->TITLE,
                 "author", part->AUTHOR,
                 "date", part->DATE,
                 "drawing", part->DRAWING,
                 "logo", adg_logo_new(),
                 "projection", adg_projection_new(ADG_PROJECTION_SCHEME_FIRST_ANGLE),
                 "size", "A4",
                 NULL);
}

static void
_adg_part_define_hole(DemoPart *part)
{
    AdgPath *path;
    AdgModel *model;
    CpmlPair pair, edge;

    path = part->hole;
    model = ADG_MODEL(path);

    pair.x = part->LHOLE;
    pair.y = 0;

    adg_path_move_to(path, &pair);
    adg_model_set_named_pair(model, "LHOLE", &pair);

    pair.y = part->DHOLE / 2;
    pair.x -= pair.y / SQRT3;
    adg_path_line_to(path, &pair);
    cpml_pair_copy(&edge, &pair);

    pair.x = 0;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "DHOLE", &pair);

    pair.y = (part->D1 + part->DHOLE) / 4;
    adg_path_line_to(path, &pair);

    adg_path_curve_to_explicit(path,
                               part->LHOLE / 2, part->DHOLE / 2,
                               part->LHOLE + 2, part->D1 / 2,
                               part->LHOLE + 2, 0);
    adg_path_reflect(path, NULL);
    adg_path_join(path);
    adg_path_close(path);

    /* No need to incomodate an AdgEdge model for two reasons:
     * it is only a single line and it is always needed */
    adg_path_move_to(path, &edge);
    edge.y = -edge.y;
    adg_path_line_to(path, &edge);
}

static void
_adg_part_define_body(DemoPart *part)
{
    AdgModel *model;
    AdgPath *path;
    CpmlPair pair, tmp;
    const CpmlPrimitive *primitive;

    path = part->body;
    model = ADG_MODEL(path);

    pair.x = 0;
    pair.y = part->D1 / 2;
    adg_path_move_to(path, &pair);
    adg_model_set_named_pair(model, "D1I", &pair);

    if (part->GROOVE) {
        _adg_path_add_groove(path, part);
    }

    pair.x = part->A - part->B - part->LD2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D1F", &pair);

    pair.y = part->D3 / 2;
    adg_model_set_named_pair(model, "D2_POS", &pair);

    pair.x += (part->D1 - part->D2) / 2;
    pair.y = part->D2 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D2I", &pair);

    pair.x = part->A - part->B;
    adg_path_line_to(path, &pair);
    adg_path_fillet(path, 0.4);

    pair.x = part->A - part->B;
    pair.y = part->D3 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D3I", &pair);

    pair.x = part->A;
    adg_model_set_named_pair(model, "East", &pair);

    pair.x = 0;
    adg_model_set_named_pair(model, "West", &pair);

    adg_path_chamfer(path, CHAMFER, CHAMFER);

    pair.x = part->A - part->B + part->LD3;
    pair.y = part->D3 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, 0, &tmp);
    adg_model_set_named_pair(model, "D3I_X", &tmp);

    cpml_primitive_put_point(primitive, -1, &tmp);
    adg_model_set_named_pair(model, "D3I_Y", &tmp);

    adg_path_chamfer(path, CHAMFER, CHAMFER);

    pair.y = part->D4 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, 0, &tmp);
    adg_model_set_named_pair(model, "D3F_Y", &tmp);
    cpml_primitive_put_point(primitive, -1, &tmp);
    adg_model_set_named_pair(model, "D3F_X", &tmp);

    adg_path_fillet(path, part->RD34);

    pair.x += part->RD34;
    adg_model_set_named_pair(model, "D4I", &pair);

    pair.x = part->A - part->C - part->LD5;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D4F", &pair);

    pair.y = part->D3 / 2;
    adg_model_set_named_pair(model, "D4_POS", &pair);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, 0, &tmp);
    tmp.x += part->RD34;
    adg_model_set_named_pair(model, "RD34", &tmp);

    tmp.x -= cos(G_PI_4) * part->RD34,
    tmp.y -= sin(G_PI_4) * part->RD34,
    adg_model_set_named_pair(model, "RD34_R", &tmp);

    tmp.x += part->RD34,
    tmp.y += part->RD34,
    adg_model_set_named_pair(model, "RD34_XY", &tmp);

    pair.x += (part->D4 - part->D5) / 2;
    pair.y = part->D5 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D5I", &pair);

    pair.x = part->A - part->C;
    adg_path_line_to(path, &pair);

    adg_path_fillet(path, 0.2);

    pair.y = part->D6 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, 0, &tmp);
    adg_model_set_named_pair(model, "D5F", &tmp);

    adg_path_fillet(path, 0.1);

    pair.x += part->LD6;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D6F", &pair);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, 0, &tmp);
    adg_model_set_named_pair(model, "D6I_X", &tmp);

    primitive = adg_path_over_primitive(path);
    cpml_primitive_put_point(primitive, -1, &tmp);
    adg_model_set_named_pair(model, "D6I_Y", &tmp);

    pair.x = part->A - part->LD7;
    pair.y -= (part->C - part->LD7 - part->LD6) / SQRT3;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D67", &pair);

    pair.y = part->D7 / 2;
    adg_path_line_to(path, &pair);

    pair.x = part->A;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D7F", &pair);

    adg_path_reflect(path, NULL);
    adg_path_join(path);
    adg_path_close(path);
}

static void
_adg_part_define_axis(DemoPart *part)
{
    AdgPath *path;

    path = part->axis;

    /* XXX: actually the end points can extend outside the body
     * only in local space. The proper extension values should be
     * expressed in global space but actually is impossible to
     * combine local and global space in the AdgPath API.
     */
    adg_path_move_to_explicit(path, -1, 0);
    adg_path_line_to_explicit(path, part->A + 1, 0);
}

static void
_adg_part_lock(DemoPart *part)
{
    gtk_widget_set_sensitive(GTK_WIDGET(part->apply), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(part->reset), FALSE);
}

static void
_adg_part_unlock(DemoPart *part)
{
    gtk_widget_set_sensitive(GTK_WIDGET(part->apply), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(part->reset), TRUE);
}

static void
_adg_part_link(DemoPart *part, gpointer data, GObject *widget)
{
    const gchar *edit_signal;

    g_assert(GTK_IS_WIDGET(widget));
    g_object_ref(widget);
    g_hash_table_insert(part->widgets, data, widget);

    edit_signal = GTK_IS_TOGGLE_BUTTON(widget) ? "toggled" : "changed";
    g_signal_connect_swapped(widget, edit_signal,
                             G_CALLBACK(_adg_part_unlock), part);
}

static void
_adg_part_ui_to_boolean(DemoPart *part, gboolean *data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_TOGGLE_BUTTON(widget));

    *data = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
}

static void
_adg_part_ui_to_double(DemoPart *part, gdouble *data)
{
    GtkWidget *widget;
    GtkSpinButton *spin_button;

    widget = g_hash_table_lookup(part->widgets, data);
    g_assert(GTK_IS_SPIN_BUTTON(widget));

    spin_button = GTK_SPIN_BUTTON(widget);
    gtk_spin_button_update(spin_button);
    *data = gtk_spin_button_get_value(spin_button);
}

static void
_adg_part_ui_to_string(DemoPart *part, gchar **data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_ENTRY(widget));

    g_free(*data);
    *data = g_strdup(gtk_entry_get_text(GTK_ENTRY(widget)));
}

static void
_adg_part_boolean_to_ui(DemoPart *part, gboolean *data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_TOGGLE_BUTTON(widget));

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), *data);
}

static void
_adg_part_double_to_ui(DemoPart *part, gdouble *data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_SPIN_BUTTON(widget));

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), *data);
}

static void
_adg_part_string_to_ui(DemoPart *part, gchar **data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_ENTRY(widget));

    gtk_entry_set_text(GTK_ENTRY(widget), *data);
}

static void
_adg_demo_canvas_add_dimensions(AdgCanvas *canvas, AdgModel *model)
{
    AdgLDim *ldim;
    AdgADim *adim;
    AdgRDim *rdim;
    AdgStyle *style, *diameter;

    style = adg_dress_get_fallback(ADG_DRESS_DIMENSION);
    diameter = adg_style_clone(style);
    adg_dim_style_set_number_format(ADG_DIM_STYLE(diameter), ADG_UTF8_DIAMETER "%g");

    /* NORTH */
    ldim = adg_ldim_new_full_from_model(model, "-D3I_X", "-D3F_X", "-D3F_Y",
                                        ADG_DIR_UP);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "-D6I_X", "-D67", "-East",
                                        ADG_DIR_UP);
    adg_dim_set_level(ADG_DIM(ldim), 0);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "-D6I_X", "-D7F", "-East",
                                        ADG_DIR_UP);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.06", NULL);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    adim = adg_adim_new_full_from_model(model, "-D6I_Y", "-D6F",
                                        "-D6F", "-D67", "-D6F");
    adg_dim_set_level(ADG_DIM(adim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    rdim = adg_rdim_new_full_from_model(model, "-RD34", "-RD34_R", "-RD34_XY");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(rdim));

    ldim = adg_ldim_new_full_from_model(model, "-DGROOVEI_X", "-DGROOVEF_X",
                                        "-DGROOVEX_POS", ADG_DIR_UP);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D2I", "-D2I", "-D2_POS",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "DGROOVEI_Y", "-DGROOVEI_Y",
                                        "-DGROOVEY_POS", ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    /* SOUTH */
    adim = adg_adim_new_full_from_model(model, "D1F", "D1I", "D2I", "D1F", "D1F");
    adg_dim_set_level(ADG_DIM(adim), 2);
    adg_adim_switch_extension2(adim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "LHOLE", "West",
                                        ADG_DIR_DOWN);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "DGROOVEI_X", "West",
                                        ADG_DIR_DOWN);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D4F", "D6I_X", "D4_POS",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), NULL, "+0.2");
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1F", "D3I_X", "D2_POS",
                                        ADG_DIR_DOWN);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_ldim_switch_extension2(ldim, FALSE);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D3I_X", "D7F", "East",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), NULL, "+0.1");
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_ldim_switch_extension2(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "D7F", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.05", "+0.05");
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    adim = adg_adim_new_full_from_model(model, "D4F", "D4I",
                                        "D5I", "D4F", "D4F");
    adg_dim_set_level(ADG_DIM(adim), 1.5);
    adg_adim_switch_extension2(adim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    /* EAST */
    ldim = adg_ldim_new_full_from_model(model, "D6F", "-D6F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 4);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D4F", "-D4F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D5F", "-D5F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D7F", "-D7F", "East",
                                        ADG_DIR_RIGHT);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    /* WEST */
    ldim = adg_ldim_new_full_from_model(model, "DHOLE", "-DHOLE", "-West",
                                        ADG_DIR_LEFT);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "-D1I", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.05", "+0.05");
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D3I_Y", "-D3I_Y", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_entity_set_style(ADG_ENTITY(ldim), ADG_DRESS_DIMENSION, diameter);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));
}

static void
_adg_demo_canvas_add_axis(AdgCanvas *canvas, AdgTrail *trail)
{
    AdgStroke *stroke = adg_stroke_new(trail);
    adg_stroke_set_line_dress(stroke, ADG_DRESS_LINE_AXIS);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(stroke));
}

static AdgCanvas *
_adg_canvas_init(AdgCanvas *canvas, DemoPart *part)
{
    AdgContainer *container;
    AdgEntity *entity;

    container = (AdgContainer *) canvas;

    adg_canvas_set_paper(canvas, GTK_PAPER_NAME_A4,
                         GTK_PAGE_ORIENTATION_LANDSCAPE);
    adg_canvas_set_title_block(canvas, part->title_block);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(part->body)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_hatch_new(ADG_TRAIL(part->hole)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(part->hole)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(part->edges)));
    adg_container_add(container, entity);

    _adg_demo_canvas_add_dimensions(canvas, ADG_MODEL(part->body));

    _adg_demo_canvas_add_axis(canvas, ADG_TRAIL(part->axis));

    return canvas;
}

static GtkRadioButton *
_adg_group_get_active(GtkRadioButton *radio_group)
{
    GSList *list = gtk_radio_button_get_group(radio_group);

    while (list) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(list->data)))
            return list->data;

        list = list->next;
    }

    return NULL;
}

static void
_adg_do_edit(DemoPart *part)
{
    _adg_part_ui_to_double(part, &part->A);
    _adg_part_ui_to_double(part, &part->B);
    _adg_part_ui_to_double(part, &part->C);
    _adg_part_ui_to_double(part, &part->DHOLE);
    _adg_part_ui_to_double(part, &part->LHOLE);
    _adg_part_ui_to_double(part, &part->D1);
    _adg_part_ui_to_double(part, &part->D2);
    _adg_part_ui_to_double(part, &part->LD2);
    _adg_part_ui_to_double(part, &part->D3);
    _adg_part_ui_to_double(part, &part->LD3);
    _adg_part_ui_to_double(part, &part->D4);
    //_adg_part_ui_to_double(part, &part->D5);
    _adg_part_ui_to_double(part, &part->D6);
    //_adg_part_ui_to_double(part, &part->LD6);
    _adg_part_ui_to_double(part, &part->D7);
    //_adg_part_ui_to_double(part, &part->LD7);

    _adg_part_ui_to_boolean(part, &part->GROOVE);
    _adg_part_ui_to_double(part, &part->ZGROOVE);
    _adg_part_ui_to_double(part, &part->DGROOVE);
    _adg_part_ui_to_double(part, &part->LGROOVE);

    _adg_part_ui_to_string(part, &part->TITLE);
    _adg_part_ui_to_string(part, &part->DRAWING);
    _adg_part_ui_to_string(part, &part->AUTHOR);
    _adg_part_ui_to_string(part, &part->DATE);

    _adg_part_lock(part);

    adg_model_reset(ADG_MODEL(part->body));
    adg_model_reset(ADG_MODEL(part->hole));
    adg_model_reset(ADG_MODEL(part->axis));
    adg_model_reset(ADG_MODEL(part->edges));

    _adg_part_define_title_block(part);
    _adg_part_define_body(part);
    _adg_part_define_hole(part);
    _adg_part_define_axis(part);

    adg_model_changed(ADG_MODEL(part->body));
    adg_model_changed(ADG_MODEL(part->hole));
    adg_model_changed(ADG_MODEL(part->axis));
    adg_model_changed(ADG_MODEL(part->edges));

    gtk_widget_queue_draw(GTK_WIDGET(part->area));
}

static void
_adg_do_reset(DemoPart *part)
{
    _adg_part_double_to_ui(part, &part->A);
    _adg_part_double_to_ui(part, &part->B);
    _adg_part_double_to_ui(part, &part->C);
    _adg_part_double_to_ui(part, &part->DHOLE);
    _adg_part_double_to_ui(part, &part->LHOLE);
    _adg_part_double_to_ui(part, &part->D1);
    _adg_part_double_to_ui(part, &part->D2);
    _adg_part_double_to_ui(part, &part->LD2);
    _adg_part_double_to_ui(part, &part->D3);
    _adg_part_double_to_ui(part, &part->LD3);
    _adg_part_double_to_ui(part, &part->D4);
    //_adg_part_double_to_ui(part, &part->D5);
    _adg_part_double_to_ui(part, &part->D6);
    //_adg_part_double_to_ui(part, &part->LD6);
    _adg_part_double_to_ui(part, &part->D7);
    //_adg_part_double_to_ui(part, &part->LD7);

    _adg_part_boolean_to_ui(part, &part->GROOVE);
    _adg_part_double_to_ui(part, &part->ZGROOVE);
    _adg_part_double_to_ui(part, &part->DGROOVE);
    _adg_part_double_to_ui(part, &part->LGROOVE);

    _adg_part_string_to_ui(part, &part->TITLE);
    _adg_part_string_to_ui(part, &part->DRAWING);
    _adg_part_string_to_ui(part, &part->AUTHOR);
    _adg_part_string_to_ui(part, &part->DATE);

    _adg_part_lock(part);
}

static void
_adg_do_save_as(GtkWindow *window, GtkResponseType response, AdgCanvas *canvas)
{
    GtkRadioButton *type_radio;
    gchar *file, *suffix;
    cairo_surface_type_t type;
    GError *error;

    if (response != GTK_RESPONSE_OK) {
        gtk_widget_hide(GTK_WIDGET(window));
        return;
    }

    file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(window));
    if (file == NULL)
        return;

    type_radio = _adg_group_get_active(g_object_get_data(G_OBJECT(window),
                                                         "type-group"));
    g_assert(GTK_IS_RADIO_BUTTON(type_radio));

    suffix = gtk_widget_get_tooltip_markup(GTK_WIDGET(type_radio));
    if (suffix != NULL) {
        /* Avoid double extension on the filename */
        if (! g_str_has_suffix(file, suffix)) {
            gchar *tmp = file;
            file = g_strconcat(file, suffix, NULL);
            g_free(tmp);
        }
        g_free(suffix);
    }

    error = NULL;
    type = adg_type_from_filename(file);

    if (type == CAIRO_SURFACE_TYPE_XLIB) {
        g_set_error(&error, ADG_CANVAS_ERROR, ADG_CANVAS_ERROR_SURFACE, "%s",
                    _("Requested format not supported"));
    } else {
        adg_canvas_export(canvas, type, file, &error);
    }

    g_free(file);
    if (error != NULL) {
        _adg_error(error->message, window);
        g_error_free(error);
    }

    gtk_widget_hide(GTK_WIDGET(window));
}

static void
_adg_print_prepare(GtkPrintOperation *operation, GtkPrintContext *context)
{
    gtk_print_operation_set_n_pages(operation, 1);
}

static void
_adg_print_page(GtkPrintOperation *operation, GtkPrintContext *context,
                gint page_nr, AdgEntity *canvas)
{
    cairo_t *cr = gtk_print_context_get_cairo_context(context);
    cairo_matrix_t old_map;

    adg_matrix_copy(&old_map, adg_entity_get_global_map(canvas));;

    adg_entity_set_global_map(canvas, adg_matrix_identity());
    adg_entity_render(canvas, cr);

    adg_entity_set_global_map(canvas, &old_map);
}

static void
_adg_do_print(GtkWidget *button, AdgCanvas *canvas)
{
    static GtkPrintSettings *settings = NULL;
    GtkWindow *window;
    GtkPrintOperation *operation;
    GtkPageSetup *page_setup;
    GError *error;

    window = (GtkWindow *) gtk_widget_get_toplevel(button);
    operation = gtk_print_operation_new();
    page_setup = adg_canvas_get_page_setup(canvas);
    error = NULL;

    if (settings)
        gtk_print_operation_set_print_settings(operation, settings);
    if (page_setup)
        gtk_print_operation_set_default_page_setup(operation, page_setup);

    g_signal_connect(operation, "begin-print",
                     G_CALLBACK(_adg_print_prepare), NULL);
    g_signal_connect(operation, "draw-page",
                     G_CALLBACK(_adg_print_page), canvas);

    gtk_print_operation_set_use_full_page(operation, FALSE);
    gtk_print_operation_set_unit(operation, GTK_UNIT_POINTS);
#if GTK_CHECK_VERSION(2, 18, 0)
    gtk_print_operation_set_embed_page_setup(operation, TRUE);
#endif

    switch (gtk_print_operation_run(operation,
                                    GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
                                    window, &error)) {

    case GTK_PRINT_OPERATION_RESULT_APPLY:
        if (settings)
            g_object_unref(settings);
        settings = gtk_print_operation_get_print_settings(operation);
        if (settings)
            g_object_ref(settings);
        break;

    default:
        break;
    }

    g_object_unref(operation);

    if (error)
        _adg_error(error->message, window);
}

static gboolean
_adg_button_press(AdgGtkArea *area, GdkEventButton *event)
{
    AdgCanvas *canvas;

    if (event->button != 1 && event->button != 3)
        return FALSE;

    canvas = adg_gtk_area_get_canvas(area);
    g_return_val_if_fail(ADG_IS_CANVAS(canvas), FALSE);

    if (event->button == 1) {
        /* Restore the original zoom */
        adg_gtk_area_reset(area);
    } else if (event->button == 3) {
        adg_canvas_autoscale(canvas);
        gtk_widget_queue_draw((GtkWidget *) area);
    }

    return FALSE;
}

static DemoPart *
_adg_part_new(GtkBuilder *builder)
{
    DemoPart *part;
    GObject *object, *toggle_object;

    part = g_new0(DemoPart, 1);
    part->widgets = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                          NULL, g_object_unref);
    part->area = (AdgGtkArea *) gtk_builder_get_object(builder, "mainCanvas");
    part->apply = (GtkButton *) gtk_builder_get_object(builder, "btnApply");
    part->reset = (GtkButton *) gtk_builder_get_object(builder, "btnReset");
    part->body = adg_path_new();
    part->hole = adg_path_new();
    part->axis = adg_path_new();
    part->title_block = adg_title_block_new();
    part->edges = adg_edges_new_with_source(ADG_TRAIL(part->body));

    g_assert(ADG_GTK_IS_AREA(part->area));
    g_assert(GTK_IS_BUTTON(part->apply));
    g_assert(GTK_IS_BUTTON(part->reset));

    _adg_part_link(part, &part->A, gtk_builder_get_object(builder, "editA"));
    _adg_part_link(part, &part->B, gtk_builder_get_object(builder, "editB"));
    _adg_part_link(part, &part->C, gtk_builder_get_object(builder, "editC"));
    _adg_part_link(part, &part->D1, gtk_builder_get_object(builder, "editD1"));
    _adg_part_link(part, &part->D2, gtk_builder_get_object(builder, "editD2"));
    _adg_part_link(part, &part->LD2, gtk_builder_get_object(builder, "editLD2"));
    _adg_part_link(part, &part->D3, gtk_builder_get_object(builder, "editD3"));
    _adg_part_link(part, &part->LD3, gtk_builder_get_object(builder, "editLD3"));
    _adg_part_link(part, &part->D4, gtk_builder_get_object(builder, "editD4"));
    //_adg_part_link(part, &part->D5, gtk_builder_get_object(builder, "editD5"));
    _adg_part_link(part, &part->D6, gtk_builder_get_object(builder, "editD6"));
    //_adg_part_link(part, &part->LD6, gtk_builder_get_object(builder, "editLD6"));
    _adg_part_link(part, &part->D7, gtk_builder_get_object(builder, "editD7"));
    //_adg_part_link(part, &part->LD7, gtk_builder_get_object(builder, "editLD7"));
    _adg_part_link(part, &part->DHOLE, gtk_builder_get_object(builder, "editDHOLE"));
    _adg_part_link(part, &part->LHOLE, gtk_builder_get_object(builder, "editLHOLE"));

    g_signal_connect(part->area, "button-press-event",
                     G_CALLBACK(_adg_button_press), NULL);

    toggle_object = gtk_builder_get_object(builder, "editGROOVE");
    _adg_part_link(part, &part->GROOVE, toggle_object);

    object = gtk_builder_get_object(builder, "editZGROOVE");
    _adg_part_link(part, &part->ZGROOVE, object);
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);
    object = gtk_builder_get_object(builder, "editZGROOVELabel");
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);

    object = gtk_builder_get_object(builder, "editDGROOVE");
    _adg_part_link(part, &part->DGROOVE, object);
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);
    object = gtk_builder_get_object(builder, "editDGROOVELabel");
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);

    object = gtk_builder_get_object(builder, "editLGROOVE");
    _adg_part_link(part, &part->LGROOVE, object);
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);
    object = gtk_builder_get_object(builder, "editLGROOVELabel");
    g_signal_connect(toggle_object, "toggled",
                     G_CALLBACK(adg_gtk_toggle_button_sensitivize), object);

    _adg_part_link(part, &part->TITLE, gtk_builder_get_object(builder, "editTITLE"));
    _adg_part_link(part, &part->DRAWING, gtk_builder_get_object(builder, "editDRAWING"));
    _adg_part_link(part, &part->AUTHOR, gtk_builder_get_object(builder, "editAUTHOR"));
    _adg_part_link(part, &part->DATE, gtk_builder_get_object(builder, "editDATE"));

    part->D5 = 4.5;
    part->RD34 = 1;
    part->LD5 = 5;
    part->LD6 = 1;
    part->LD7 = 0.5;

    _adg_do_edit(part);

    return part;
}

static void
_adg_part_destroy(DemoPart *part)
{
    g_hash_table_destroy(part->widgets);
    g_free(part);
}

static GtkWidget *
_adg_help_window(GtkBuilder *builder)
{
    GtkWidget *window;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndHelp");
    g_assert(GTK_IS_MESSAGE_DIALOG(window));
    g_signal_connect(window, "response", G_CALLBACK(gtk_widget_hide), NULL);

    return window;
}

static GtkWidget *
_adg_about_window(GtkBuilder *builder)
{
    GtkWidget *window;
    GList *icon_list, *last_node;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndAbout");
    g_assert(GTK_IS_ABOUT_DIALOG(window));
    g_signal_connect(window, "response", G_CALLBACK(gtk_widget_hide), NULL);

    icon_list = gtk_window_get_default_icon_list();
    last_node = g_list_last(icon_list);
    if (last_node != NULL) {
        /* The last icon is supposed to be the largest one:
         * check adg_gtk_use_default_icons() implementation. */
        GdkPixbuf *last_icon = last_node->data;
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(window), last_icon);
    }
    g_list_free(icon_list);

    return window;
}

static GtkWidget *
_adg_edit_window(GtkBuilder *builder, DemoPart *part)
{
    GtkWidget *window;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndEdit");
    g_assert(GTK_IS_DIALOG(window));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

    g_signal_connect_swapped(part->apply, "clicked",
                             G_CALLBACK(_adg_do_edit), part);
    g_signal_connect_swapped(part->reset, "clicked",
                             G_CALLBACK(_adg_do_reset), part);
    g_signal_connect(window, "response",
                     G_CALLBACK(adg_gtk_window_hide_here), NULL);

    return window;
}

static GtkWidget *
_adg_save_as_window(GtkBuilder *builder, AdgCanvas *canvas)
{
    GtkWidget *window;
    GtkWidget *type_group;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndSaveAs");
    g_assert(GTK_IS_WINDOW(window));
    type_group = (GtkWidget *) gtk_builder_get_object(builder, "saveAsPng");
    g_assert(GTK_IS_RADIO_BUTTON(type_group));

    g_object_set_data(G_OBJECT(window), "type-group", type_group);

    /* Set the default destination file */
    if (GTK_IS_FILE_CHOOSER(window)) {
        GtkFileChooser *file_chooser;
        const gchar *dir;

        file_chooser = (GtkFileChooser *) window;

#if GLIB_CHECK_VERSION(2, 14, 0)
        dir = g_get_user_special_dir(G_USER_DIRECTORY_DOCUMENTS);
#else
        dir = NULL;
#endif

        if (dir == NULL)
            dir = g_get_home_dir();

        gtk_file_chooser_set_current_folder(file_chooser, dir);
        gtk_file_chooser_set_current_name(file_chooser, "adg-demo");
    }

    g_signal_connect(window, "response", G_CALLBACK(_adg_do_save_as), canvas);

    return window;
}

static GtkWidget *
_adg_main_window(GtkBuilder *builder)
{
    GtkWidget *window;
    DemoPart *part;
    AdgCanvas *canvas;
    GtkWidget *button_edit, *button_save_as, *button_print;
    GtkWidget *button_help, *button_about, *button_quit;

    if (is_installed) {
#ifdef G_OS_WIN32
        gchar *icondir = g_build_filename(basedir, PKGDATADIR, NULL);
        adg_gtk_use_default_icons(icondir);
        g_free(icondir);
#else
        adg_gtk_use_default_icons(PKGDATADIR);
#endif
    } else {
        adg_gtk_use_default_icons(SRCDIR);
    }

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndMain");
    part = _adg_part_new(builder);
    canvas = adg_canvas_new();

    _adg_canvas_init(canvas, part);
    adg_gtk_area_set_canvas(part->area, canvas);
    adg_canvas_autoscale(canvas);

    button_help = (GtkWidget *) gtk_builder_get_object(builder, "mainHelp");
    g_assert(GTK_IS_BUTTON(button_help));
    button_about = (GtkWidget *) gtk_builder_get_object(builder, "mainAbout");
    g_assert(GTK_IS_BUTTON(button_about));
    g_assert(GTK_IS_WINDOW(window));
    button_edit = (GtkWidget *) gtk_builder_get_object(builder, "mainEdit");
    g_assert(GTK_IS_BUTTON(button_edit));
    button_save_as = (GtkWidget *) gtk_builder_get_object(builder, "mainSaveAs");
    g_assert(GTK_IS_BUTTON(button_save_as));
    button_print = (GtkWidget *) gtk_builder_get_object(builder, "mainPrint");
    g_assert(GTK_IS_BUTTON(button_print));
    button_quit = (GtkWidget *) gtk_builder_get_object(builder, "mainQuit");
    g_assert(GTK_IS_BUTTON(button_quit));

    g_signal_connect_swapped(button_help, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_help_window(builder));
    g_signal_connect_swapped(button_about, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_about_window(builder));
    g_signal_connect_swapped(button_edit, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_edit_window(builder, part));
    g_signal_connect_swapped(button_save_as, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_save_as_window(builder, canvas));
    g_signal_connect(button_print, "clicked",
                     G_CALLBACK(_adg_do_print), canvas);
    g_signal_connect(button_quit, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect_swapped(window, "destroy",
                             G_CALLBACK(_adg_part_destroy), part);

    return window;
}


int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *main_window;

    _demo_init(argc, argv);
    parse_args(&argc, &argv);
    adg_switch_extents(show_extents);

    path = _demo_file("adg-demo.ui");
    if (path == NULL) {
        g_printerr(_("adg-demo.ui not found!\n"));
        return 1;
    }

    builder = gtk_builder_new();
    error = NULL;

    gtk_builder_set_translation_domain(builder, GETTEXT_PACKAGE);
    gtk_builder_add_from_file(builder, path, &error);
    g_free(path);

    if (error != NULL) {
        g_critical("%s", error->message);
        g_error_free(error);
        return 2;
    }

    main_window = _adg_main_window(builder);
    g_object_unref(builder);

    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
