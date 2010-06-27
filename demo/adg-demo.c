#include "demo.h"

#include <adg-gtk.h>
#include <math.h>

#define SQRT3   1.732050808
#define CHAMFER 0.3


typedef struct _AdgPart AdgPart;

struct _AdgPart {
    /* Raw data */
    gdouble     A, B, C;
    gdouble     DHOLE, LHOLE;
    gdouble     D1, D2, D3, D4, D5, D6, D7;
    gdouble     RD34, RD56;
    gdouble     LD2, LD3, LD5, LD6, LD7;

    /* User interface widgets */
    AdgGtkArea *area;
    GHashTable *widgets;
    GtkButton  *apply, *reset;

    /* Models */
    AdgPath    *shape, *hatch;
    AdgEdges   *edges;
};


static void
_adg_version(void)
{
    g_print("adg-demo " PACKAGE_VERSION "\n");
    exit(0);
}

static void
_adg_parse_args(gint *p_argc, gchar **p_argv[], gboolean *show_extents)
{
    GError *error = NULL;
    GOptionEntry entries[] = {
        {"version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
         _adg_version, "Display version information", NULL},
        {"show-extents", 'E', 0, G_OPTION_ARG_NONE,
         NULL, "Show the boundary boxes of every entity", NULL},
        {NULL}
    };

    entries[1].arg_data = show_extents;
    *show_extents = FALSE;
    gtk_init_with_args(p_argc, p_argv, "- ADG demonstration program",
                       entries, GETTEXT_PACKAGE, &error);

    if (error != NULL) {
        gint error_code = error->code;

        g_printerr("%s\n", error->message);

        g_error_free(error);
        exit(error_code);
    }
}

static void
_adg_define_hole(AdgPath *path, const AdgPart *part, gdouble height)
{
    AdgModel *model;
    AdgPair pair;

    model = ADG_MODEL(path);

    pair.x = part->LHOLE;
    pair.y = 0;
    adg_path_move_to(path, &pair);
    adg_model_set_named_pair(model, "LHOLE", &pair);

    pair.y = part->DHOLE / 2;
    pair.x -= pair.y / SQRT3;
    adg_path_line_to(path, &pair);

    pair.x = 0;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "DHOLE", &pair);

    pair.y = part->D1 / 2;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D1I", &pair);

    pair.x = height;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D1F", &pair);
}

static void
_adg_define_hatch(AdgPath *path, const AdgPart *part)
{
    _adg_define_hole(path, part, part->LHOLE + 2);
    adg_path_reflect(path, NULL);
    adg_path_close(path);
}

static void
_adg_define_shape(AdgPath *path, const AdgPart *part)
{
    AdgModel *model;
    AdgPair pair, tmp;
    const AdgPrimitive *primitive;

    pair.x = part->A - part->B - part->LD2;
    _adg_define_hole(path, part, pair.x);
    model = ADG_MODEL(path);

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
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D3I_X", &tmp);

    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, -1));
    adg_model_set_named_pair(model, "D3I_Y", &tmp);

    adg_path_chamfer(path, CHAMFER, CHAMFER);

    pair.y = part->D4 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D3F_Y", &tmp);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, -1));
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
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
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
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D5F", &tmp);

    adg_path_fillet(path, 0.1);

    pair.x += part->LD6;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D6F", &pair);

    primitive = adg_path_over_primitive(path);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D6I_X", &tmp);

    primitive = adg_path_over_primitive(path);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, -1));
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
    adg_path_close(path);
    adg_path_move_to_explicit(path, part->LHOLE + 2, part->D1 / 2);
    adg_path_line_to_explicit(path, part->LHOLE + 2, -part->D1 / 2);
}

static void
_adg_part_lock(AdgPart *part)
{
    gtk_widget_set_sensitive(GTK_WIDGET(part->apply), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(part->reset), FALSE);
}

static void
_adg_part_unlock(AdgPart *part)
{
    gtk_widget_set_sensitive(GTK_WIDGET(part->apply), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(part->reset), TRUE);
}

static void
_adg_part_link(AdgPart *part, gpointer data, GObject *widget)
{
    g_assert(GTK_IS_WIDGET(widget));
    g_object_ref(widget);
    g_hash_table_insert(part->widgets, data, widget);

    if (GTK_IS_SPIN_BUTTON(widget)) {
        GtkSpinButton *spin_button = GTK_SPIN_BUTTON(widget);
        gtk_adjustment_value_changed(gtk_spin_button_get_adjustment(spin_button));
    }

    g_signal_connect_swapped(widget, "changed",
                             G_CALLBACK(_adg_part_unlock), part);
}

static void
_adg_part_ui_to_double(AdgPart *part, gdouble *data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_WIDGET(widget));

    *data = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
}

static void
_adg_part_double_to_ui(AdgPart *part, gdouble *data)
{
    GtkWidget *widget = g_hash_table_lookup(part->widgets, data);

    g_assert(GTK_IS_WIDGET(widget));

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), *data);
}

static void
_adg_demo_canvas_add_sheet(AdgCanvas *canvas)
{
    AdgTitleBlock *title_block = adg_title_block_new();

    g_object_set(title_block,
                 "title", "SAMPLE DRAWING",
                 "author", "NtD",
                 "date", NULL,
                 "drawing", "TEST123",
                 "logo", adg_logo_new(),
                 "projection", adg_projection_new(ADG_PROJECTION_FIRST_ANGLE),
                 "scale", "NONE",
                 "size", "A4",
                 NULL);

    adg_canvas_set_title_block(canvas, title_block);
}

static void
_adg_demo_canvas_add_dimensions(AdgCanvas *canvas, AdgModel *model)
{
    AdgLDim *ldim;
    AdgADim *adim;
    AdgRDim *rdim;

    /* NORTH */
    adim = adg_adim_new_full_from_model(model, "-D1I", "-D1F",
                                        "-D1F", "-D2I", "-D1F");
    adg_dim_set_level(ADG_DIM(adim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    ldim = adg_ldim_new_full_from_model(model, "-D1F", "-D3I_X", "-D3F_Y",
                                        ADG_DIR_UP);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "-D3I_X", "-D3F_X", "-D3F_Y",
                                        ADG_DIR_UP);
    adg_ldim_switch_extension1(ldim, FALSE);
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


    /* SOUTH */
    ldim = adg_ldim_new_full_from_model(model, "D1I", "LHOLE", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D4F", "D6I_X", "D4_POS",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), NULL, "+0.2");
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D3I_X", "D7F", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), NULL, "+0.1");
    adg_dim_set_level(ADG_DIM(ldim), 2);
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
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    /* EAST */
    ldim = adg_ldim_new_full_from_model(model, "D6F", "-D6F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 4);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D4F", "-D4F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D5F", "-D5F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D7F", "-D7F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));


    /* WEST */
    ldim = adg_ldim_new_full_from_model(model, "D3I_Y", "-D3I_Y", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 4);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "-D1I", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D2I", "-D2I", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "DHOLE", "-DHOLE", "-West",
                                        ADG_DIR_LEFT);
    adg_dim_set_value(ADG_DIM(ldim), ADG_UTF8_DIAMETER "<>");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));
}

static void
_adg_demo_canvas_add_stuff(AdgCanvas *canvas, AdgModel *model)
{
    AdgToyText *toy_text;
    AdgMatrix map;

    toy_text = adg_toy_text_new("Rotate the mouse wheel to zoom in and out");
    adg_entity_set_local_method(ADG_ENTITY(toy_text), ADG_MIX_DISABLED);
    cairo_matrix_init_translate(&map, 15, 500);
    adg_entity_set_global_map(ADG_ENTITY(toy_text), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(toy_text));

    toy_text = adg_toy_text_new("Drag with the wheel pressed to pan");
    adg_entity_set_local_method(ADG_ENTITY(toy_text), ADG_MIX_DISABLED);
    cairo_matrix_init_translate(&map, 15, 515);
    adg_entity_set_global_map(ADG_ENTITY(toy_text), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(toy_text));
}

static AdgCanvas *
_adg_canvas_init(AdgCanvas *canvas, AdgPart *part)
{
    AdgContainer *container;
    AdgEntity *entity;
    AdgMatrix map;

    container = (AdgContainer *) canvas;

    adg_canvas_set_paper(canvas, GTK_PAPER_NAME_A4,
                         GTK_PAGE_ORIENTATION_LANDSCAPE);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(part->shape)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_hatch_new(ADG_TRAIL(part->hatch)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(part->edges)));
    adg_container_add(container, entity);

    _adg_demo_canvas_add_sheet(canvas);
    _adg_demo_canvas_add_dimensions(canvas, ADG_MODEL(part->shape));
    _adg_demo_canvas_add_stuff(canvas, ADG_MODEL(part->shape));

    cairo_matrix_init_translate(&map, 150, 250);
    cairo_matrix_scale(&map, 8, 8);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);

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
_adg_do_edit(AdgPart *part)
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

    _adg_part_lock(part);

    adg_model_clear(ADG_MODEL(part->shape));
    _adg_define_shape(part->shape, part);
    adg_model_changed(ADG_MODEL(part->shape));

    adg_model_clear(ADG_MODEL(part->hatch));
    _adg_define_hatch(part->hatch, part);
    adg_model_changed(ADG_MODEL(part->hatch));

    adg_model_clear(ADG_MODEL(part->edges));
    adg_model_changed(ADG_MODEL(part->edges));

    gtk_widget_queue_draw(GTK_WIDGET(part->area));
}

static void
_adg_do_reset(AdgPart *part)
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
    _adg_part_lock(part);
}

static void
_adg_do_save_as(GtkWindow *window, GtkResponseType response, AdgCanvas *canvas)
{
    GtkRadioButton *type_radio;
    gchar *file, *suffix;
    cairo_surface_t *surface;

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
    g_assert(suffix != NULL);

    if (!g_str_has_suffix(file, suffix)) {
        gchar *tmp = file;
        file = g_strconcat(file, suffix, NULL);
        g_free(tmp);
    }

#ifdef CAIRO_HAS_PNG_FUNCTIONS
    if (strcmp(suffix, ".png") == 0) {
        surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 800, 600);
    } else
#endif
#ifdef CAIRO_HAS_PDF_SURFACE
    if (strcmp(suffix, ".pdf") == 0) {
#include <cairo-pdf.h>
        surface = cairo_pdf_surface_create(file, 841, 595);
    } else
#endif
#ifdef CAIRO_HAS_PS_SURFACE
    if (strcmp(suffix, ".ps") == 0) {
#include <cairo-ps.h>
        surface = cairo_ps_surface_create(file, 841, 595);
        cairo_ps_surface_dsc_comment(surface, "%%Title: " PACKAGE_STRING);
        cairo_ps_surface_dsc_comment(surface, "%%Copyright: Copyleft (C) 2006-2010 Fontana Nicola");
        cairo_ps_surface_dsc_comment(surface, "%%Orientation: Landscape");
        cairo_ps_surface_dsc_begin_setup(surface);
        cairo_ps_surface_dsc_begin_page_setup(surface);
        cairo_ps_surface_dsc_comment(surface, "%%IncludeFeature: *PageSize A4");
    } else
#endif
    {
        adg_gtk_notify_error(_("Requested format not supported"), window);
        surface = NULL;
    }

    if (surface) {
        cairo_t *cr;
        cairo_status_t status;

        cr = cairo_create(surface);
        cairo_surface_destroy(surface);
        adg_entity_render(ADG_ENTITY(canvas), cr);

        if (cairo_surface_get_type(surface) == CAIRO_SURFACE_TYPE_IMAGE) {
            status = cairo_surface_write_to_png(surface, file);
        } else {
            cairo_show_page(cr);
            status = cairo_status(cr);
        }

        cairo_destroy(cr);

        if (status != CAIRO_STATUS_SUCCESS)
            adg_gtk_notify_error(cairo_status_to_string(status), window);
    }

    g_free(file);
    g_free(suffix);
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
    AdgMatrix old_map;

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
    page_setup = g_object_get_data(G_OBJECT(canvas), "_adg_page_setup");
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
        adg_gtk_notify_error(error->message, window);
}

static AdgPart *
_adg_part_new(GtkBuilder *builder)
{
    AdgPart *part;

    part = g_new0(AdgPart, 1);
    part->widgets = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                          NULL, g_object_unref);
    part->area = (AdgGtkArea *) gtk_builder_get_object(builder, "mainCanvas");
    part->apply = (GtkButton *) gtk_builder_get_object(builder, "editApply");
    part->reset = (GtkButton *) gtk_builder_get_object(builder, "editReset");
    part->shape = adg_path_new();
    part->hatch = adg_path_new();
    part->edges = adg_edges_new_with_source(ADG_TRAIL(part->shape));

    g_assert(ADG_GTK_IS_AREA(part->area));
    g_assert(GTK_IS_BUTTON(part->apply));
    g_assert(GTK_IS_BUTTON(part->reset));

    _adg_part_link(part, &part->A, gtk_builder_get_object(builder, "editA"));
    _adg_part_link(part, &part->B, gtk_builder_get_object(builder, "editB"));
    _adg_part_link(part, &part->C, gtk_builder_get_object(builder, "editC"));
    _adg_part_link(part, &part->DHOLE, gtk_builder_get_object(builder, "editDHOLE"));
    _adg_part_link(part, &part->LHOLE, gtk_builder_get_object(builder, "editLHOLE"));
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

    part->D5 = 4.5;
    part->RD34 = 1;
    part->LD5 = 5;
    part->LD6 = 1;
    part->LD7 = 0.5;

    _adg_do_edit(part);

    return part;
}

static void
_adg_part_destroy(AdgPart *part)
{
    g_hash_table_destroy(part->widgets);
    g_free(part);
}

static GtkWidget *
_adg_about_window(GtkBuilder *builder)
{
    GtkWidget *window;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndAbout");
    g_assert(GTK_IS_ABOUT_DIALOG(window));

    g_signal_connect(window, "response", G_CALLBACK(gtk_widget_hide), NULL);

    return window;
}

/**
 * _adg_window_hide:
 * @window: a #GtkWindow
 *
 * A convenient function that hides @window storing the current position
 * so any subsequent call to gtk_widget_show() will hopefully reopen
 * the window at the same position.
 *
 * It is useful to connect this callback to a #GtkDialog::response signal.
 **/
static void
_adg_window_hide(GtkWindow *window)
{
    gint x, y;

    g_return_if_fail(GTK_IS_WINDOW(window));

    gtk_window_get_position(window, &x, &y);
    gtk_widget_hide((GtkWidget *) window);
    gtk_window_set_position(window, GTK_WIN_POS_NONE);
    gtk_window_move(window, x, y);
}

static GtkWidget *
_adg_edit_window(GtkBuilder *builder, AdgPart *part)
{
    GtkWidget *window;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndEdit");
    g_assert(GTK_IS_DIALOG(window));

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);

    g_signal_connect_swapped(part->apply, "clicked",
                             G_CALLBACK(_adg_do_edit), part);
    g_signal_connect_swapped(part->reset, "clicked",
                             G_CALLBACK(_adg_do_reset), part);
    g_signal_connect(window, "response", G_CALLBACK(_adg_window_hide), NULL);

    return window;
}

static GtkWidget *
_adg_save_as_window(GtkBuilder *builder, AdgCanvas *canvas)
{
    GtkWidget *window;
    GtkWidget *type_group, *button_save;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndSaveAs");
    g_assert(GTK_IS_WINDOW(window));
    type_group = (GtkWidget *) gtk_builder_get_object(builder, "saveAsPng");
    g_assert(GTK_IS_RADIO_BUTTON(type_group));
    button_save = (GtkWidget *) gtk_builder_get_object(builder, "saveAsSave");
    g_assert(GTK_IS_BUTTON(button_save));

    g_object_set_data(G_OBJECT(window), "type-group", type_group);

    g_signal_connect(window, "response", G_CALLBACK(_adg_do_save_as), canvas);

    return window;
}

static GtkWidget *
_adg_main_window(GtkBuilder *builder)
{
    GtkWidget *window;
    AdgPart *part;
    AdgCanvas *canvas;
    GtkWidget *button_edit, *button_save_as, *button_print;
    GtkWidget *button_about, *button_quit;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndMain");
    part = _adg_part_new(builder);
    canvas = adg_canvas_new();

    _adg_canvas_init(canvas, part);
    adg_gtk_area_set_canvas(part->area, canvas);

    g_assert(GTK_IS_WINDOW(window));
    button_edit = (GtkWidget *) gtk_builder_get_object(builder, "mainEdit");
    g_assert(GTK_IS_BUTTON(button_edit));
    button_save_as = (GtkWidget *) gtk_builder_get_object(builder, "mainSaveAs");
    g_assert(GTK_IS_BUTTON(button_save_as));
    button_print = (GtkWidget *) gtk_builder_get_object(builder, "mainPrint");
    g_assert(GTK_IS_BUTTON(button_print));
    button_about = (GtkWidget *) gtk_builder_get_object(builder, "mainAbout");
    g_assert(GTK_IS_BUTTON(button_about));
    button_quit = (GtkWidget *) gtk_builder_get_object(builder, "mainQuit");
    g_assert(GTK_IS_BUTTON(button_quit));

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
    gboolean show_extents;
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *main_window;

    _adg_parse_args(&argc, &argv, &show_extents);
    adg_switch_extents(show_extents);

    path = demo_find_data_file("adg-demo.ui", argv[0]);
    if (path == NULL) {
        g_print("adg-demo.ui not found!\n");
        return 1;
    }

    builder = gtk_builder_new();
    error = NULL;
    gtk_builder_add_from_file(builder, path, &error);

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
