#include "demo.h"

/**
 * demo_find_data_file:
 * @file: the name of a data file
 * @caller: caller program (argv[0])
 *
 * Builds the proper path of a data file, without checking
 * for @file existence. This wrapper is needed to allow
 * running the program either when uninstalled and installed
 * and to deal with different platform issues.
 *
 * A call is considered coming from an uninstalled program
 * if @caller (containing the supposed absolute path of
 * the program) contains the LT_OBJDIR constant (usually
 * ".libs"), typically used to store an uninstalled binary.
 *
 * If the call comes from an uninstalled program, @file will
 * be searched in the parent directory of @caller dirname.
 *
 * If the call comes from an installed program, the
 * %ADG_DATADIR will be used instead. This constant is set
 * by the makefile throught preprocessor flags and will be
 * considered an absolute path on non-windows platforms,
 * hence resolving to $(pkgdatadir), or a path relative to
 * the @caller dirname on windows platforms.
 *
 * Returns: the full path to @file that must bel freed with
 *          g_free() or %NULL on errors
 **/
gchar *
demo_find_data_file(const gchar *file, const gchar *caller)
{
    static gchar *data_root = NULL;

    if (data_root == NULL) {
        if (caller == NULL || strstr(caller, LT_OBJDIR) == NULL) {
            /* Supposedly, this is a call to an installed program */
#ifdef G_OS_WIN32

            /* On windows, ADG_DATADIR is a path relative to the
             * caller dirname. This is needed because the data
             * files must be relocatable, so the installer is free
             * to put the project tree under custom directories.
             * Other files, such as headers and libraries, are
             * yet properly managed by autotools.
             */
            gchar *caller_dirname = g_path_get_dirname(caller);
            data_root = g_build_filename(caller_dirname, ADG_DATADIR, NULL);
            g_free(caller_dirname);

#else /* ! G_OS_WIN32 */

            /* On other operating systems, ADG_DATADIR is an
             * absolute path, so no further processing is required.
             */
            data_root = ADG_DATADIR;

#endif
        } else {
            /* This is likely an uninstalled program call: set
             * data_root to the parent directory of the caller dirname.
             */
            gchar *caller_dirname = g_path_get_dirname(caller);
            data_root = g_build_filename(caller_dirname, "..", NULL);
            g_free(caller_dirname);
        }
    }

    return g_build_filename(data_root, file, NULL);
}

/**
 * demo_notify_error:
 * @message: a custom error message
 * @parent_window: the parent window or %NULL
 *
 * Convenient function that presents an error dialog and waits the user
 * to close this modal dialog.
 **/
void
demo_notify_error(const gchar *message, GtkWindow *parent_window)
{
    GtkWidget *dialog = gtk_message_dialog_new(parent_window,
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), _("Error from ADG"));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
