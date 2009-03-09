#include <config.h>
#include <cpml/cpml.h>
#include <gtk/gtk.h>

static gchar *  find_data_file          (const gchar    *file);

static cairo_path_t *
                duplicate_and_stroke    (cairo_t        *cr);
static void     stroke_and_destroy      (cairo_t        *cr,
                                         cairo_path_t   *path);

static void     browsing                (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         gpointer        data);
static void     offset_curves           (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         gpointer        data);
static void     offset_segments         (GtkWidget      *widget,
                                         GdkEventExpose *event,
                                         gpointer        data);

static void     circle_callback         (cairo_t        *cr);
static void     piston_callback         (cairo_t        *cr);
static void     curve1_callback         (cairo_t        *cr);

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

static void (*path_samples[]) (cairo_t *cr) = {
    circle_callback,
    piston_callback,
    curve1_callback
};


int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *window;

    gtk_init(&argc, &argv);

    path = find_data_file("cpml-demo.ui");
    if (path == NULL) {
        g_print("cpml-demo.ui not found!\n");
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
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaBrowsing"),
                     "expose-event", G_CALLBACK(browsing), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetCurves"),
                     "expose-event", G_CALLBACK(offset_curves), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetSegments"),
                     "expose-event", G_CALLBACK(offset_segments), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnQuit"),
                     "clicked", G_CALLBACK(gtk_main_quit), NULL);

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


static gchar *
find_data_file(const gchar *file)
{
    gchar *path;

    path = g_build_filename(SOURCEDIR, file, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    path = g_build_filename(PKGDATADIR, PACKAGE_NAME, file, NULL);
    if (g_file_test(path, G_FILE_TEST_EXISTS))
        return path;

    g_free(path);
    return NULL;
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
browsing(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    cairo_t *cr;
    cairo_path_t *path;
    CpmlSegment segment;
    int n;

    cr = gdk_cairo_create(widget->window);

    /* Append all the path samples to the cairo context */
    for (n = 0; n < G_N_ELEMENTS(path_samples); ++n) {
        (path_samples[n]) (cr);
    }

    cairo_destroy(cr);
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
    int n;

    cr = gdk_cairo_create(widget->window);
    cairo_translate(cr, 270.5, -120.5);

    /* Offset the path samples */
    for (n = 0; n < G_N_ELEMENTS(path_samples); ++n) {
        if ((n & 1) == 0) {
            cairo_translate(cr, -270., 240.);
        } else {
            cairo_translate(cr, 270., 0.);
        }

        /* Call the path callback */
        (path_samples[n]) (cr);

        path = duplicate_and_stroke(cr);
        cpml_segment_from_cairo(&segment, path);
        cpml_segment_offset(&segment, 15.);
        stroke_and_destroy(cr, path);
    }

    cairo_destroy(cr);
}


static void
circle_callback(cairo_t *cr)
{
    cairo_new_sub_path(cr);
    cairo_arc(cr, 120., 0., 100., 0., M_PI*2);
}

static void
piston_callback(cairo_t *cr)
{
    cairo_path_t *path;
    cairo_matrix_t matrix;
    CpmlSegment segment;

    cairo_move_to(cr,  0.,   46.5);
    cairo_line_to(cr, 210.,   46.5);
    cairo_line_to(cr, 222.5,  35.);
    cairo_line_to(cr, 270.,   35.);
    cairo_line_to(cr, 270.,   56.);
    cairo_line_to(cr, 273.,   59.);
    cairo_line_to(cr, 302.,   59.);
    cairo_line_to(cr, 305.,   56.);
    cairo_arc(cr,     325.,   52.5,  20., G_PI, 3. * G_PI_2);
    cairo_line_to(cr, 400.,   32.5);
    cairo_line_to(cr, 410.,   22.5);
    cairo_line_to(cr, 450.,   22.5);
    cairo_arc_negative(cr,
                      452.,   34.,    2., G_PI,      G_PI_2);
    cairo_line_to(cr, 460.,   36.);
    cairo_line_to(cr, 470.,   30.);
    cairo_line_to(cr, 472.,   12.5);

    /* Mirror a reversed copy of the current path on the y = 0 axis */
    path = cairo_copy_path(cr);
    cpml_segment_from_cairo(&segment, path);

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

static void
curve1_callback(cairo_t *cr)
{
    cairo_move_to(cr,    30.,   0.);
    cairo_curve_to(cr,  120., 120.,  180., 100.,  180.,  20.);
    cairo_curve_to(cr,  180., -20.,   50.,  40.,  150.,  40.);
    cairo_curve_to(cr,  220.,  40.,  190., -60.,  150., -60.);
    cairo_curve_to(cr,  100., -60.,   80., -40.,   60., -60.);
}
