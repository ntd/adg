#include <cpml/cpml.h>
#include <gtk/gtk.h>


static cairo_path_t *
                duplicate_and_stroke    (cairo_t        *cr);
static void     stroke_and_destroy      (cairo_t        *cr,
                                         cairo_path_t   *path);

static void     offset_curves           (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         gpointer        data);
static void     offset_segments         (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         gpointer        data);

static void     append_circle           (cairo_t        *cr);
static void     append_piston           (cairo_t        *cr);

static CpmlPair bezier_samples[][4] = {
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
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *widget;
    GtkWidget *button_box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_vbox_new(FALSE, 0);

    notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_LEFT);
    gtk_container_add(GTK_CONTAINER(vbox), notebook);

    label = gtk_label_new("Offset curves");
    widget = gtk_drawing_area_new();
    gtk_widget_set_size_request(widget, 800, 800);
    g_signal_connect(widget, "expose-event", G_CALLBACK(offset_curves), NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget, label);

    label = gtk_label_new("Offset segments");
    widget = gtk_drawing_area_new();
    gtk_widget_set_size_request(widget, 800, 800);
    g_signal_connect(widget, "expose-event", G_CALLBACK(offset_segments), NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), widget, label);

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


static cairo_path_t *
duplicate_and_stroke(cairo_t *cr)
{
    cairo_path_t *path = cairo_copy_path(cr);

    cairo_set_line_width(cr, 2.);
    cairo_stroke(cr);

    return path;
}

static void
stroke_and_destroy(cairo_t *cr, cairo_path_t *path)
{
    cairo_append_path(cr, path);
    cairo_path_destroy(path);

    cairo_set_line_width(cr, 1.);
    cairo_stroke(cr);
}


static void
offset_curves(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    cairo_t *cr;
    gint n;
    CpmlPair *bezier;
    cairo_path_t *path, *path_copy;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;
    CpmlVector vector;
    double t;

    cr = gdk_cairo_create(widget->window);

    /* Add the Bézier curve samples */
    for (n = 0; n < G_N_ELEMENTS(bezier_samples); ++n) {
        bezier = &bezier_samples[n][0];

        /* The samples are arranged in a 4x? matrix of 200x150 cells */
        if (n == 0)
            cairo_translate(cr, 25., 25.);
        else if (n % 4 == 0)
            cairo_translate(cr, -600., 150.);
        else
            cairo_translate(cr, 200., 0.);

        /* Draw the Bézier curve */
        cairo_move_to(cr, bezier[0].x, bezier[0].y);
        cairo_curve_to(cr, bezier[1].x, bezier[1].y,
                       bezier[2].x, bezier[2].y, bezier[3].x, bezier[3].y);

        /* Create a copy, to be used after */
        path_copy = cairo_copy_path(cr);

        path = duplicate_and_stroke(cr);
        cpml_segment_from_cairo(&segment, path);
        cpml_segment_offset(&segment, 20.);
        stroke_and_destroy(cr, path);

        cpml_segment_from_cairo(&segment, path_copy);
        cpml_primitive_from_segment(&primitive, &segment);

        /* Checking cpml_curve_pair_at_time and cpml_curve_vector_at_time */
        cairo_set_line_width(cr, 1.);
        for (t = 0; t < 1; t += 0.1) {
            cpml_curve_pair_at_time(&primitive, &pair, t);
            cpml_curve_vector_at_time(&primitive, &vector, t);
            cpml_vector_set_length(&vector, 20.);
            cpml_vector_normal(&vector);

            cairo_new_sub_path(cr);
            cairo_arc(cr, pair.x, pair.y, 2.5, 0, M_PI*2);
            cairo_fill(cr);

            cairo_move_to(cr, pair.x, pair.y);
            cairo_line_to(cr, pair.x + vector.x, pair.y + vector.y);
            cairo_stroke(cr);
        }

        cairo_path_destroy(path_copy);
    }

    cairo_destroy(cr);
}

static void
offset_segments(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    cairo_t *cr;
    cairo_path_t *path;
    CpmlSegment segment;

    cr = gdk_cairo_create(widget->window);

    /* Offset a circle approximation */
    append_circle(cr);

    path = duplicate_and_stroke(cr);
    cpml_segment_from_cairo(&segment, path);
    cpml_segment_offset(&segment, 20.);
    stroke_and_destroy(cr, path);

    /* Offset a more complex path */
    cairo_save(cr);
    cairo_translate(cr, 240.5, 120.5);
    cairo_scale(cr, 10., 10.);
    append_piston(cr);
    cairo_restore(cr);

    path = duplicate_and_stroke(cr);
    cpml_segment_from_cairo(&segment, path);
    cpml_segment_offset(&segment, 10.);
    stroke_and_destroy(cr, path);

    cairo_destroy(cr);
}


static void
append_circle(cairo_t *cr)
{
    cairo_new_sub_path(cr);
    cairo_arc(cr, 120.5, 120.5, 100, 0, M_PI*2);
}

static void
append_piston(cairo_t *cr)
{
    cairo_path_t *path;
    cairo_matrix_t matrix;
    CpmlSegment segment;

    cairo_move_to(cr,  0.,    4.65);
    cairo_line_to(cr, 26.,    4.65);
    cairo_line_to(cr, 27.25,  3.5);
    cairo_line_to(cr, 32,     3.5);
    cairo_line_to(cr, 32,     5.6);
    cairo_line_to(cr, 32.3,   5.9);
    cairo_line_to(cr, 35.2,   5.9);
    cairo_line_to(cr, 35.5,   5.6);
    cairo_arc(cr,     37.5,   5.25,  2., G_PI, 3. * G_PI_2);
    cairo_line_to(cr, 45.,    3.25);
    cairo_line_to(cr, 46.,    2.25);
    cairo_line_to(cr, 50.,    2.25);
    cairo_arc_negative(cr,
                      50.2,   3.4,   0.2, G_PI, G_PI_2);
    cairo_line_to(cr, 51.,    3.6);
    cairo_line_to(cr, 52.,    3.);
    cairo_line_to(cr, 52.2,   1.25);

    /* Mirror a reversed copy of the current path on the y = 0 axis */
    path = cairo_copy_path(cr);
    cpml_segment_from_cairo(&segment, path);

    cpml_segment_dump(&segment);

    cpml_segment_reverse(&segment);
    cairo_matrix_init_scale(&matrix, 1., -1.);
    cpml_segment_transform(&segment, &matrix);

    /* Join the mirrored path to the old path... */
    path->data[0].header.type = CAIRO_PATH_LINE_TO;
    cairo_append_path(cr, path);
    cairo_path_destroy(path);

    /* ...and close the shape */
    cairo_close_path(cr);
}
