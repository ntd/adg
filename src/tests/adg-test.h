/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2015  Nicola Fontana <ntd at entidi.it>
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


#ifndef __ADG_TEST_H__
#define __ADG_TEST_H__

#include <cairo.h>
#include <glib-object.h>


G_BEGIN_DECLS

/* Some backward compatible macros */
#ifndef g_assert_true
#define g_assert_true(expr)             do { if G_LIKELY (expr) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be TRUE"); \
                                           } while (0)
#endif

#ifndef g_assert_false
#define g_assert_false(expr)            do { if G_LIKELY (!(expr)) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be FALSE"); \
                                           } while (0)
#endif

#ifndef g_assert_null
#define g_assert_null(expr)             do { if G_LIKELY ((expr) == NULL) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should be NULL"); \
                                           } while (0)
#endif

#ifndef g_assert_nonnull
#define g_assert_nonnull(expr)          do { if G_LIKELY ((expr) != NULL) ; else \
                                               g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                                                    "'" #expr "' should not be NULL"); \
                                           } while (0)
#endif

/* This macro is similar to g_assert_cmpfloat with the == operator but
 * considers only the first 3 decimal digits. This can be used instead of
 * the strict equality to avoid some false assertions due to rounding problems.
 **/
#define adg_assert_isapprox(n1,n2)  G_STMT_START { \
                                        long double __n3 = (n1), __n4 = (n2); \
                                        gint64 __n1 = __n3 * 1000 + (__n3 > 0 ? +0.5 : -0.5), \
                                               __n2 = __n4 * 1000 + (__n4 > 0 ? +0.5 : -0.5); \
                                        if (__n1 == __n2) ; else \
                                          g_assertion_message_cmpnum (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                            #n1 " is not approximately equal to " #n2, __n3, "~=", __n4, 'f'); \
                                    } G_STMT_END


/* The following type is used by adg_test_add_traps() to handle
 * in a consistent way trap assertions. The AdgTrapsFunc function
 * must implement one or more code fragments and a serie of
 * assertions to be applied on all those fragments, e.g.:
 *
 * <informalexample><programlisting language="C">
 * static void
 * traps_function(gint i)
 * {
 *     switch (i) {
 *     case 1:
 *         // First code fragment
 *         g_print("This will be successful");
 *         break;
 *     case 2:
 *         // Second code fragment
 *         g_print("This will fail");
 *         break;
 *     default:
 *         // Assertions
 *         g_test_trap_assert_passed();
 *         g_test_trap_assert_stdout("*successful*");
 *         break;
 *     }
 * }
 * </programlisting></informalexample>
 */
typedef void (*AdgTrapsFunc)(gint i);


void            adg_test_init                   (int            *p_argc,
                                                 char          **p_argv[]);
const gpointer  adg_test_invalid_pointer        (void);
cairo_t *       adg_test_cairo_context          (void);
int             adg_test_cairo_num_data         (cairo_t        *cr);
const cairo_path_t *
                adg_test_path                   (void);
void            adg_test_add_enum_checks        (const gchar    *testpath,
                                                 GType           type);
void            adg_test_add_boxed_checks       (const gchar    *testpath,
                                                 GType           type,
                                                 gpointer        instance);
void            adg_test_add_object_checks      (const gchar    *testpath,
                                                 GType           type);
void            adg_test_add_entity_checks      (const gchar    *testpath,
                                                 GType           type);
void            adg_test_add_global_space_checks(const gchar    *testpath,
                                                 gpointer        entity);
void            adg_test_add_local_space_checks (const gchar    *testpath,
                                                 gpointer        entity);
void            adg_test_add_traps              (const gchar    *testpath,
                                                 AdgTrapsFunc    func,
                                                 gint            n_fragments);

G_END_DECLS


#endif /* __ADG_TEST_H__ */
