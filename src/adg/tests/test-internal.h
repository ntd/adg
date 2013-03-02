/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007,2008,2009,2010,2011,2012,2013  Nicola Fontana <ntd at entidi.it>
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


#ifndef __TEST_INIT_H__
#define __TEST_INIT_H__

#include <adg.h>


G_BEGIN_DECLS

/* Redefine the g_assert_cmpfloat() macro by using "float" instead of
 * "long double" helper variables. This is needed by FreeBSD to pass
 * the tests: g_value_get_double() seems to suffer a precision loss on
 * this OS that invalidates the comparation between doubles.
 */
#undef g_assert_cmpfloat
#define g_assert_cmpfloat(n1,cmp,n2) \
    do { \
        float __n1 = (n1), __n2 = (n2); \
        if (__n1 cmp __n2) \
            ; \
        else \
          g_assertion_message_cmpnum(G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
                                     #n1 " " #cmp " " #n2, __n1, #cmp, __n2, 'f'); \
    } while (0)

typedef void (*AdgCallback) (void);


void            adg_test_init                   (int            *p_argc,
                                                 char          **p_argv[]);
const gpointer  adg_test_invalid_pointer        (void) G_GNUC_CONST;
void            adg_test_add_func               (const char     *testpath,
                                                 AdgCallback     test_func);

G_END_DECLS


#endif /* __TEST_INIT_H__ */
