#ifndef ADG_PATH_PRIVATE_H
#define ADG_PATH_PRIVATE_H

#include "adgpath.h"

#define ADG_TOLERANCE   0.1


G_BEGIN_DECLS

typedef enum _AdgDirection  AdgDirection;

enum _AdgDirection
{
  ADG_DIRECTION_FORWARD,
  ADG_DIRECTION_REVERSE
};


void    _adg_path_arc                   (AdgPath *path,
                                         double	  xc,
                                         double	  yc,
                                         double	  radius,
                                         double	  angle1,
                                         double	  angle2);

void    _adg_path_arc_negative          (AdgPath *path,
                                         double   xc,
                                         double   yc,
                                         double   radius,
                                         double   angle1,
                                         double   angle2);

G_END_DECLS

#endif /* ADG_PATH_PRIVATE_H */
