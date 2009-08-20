#include <adg/adg.h>
#include <math.h>

#include "demo.h"


static AdgCanvas *      sample_canvas           (void);
static AdgCanvas *      operations_canvas       (void);
static AdgCanvas *      mapping_canvas          (void);
static void             to_pdf                  (AdgWidget      *widget,
                                                 GtkWidget      *caller);
static void             to_png                  (AdgWidget      *widget,
                                                 GtkWidget      *caller);
static void             to_ps                   (AdgWidget      *widget,
                                                 GtkWidget      *caller);


int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *window;
    GtkWidget *sample;
    GtkWidget *operations;
    GtkWidget *mapping;

    gtk_init(&argc, &argv);

    path = demo_find_data_file("adg-demo.ui");
    if (path == NULL) {
        g_print("adg-demo.ui not found!\n");
        return 1;
    }

    builder = gtk_builder_new();
    error = NULL;

    gtk_builder_add_from_file(builder, path, &error);
    if (error != NULL) {
        g_print("%s\n", error->message);
        return 2;
    }

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndMain");

    sample = (GtkWidget *) gtk_builder_get_object(builder, "areaSample");
    adg_widget_set_canvas(ADG_WIDGET(sample), sample_canvas());

    operations = (GtkWidget *) gtk_builder_get_object(builder, "areaOperations");
    adg_widget_set_canvas(ADG_WIDGET(operations), operations_canvas());

    mapping = (GtkWidget *) gtk_builder_get_object(builder, "areaMapping");
    adg_widget_set_canvas(ADG_WIDGET(mapping), mapping_canvas());

    /* Connect signals */
    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnQuit"),
                     "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPng"),
                             "clicked", G_CALLBACK(to_png), sample);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPdf"),
                             "clicked", G_CALLBACK(to_pdf), sample);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPs"),
                             "clicked", G_CALLBACK(to_ps), sample);

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


static AdgPath *        non_trivial_model       (void);


/**********************************************
 * A sample mechanical part example
 **********************************************/

#define SQRT3   1.732050808
#define CHAMFER 0.3

typedef struct _SampleData SampleData;

struct _SampleData {
    gdouble A, B, C;
    gdouble D1, D2, D3, D4, D5, D6, D7;
    gdouble RD34, RD56;
    gdouble LD2, LD3, LD5, LD6, LD7;
};

static void     sample_get              (SampleData             *data);
static AdgPath *sample_path             (const SampleData       *data);
static void     sample_add_dimensions   (AdgCanvas              *canvas,
                                         const SampleData       *data);
static void     sample_add_stuff        (AdgCanvas              *canvas,
                                         const SampleData       *data);


static AdgCanvas *
sample_canvas(void)
{
    SampleData data;
    AdgPath *path;
    AdgCanvas *canvas;
    AdgContainer *container;
    AdgEntity *entity;
    AdgMatrix map;

    sample_get(&data);
    path = sample_path(&data);
    canvas = adg_canvas_new();
    container = (AdgContainer *) canvas;

    entity = adg_stroke_new(path);
    adg_container_add(container, entity);

    sample_add_dimensions(canvas, &data);
    sample_add_stuff(canvas, &data);

    cairo_matrix_init_translate(&map, 100, 70);
    cairo_matrix_scale(&map, 6.883, 6.883);
    cairo_matrix_translate(&map, 0, 10);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);

    return canvas;
}

static void
sample_get(SampleData *data)
{
    data->A = 52.3;
    data->B = 20.6;
    data->C = 2;
    data->D1 = 9.3;
    data->D2 = 6.5;
    data->D3 = 11.9;
    data->D4 = 6.5;
    data->D5 = 4.5;
    data->D6 = 7.2;
    data->D7 = 3;
    data->RD34 = 1;
    data->LD2 = 7;
    data->LD3 = 3.5;
    data->LD5 = 5;
    data->LD6 = 1;
    data->LD7 = 0.5;
}

static AdgPath *
sample_path(const SampleData *data)
{
    AdgPath *path;
    double x, y;
    AdgSegment segment_org;
    AdgSegment *segment;
    cairo_matrix_t matrix;

    path = (AdgPath *) adg_path_new();

    adg_path_move_to(path, 0, data->D1 / 2);
    adg_path_line_to(path, data->A - data->B - data->LD2, data->D1 / 2);
    y = (data->D1 - data->D2) / 2;
    adg_path_line_to(path, data->A - data->B - data->LD2 + y * SQRT3, data->D1 / 2 - y);
    adg_path_line_to(path, data->A - data->B, data->D2 / 2);
    adg_path_fillet(path, 0.4);
    adg_path_line_to(path, data->A - data->B, data->D3 / 2);
    adg_path_chamfer(path, CHAMFER, CHAMFER);
    adg_path_line_to(path, data->A - data->B + data->LD3, data->D3 / 2);
    adg_path_chamfer(path, CHAMFER, CHAMFER);
    adg_path_line_to(path, data->A - data->B + data->LD3, data->D4 / 2);
    adg_path_fillet(path, data->RD34);
    adg_path_line_to(path, data->A - data->C - data->LD5, data->D4 / 2);
    y = (data->D4 - data->D5) / 2;
    adg_path_line_to(path, data->A - data->C - data->LD5 + y, data->D4 / 2 - y);
    adg_path_line_to(path, data->A - data->C, data->D5 / 2);
    adg_path_fillet(path, 0.2);
    adg_path_line_to(path, data->A - data->C, data->D6 / 2);
    adg_path_fillet(path, 0.1);
    adg_path_line_to(path, data->A - data->C + data->LD6, data->D6 / 2);
    x = data->C - data->LD7 - data->LD6;
    y = x / SQRT3;
    adg_path_line_to(path, data->A - data->C + data->LD6 + x, data->D6 / 2 - y);
    adg_path_line_to(path, data->A - data->LD7, data->D7 / 2);
    adg_path_line_to(path, data->A, data->D7 / 2);

    /* Build the rounded shape by duplicating the first segment of
     * the current path, reflecting it on the y=0 axis, reversing and
     * joining it the result to the original path */
    cpml_segment_from_cairo(&segment_org, adg_path_get_cpml_path(path));
    segment = adg_segment_deep_dup(&segment_org);
    cpml_segment_reverse(segment);
    adg_matrix_init_reflection(&matrix, 0);
    cpml_segment_transform(segment, &matrix);
    segment->data[0].header.type = CAIRO_PATH_LINE_TO;

    adg_path_append_segment(path, segment);

    g_free(segment);

    adg_path_close(path);
    return path;
}

static void
sample_add_dimensions(AdgCanvas *canvas, const SampleData *data)
{
    AdgEntity *entity;
    double x, y;

    /* NORTH */

    /* LD2 */
    entity = adg_ldim_new_full_explicit(data->A - data->B - data->LD2, -data->D1 / 2, data->A - data->B,
                                        -data->D3 / 2 + CHAMFER, ADG_DIR_UP, 0,
                                        -data->D3 / 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* LD3 */
    entity = adg_ldim_new_full_explicit(data->A - data->B, -data->D3 / 2 + CHAMFER, data->A - data->B + data->LD3,
                                        -data->D3 / 2 + CHAMFER, ADG_DIR_UP, 0,
                                        -data->D3 / 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* SOUTH */

    /* B */
    entity = adg_ldim_new_full_explicit(data->A - data->B, data->D3 / 2 - CHAMFER, data->A, data->D7 / 2,
                                        ADG_DIR_DOWN, 0, data->D3 / 2);
    adg_dim_set_tolerances(ADG_DIM(entity), NULL, "+0.1");
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* A */
    entity = adg_ldim_new_full_explicit(0, data->D1 / 2, data->A, data->D7 / 2,
                                        ADG_DIR_DOWN, 0, data->D3 / 2);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.05", "+0.05");
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* EAST */

    /* D3 */
    x = data->A - data->B + data->LD3 - CHAMFER;
    entity = adg_ldim_new_full_explicit(x, -data->D3 / 2, x, data->D3 / 2,
                                        ADG_DIR_RIGHT, data->A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(entity), 5);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D6 */
    x = data->A - data->C + data->LD6;
    entity = adg_ldim_new_full_explicit(x, -data->D6 / 2, x, data->D6 / 2,
                                        ADG_DIR_RIGHT, data->A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 4);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D4 */
    x = data->A - data->C - data->LD5;
    entity = adg_ldim_new_full_explicit(x, -data->D4 / 2, x, data->D4 / 2,
                                        ADG_DIR_RIGHT, data->A, 0);
    adg_dim_set_level(ADG_DIM(entity), 3);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D5 */
    x = data->A - data->C;
    entity = adg_ldim_new_full_explicit(x, -data->D5 / 2, x, data->D5 / 2,
                                        ADG_DIR_RIGHT, data->A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D7 */
    entity = adg_ldim_new_full_explicit(data->A, -data->D7 / 2, data->A, data->D7 / 2,
                                        ADG_DIR_RIGHT, data->A, 0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* WEST */

    /* D1 */
    entity = adg_ldim_new_full_explicit(0, -data->D1 / 2, 0, data->D1 / 2,
                                        ADG_DIR_LEFT, 0, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D2 */
    y = (data->D1 - data->D2) / 2;
    x = data->A - data->B - data->LD2 + y * SQRT3;
    entity = adg_ldim_new_full_explicit(x, -data->D2 / 2, x, data->D2 / 2,
                                        ADG_DIR_LEFT, 0, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);
}

static void
sample_add_stuff(AdgCanvas *canvas, const SampleData *data)
{
    AdgEntity *toy_text;
    AdgMatrix map;

    toy_text = adg_toy_text_new("Rotate the mouse wheel to zoom in and out");
    cairo_matrix_init_translate(&map, 0, data->D3 / 2);
    adg_entity_set_local_map(toy_text, &map);
    cairo_matrix_translate(&map, 10, 30 + 30 * 2);
    adg_entity_set_global_map(toy_text, &map);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);

    toy_text = adg_toy_text_new("Keep the wheel pressed while dragging the mouse to translate");
    cairo_matrix_init_translate(&map, 0, data->D3 / 2);
    adg_entity_set_local_map(toy_text, &map);
    cairo_matrix_init_translate(&map, 10, 50 + 30 * 2);
    adg_entity_set_global_map(toy_text, &map);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);
}


#if defined(CAIRO_HAS_PNG_FUNCTIONS) || defined(CAIRO_HAS_PDF_SURFACE) || defined(CAIRO_HAS_PS_SURFACE)

/* Only needed if there is at least one supported surface */
static void
file_generated(GtkWidget *caller, const gchar *file)
{
    GtkWindow *window;
    GtkWidget *dialog;

    window = (GtkWindow *) gtk_widget_get_toplevel(caller);
    dialog = gtk_message_dialog_new_with_markup(window, GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO,
                                                GTK_BUTTONS_CLOSE,
                                                "The requested operation generated\n"
                                                "<b>%s</b> in the current directory.",
                                                file);
    gtk_window_set_title(GTK_WINDOW(dialog), "Operation completed");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

#endif

#if !defined(CAIRO_HAS_PNG_FUNCTIONS) || !defined(CAIRO_HAS_PDF_SURFACE) || !defined(CAIRO_HAS_PS_SURFACE)

/* Only needed if there is a missing surface */
static void
missing_feature(GtkWidget *caller, const gchar *feature)
{
    GtkWindow *window;
    GtkWidget *dialog;

    window = (GtkWindow *) gtk_widget_get_toplevel(caller);
    dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_WARNING,
                                    GTK_BUTTONS_OK,
                                    "The provided cairo library\n"
                                    "was compiled with no %s support!",
                                    feature);
    gtk_window_set_title(GTK_WINDOW(dialog), "Missing feature");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

#endif


#ifdef CAIRO_HAS_PNG_FUNCTIONS

static void
to_png(AdgWidget *widget, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(adg_widget_get_canvas(widget)), cr);

    cairo_show_page(cr);
    cairo_surface_write_to_png(surface, "test.png");
    cairo_destroy(cr);

    file_generated(caller, "test.png");
}

#else

static void
to_png(AdgWidget *widget, GtkWidget *caller)
{
    missing_feature(caller, "PNG");
}

#endif

#ifdef CAIRO_HAS_PDF_SURFACE

#include <cairo-pdf.h>

static void
to_pdf(AdgWidget *widget, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_pdf_surface_create("test.pdf", 841, 595);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    adg_entity_render(ADG_ENTITY(adg_widget_get_canvas(widget)), cr);

    cairo_show_page(cr);
    cairo_destroy(cr);

    file_generated(caller, "test.pdf");
}

#else

static void
to_pdf(AdgWidget *widget, GtkWidget *caller)
{
    missing_feature(caller, "PDF");
}

#endif

#ifdef CAIRO_HAS_PS_SURFACE

#include <cairo-ps.h>

static void
to_ps(AdgWidget *widget, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    /* Surface creation: A4 size */
    surface = cairo_ps_surface_create("test.ps", 841, 595);
    cairo_ps_surface_dsc_comment(surface,
                                 "%%Title: Automatic Drawing Generation (ADG) demo");
    cairo_ps_surface_dsc_comment(surface,
                                 "%%Copyright: Copyright (C) 2006-2009 Fontana Nicola");
    cairo_ps_surface_dsc_comment(surface, "%%Orientation: Portrait");

    cairo_ps_surface_dsc_begin_setup(surface);

    cairo_ps_surface_dsc_begin_page_setup(surface);
    cairo_ps_surface_dsc_comment(surface,
                                 "%%IncludeFeature: *PageSize A4");

    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    adg_entity_render(ADG_ENTITY(adg_widget_get_canvas(widget)), cr);

    cairo_show_page(cr);
    cairo_destroy(cr);

    file_generated(caller, "test.ps");
}

#else

static void
to_ps(AdgWidget *widget, GtkWidget *caller)
{
    missing_feature(caller, "PostScript");
}

#endif


/**********************************************
 * Test case for basic operations,
 * such as chamfer and fillet
 **********************************************/

static AdgPath *        operations_chamfer      (const AdgPath  *path,
                                                 gdouble         delta1,
                                                 gdouble         delta2);
static AdgPath *        operations_fillet       (const AdgPath  *path,
                                                 gdouble         radius);

static AdgCanvas *
operations_canvas(void)
{
    AdgPath *path, *chamfer_path, *fillet_path;
    AdgCanvas *canvas;
    AdgContainer *container;
    AdgEntity *entity;
    AdgMatrix map;

    path = non_trivial_model();
    chamfer_path = operations_chamfer(path, 0.25, 0.25);
    fillet_path = operations_fillet(path, 0.20);
    canvas = adg_canvas_new();

    /* Add the original shape */
    container = (AdgContainer *) adg_container_new();
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));

    adg_container_add(container, adg_stroke_new(path));

    entity = adg_toy_text_new("Original shape");
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    cairo_matrix_init_translate(&map, -50, 20);
    adg_entity_set_global_map(entity, &map);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* Add the shape with 0.25x0.25 chamfer */
    container = (AdgContainer *) adg_container_new();
    cairo_matrix_init_translate(&map, 15, 0);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));

    adg_container_add(container, adg_stroke_new(chamfer_path));

    entity = adg_toy_text_new("Shape with 0.25x0.25 chamfer");
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    cairo_matrix_init_translate(&map, -120, 20);
    adg_entity_set_global_map(entity, &map);
    adg_container_add(container, entity);

    /* Add the shape with fillets with 0.20 of radius */
    container = (AdgContainer *) adg_container_new();
    cairo_matrix_init_translate(&map, 30, 0);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));

    adg_container_add(container, adg_stroke_new(fillet_path));

    entity = adg_toy_text_new("Shape with R=20 fillet");
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    cairo_matrix_init_translate(&map, -90, 20);
    adg_entity_set_global_map(entity, &map);
    adg_container_add(container, entity);

    /* Set a decent start position and zoom */
    cairo_matrix_init_translate(&map, 10, -140);
    cairo_matrix_scale(&map, 15, 15);
    cairo_matrix_translate(&map, 0, 10);
    adg_entity_set_local_map(ADG_ENTITY(canvas), &map);

    return canvas;
}

static AdgPath *
operations_chamfer(const AdgPath *model, gdouble delta1, gdouble delta2)
{
    AdgPath *path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    path = (AdgPath *) adg_path_new();
    cpml_segment_from_cairo(&segment, adg_path_get_cpml_path((AdgPath *) model));
    cpml_primitive_from_segment(&primitive, &segment);

    adg_path_move_to(path, (primitive.org)->point.x, (primitive.org)->point.y);

    do {
        adg_path_append_primitive(path, &primitive);
        if (primitive.data[0].header.type == CAIRO_PATH_LINE_TO)
            adg_path_chamfer(path, delta1, delta2);
    } while (cpml_primitive_next(&primitive));

    return path;
}

static AdgPath *
operations_fillet(const AdgPath *model, gdouble radius)
{
    AdgPath *path;
    CpmlSegment segment;
    CpmlPrimitive primitive;

    path = (AdgPath *) adg_path_new();
    cpml_segment_from_cairo(&segment, adg_path_get_cpml_path((AdgPath *) model));
    cpml_primitive_from_segment(&primitive, &segment);

    adg_path_move_to(path, (primitive.org)->point.x, (primitive.org)->point.y);

    do {
        adg_path_append_primitive(path, &primitive);
        if (primitive.data[0].header.type == CAIRO_PATH_LINE_TO)
            adg_path_fillet(path, radius);
    } while (cpml_primitive_next(&primitive));

    return path;
}


/**********************************************
 * Test case for mapping transformations,
 * either on the local and global map
 **********************************************/

static AdgCanvas *
mapping_canvas(void)
{
    AdgPath *path;
    AdgCanvas *canvas;
    AdgContainer *container;
    AdgEntity *entity;
    AdgMatrix map;

    path = non_trivial_model();
    canvas = adg_canvas_new();

    /* Add the original shape */
    container = (AdgContainer *) adg_container_new();
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));

    adg_container_add(container, adg_stroke_new(path));

    entity = adg_toy_text_new("Original shape");
    cairo_matrix_init_translate(&map, -50, 20);
    adg_entity_set_global_map(entity, &map);
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* Original shape with global rotated by 90 and local translated x+=10 */
    container = (AdgContainer *) adg_container_new();
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));
    cairo_matrix_init_translate(&map, 15, 0);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);

    entity = adg_stroke_new(path);
    cairo_matrix_init_rotate(&map, M_PI_2);
    adg_entity_set_global_map(ADG_ENTITY(entity), &map);
    cairo_matrix_init_translate(&map, 10, 0);
    adg_entity_set_local_map(ADG_ENTITY(entity), &map);
    adg_container_add(container, entity);

    entity = adg_toy_text_new("Global map rotated by 90");
    cairo_matrix_init_translate(&map, -120, 20);
    adg_entity_set_global_map(entity, &map);
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    adg_container_add(container, entity);

    /* Original shape with local translated x+=10 and rotated by 90 */
    container = (AdgContainer *) adg_container_new();
    adg_container_add(ADG_CONTAINER(canvas), ADG_ENTITY(container));
    cairo_matrix_init_translate(&map, 30, 0);
    adg_entity_set_local_map(ADG_ENTITY(container), &map);

    entity = adg_stroke_new(path);
    cairo_matrix_init_translate(&map, 10, 0);
    cairo_matrix_rotate(&map, M_PI_2);
    adg_entity_set_local_map(ADG_ENTITY(entity), &map);
    adg_container_add(container, entity);

    entity = adg_toy_text_new("Local map rotated by 90");
    cairo_matrix_init_translate(&map, -120, 20);
    adg_entity_set_global_map(entity, &map);
    cairo_matrix_init_translate(&map, 5, 10);
    adg_entity_set_local_map(entity, &map);
    adg_container_add(container, entity);

    /* Set a decent start position and zoom */
    cairo_matrix_init_translate(&map, 10, -140);
    cairo_matrix_scale(&map, 15, 15);
    cairo_matrix_translate(&map, 0, 10);
    adg_entity_set_local_map(ADG_ENTITY(canvas), &map);

    return canvas;
}


/**********************************************
 * Non specific test related stuff
 **********************************************/

static AdgPath *
non_trivial_model()
{
    AdgPath *path = (AdgPath *) adg_path_new();

    adg_path_move_to(path,  2,  0);
    adg_path_line_to(path,  0,  5);
    adg_path_line_to(path,  2,  2);
    adg_path_line_to(path,  0,  8);
    adg_path_line_to(path,  2,  8);
    adg_path_line_to(path,  2, 10);
    adg_path_line_to(path,  3, 10);
    adg_path_line_to(path, 10,  9);
    adg_path_line_to(path,  5,  5);
    adg_path_line_to(path,  3,  0);
    adg_path_close(path);

    return path;
}
