% ADG logo in MetaPost

u  := 5mm;
r1 := 4u;	% Radius for D and G
d1 := (1 - sind(30)) * r1;
r2 := 4u;	% Radius of video screen
d2 := (1 - sind(45)) * r2;

beginfig(0)

linecap := rounded;
linejoin := rounded;

drawoptions(withpen pencircle scaled 3/2u);
draw (4u, 6u) -- (12u, 14u) .. (16u, 10u) .. (12u, 6u) -- (12u, 14u);
draw (20u, 10u + r1) .. (20u - r1, 10u) .. (20u, 10u - r1) --
     (20u, 8u) -- (19u, 8u);
draw (12u, 8u) -- (8u, 8u);

drawoptions(withpen pencircle scaled 4/5u);
draw (2u, 2u) -- (23u, 2u) -- (23u, 18u) -- (2u, 18u) -- cycle;
draw (3u, 3u + r2) .. (3u + d2, 3u + d2) .. (3u + r2, 3u) --
     (22u - r2, 3u) .. (22u - d2, 3u + d2) .. (22u, 3u + r2) --
     (22u, 17u - r2) .. (22u - d2, 17u - d2) .. (22u - r2, 17u) --
     (3u + r2, 17u) .. (3u + d2, 17u - d2) .. (3u, 17u - r2) --
     cycle;

endfig;

end.
