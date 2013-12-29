% Miscellaneous symbols

filenametemplate "%j-%c.mps";

linecap := rounded;
linejoin := rounded;

pen thin, thick;
thin := pencircle scaled 3;
thick := thin scaled 2;


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
draw halfcircle scaled 200 yscaled 0.2 shifted (100, 100) --
     halfcircle rotated 180 scaled 200 yscaled 0.2 shifted (100, 20) --
     cycle;

pickup thin;
draw halfcircle rotated 180 scaled 200 yscaled 0.2 shifted (100, 100);

endfig;


beginfig(2) % Printer

pickup thick;
draw (100, 0) -- (300, 0) -- (300, 70) -- (100, 70) -- cycle;
draw (300, 50) --
     quartercircle rotated -90 scaled 60 shifted (370, 80) ..
     quartercircle scaled 60 shifted (370, 200) ..
     quartercircle rotated 90 scaled 60 shifted (30, 200) ..
     quartercircle rotated 180 scaled 60 shifted (30, 80) .. (100, 50);
draw (300, 230) -- (300, 330) -- (100, 330) -- (100, 230);

pickup thin;
draw (120, 20) -- (280, 20);
draw (120, 35) -- (280, 35);
draw (120, 50) -- (280, 50);
draw (40, 200) -- (360, 200) -- (360, 190) -- (40, 190) -- cycle;

endfig;


beginfig(3) % Sheet

pickup thick;
draw (0, 0) -- (200, 0) -- (200, 180) -- (100, 280) -- (0, 280) -- cycle;
linecap := butt;
draw (200, 180) -- (100, 180) -- (100, 280);

pickup thin;
draw (170, 30) -- (30, 30) -- (30, 250) -- cycle;
draw (115, 60) -- (60, 60) -- (60, 145) -- cycle;
for n = 50 step 20 until 210: draw (30, n) -- (40, n); endfor
linecap := rounded;

endfig;


beginfig(4) % Laptop

pickup thick;
draw (50, 100) -- (350, 100) -- (350, 300) -- (50, 300) -- cycle;
draw (50, 100) -- (0, 20) -- (10, 0) -- (390, 0) -- (400, 20) -- (350, 100);
draw (0, 20) -- (400, 20);

pickup thin;
draw quartercircle rotated -90 scaled 40 shifted (320, 130) ..
     quartercircle scaled 40 shifted (320, 270) ..
     quartercircle rotated 90 scaled 40 shifted (80, 270) ..
     quartercircle rotated 180 scaled 40 shifted (80, 130) .. cycle;
draw (35, 55) -- (55, 92) -- (345, 92) -- (365, 55) -- cycle;
draw (170, 50) -- (230, 50) -- (233, 28) -- (167, 28) -- cycle;

endfig;


beginfig(5) % Cloud

pickup thick;
draw (305,  25) .. controls (279,  20) and (253,  26) ..
     (235,  39) .. controls (225,  30) and (223,  24) ..
     (198,  20) .. controls (148,  12) and (118,  30) ..
     (107,  60) .. controls (106,  62) and (106,  65) ..
     (105,  67) .. controls ( 70,  64) and ( 35,  81) ..
     ( 25, 108) .. controls ( 15, 135) and ( 33, 162) ..
     ( 66, 174) .. controls ( 64, 177) and ( 63, 180) ..
     ( 61, 183) .. controls ( 50, 213) and ( 74, 244) ..
     (113, 252) .. controls (132, 256) and (152, 254) ..
     (168, 247) .. controls (177, 262) and (193, 273) ..
     (216, 278) .. controls (248, 284) and (280, 274) ..
     (297, 255) .. controls (304, 259) and (312, 262) ..
     (321, 264) .. controls (361, 272) and (402, 254) ..
     (412, 224) .. controls (417, 209) and (414, 194) ..
     (404, 181) .. controls (414, 174) and (421, 165) ..
     (425, 155) .. controls (436, 125) and (412,  94) ..
     (373,  85) .. controls (369,  84) and (365,  84) ..
     (360,  83) .. controls (362,  56) and (340,  31) .. cycle;

endfig;


beginfig(6) % Tablet

pickup thick;
draw quartercircle rotated -90 scaled 60 shifted (140, 30) ..
     quartercircle scaled 60 shifted (140, 170) ..
     quartercircle rotated 90 scaled 60 shifted (60, 170) ..
     quartercircle rotated 180 scaled 60 shifted (60, 30) .. cycle;

pickup thin;
draw (45, 35) -- (45, 165) -- (155, 165) -- (155, 35) -- cycle;
draw fullcircle scaled 25 shifted (100, 18);
draw (75, 185) -- (125, 185) -- (120, 180) -- (80, 180) -- cycle;

endfig;


beginfig(7) % Workstation

pickup thick;
draw (0, 100) -- (300, 100) -- (300, 300) -- (0, 300) -- cycle;
draw (130, 90) -- (170, 90){down} .. (180, 65) -- (120, 65) .. {up}cycle;
draw (80, 50) -- (0, 10) -- (10, 0) -- (290, 0) -- (300, 10) -- (220, 50) -- cycle;
draw (318, 28) .. controls (320, 33) and (335, 43) ..
     (350, 38) .. controls (367, 32) and (380, 15) ..
     (368,  4) .. controls (360, -5) and (344,  7) ..
     (339, 11) .. controls (334, 15) and (318, 23) .. (318, 28) .. cycle;

pickup thin;
draw quartercircle rotated -90 scaled 40 shifted (270, 130) ..
     quartercircle scaled 40 shifted (270, 270) ..
     quartercircle rotated 90 scaled 40 shifted (30, 270) ..
     quartercircle rotated 180 scaled 40 shifted (30, 130) .. cycle;
draw (0, 10) -- (300, 10);
draw (370, 10) .. {dir 135}(340, 20) .. (320, 30);

draw (313, 35){dir 135} .. (280, 50) .. (290, 60) .. (250, 70) ..
     (220, 70) .. {dir 220}(185, 75);


endfig;


beginfig(8) % ADG logo

pickup thick scaled 2;
draw (20, 40) -- (100, 120) .. (140, 80) .. (100, 40) -- (100, 120);
draw (180, 120) .. (140, 80) .. (180, 40) -- (180, 60) -- (170, 60);
draw (100, 60) -- (60, 60);

pickup thick;
draw (0, 0) -- (210, 0) -- (210, 160) -- (0, 160) -- cycle;

pickup thin;
draw quartercircle rotated -90 scaled 80 shifted (160, 50) ..
     quartercircle scaled 80 shifted (160, 110) ..
     quartercircle rotated 90 scaled 80 shifted (50, 110) ..
     quartercircle rotated 180 scaled 80 shifted (50, 50) .. cycle;

endfig;


beginfig(9) % Gear

pickup thick;
draw fullcircle scaled 100 shifted (115, 115);
draw for n = 0 step 1 until 7:
         subpath (n, n + 0.5) of fullcircle scaled 190 --
         subpath (n + 0.5, n + 1) of fullcircle scaled 230 --
     endfor
  cycle shifted (115, 115);

endfig;


end.
