Julia Heil
CSCI 4229
finalproject.exe
Fish Tank
Time Spent: 50 hours

--- CONTROLS -------------------------------------------------------------------------
 
CAMERA:
 *  f          Toggle first person view on and off
 *  w/s        Move in first person forward and back  (also arrow up and down)
 *  a/d        Rotate camera in first person  (also arrow left and arrow right)
 *  PgDn/PgUp  Move first person up and down (after all, we are fish :-)
 *  p          Perspective view
 *  o          Othogonal view
 *  arrows     Change view angle of camera
 *  1/2        Changes field of view for perspective
 *  PgDn/PgUp  Zoom in and out

LIGHTING: 
 *  F8	       Top Light (on/off)
 *  F9	       Sub Light (on/off)
 *  F10        Floor Light (on/off)
 *  F11        Tank Light (on/off)
 *  F12        Tank Walls (on/off)

OTHER MODES:
 *  F7	       Sub animation (moving/stopped)
 *  [/]        Sub elevation (works when sub is stopped)
 *  F6 	       Globe texture
 *  F1	       Bubbler adjuster 
 *  x          Tuna swarm mode
 *  b          Toggle globe
 *  9          Toggle axes
 *  0          Reset orientation to perspective and adds axis
 *  ESC        Exit

--- DESCRIPTION ----------------------------------------------------------------------

Special note: 
My home is in San Jose, CA. I visited the Monterey Bay aquarium many
times and there is a visually stunning live display of 'yawning' anchovies swimming
in a circular glass tank. My tuna 'swarm' is my CS4229 version of that great memory.

					*********
Why I should get an 'A':
My project is visually interesting and realistic. Given the highly compressed
timeframe, I managed to use a variety of interesting techniques. The bubbler and 
caustic water motion show this, and are definitely the most impressive pieces of my
scene. Additionally, I spent a lot of time trying to optimize for a better frame rate
and have come to understand different methods for doing so.
 
					*********

Below are some things to look for when running my program:

    1. Tuna swarm - hit 'x' (swarm mode) and then F11 (turn of main tank light) and
       you will see the sub light illuminating the tuna - you will notice there are 
       two cylinders of fish (top view). Each cylinder has six 'rings' of fish.
       Each ring has 6 'pods' of tree fish (each of the three have their own 
       motion.
    2.  Bubbler - rotate the tank to the back and you can see they are transparent
        and are illuminated when sub light passes in front of them.
    3.  Bubbles - bubbles have a half life as they near the top of the tank. Adjust
        the bubbler speed (F1) and a cool bubble "burst" happens when you increase 
        between 700, 1400 and 2800 bubbles.
    4.  Grass is waving, sub propeller turns, fish tails are all moving.
    5.  Caustic motion on tank floor.
    6.  Four independent light (hit F12 to turn on tank walls and you can see them 
        independently - turn off F8, F9, F10, F11, and then turn them on one at a 
        time).
    7.  Notice that the sub light 'flickers' a bit as it moves. That is because I 
        placed the light 'inside' the sub from globe, and its position somehow
        isn't 100% centered in the globe. I set the light in orbit using different 
        code from the sub motion. This goes on my bug list for the next release :)

					*********
Testing environment:
This animation was designed and built in a Windows environment using the modest
graphics card in my Dell Inspiron (intel graphics HD620 chip) and on my desktop.
Framerate ranges from about 40-50 in opening scene to about 4-8fps when tuna swam with
2800 bubbles AND tank walls are lit up. I also used Codeblocks IDE and Irfan to make 
correct bitmap formats.

					*********

Techniques used:
    - Basic texture
    - mipMap textures and filter
    - First person view / scene view
    - Orthogonal/perspective projections
    - 3D objects (fish, plants, scrub, rotating sphere, submarine, bubbles)
    - Animation (fish tails, fish motion, plant waving, sub motion, sub propeller,
      globe rotation)
    - Swarm motion (devised my own method for moving 'pods' of three tunas (which 
      have their own motion) in to 6 rings of 3 pods and two cylinders
    - Matrix operations (all over, and for fish and sub motion)
    - Caustic effect - water motion
    - Transparency / alpha blend - tank walls, bubbles
    - Particle generation - bubbler
    - Display lists for performance - all spheres. (fish eyes, bubbles, sub 
      windows, globe), plant scrub, tuna swarm

					*********
Performance:
Each frame has a 'native' number of approx 340K vertices (this is for the tuna swarm
mode with 2800 bubbles, 210 fish). I used these techniques to improve performance:
   - Display lists
   - Variable 'slicing' for large and small objects
   - mipMaps
   - Smaller starting texture bitmap sizes (started at 512x512, most are 128x128 as 
     baseline for mipmap).

					*********

Future Improvements:
Restructure the code base - I wrote this program incrementally, adding techniques as 
I went along. I added two techniques (caustic motion - added the idea of two pass 
rendering) and a particle-engine based bubbler (all of the examples I saw were C++ 
based, and my code was C until that time). So my final project is a bit of 
'frankencode'. Now that I know what I want to accomplish AND I know more about OpenGl, 
I would start from scratch and develop a lot cleaner program. You will notice a TON of
external variables in my C code - a lot of that was from the class examples where 
external were exposed to show the effects of changing parameters on an opengl operation. 

Make all loaded files the same format. I leverage the caustic code which used cautics
maps generate from SGI workstations(!) using their image format. I would convert these
to BMP and use a single texture loader.

Fix the wall texturing to make look better (it was fine before I added caustic motion).

Add sound and other interesting features (I found a sound library that played simple 
.wav files) - feature list for fish tanks seems endless.

Work to consistently get framerates over 30fps.

Use shaders to generate more interesting bubble effects.
