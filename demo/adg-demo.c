#include <adg/adg.h>
#include <gtk/gtk.h>
#include <math.h>


#ifndef G_SQRT3
#define G_SQRT3 1.732050808
#endif

#define CHAMFER 0.3

typedef struct _Piston Piston;

struct _Piston {
    gdouble A, B, C;
    gdouble D1, D2, D3, D4, D5, D6, D7;
    gdouble RD34, RD56;
    gdouble LD2, LD3, LD5, LD6, LD7;
};


static void     fill_piston_model       (Piston         *piston);
static void     add_piston_dimensions   (AdgCanvas      *canvas,
                                         Piston         *piston);
static void     add_sample_stuff        (AdgCanvas      *canvas);
static void     piston_path_extern      (AdgEntity      *entity,
                                         cairo_t        *cr);
static void     piston_expose           (GtkWidget      *widget,
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
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_box;
    GtkWidget *widget;
    AdgCanvas *canvas;
    AdgEntity *path;

    gtk_init(&argc, &argv);

    /* Fill the model structure with some data */
    fill_piston_model(&model);

    /* Create the canvas and populate it */
    canvas = adg_canvas_new();

    path = adg_path_new(piston_path_extern);
    adg_container_add(ADG_CONTAINER(canvas), path);

    add_piston_dimensions(canvas, &model);
    add_sample_stuff(canvas);

    /* User interface stuff */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_vbox_new(FALSE, 0);

    widget = gtk_drawing_area_new();
    gtk_widget_set_size_request(widget, 790, 240);
    g_signal_connect(widget, "expose-event", G_CALLBACK(piston_expose),
                     canvas);
    gtk_container_add(GTK_CONTAINER(vbox), widget);

    button_box = gtk_hbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(button_box), 5);
    gtk_box_set_spacing(GTK_BOX(button_box), 5);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
    gtk_box_pack_end(GTK_BOX(vbox), button_box, FALSE, TRUE, 0);

    widget = gtk_button_new_with_mnemonic("P_NG image");
    g_signal_connect_swapped(widget, "clicked", G_CALLBACK(to_png), canvas);
    gtk_container_add(GTK_CONTAINER(button_box), widget);

    widget = gtk_button_new_with_mnemonic("P_DF file");
    g_signal_connect_swapped(widget, "clicked", G_CALLBACK(to_pdf), canvas);
    gtk_container_add(GTK_CONTAINER(button_box), widget);

    widget = gtk_button_new_with_mnemonic("_PostScript");
    g_signal_connect_swapped(widget, "clicked", G_CALLBACK(to_ps), canvas);
    gtk_container_add(GTK_CONTAINER(button_box), widget);

    widget = gtk_button_new_from_stock(GTK_STOCK_QUIT);
    g_signal_connect(widget, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), widget);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);

    gtk_main();

    adg_path_dump(ADG_PATH(path));

    return 0;
}


static void
fill_piston_model(Piston *piston)
{
    piston->A = 62.3;
    piston->B = 20.6;
    piston->C = 2;
    piston->D1 = 9.3;
    piston->D2 = 6.5;
    piston->D3 = 11.9;
    piston->D4 = 6.5;
    piston->D5 = 4.5;
    piston->D6 = 7.2;
    piston->D7 = 3.0;
    piston->RD34 = 1.0;
    piston->LD2 = 7.0;
    piston->LD3 = 3.5;
    piston->LD5 = 5.0;
    piston->LD6 = 1.0;
    piston->LD7 = 0.5;
}

static void
add_piston_dimensions(AdgCanvas *canvas, Piston *piston)
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
    entity =
        adg_ldim_new_full_explicit(A - B - LD2, -D1 / 2.0, A - B,
                                   -D3 / 2.0 + CHAMFER, CPML_DIR_UP, 0.0,
                                   -D3 / 2.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* LD3 */
    entity =
        adg_ldim_new_full_explicit(A - B, -D3 / 2.0 + CHAMFER, A - B + LD3,
                                   -D3 / 2.0 + CHAMFER, CPML_DIR_UP, 0.0,
                                   -D3 / 2.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* South */

    /* B */
    entity =
        adg_ldim_new_full_explicit(A - B, D3 / 2.0 - CHAMFER, A, D7 / 2.0,
                                   CPML_DIR_DOWN, 0.0, D3 / 2.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* A */
    entity =
        adg_ldim_new_full_explicit(0.0, D1 / 2.0, A, D7 / 2.0,
                                   CPML_DIR_DOWN, 0.0, D3 / 2.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(entity), 2.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* East */

    /* D3 */
    x = A - B + LD3 - CHAMFER;
    entity =
        adg_ldim_new_full_explicit(x, -D3 / 2.0, x, D3 / 2.0,
                                   CPML_DIR_RIGHT, A, 0.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.25", NULL);
    adg_dim_set_level(ADG_DIM(entity), 5.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D6 */
    x = A - C + LD6;
    entity =
        adg_ldim_new_full_explicit(x, -D6 / 2.0, x, D6 / 2.0,
                                   CPML_DIR_RIGHT, A, 0.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 4.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D4 */
    x = A - C - LD5;
    entity =
        adg_ldim_new_full_explicit(x, -D4 / 2.0, x, D4 / 2.0,
                                   CPML_DIR_RIGHT, A, 0.0);
    adg_dim_set_level(ADG_DIM(entity), 3.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D5 */
    x = A - C;
    entity =
        adg_ldim_new_full_explicit(x, -D5 / 2.0, x, D5 / 2.0,
                                   CPML_DIR_RIGHT, A, 0.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_dim_set_level(ADG_DIM(entity), 2.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D7 */
    entity =
        adg_ldim_new_full_explicit(A, -D7 / 2.0, A, D7 / 2.0,
                                   CPML_DIR_RIGHT, A, 0.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);


    /* West */

    /* D1 */
    entity =
        adg_ldim_new_full_explicit(0.0, -D1 / 2.0, 0.0, D1 / 2.0,
                                   CPML_DIR_LEFT, 0.0, 0.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "+0.05", "-0.05");
    adg_dim_set_level(ADG_DIM(entity), 2.0);
    adg_container_add(ADG_CONTAINER(canvas), entity);

    /* D2 */
    y = (D1 - D2) / 2.0;
    x = A - B - LD2 + y * G_SQRT3;
    entity =
        adg_ldim_new_full_explicit(x, -D2 / 2.0, x, D2 / 2.0,
                                   CPML_DIR_LEFT, 0.0, 0.0);
    adg_dim_set_tolerances(ADG_DIM(entity), "-0.1", NULL);
    adg_container_add(ADG_CONTAINER(canvas), entity);
}

static void
add_sample_stuff(AdgCanvas *canvas)
{
    AdgEntity *toy_text;

    toy_text = adg_toy_text_new("Horizontal toy_text above the piston");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., -4.65,
                                         0., -5.);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);

    toy_text = adg_toy_text_new("Rotated toy_text");
    adg_translatable_set_origin_explicit(ADG_TRANSLATABLE(toy_text),
                                         0., 4.65,
                                         0., -5.);
    adg_rotable_set_angle(ADG_ROTABLE(toy_text), M_PI * 3./2.);
    adg_container_add(ADG_CONTAINER(canvas), toy_text);
}

static void
piston_path_extern(AdgEntity *entity, cairo_t *cr)
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

    cairo_move_to(cr, 0, D1 / 2.0);
    cairo_line_to(cr, A - B - LD2, D1 / 2.0);
    y = (D1 - D2) / 2.0;
    cairo_line_to(cr, A - B - LD2 + y * G_SQRT3, D1 / 2.0 - y);
    cairo_line_to(cr, A - B, D2 / 2.0);
    cairo_line_to(cr, A - B, D3 / 2.0 - CHAMFER);
    cairo_line_to(cr, A - B + CHAMFER, D3 / 2.0);
    cairo_line_to(cr, A - B + LD3 - CHAMFER, D3 / 2.0);
    cairo_line_to(cr, A - B + LD3, D3 / 2.0 - CHAMFER);
    x = A - B + LD3 + RD34;
    y = D4 / 2.0 + RD34;
    cairo_arc(cr, x, y, RD34, G_PI, 3.0 * G_PI_2);
    cairo_line_to(cr, A - C - LD5, D4 / 2.0);
    y = (D4 - D5) / 2.0;
    cairo_line_to(cr, A - C - LD5 + y, D4 / 2.0 - y);
    cairo_line_to(cr, A - C, D5 / 2.0);
    cairo_line_to(cr, A - C, D6 / 2.0);
    cairo_line_to(cr, A - C + LD6, D6 / 2.0);
    x = C - LD7 - LD6;
    y = x / G_SQRT3;
    cairo_line_to(cr, A - C + LD6 + x, D6 / 2.0 - y);
    cairo_line_to(cr, A - LD7, D7 / 2.0);
    cairo_line_to(cr, A, D7 / 2.0);

    /* Build the shape by reflecting the current path, reversing the order
     * and joining the result to the current path */
    path = cairo_copy_path(cr);
    cpml_segment_init(&segment, path);
    cpml_segment_reverse(&segment);
    adg_matrix_init_reflection(&matrix, 0);
    cpml_segment_transform(&segment, &matrix);
    path->data[0].header.type = CAIRO_PATH_LINE_TO;
    cairo_append_path(cr, path);
    cairo_path_destroy(path);
    cairo_close_path(cr);
}

static void
piston_expose(GtkWidget *widget, GdkEventExpose *event, AdgCanvas *canvas)
{
    cairo_t *cr;
    gint width, height;
    double scale;
    AdgMatrix matrix;

    cr = gdk_cairo_create(widget->window);
    width = widget->allocation.width;
    height = widget->allocation.height;
    scale = (double) (width - 80.0) / 80.0;

    cairo_matrix_init(&matrix, scale, 0.0, 0.0, scale, scale + 71.0,
                      12.0 * scale);
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
