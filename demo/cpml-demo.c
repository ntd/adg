/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2017  Nicola Fontana <ntd at entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "demo.h"
#include <cpml.h>
#include <math.h>
#include <gtk/gtk.h>


static void     parse_args              (gint           *p_argc,
                                         gchar         **p_argv[]);
static cairo_path_t *
                duplicate_and_stroke    (cairo_t        *cr);
static void     stroke_and_destroy      (cairo_t        *cr,
                                         cairo_path_t   *path);

static void     browsing                (GtkWidget      *widget,
                                         cairo_t        *cr);

static void     browsing_segment        (GtkToggleButton*togglebutton,
                                         gpointer        user_data);
static void     browsing_primitive      (GtkToggleButton*togglebutton,
                                         gpointer        user_data);
static void     browsing_reset          (GtkButton      *button,
                                         gpointer        user_data);
static void     browsing_next           (GtkButton      *button,
                                         gpointer        user_data);

static gboolean arcs                    (GtkWidget      *widget,
                                         cairo_t        *cr);

static void     arc3p                   (cairo_t        *cr,
                                         double          x1,
                                         double          y1,
                                         double          x2,
                                         double          y2,
                                         double          x3,
                                         double          y3);

static gboolean intersections           (GtkWidget      *widget,
                                         cairo_t        *cr);

static void     algorithm_changed       (GtkRadioButton *button,
                                         GtkWidget      *area);
static gboolean offset_curves           (GtkWidget      *widget,
                                         cairo_t        *cr);

static gboolean offset_segments         (GtkWidget      *widget,
                                         cairo_t        *cr);

#ifdef GTK2_ENABLED

static void
browsing_gtk2(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    browsing(widget, cr);
    cairo_destroy(cr);
}

static void
arcs_gtk2(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    arcs(widget, cr);
    cairo_destroy(cr);
}

static void
intersections_gtk2(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    intersections(widget, cr);
    cairo_destroy(cr);
}

static void
offset_curves_gtk2(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    offset_curves(widget, cr);
    cairo_destroy(cr);
}

static void
offset_segments_gtk2(GtkWidget *widget)
{
    cairo_t *cr = gdk_cairo_create(widget->window);
    offset_segments(widget, cr);
    cairo_destroy(cr);
}

#endif

static void     circle_callback         (cairo_t        *cr);
static void     piston_callback         (cairo_t        *cr);
static void     curve1_callback         (cairo_t        *cr);
static void     line1_callback          (cairo_t        *cr);

static struct {
    GtkWidget           *area;
    cairo_path_t        *cairo_path;
    gboolean             use_segment;
    CpmlSegment          segment;
    CpmlPrimitive        primitive;
} browsing_data = {
    NULL,
};

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
    curve1_callback,
    line1_callback,
};

static void
switch_page(GtkTreeSelection *selection, GtkNotebook *notebook)
{
    GList *list = gtk_tree_selection_get_selected_rows(selection, NULL);
    if (list != NULL) {
        GtkTreePath *path = g_list_first(list)->data;
        gint page = path != NULL ? *gtk_tree_path_get_indices(path) : -1;
        g_list_free_full(list, (GDestroyNotify) gtk_tree_path_free);
        if (page >= 0)
            gtk_notebook_set_current_page(notebook, page);
    }
}

int
main(gint argc, gchar **argv)
{
    gchar *path;
    GtkBuilder *builder;
    GError *error;
    GtkWidget *window;
    gchar *icons_dir;

    _demo_init(argc, argv);
    parse_args(&argc, &argv);

    /* Prepend the package icons path */
    if (is_installed) {
#ifdef G_OS_WIN32
        icons_dir = g_build_filename(basedir, PKGDATADIR, "icons", NULL);
#else
        icons_dir = g_strdup(PKGDATADIR "/icons");
#endif
    } else {
        icons_dir = g_strdup(SRCDIR "/icons");
    }

    gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(), icons_dir);
    g_free(icons_dir);

    path = _demo_file("cpml-demo.ui");
    if (path == NULL) {
        g_printerr(_("cpml-demo.ui not found!\n"));
        return 1;
    }

    builder = gtk_builder_new();
    error = NULL;

    gtk_builder_set_translation_domain(builder, GETTEXT_PACKAGE);
    gtk_builder_add_from_file(builder, path, &error);
    g_free(path);

    if (error != NULL) {
        g_print("%s\n", error->message);
        return 2;
    }

    window = (GtkWidget *) gtk_builder_get_object(builder, "wndMain");

    /* Connect signals */
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(gtk_tree_view_get_selection(GTK_TREE_VIEW(gtk_builder_get_object(builder, "tvPages"))),
                     "changed", G_CALLBACK(switch_page),
                     gtk_builder_get_object(builder, "nbPages"));
    g_signal_connect(gtk_builder_get_object(builder, "optBrowsingSegment"),
                     "toggled", G_CALLBACK(browsing_segment), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "optBrowsingPrimitive"),
                     "toggled", G_CALLBACK(browsing_primitive), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnBrowsingReset"),
                     "clicked", G_CALLBACK(browsing_reset), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnBrowsingNext"),
                     "clicked", G_CALLBACK(browsing_next), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "optAlgorithmDefault"),
                     "toggled", G_CALLBACK(algorithm_changed),
                     gtk_builder_get_object(builder, "areaOffsetCurves"));
    g_signal_connect(gtk_builder_get_object(builder, "optAlgorithmBaioca"),
                     "toggled", G_CALLBACK(algorithm_changed),
                     gtk_builder_get_object(builder, "areaOffsetCurves"));
    g_signal_connect(gtk_builder_get_object(builder, "optAlgorithmHandcraft"),
                     "toggled", G_CALLBACK(algorithm_changed),
                     gtk_builder_get_object(builder, "areaOffsetCurves"));
    g_signal_connect(gtk_builder_get_object(builder, "optAlgorithmGeometrical"),
                     "toggled", G_CALLBACK(algorithm_changed),
                     gtk_builder_get_object(builder, "areaOffsetCurves"));
#ifdef GTK2_ENABLED
    g_signal_connect(gtk_builder_get_object(builder, "areaBrowsing"),
                     "expose-event", G_CALLBACK(browsing_gtk2), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaArcs"),
                     "expose-event", G_CALLBACK(arcs_gtk2), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaIntersections"),
                     "expose-event", G_CALLBACK(intersections_gtk2), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetCurves"),
                     "expose-event", G_CALLBACK(offset_curves_gtk2), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetSegments"),
                     "expose-event", G_CALLBACK(offset_segments_gtk2), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnQuit"),
                     "clicked", G_CALLBACK(gtk_main_quit), NULL);
#else
    g_signal_connect(gtk_builder_get_object(builder, "areaBrowsing"),
                     "draw", G_CALLBACK(browsing), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaArcs"),
                     "draw", G_CALLBACK(arcs), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaIntersections"),
                     "draw", G_CALLBACK(intersections), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetCurves"),
                     "draw", G_CALLBACK(offset_curves), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "areaOffsetSegments"),
                     "draw", G_CALLBACK(offset_segments), NULL);
    g_signal_connect(gtk_builder_get_object(builder, "btnQuit"),
                     "clicked", G_CALLBACK(gtk_main_quit), NULL);
#endif

    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}


/**********************************************
 * Command line options parser
 **********************************************/

static void
version(void)
{
    g_print("cpml-demo " PACKAGE_VERSION "\n");
    exit(0);
}

static void
parse_args(gint *p_argc, gchar **p_argv[])
{
    GError *error = NULL;
    GOptionEntry entries[] = {
        {"version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
         (gpointer) version, _("Display version information"), NULL},
        {NULL}
    };

    gtk_init_with_args(p_argc, p_argv, _("- CPML demonstration program"),
                       entries, GETTEXT_PACKAGE, &error);

    if (error != NULL) {
        gint error_code = error->code;

        g_printerr("%s\n", error->message);

        g_error_free(error);
        exit(error_code);
    }
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
browsing(GtkWidget *widget, cairo_t *cr)
{
    if (browsing_data.area == NULL) {
        int n;

        /* Initialize browsing_data */
        browsing_data.area = widget;
        browsing_data.use_segment = TRUE;

        /* Append all the path samples to the cairo context */
        cairo_save(cr);
        cairo_translate(cr, 270.5, -120.5);
        for (n = 0; n < G_N_ELEMENTS(path_samples); ++n) {
            if ((n & 1) == 0) {
                cairo_translate(cr, -270., 240.);
            } else {
                cairo_translate(cr, 270., 0.);
            }

            (path_samples[n]) (cr);
        }

        cairo_restore(cr);
        browsing_data.cairo_path = cairo_copy_path(cr);
        cpml_segment_from_cairo(&browsing_data.segment,
                                browsing_data.cairo_path);
        cpml_primitive_from_segment(&browsing_data.primitive,
                                    &browsing_data.segment);
    } else {
        cairo_append_path(cr, browsing_data.cairo_path);
    }

    cairo_set_line_width(cr, 2.);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 1., 0.4, 0.);
    cairo_set_line_width(cr, 5.);

    if (browsing_data.use_segment)
        cpml_segment_to_cairo(&browsing_data.segment, cr);
    else
        cpml_primitive_to_cairo(&browsing_data.primitive, cr);

    cairo_stroke(cr);
}

static void
browsing_segment(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (!gtk_toggle_button_get_active(togglebutton))
        return;

    browsing_data.use_segment = TRUE;
    gtk_widget_queue_draw(browsing_data.area);
}

static void
browsing_primitive(GtkToggleButton*togglebutton, gpointer user_data)
{
    if (!gtk_toggle_button_get_active(togglebutton))
        return;

    browsing_data.use_segment = FALSE;
    gtk_widget_queue_draw(browsing_data.area);
}

static void
browsing_reset(GtkButton *button, gpointer user_data)
{
    if (browsing_data.use_segment) {
        cpml_segment_reset(&browsing_data.segment);
        cpml_primitive_from_segment(&browsing_data.primitive,
                                    &browsing_data.segment);
    } else {
        cpml_primitive_reset(&browsing_data.primitive);
    }

    gtk_widget_queue_draw(browsing_data.area);
}

static void
browsing_next(GtkButton *button, gpointer user_data)
{
    if (browsing_data.use_segment) {
        cpml_segment_next(&browsing_data.segment);
        cpml_primitive_from_segment(&browsing_data.primitive,
                                    &browsing_data.segment);
    } else {
        cpml_primitive_next(&browsing_data.primitive);
    }

    gtk_widget_queue_draw(browsing_data.area);
}


static gboolean
arcs(GtkWidget *widget, cairo_t *cr)
{
    cairo_translate(cr, 100.5, 100.5);
    arc3p(cr, 0, 0, 0, 120, 120, 120);

    cairo_translate(cr, 200, 0.);
    arc3p(cr, 0, 0, 120, 0, 120, 120);

    cairo_translate(cr, 200, 0.);
    arc3p(cr, 60, 0, 0, 120, 120, 120);

    cairo_translate(cr, -400, 200);
    arc3p(cr, 0, 50, -2, 85, 120, 0);

    cairo_translate(cr, 200, 0);
    arc3p(cr, -2, 85, 0, 50, 120, 0);

    return FALSE;
}

static void
arc3p(cairo_t *cr, double x1, double y1,
      double x2, double y2, double x3, double y3)
{
    CpmlPrimitive arc;
    cairo_path_data_t p[4];
    CpmlPair center;
    double r, start, end;

    arc.segment = NULL;
    arc.org = &p[0];
    arc.data = &p[1];

    p[1].header.type = CPML_ARC;
    p[1].header.length = 3;

    p[0].point.x = x1;
    p[0].point.y = y1;
    p[2].point.x = x2;
    p[2].point.y = y2;
    p[3].point.x = x3;
    p[3].point.y = y3;

    cpml_primitive_to_cairo(&arc, cr);

    cairo_set_line_width(cr, 1.);
    cairo_stroke(cr);

    /* Add an arc generated by cairo, just for reference */
    if (!cpml_arc_info(&arc, &center, &r, &start, &end)) {
        g_warning(_("Unable to get arc info (%lf, %lf) (%lf, %lf) (%lf, %lf)\n"),
                  x1, y1, x2, y2, x3, y3);
        return;
    }

    if (start < end)
        cairo_arc(cr, center.x, center.y, r-5., start, end);
    else
        cairo_arc_negative(cr, center.x, center.y, r-5., start, end);

    /* Show the inscribed triangle */
    cairo_move_to(cr, x1, y1);
    cairo_line_to(cr, x2, y2);
    cairo_line_to(cr, x3, y3);

    cairo_set_line_width(cr, 0.5);
    cairo_stroke(cr);
}


static gboolean
intersections(GtkWidget *widget, cairo_t *cr)
{
    cairo_path_t *path;
    CpmlSegment segment1, segment2;
    CpmlPair intersection;

    cairo_translate(cr, 10.5, 120.5);

    line1_callback(cr);

    path = cairo_copy_path(cr);

    cairo_set_line_width(cr, 1.);
    cairo_stroke(cr);

    cpml_segment_from_cairo(&segment1, path);
    cpml_segment_from_cairo(&segment2, path);

    while (cpml_segment_next(&segment2)) {
        cpml_segment_put_intersections(&segment1, &segment2, 1, &intersection);

        cairo_arc(cr, intersection.x, intersection.y, 2.5, 0, 2 * M_PI);
        cairo_fill(cr);

        cpml_segment_next(&segment1);
    }

    cairo_path_destroy(path);
    return FALSE;
}


static void
algorithm_changed(GtkRadioButton *button, GtkWidget *area)
{
    const gchar *button_name;
    CpmlCurveOffsetAlgorithm new_algorithm;

    if (! gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
        return;


    button_name = gtk_widget_get_name(GTK_WIDGET(button));

    if (g_strcmp0(button_name, "DEFAULT") == 0) {
        new_algorithm = CPML_CURVE_OFFSET_ALGORITHM_DEFAULT;
    } else if (g_strcmp0(button_name, "BAIOCA") == 0) {
        new_algorithm = CPML_CURVE_OFFSET_ALGORITHM_BAIOCA;
    } else if (g_strcmp0(button_name, "HANDCRAFT") == 0) {
        new_algorithm = CPML_CURVE_OFFSET_ALGORITHM_HANDCRAFT;
    } else if (g_strcmp0(button_name, "GEOMETRICAL") == 0) {
        new_algorithm = CPML_CURVE_OFFSET_ALGORITHM_GEOMETRICAL;
    } else {
        g_warning("Unknown offset algorithm name (%s)", button_name);
        new_algorithm = CPML_CURVE_OFFSET_ALGORITHM_NONE;
    }

    cpml_curve_offset_algorithm(new_algorithm);
    gtk_widget_queue_draw(area);
}

static gboolean
offset_curves(GtkWidget *widget, cairo_t *cr)
{
    gint n;
    CpmlPair *bezier;
    cairo_path_t *path, *path_copy;
    CpmlSegment segment;
    CpmlPrimitive primitive;
    CpmlPair pair;
    double t;

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

        /* Draw the rays for visual debugging */
        cairo_set_line_width(cr, 1.);
        for (t = 0; t < 1; t += 0.1) {
            cpml_curve_put_pair_at_time(&primitive, t, &pair);

            cairo_new_sub_path(cr);
            cairo_arc(cr, pair.x, pair.y, 2.5, 0, M_PI*2);
            cairo_fill(cr);

            cairo_move_to(cr, pair.x, pair.y);
            cpml_curve_put_offset_at_time(&primitive, t, 20., &pair);
            cairo_line_to(cr, pair.x, pair.y);
            cairo_stroke(cr);
        }

        cairo_path_destroy(path_copy);
    }

    return FALSE;
}


static gboolean
offset_segments(GtkWidget *widget, cairo_t *cr)
{
    cairo_path_t *path;
    CpmlSegment segment;
    int n;

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

    return FALSE;
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
    cairo_path_t *old_path, *path;
    cairo_matrix_t matrix;
    CpmlSegment segment;

    /* Save the previous path, if any */
    old_path = cairo_copy_path(cr);

    cairo_new_path(cr);
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
    path->data[0].header.type = CPML_LINE;
    cairo_append_path(cr, path);
    cairo_path_destroy(path);

    /* ...and close the shape */
    cairo_close_path(cr);

    /* Save the resulting path and clear the path memory */
    path = cairo_copy_path(cr);
    cairo_new_path(cr);

    /* Restore the previous path and reappend the new path */
    cairo_append_path(cr, old_path);
    cairo_path_destroy(old_path);
    cairo_append_path(cr, path);
    cairo_path_destroy(path);
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

static void
line1_callback(cairo_t *cr)
{
    cairo_move_to(cr, 0, -50);
    cairo_line_to(cr, 100, 50);

    cairo_move_to(cr, 100, -50);
    cairo_line_to(cr, 0, 50);

    cairo_move_to(cr, 120, -50);
    cairo_line_to(cr, 200, -10);

    cairo_move_to(cr, 120, 50);
    cairo_line_to(cr, 200, 10);

    cairo_move_to(cr, 220, 0);
    cairo_line_to(cr, 280, 0);

    cairo_move_to(cr, 270, -40);
    cairo_line_to(cr, 270, 20);

    cairo_move_to(cr, 320, 60);
    cairo_line_to(cr, 380, 60);

    cairo_move_to(cr, 300, -40);
    cairo_line_to(cr, 340, 0);

    cairo_move_to(cr, 480, 10);
    cairo_line_to(cr, 400, 40);

    cairo_move_to(cr, 400, 40);
    cairo_line_to(cr, 450, -40);
}
