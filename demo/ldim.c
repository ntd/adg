#include <adg/adg.h>
#include <gcontainer/gcontainer.h>
#include <gtk/gtk.h>


static void     draw_shape              (AdgEntity      *entity,
                                         cairo_t        *cr);
static void     drawing_expose          (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         AdgCanvas      *canvas);


int
main(gint argc, gchar **argv)
{
    GtkWidget *window;
    GtkWidget *widget;
    AdgCanvas *canvas;
    AdgEntity *entity;

    gtk_init(&argc, &argv);

    /* Create the canvas */
    canvas = adg_canvas_new();

    /* Add the shape to quote */
    entity = adg_path_new(draw_shape);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    /* Add the north quotes */
    entity = adg_ldim_new_full_explicit(2., -10., 8., -10., CPML_DIR_UP,
                                        0.0, -10.);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    entity = adg_ldim_new_full_explicit(0., -5., 10., -5., CPML_DIR_UP,
                                        0.0, -10.);
    adg_dim_set_level(ADG_DIM(entity), 2);
    g_containerable_add(G_CONTAINERABLE(canvas), G_CHILDABLE(entity));

    /* User interface stuff */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    widget = gtk_drawing_area_new();
    g_signal_connect(widget, "expose-event", G_CALLBACK(drawing_expose),
		     canvas);
    gtk_container_add(GTK_CONTAINER(window), widget);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

static void
draw_shape(AdgEntity *entity, cairo_t *cr)
{
    cairo_move_to(cr, 0., 0.);
    cairo_line_to(cr, 0., -5.);
    cairo_line_to(cr, 2., -5.);
    cairo_line_to(cr, 2., -10.);
    cairo_line_to(cr, 8., -10.);
    cairo_line_to(cr, 8., -5.);
    cairo_line_to(cr, 10., -5.);
    cairo_line_to(cr, 10., 0.);
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
    scale = (double) (width - 10.) / 10.;

    cairo_matrix_init(&matrix, scale, 0., 0., scale, 5.,
		      75. + 10. * scale);
    adg_container_set_model_transformation(ADG_CONTAINER(canvas), &matrix);

    /* Rendering process */
    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_destroy(cr);
}
