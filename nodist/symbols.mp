% Miscellaneous symbols

linecap := rounded;
linejoin := rounded;

pen thin, thick;
thin := pencircle scaled 3;
thick := pencircle scaled 5;


beginfig(0) % Server

pickup thick;
draw (300, 300) -- (180, 260) --
     (180,  50) .. controls (200, 30) and (250, 0) .. (280, 0) --
     (400, 100) -- (400, 280) -- cycle;

pickup thin;
draw (280, 210) .. controls (250, 210) and (200, 230) .. (180, 260);
draw fullcircle scaled 15 yscaled 1.4 shifted (260, 80);
draw fullcircle scaled 15 yscaled 1.4 shifted (260, 50);
draw (260, 185) .. controls (240, 190) and (220, 190) .. (190, 220);
draw (260, 165) .. controls (240, 170) and (220, 170) .. (190, 200);
draw (400, 280) -- (280, 210) -- (280, 0);

endfig;


beginfig(1) % Database

pickup thick;
draw halfcircle scaled 400 yscaled 0.2 shifted (200, 200) --
     halfcircle rotated 180 scaled 400 yscaled 0.2 shifted (200, 40) --
     cycle;

pickup thin;
draw halfcircle rotated 180 scaled 400 yscaled 0.2 shifted (200, 200);

endfig;


beginfig(2) % Printer

pickup thick;
draw (100, 0) -- (300, 0) -- (300, 70) -- (100, 70) -- cycle;
draw (300, 50) --
     quartercircle rotated -90 scaled 60 shifted (370, 80) --
     quartercircle scaled 60 shifted (370, 200) --
     quartercircle rotated 90 scaled 60 shifted (30, 200) --
     quartercircle rotated 180 scaled 60 shifted (30, 80) -- (100, 50);
draw (300, 230) -- (300, 330) -- (100, 330) -- (100, 230);

pickup thin;
draw (120, 20) -- (280, 20);
draw (120, 35) -- (280, 35);
draw (120, 50) -- (280, 50);
draw (40, 200) -- (360, 200) -- (360, 190) -- (40, 190) -- cycle;

endfig;


beginfig(3) % Sheet

pickup thick;
draw (50, 0) -- (350, 0) -- (350, 300) -- (250, 400) -- (50, 400) -- cycle;
linecap := butt;
draw (350, 300) -- (250, 300) -- (250, 400);

pickup thin;
for n = 50 step 30 until 290:
    draw (100, n) -- (300, n);
endfor

draw (100, 320) -- (250, 320);
draw (100, 350) -- (250, 350);

endfig;


beginfig(4) % Laptop

pickup thick;
draw (50, 100) -- (350, 100) -- (350, 300) -- (50, 300) -- cycle;
draw (50, 100) -- (0, 20) -- (10, 0) -- (390, 0) -- (400, 20) -- (350, 100);
draw (0, 20) -- (400, 20);

pickup thin;
draw quartercircle rotated -90 scaled 40 shifted (320, 130) --
     quartercircle scaled 40 shifted (320, 270) --
     quartercircle rotated 90 scaled 40 shifted (80, 270) --
     quartercircle rotated 180 scaled 40 shifted (80, 130) -- cycle;
draw (35, 55) -- (55, 92) -- (345, 92) -- (365, 55) -- cycle;
draw (170, 50) -- (230, 50) -- (233, 28) -- (167, 28) -- cycle;

endfig;


end.
