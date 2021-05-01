/* ADG - Automatic Drawing Generation
 * Copyright (C) 2007-2021  Nicola Fontana <ntd at entidi.it>
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


#include <adg-test.h>
#include <adg.h>


static void
_adg_method_clone(void)
{
    AdgStyleClass *klass;

    /* Just check that the default implementation is adg_object_clone */
    klass = g_type_class_ref(ADG_TYPE_STYLE);
    g_assert_true(G_CALLBACK(klass->clone) == G_CALLBACK(adg_object_clone));
    g_type_class_unref(klass);
}


int
main(int argc, char *argv[])
{
    adg_test_init(&argc, &argv);

    adg_test_add_object_checks("/adg/style/type/object", ADG_TYPE_STYLE);

    g_test_add_func("/adg/style/method/clone", _adg_method_clone);

    return g_test_run();
}
