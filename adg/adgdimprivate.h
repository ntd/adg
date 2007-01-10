#ifndef __ADGDIM_PRIVATE_H__
#define __ADGDIM_PRIVATE_H__

#include <adg/adgdim.h>
#include <adg/adgldim.h>


G_BEGIN_DECLS

#define _ADG_DIM_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_DIM, _AdgDimPrivate))
#define _ADG_LDIM_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_LDIM, _AdgLDimPrivate))
#define _ADG_ADIM_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), ADG_TYPE_ADIM, _AdgADimPrivate))


typedef struct __AdgDimPrivate   _AdgDimPrivate;
typedef struct __AdgLDimPrivate  _AdgLDimPrivate;
typedef struct __AdgADimPrivate  _AdgADimPrivate;


struct __AdgDimPrivate
{
  /* Device and user matrix dependent */
  AdgPair                quote_org;

  /* Only device matrix dependent */
  double                 quote_angle;
  AdgPair                quote_offset;
  AdgPair                tolerance_up_offset;
  AdgPair                tolerance_down_offset;
  AdgPair                note_offset;
};

struct __AdgLDimPrivate
{
  cairo_path_t           extension1;
  cairo_path_t           extension2;
  cairo_path_t           arrow_path;
  cairo_path_t           baseline;
};

struct __AdgADimPrivate
{
  cairo_path_t           extension1;
  cairo_path_t           extension2;
  cairo_path_t           arrow_path;
  cairo_path_t           baseline;
};


void            _adg_dim_render_quote           (AdgDim         *dim,
                                                 cairo_t        *cr);

G_END_DECLS


#endif /* __ADGDIM_PRIVATE_H__ */
