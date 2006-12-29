#ifndef __ADGMATRIX_H__
#define __ADGMATRIX_H__

#include <glib-object.h>
#include <cairo.h>


G_BEGIN_DECLS

#define ADG_TYPE_MATRIX  (adg_matrix_get_type ())

typedef cairo_matrix_t AdgMatrix;


GType                   adg_matrix_get_type             (void) G_GNUC_CONST;

const AdgMatrix *       adg_matrix_get_fallback         (void);
AdgMatrix *             adg_matrix_dup                  (const AdgMatrix *matrix);
AdgMatrix *             adg_matrix_set                  (AdgMatrix       *matrix,
                                                         const AdgMatrix *src);
gboolean                adg_matrix_equal                (const AdgMatrix *matrix1,
                                                         const AdgMatrix *matrix2);

G_END_DECLS


#endif /* __ADGMATRIX_H__ */
