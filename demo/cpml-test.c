#include <adg/adg.h>
#include <gtk/gtk.h>


static void     path_constructor        (AdgEntity      *entity,
                                         cairo_t        *cr,
                                         gpointer        user_data);
static void     path_expose             (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         AdgCanvas      *canvas);


typedef struct {
    AdgPair p1;
    AdgPair p2;
    AdgPair p3;
    AdgPair p4;
} Bezier;

static Bezier bezier_samples[] = {
    { { 0, 0 }, { 0, 40 }, { 120, 40 }, { 120, 0 } },   /* Simmetric low */
    { { 40, 0 }, { 40, 160 }, { 80, 160 }, { 80, 0 } }, /* Simmetric high */
    { { 0, 0 }, { 33.1371, 33.1371 }, { 86.8629, 33.1371 }, { 120, 0 } },
                                                        /* Arc approximation */
    { { 0, 0 }, { 70, 120 }, { 50, 120 }, { 120, 0 } }, /* Twisted controls */

    { { 0, 0 }, { 0, 120 }, { 60, 120 }, { 120, 0 } },  /* Vertical p1-p2 */
    { { 0, 0 }, { 60, 120 }, { 120, 120 }, { 120, 0 } },/* Vertical p3-p4 */
    { { 0, 120 }, { 120, 120 }, { 120, 60 }, { 0, 0 } },/* Horizontal p1-p2 */
    { { 0, 120 }, { 120, 60 }, { 120, 0 }, { 0, 0 } },  /* Horizontal p3-p4 */

    { { 0, 0 }, { 0, 120 }, { 120, 120 }, { 120, 0 } }, /* Down */
    { { 0, 120 }, { 120, 120 }, { 120, 0 }, { 0, 0 } }, /* Right */
    { { 0, 120 }, { 0, 0 }, { 120, 0 }, { 120, 120 } }, /* Up */
    { { 120, 120 }, { 0, 120 }, { 0, 0 }, { 120, 0 } }, /* Left */

    { { 0, 60 }, { 60, 120 }, { 120, 60 }, { 60, 0 } }, /* Down-right */
    { { 60, 120 }, { 120, 60 }, { 60, 0 }, { 0, 60 } }, /* Up-right */
    { { 120, 60 }, { 60, 0 }, { 0, 60 }, { 60, 120 } }, /* Up-left */
    { { 60, 0 }, { 0, 60 }, { 60, 120 }, { 120, 60 } }, /* Down-left*/

    { { 0, 0 }, { 60, 0 }, { 60, 120 }, { 120, 120 } }, /* Step left */
    { { 120, 0 }, { 60, 0 }, { 60, 120 }, { 0, 120 } }, /* Step right */
    { { 0, 0 }, { 60, 90 }, { 90, 120 }, { 120, 90 } }, /* Unbalanced opened */
    { { 0, 0 }, { 40, 120 }, { 120, 120 }, { 60, 80 } } /* Unbalanced closed */
};


int
main(gint argc, gchar **argv)
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button_box;
    GtkWidget *widget;
    AdgCanvas *canvas;
    AdgEntity *entity;
    gint n;

    gtk_init(&argc, &argv);

    /* Create the canvas and populate it */
    canvas = adg_canvas_new();

    /* Add the Bézier curve samples */
    for (n = 0; n < G_N_ELEMENTS(bezier_samples); ++n) {
        entity = adg_path_new(path_constructor, GINT_TO_POINTER(n));
        adg_container_add(ADG_CONTAINER(canvas), entity);
    }

    /* User interface stuff */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_vbox_new(FALSE, 0);

    widget = gtk_drawing_area_new();
    gtk_widget_set_size_request(widget, 800, 800);
    g_signal_connect(widget, "expose-event", G_CALLBACK(path_expose), canvas);
    gtk_container_add(GTK_CONTAINER(vbox), widget);

    button_box = gtk_hbutton_box_new();
    gtk_container_set_border_width(GTK_CONTAINER(button_box), 5);
    gtk_box_set_spacing(GTK_BOX(button_box), 5);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_END);
    gtk_box_pack_end(GTK_BOX(vbox), button_box, FALSE, TRUE, 0);

    widget = gtk_button_new_from_stock(GTK_STOCK_QUIT);
    g_signal_connect(widget, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(button_box), widget);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}


static void
path_constructor(AdgEntity *entity, cairo_t *cr, gpointer user_data)
{
    gint n;
    Bezier *bezier;
    cairo_path_t *path;
    CpmlSegment segment;
    CpmlPair pair;
    CpmlVector vector;
    double t;

    n = GPOINTER_TO_INT(user_data);
    bezier = bezier_samples + n;

    /* The samples are arranged in a 4x? matrix of 200x150 cells */
    if (n == 0)
        cairo_translate(cr, 25., 25.);
    else if (n % 4 == 0)
        cairo_translate(cr, -600., 150.);
    else
        cairo_translate(cr, 200., 0.);

    /* Draw the Bézier curve */
    cairo_move_to(cr, bezier->p1.x, bezier->p1.y);
    cairo_curve_to(cr, bezier->p2.x, bezier->p2.y,
                   bezier->p3.x, bezier->p3.y, bezier->p4.x, bezier->p4.y);

    /* Checking cpml_segment_offset */
    path = cairo_copy_path(cr);
    cpml_segment_init(&segment, path);
    cpml_segment_offset(&segment, 20);
    cairo_append_path(cr, path);
    cairo_path_destroy(path);

    /* Checking cpml_pair_at_curve and cpml_vector_at_curve */
    for (t = 0; t < 1; t += 0.1) {
        cpml_pair_at_curve(&pair, &bezier->p1, &bezier->p2,
                           &bezier->p3, &bezier->p4, t);
        cpml_vector_at_curve(&vector, &bezier->p1, &bezier->p2,
                             &bezier->p3, &bezier->p4, t, 20);
        cpml_vector_normal(&vector);

        cairo_new_sub_path(cr);
        cairo_arc(cr, pair.x, pair.y, 2.5, 0, M_PI*2);
        cairo_move_to(cr, pair.x, pair.y);
        cairo_line_to(cr, pair.x + vector.x, pair.y + vector.y);
    }
}

static void
path_expose(GtkWidget *widget, GdkEventExpose *event, AdgCanvas *canvas)
{
    cairo_t *cr = gdk_cairo_create(widget->window);

    adg_entity_render(ADG_ENTITY(canvas), cr);

    cairo_destroy(cr);
}
