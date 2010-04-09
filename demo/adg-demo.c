#include <adg-gtk.h>
#include <math.h>

#include "demo.h"


#define SQRT3   1.732050808
#define CHAMFER 0.3


typedef struct _AdgPart AdgPart;

struct _AdgPart {
    gdouble A, B, C;
    gdouble DHOLE, LHOLE;
    gdouble D1, D2, D3, D4, D5, D6, D7;
    gdouble RD34, RD56;
    gdouble LD2, LD3, LD5, LD6, LD7;
};

static AdgGtkArea *_adg_area = NULL;


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
_adg_part_init_data(AdgPart *part)
{
    part->A = 52.3;
    part->B = 20.6;
    part->C = 2;
    part->DHOLE = 2;
    part->LHOLE = 3;
    part->D1 = 9.3;
    part->D2 = 6.5;
    part->D3 = 11.9;
    part->D4 = 6.5;
    part->D5 = 4.5;
    part->D6 = 7.2;
    part->D7 = 3;
    part->RD34 = 1;
    part->LD2 = 7;
    part->LD3 = 3.5;
    part->LD5 = 5;
    part->LD6 = 1;
    part->LD7 = 0.5;
}

static AdgPath *
_adg_part_hole(const AdgPart *part, gdouble height)
{
    AdgPath *path;
    AdgModel *model;
    AdgPair pair;

    path = adg_path_new();
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

    return path;
}

static AdgPath *
_adg_part_shape(const AdgPart *part)
{
    AdgPath *path;
    AdgModel *model;
    AdgPair pair, tmp;
    const AdgPrimitive *primitive;

    pair.x = part->A - part->B - part->LD2;
    path = _adg_part_hole(part, pair.x);
    model = ADG_MODEL(path);

    pair.x += (part->D1 - part->D2) * SQRT3 / 2;
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

    adg_path_chamfer(path, CHAMFER, CHAMFER);

    pair.x = part->A - part->B + part->LD3;
    pair.y = part->D3 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D3I_X", &tmp);

    adg_path_chamfer(path, CHAMFER, CHAMFER);

    pair.y = part->D4 / 2;
    adg_path_line_to(path, &pair);

    primitive = adg_path_over_primitive(path);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, 0));
    adg_model_set_named_pair(model, "D3F_Y", &tmp);
    cpml_pair_from_cairo(&tmp, cpml_primitive_get_point(primitive, -1));
    adg_model_set_named_pair(model, "D3F_X", &tmp);

    adg_path_fillet(path, part->RD34);

    pair.x = part->A - part->C - part->LD5;
    adg_path_line_to(path, &pair);
    adg_model_set_named_pair(model, "D4F", &pair);

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

    return path;
}

static void
_adg_demo_canvas_add_sheet(AdgCanvas *canvas)
{
    AdgTitleBlock *title_block;
    AdgLogo *logo;
    AdgMatrix map;

    title_block = adg_title_block_new();

    logo = adg_logo_new();
    cairo_matrix_init_scale(&map, 2, 2);
    adg_entity_set_global_map(ADG_ENTITY(logo), &map);

    g_object_set(title_block,
                 "title", "SAMPLE DRAWING",
                 "author", "NtD",
                 "date", "",
                 "drawing", "TEST123",
                 "logo", logo,
                 "projection", adg_projection_new(ADG_PROJECTION_FIRST_ANGLE),
                 "scale", "NONE",
                 "size", "A4",
                 NULL);

    cairo_matrix_init_translate(&map, 300, 150);
    adg_entity_set_global_map(ADG_ENTITY(title_block), &map);

    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(title_block));
}

static void
_adg_demo_canvas_add_dimensions(AdgCanvas *canvas, AdgModel *model)
{
    AdgLDim *ldim;
    AdgADim *adim;
    AdgRDim *rdim;

    /* NORTH */

    ldim = adg_ldim_new_full_from_model(model, "-D1F", "-D3I_X", "-D3F_Y",
                                        ADG_DIR_UP);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "-D3I_X", "-D3F_X", "-D3F_Y",
                                        ADG_DIR_UP);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_dim_set_outside(ADG_DIM(ldim), ADG_THREE_STATE_OFF);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    /* SOUTH */

    ldim = adg_ldim_new_full_from_model(model, "D1I", "LHOLE", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_ldim_switch_extension1(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D3I_X", "D7F", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), NULL, "+0.1");
    adg_ldim_switch_extension2(ldim, FALSE);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D1I", "D7F", "D3F_Y",
                                        ADG_DIR_DOWN);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.05", "+0.05");
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    adim = adg_adim_new_full_from_model(model, "D6F", "D6I_Y", "D67",
                                        "D6F", "D6F");
    adg_dim_set_level(ADG_DIM(adim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(adim));

    rdim = adg_rdim_new_full_from_model(model, "RD34", "RD34_R", "RD34_XY");
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(rdim));

    /* EAST */

    ldim = adg_ldim_new_full_from_model(model, "D3F_Y", "-D3F_Y", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 5);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D6F", "-D6F", "-East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 4);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D4F", "-D4F", "East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D5F", "-D5F", "-East",
                                        ADG_DIR_RIGHT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D7F", "-D7F", "East",
                                        ADG_DIR_RIGHT);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    /* WEST */

    ldim = adg_ldim_new_full_from_model(model, "D1I", "-D1I", "D1I",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(ldim), 3);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "D2I", "-D2I", "D1I",
                                        ADG_DIR_LEFT);
    adg_dim_set_limits(ADG_DIM(ldim), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(ldim), 2);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));

    ldim = adg_ldim_new_full_from_model(model, "DHOLE", "-DHOLE", "D1I",
                                        ADG_DIR_LEFT);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(ldim));
}

static void
_adg_demo_canvas_add_stuff(AdgCanvas *canvas, AdgModel *model)
{
    AdgToyText *toy_text;
    AdgMatrix map;
    const AdgPair *pair;

    toy_text = adg_toy_text_new("Rotate the mouse wheel to zoom in and out");
    pair = adg_model_get_named_pair(model, "D3I");
    cairo_matrix_init_translate(&map, 0, pair->y);
    adg_entity_set_local_map(ADG_ENTITY(toy_text), &map);
    cairo_matrix_init_translate(&map, 10, 30 + 30 * 2);
    adg_entity_set_global_map(ADG_ENTITY(toy_text), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(toy_text));

    toy_text = adg_toy_text_new("Keep the wheel pressed while dragging the mouse to translate");
    cairo_matrix_init_translate(&map, 0, pair->y);
    adg_entity_set_local_map(ADG_ENTITY(toy_text), &map);
    cairo_matrix_init_translate(&map, 10, 50 + 30 * 2);
    adg_entity_set_global_map(ADG_ENTITY(toy_text), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(toy_text));
}

static AdgCanvas *
_adg_build_canvas(void)
{
    AdgPart part;
    AdgCanvas *canvas;
    AdgContainer *container;
    AdgPath *bottom, *shape;
    AdgEdges *edges;
    AdgEntity *entity;
    AdgMatrix map;

    _adg_part_init_data(&part);
    canvas = adg_canvas_new();
    container = (AdgContainer *) canvas;

    bottom = _adg_part_hole(&part, part.LHOLE + 2);
    adg_path_reflect(bottom, NULL);
    adg_path_close(bottom);

    shape = _adg_part_shape(&part);
    adg_path_reflect(shape, NULL);
    adg_path_close(shape);
    adg_path_move_to_explicit(shape, part.LHOLE + 2, part.D1 / 2);
    adg_path_line_to_explicit(shape, part.LHOLE + 2, -part.D1 / 2);

    edges = adg_edges_new_with_source(ADG_TRAIL(shape));

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(shape)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_hatch_new(ADG_TRAIL(bottom)));
    adg_container_add(container, entity);

    entity = ADG_ENTITY(adg_stroke_new(ADG_TRAIL(edges)));
    adg_container_add(container, entity);

    _adg_demo_canvas_add_sheet(canvas);
    _adg_demo_canvas_add_dimensions(canvas, ADG_MODEL(shape));
    _adg_demo_canvas_add_stuff(canvas, ADG_MODEL(shape));

    cairo_matrix_init_scale(&map, 7, 7);
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
_adg_save_as_response(GtkWidget *window, GtkResponseType response)
{
    gchar *file, *suffix;
    GtkRadioButton *type_radio;
    cairo_surface_t *surface = NULL;
    void (*callback)(cairo_surface_t *, gchar *) = NULL;

    if (response != GTK_RESPONSE_OK) {
        gtk_widget_hide(window);
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

    if (strcmp(suffix, ".png") == 0) {
#ifdef CAIRO_HAS_PNG_FUNCTIONS
        surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
        callback = (gpointer) cairo_surface_write_to_png;
#endif
    } else if (strcmp(suffix, ".pdf") == 0) {
#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
        surface = cairo_pdf_surface_create(file, 841, 595);
#endif
    } else if (strcmp(suffix, ".ps") == 0) {
#ifdef CAIRO_HAS_PS_SURFACE
#include <cairo-ps.h>
        surface = cairo_ps_surface_create(file, 841, 595);
        cairo_ps_surface_dsc_comment(surface, "%%Title: " PACKAGE_STRING);
        cairo_ps_surface_dsc_comment(surface, "%%Copyright: Copyright (C) 2006-2010 Fontana Nicola");
        cairo_ps_surface_dsc_comment(surface, "%%Orientation: Portrait");
        cairo_ps_surface_dsc_begin_setup(surface);
        cairo_ps_surface_dsc_begin_page_setup(surface);
        cairo_ps_surface_dsc_comment(surface, "%%IncludeFeature: *PageSize A4");
#endif
    }

    if (surface) {
        cairo_t *cr = cairo_create(surface);
        cairo_surface_destroy(surface);
        adg_entity_render(ADG_ENTITY(adg_gtk_area_get_canvas(_adg_area)), cr);
        cairo_show_page(cr);
        if (callback)
            callback(surface, file);
        cairo_destroy(cr);
    }

    g_free(file);
    g_free(suffix);
    gtk_widget_hide(window);
}

static GtkWidget *
_adg_data_window(GtkBuilder *builder)
{
    GtkWidget *window;
    GtkWidget *button_close;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndData");
    g_assert(window != NULL);
    button_close = (GtkWidget *) gtk_builder_get_object(builder, "dataClose");
    g_assert(button_close != NULL);

    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_widget_hide), NULL);
    g_signal_connect_swapped(button_close, "clicked",
                             G_CALLBACK(gtk_widget_hide), window);

    return window;
}

static GtkWidget *
_adg_save_as_window(GtkBuilder *builder)
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

    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_true), NULL);
    g_signal_connect(window, "response",
                     G_CALLBACK(_adg_save_as_response), NULL);

    return window;
}

static GtkWidget *
_adg_about_window(GtkBuilder *builder)
{
    GtkWidget *window;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndAbout");
    g_assert(window != NULL);

    g_signal_connect(window, "response",
                     G_CALLBACK(gtk_widget_hide), NULL);

    return window;
}

static GtkWidget *
_adg_main_window(GtkBuilder *builder)
{
    GtkWidget *window;
    GtkWidget *button_edit, *button_save_as, *button_about, *button_quit;

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndMain");
    g_assert(window != NULL);

    button_edit = (GtkWidget *) gtk_builder_get_object(builder, "mainEdit");
    g_assert(button_edit != NULL);
    button_save_as = (GtkWidget *) gtk_builder_get_object(builder, "mainSaveAs");
    g_assert(button_save_as != NULL);
    button_about = (GtkWidget *) gtk_builder_get_object(builder, "mainAbout");
    g_assert(button_about != NULL);
    button_quit = (GtkWidget *) gtk_builder_get_object(builder, "mainQuit");
    g_assert(button_quit != NULL);
    _adg_area = (AdgGtkArea *) gtk_builder_get_object(builder, "mainCanvas");
    g_assert(ADG_GTK_IS_AREA(_adg_area));

    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect_swapped(button_edit, "clicked",
                             G_CALLBACK(gtk_widget_show),
                             _adg_data_window(builder));
    g_signal_connect_swapped(button_save_as, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_save_as_window(builder));
    g_signal_connect_swapped(button_about, "clicked",
                             G_CALLBACK(gtk_dialog_run),
                             _adg_about_window(builder));
    g_signal_connect(button_quit, "clicked",
                     G_CALLBACK(gtk_main_quit), NULL);

    adg_gtk_area_set_canvas(_adg_area, _adg_build_canvas());

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

    path = demo_find_data_file("adg-demo.ui");
    if (path == NULL) {
        g_print("adg-demo.ui not found!\n");
        return 1;
    }

    builder = gtk_builder_new();
    error = NULL;
    gtk_builder_add_from_file(builder, path, &error);

    if (error != NULL) {
        g_critical(error->message);
        g_error_free(error);
        return 2;
    }

    main_window = _adg_main_window(builder);
    g_object_unref(builder);

    gtk_widget_show_all(main_window);
    gtk_main();

    return 0;
}
