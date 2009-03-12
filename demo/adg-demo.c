#include <adg/adg.h>
#include <math.h>

#include "demo.h"


#ifndef G_SQRT3
#define G_SQRT3 1.732050808
#endif

#define CHAMFER 0.3


/* Dummy model */
typedef struct _Piston Piston;

struct _Piston {
    gdouble A, B, C;
    gdouble D1, D2, D3, D4, D5, D6, D7;
    gdouble RD34, RD56;
    gdouble LD2, LD3, LD5, LD6, LD7;
};


static void     ldim_path               (AdgEntity      *entity,
                                         cairo_t        *cr,
                                         gpointer        user_data);
static void     ldim_expose             (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         AdgCanvas      *canvas);

static void     drawing_fill_model      (Piston         *piston);
static void     drawing_add_dimensions  (AdgCanvas      *canvas,
                                         Piston         *piston);
static void     drawing_add_stuff       (AdgCanvas      *canvas);
static void     drawing_path            (AdgEntity      *entity,
                                         cairo_t        *cr,
                                         gpointer        user_data);
static void     drawing_expose          (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         AdgCanvas      *canvas);

static void     to_pdf                  (AdgCanvas      *canvas,
                                         GtkWidget      *caller);
static void     to_png                  (AdgCanvas      *canvas,
                                         GtkWidget      *caller);
static void     to_ps                   (AdgCanvas      *canvas,
                                         GtkWidget      *caller);
static void     missing_feature         (GtkWidget      *caller,
                                         const gchar    *feature);
static void     file_generated          (GtkWidget      *caller,
                                         const gchar    *file);

static Piston   model;


int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *window;
    AdgCanvas *ldim_canvas, *drawing_canvas;
    AdgEntity *entity;

    gtk_init(&argc, &argv);


    /* LDim demo */
    ldim_canvas = adg_canvas_new();

    entity = adg_path_new(ldim_path, NULL);
    adg_container_add(ADG_CONTAINER(ldim_canvas), entity);

    entity = adg_ldim_new_full_explicit(2, -10, 8, -10, ADG_DIR_UP, 0, -10);
    adg_container_add(ADG_CONTAINER(ldim_canvas), entity);

    entity = adg_ldim_new_full_explicit(0, -5, 10, -5, ADG_DIR_UP, 0, -10);
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(ldim_canvas), entity);


    /* Drawing demo */
    drawing_canvas = adg_canvas_new();

    drawing_fill_model(&model);

    entity = adg_path_new(drawing_path, NULL);
    adg_container_add(ADG_CONTAINER(drawing_canvas), entity);

    drawing_add_dimensions(drawing_canvas, &model);
    drawing_add_stuff(drawing_canvas);


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
                     "expose-event", G_CALLBACK(ldim_expose), ldim_canvas);

    g_signal_connect(gtk_builder_get_object(builder, "areaDrawing"),
                     "expose-event", G_CALLBACK(drawing_expose), drawing_canvas);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPng"),
                             "clicked", G_CALLBACK(to_png), drawing_canvas);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPdf"),
                             "clicked", G_CALLBACK(to_pdf), drawing_canvas);
    g_signal_connect_swapped(gtk_builder_get_object(builder, "btnPs"),
                             "clicked", G_CALLBACK(to_ps), drawing_canvas);

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


static void
ldim_path(AdgEntity *entity, cairo_t *cr, gpointer user_data)
{
    cairo_move_to(cr,  0,   0);
    cairo_line_to(cr,  0,  -5);
    cairo_line_to(cr,  2,  -5);
    cairo_line_to(cr,  2, -10);
    cairo_line_to(cr,  8, -10);
    cairo_line_to(cr,  8,  -5);
    cairo_line_to(cr, 10., -5);
    cairo_line_to(cr, 10.,  0);
    cairo_close_path(cr);
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


static void
drawing_fill_model(Piston *piston)
{
    piston->A = 52.3;
    piston->B = 20.6;
    piston->C = 2;
    piston->D1 = 9.3;
    piston->D2 = 6.5;
    piston->D3 = 11.9;
    piston->D4 = 6.5;
    piston->D5 = 4.5;
    piston->D6 = 7.2;
    piston->D7 = 3.;
    piston->RD34 = 1.;
    piston->LD2 = 7.;
    piston->LD3 = 3.5;
    piston->LD5 = 5.;
    piston->LD6 = 1.;
    piston->LD7 = 0.5;
}

static void
drawing_add_dimensions(AdgCanvas *canvas, Piston *piston)
{
    double A, B, C;
    double D1, D2, D3, D4, D5, D6, D7;
    double LD2, LD3, LD5, LD6, LD7;
    double RD34, RD56;
    AdgEntity *entity;
    double x, y;

    A = piston->A;
    B = piston->B;
    C = piston->C;
    D1 = piston->D1;
    D2 = piston->D2;
    D3 = piston->D3;
    D4 = piston->D4;
    D5 = piston->D5;
    D6 = piston->D6;
    D7 = piston->D7;
    LD2 = piston->LD2;
    LD3 = piston->LD3;
    LD5 = piston->LD5;
    LD6 = piston->LD6;
    LD7 = piston->LD7;
    RD34 = piston->RD34;
    RD56 = piston->RD56;


    /* North */

    /* LD2 */
    entity = adg_ldim_new_full_explicit(A - B - LD2, -D1 / 2, A - B,
                                        -D3 / 2 + CHAMFER, ADG_DIR_UP, 0,
                                        -D3 / 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* LD3 */
    entity = adg_ldim_new_full_explicit(A - B, -D3 / 2 + CHAMFER, A - B + LD3,
                                        -D3 / 2 + CHAMFER, ADG_DIR_UP, 0,
                                        -D3 / 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* South */

    /* B */
    entity = adg_ldim_new_full_explicit(A - B, D3 / 2 - CHAMFER, A, D7 / 2,
                                        ADG_DIR_DOWN, 0, D3 / 2);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* A */
    entity = adg_ldim_new_full_explicit(0, D1 / 2, A, D7 / 2,
                                        ADG_DIR_DOWN, 0, D3 / 2);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* East */

    /* D3 */
    x = A - B + LD3 - CHAMFER;
    entity = adg_ldim_new_full_explicit(x, -D3 / 2, x, D3 / 2,
                                        ADG_DIR_RIGHT, A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(entity), 5);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D6 */
    x = A - C + LD6;
    entity = adg_ldim_new_full_explicit(x, -D6 / 2, x, D6 / 2,
                                        ADG_DIR_RIGHT, A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 4);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D4 */
    x = A - C - LD5;
    entity = adg_ldim_new_full_explicit(x, -D4 / 2, x, D4 / 2,
                                        ADG_DIR_RIGHT, A, 0);
    adg_dim_set_level(ADG_DIM(entity), 3);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D5 */
    x = A - C;
    entity = adg_ldim_new_full_explicit(x, -D5 / 2, x, D5 / 2,
                                        ADG_DIR_RIGHT, A, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D7 */
    entity = adg_ldim_new_full_explicit(A, -D7 / 2, A, D7 / 2,
                                        ADG_DIR_RIGHT, A, 0);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* West */

    /* D1 */
    entity = adg_ldim_new_full_explicit(0, -D1 / 2, 0, D1 / 2,
                                        ADG_DIR_LEFT, 0, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(entity), 2);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D2 */
    y = (D1 - D2) / 2;
    x = A - B - LD2 + y * G_SQRT3;
    entity = adg_ldim_new_full_explicit(x, -D2 / 2, x, D2 / 2,
                                        ADG_DIR_LEFT, 0, 0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);
}

static void
drawing_add_stuff(AdgCanvas *canvas)
{
    AdgEntity *toy_text;

    toy_text = adg_toy_text_new("Horizontal toy_text above the piston");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., -4.65,
                                         0., -5.);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);

    toy_text = adg_toy_text_new("toy_text");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., 4.65,
                                         0., -5.);
    adg_rotable_set_angle(ADG_ROTABLE(toy_text), M_PI * 3./2.);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);
}

static void
drawing_path(AdgEntity *entity, cairo_t *cr, gpointer user_data)
{
    Piston *piston;
    double A, B, C;
    double D1, D2, D3, D4, D5, D6, D7;
    double LD2, LD3, LD5, LD6, LD7;
    double RD34, RD56;
    double x, y;
    cairo_path_t *path;
    CpmlSegment segment;
    AdgMatrix matrix;

    piston = &model;

    A = piston->A;
    B = piston->B;
    C = piston->C;
    D1 = piston->D1;
    D2 = piston->D2;
    D3 = piston->D3;
    D4 = piston->D4;
    D5 = piston->D5;
    D6 = piston->D6;
    D7 = piston->D7;
    LD2 = piston->LD2;
    LD3 = piston->LD3;
    LD5 = piston->LD5;
    LD6 = piston->LD6;
    LD7 = piston->LD7;
    RD34 = piston->RD34;
    RD56 = piston->RD56;

    cairo_move_to(cr, 0, D1 / 2);
    cairo_line_to(cr, A - B - LD2, D1 / 2);
    y = (D1 - D2) / 2;
    cairo_line_to(cr, A - B - LD2 + y * G_SQRT3, D1 / 2 - y);
    cairo_line_to(cr, A - B, D2 / 2);
    cairo_line_to(cr, A - B, D3 / 2 - CHAMFER);
    cairo_line_to(cr, A - B + CHAMFER, D3 / 2);
    cairo_line_to(cr, A - B + LD3 - CHAMFER, D3 / 2);
    cairo_line_to(cr, A - B + LD3, D3 / 2 - CHAMFER);
    x = A - B + LD3 + RD34;
    y = D4 / 2 + RD34;
    cairo_arc(cr, x, y, RD34, G_PI, 3 * G_PI_2);
    cairo_line_to(cr, A - C - LD5, D4 / 2);
    y = (D4 - D5) / 2;
    cairo_line_to(cr, A - C - LD5 + y, D4 / 2 - y);
    cairo_line_to(cr, A - C, D5 / 2);
    cairo_line_to(cr, A - C, D6 / 2);
    cairo_line_to(cr, A - C + LD6, D6 / 2);
    x = C - LD7 - LD6;
    y = x / G_SQRT3;
    cairo_line_to(cr, A - C + LD6 + x, D6 / 2 - y);
    cairo_line_to(cr, A - LD7, D7 / 2);
    cairo_line_to(cr, A, D7 / 2);

    /* Build the shape by reflecting the current path, reversing the order
     * and joining the result to the current path */
    path = cairo_copy_path(cr);
    cpml_segment_from_cairo(&segment, path);
    cpml_segment_reverse(&segment);
    adg_matrix_init_reflection(&matrix, 0);
    cpml_segment_transform(&segment, &matrix);
    path->data[0].header.type = CAIRO_PATH_LINE_TO;
    cairo_append_path(cr, path);
    cairo_path_destroy(path);
    cairo_close_path(cr);
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
