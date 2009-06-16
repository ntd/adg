#include <adg/adg.h>
#include <math.h>

#include "demo.h"


static AdgCanvas *      ldim_canvas             (void);
static void             ldim_expose             (GtkWidget      *widget,
                                                 GdkEventExpose *event,
                                                 AdgCanvas      *canvas);
static AdgCanvas *      drawing_canvas          (void);
static void             drawing_expose          (GtkWidget      *widget,
                                                 GdkEventExpose *event,
                                                 AdgCanvas      *canvas);
static void             to_pdf                  (AdgCanvas      *canvas,
                                                 GtkWidget      *caller);
static void             to_png                  (AdgCanvas      *canvas,
                                                 GtkWidget      *caller);
static void             to_ps                   (AdgCanvas      *canvas,
                                                 GtkWidget      *caller);


int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *window;
    AdgCanvas *ldim, *drawing;

    gtk_init(&argc, &argv);

    /* Get the canvas populated by the examples */
    ldim = ldim_canvas();
    drawing = drawing_canvas();

    /* User interface stuff */
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

    /* Connect signals */
    g_signal_connect(window, "delete-event",
                     G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnQuit"),
                     "clicked", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(gtk_builder_get_object(builder, "areaLDim"),
                     "expose-event", G_CALLBACK(ldim_expose), ldim);

    g_signal_connect(gtk_builder_get_object(builder, "areaDrawing"),
                     "expose-event", G_CALLBACK(drawing_expose), drawing);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPng"),
                             "clicked", G_CALLBACK(to_png), drawing);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPdf"),
                             "clicked", G_CALLBACK(to_pdf), drawing);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPs"),
                             "clicked", G_CALLBACK(to_ps), drawing);

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


/**********************************************
 * A simple example
 **********************************************/

static AdgCanvas *
ldim_canvas(void)
{
    AdgPath   *path;
    AdgCanvas *canvas;
    AdgEntity *entity;

    /* Build the path model */
    path = (AdgPath *) adg_path_new();

    adg_path_move_to(path,  0,   0);
    adg_path_line_to(path,  0,  -5);
    adg_path_line_to(path,  2,  -5);
    adg_path_line_to(path,  2, -10);
    adg_path_line_to(path,  8, -10);
    adg_path_line_to(path,  8,  -5);
    adg_path_line_to(path, 10,  -5);
    adg_path_line_to(path, 10,   0);
    adg_path_close(path);

    /* Populate the canvas */
    canvas = adg_canvas_new();

    entity = adg_stroke_new(path);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    entity = adg_ldim_new_full_explicit(2, -10, 8, -10, ADG_DIR_UP, 0, -10);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    entity = adg_ldim_new_full_explicit(0, -5, 10, -5, ADG_DIR_UP, 0, -10);
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    return canvas;
}

static void
ldim_expose(GtkWidget *widget, GdkEventExpose *event, AdgCanvas *canvas)
{
    cairo_t *cr;
    gint width, height;
    double xscale, yscale, scale;
    AdgMatrix matrix;

    cr = gdk_cairo_create(widget->window);
    width = widget->allocation.width;
    height = widget->allocation.height;

    /* Fit ldim in horizontal or vertical space keeping the aspect ratio:
     * the lesser scale factor will be used */
    xscale = (double) (width - 20) / 10;
    yscale = (double) (height - 90) / 10;

    if (xscale < yscale)
        scale = xscale;
    else
        scale = yscale;

    cairo_matrix_init_translate(&matrix, 10, 80);
    cairo_matrix_scale(&matrix, scale, scale);
    cairo_matrix_translate(&matrix, 0, 10);
    adg_container_set_model_transformation(ADG_CONTAINER(canvas), &matrix);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_destroy(cr);
}


/**********************************************
 * A more complex example
 **********************************************/

#define SQRT3   1.732050808
#define CHAMFER 0.3

typedef struct _DrawingData DrawingData;

struct _DrawingData {
    gdouble A, B, C;
    gdouble D1, D2, D3, D4, D5, D6, D7;
    gdouble RD34, RD56;
    gdouble LD2, LD3, LD5, LD6, LD7;
};

static void     drawing_get             (DrawingData            *data);
static AdgPath *drawing_path            (const DrawingData      *data);
static void     drawing_add_dimensions  (AdgCanvas              *canvas,
                                         const DrawingData      *data);
static void     drawing_add_stuff       (AdgCanvas              *canvas,
                                         const DrawingData      *data);


static AdgCanvas *
drawing_canvas(void)
{
    DrawingData data;
    AdgPath    *path;
    AdgCanvas  *canvas;
    AdgEntity  *entity;

    drawing_get(&data);
    path = drawing_path(&data);
    canvas = adg_canvas_new();

    entity = adg_stroke_new(path);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    drawing_add_dimensions(canvas, &data);
    drawing_add_stuff(canvas, &data);

    return canvas;
}

static void
drawing_expose(GtkWidget *widget, GdkEventExpose *event, AdgCanvas *canvas)
{
    cairo_t *cr;
    gint width, height;
    double scale;
    AdgMatrix matrix;

    cr = gdk_cairo_create(widget->window);
    width = widget->allocation.width;
    height = widget->allocation.height;

    /* Hardcoding sizes is a really ugly way to scale a drawing but... */
    scale = (double) (width - 100 - 180) / 52.3;

    cairo_matrix_init_translate(&matrix, 100, 70);
    cairo_matrix_scale(&matrix, scale, scale);
    cairo_matrix_translate(&matrix, 0, 6);
    adg_container_set_model_transformation(ADG_CONTAINER(canvas), &matrix);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_destroy(cr);
}

static void
drawing_get(DrawingData *data)
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
    data->D7 = 3.;
    data->RD34 = 1.;
    data->LD2 = 7.;
    data->LD3 = 3.5;
    data->LD5 = 5.;
    data->LD6 = 1.;
    data->LD7 = 0.5;
}

static AdgPath *
drawing_path(const DrawingData *data)
{
    AdgPath *path;
    double x, y;
    cairo_path_t *cairo_path;
    CpmlSegment segment;
    cairo_matrix_t matrix;

    path = (AdgPath *) adg_path_new();

    adg_path_move_to(path, 0, data->D1 / 2);
    adg_path_line_to(path, data->A - data->B - data->LD2, data->D1 / 2);
    y = (data->D1 - data->D2) / 2;
    adg_path_line_to(path, data->A - data->B - data->LD2 + y * SQRT3, data->D1 / 2 - y);
    adg_path_line_to(path, data->A - data->B, data->D2 / 2);
    adg_path_line_to(path, data->A - data->B, data->D3 / 2 - CHAMFER);
    adg_path_line_to(path, data->A - data->B + CHAMFER, data->D3 / 2);
    adg_path_line_to(path, data->A - data->B + data->LD3 - CHAMFER, data->D3 / 2);
    adg_path_line_to(path, data->A - data->B + data->LD3, data->D3 / 2 - CHAMFER);
    x = data->A - data->B + data->LD3 + data->RD34;
    y = data->D4 / 2 + data->RD34;
    adg_path_arc(path, x, y, data->RD34, G_PI, -G_PI_2);
    adg_path_line_to(path, data->A - data->C - data->LD5, data->D4 / 2);
    y = (data->D4 - data->D5) / 2;
    adg_path_line_to(path, data->A - data->C - data->LD5 + y, data->D4 / 2 - y);
    adg_path_line_to(path, data->A - data->C, data->D5 / 2);
    adg_path_line_to(path, data->A - data->C, data->D6 / 2);
    adg_path_line_to(path, data->A - data->C + data->LD6, data->D6 / 2);
    x = data->C - data->LD7 - data->LD6;
    y = x / SQRT3;
    adg_path_line_to(path, data->A - data->C + data->LD6 + x, data->D6 / 2 - y);
    adg_path_line_to(path, data->A - data->LD7, data->D7 / 2);
    adg_path_line_to(path, data->A, data->D7 / 2);

    /* Build the shape by reflecting the current path, reversing the order
     * and joining the result to the current path */
    cairo_path = adg_path_dup_cpml_path(path);

    cpml_segment_from_cairo(&segment, cairo_path);
    cpml_segment_reverse(&segment);
    adg_matrix_init_reflection(&matrix, 0);
    cpml_segment_transform(&segment, &matrix);
    cairo_path->data[0].header.type = CAIRO_PATH_LINE_TO;

    adg_path_append_cairo_path(path, cairo_path);

    g_free(cairo_path);

    adg_path_close(path);
    return path;
}

static void
drawing_add_dimensions(AdgCanvas *canvas, const DrawingData *data)
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
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* A */
    entity = adg_ldim_new_full_explicit(0, data->D1 / 2, data->A, data->D7 / 2,
                                        ADG_DIR_DOWN, 0, data->D3 / 2);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
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
drawing_add_stuff(AdgCanvas *canvas, const DrawingData *data)
{
    AdgEntity *toy_text;

    toy_text = adg_toy_text_new("Horizontal toy_text above the piston");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., -data->D1 / 2,
                                         0., -5.);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);

    toy_text = adg_toy_text_new("toy_text");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., data->D1 / 2,
                                         0., -5.);
    adg_rotable_set_angle(ADG_ROTABLE(toy_text), M_PI * 3./2.);
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
to_png(AdgCanvas *canvas, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 800, 600);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_show_page(cr);
    cairo_surface_write_to_png(surface, "test.png");
    cairo_destroy(cr);

    file_generated(caller, "test.png");
}

#else

static void
to_png(AdgCanvas *canvas, GtkWidget *caller)
{
    missing_feature(caller, "PNG");
}

#endif

#ifdef CAIRO_HAS_PDF_SURFACE

#include <cairo-pdf.h>

static void
to_pdf(AdgCanvas *canvas, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    surface = cairo_pdf_surface_create("test.pdf", 841, 595);
    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_show_page(cr);
    cairo_destroy(cr);

    file_generated(caller, "test.pdf");
}

#else

static void
to_pdf(AdgCanvas *canvas, GtkWidget *caller)
{
    missing_feature(caller, "PDF");
}

#endif

#ifdef CAIRO_HAS_PS_SURFACE

#include <cairo-ps.h>

static void
to_ps(AdgCanvas *canvas, GtkWidget *caller)
{
    cairo_surface_t *surface;
    cairo_t *cr;

    /* Surface creation: A4 size */
    surface = cairo_ps_surface_create("test.ps", 841, 595);
    cairo_ps_surface_dsc_comment(surface,
                                 "%%Title: Automatic Drawing Generation (Adg) demo");
    cairo_ps_surface_dsc_comment(surface,
                                 "%%Copyright: Copyright (C) 2006 Fontana Nicola");
    cairo_ps_surface_dsc_comment(surface, "%%Orientation: Portrait");

    cairo_ps_surface_dsc_begin_setup(surface);

    cairo_ps_surface_dsc_begin_page_setup(surface);
    cairo_ps_surface_dsc_comment(surface,
                                 "%%IncludeFeature: *PageSize A4");

    cr = cairo_create(surface);
    cairo_surface_destroy(surface);

    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_show_page(cr);
    cairo_destroy(cr);

    file_generated(caller, "test.ps");
}

#else

static void
to_ps(AdgCanvas *canvas, GtkWidget *caller)
{
    missing_feature(caller, "PostScript");
}

#endif
