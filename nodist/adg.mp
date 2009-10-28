% ADG logo in MetaPost

u  := 5mm;
r1 := 4u;	% Radius for D and G
r2 := 2u;	% Radius of video screen

vardef d@# = (1 - sind(45)) * r@# enddef;


beginfig(0)

linecap := rounded;
linejoin := rounded;

drawoptions(withpen pencircle scaled 2u);
draw (5u, 6u) -- (13u, 14u) .. (17u, 10u) .. (13u, 6u) -- (13u, 14u);
draw (21u, 10u + r1) .. (21u - r1, 10u) .. (21u + r1 - d1, 6u + d1) --
     (21u, 10u) -- (17u, 10u);
draw (13u, 10u) -- (9u, 10u);

drawoptions(withpen pencircle scaled u);
draw (2u, 2u) -- (28u, 2u) -- (28u, 18u) -- (2u, 18u) -- cycle;
draw (3u, 3u + r2) .. (3u + d2, 3u + d2) .. (3u + r2, 3u) --
     (27u - r2, 3u) .. (27u - d2, 3u + d2) .. (27u, 3u + r2) --
     (27u, 17u - r2) .. (27u - d2, 17u - d2) .. (27u - r2, 17u) --
     (3u + r2, 17u) .. (3u + d2, 17u - d2) .. (3u, 17u - r2) --
     cycle;

endfig;

end.
