#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "fishtank.h"
#include "vec3f.h"
using namespace std;
/*
 * Julia Heil
 * Time 50 hours
 * Putting it all together - FISHTANK
 *
 *  Inspiredd to do fishtank after looking a code from 4229 class example 8.
 *  The solid airplane looks a bit like a fish and had interesting flying algorithm using matrix operations.
 *  Used example 13 for lighting controls, ex14 for texture basics, ex18 for texturing a spheroid
 *  Very cool pitch, roll and yaw code from 3D objects in homework 8 (makes the fish swim)
 *  Initial Camera stuff from
 *  https://github.com/taylorjandrews/CSCI-4229/blob/master/Projections/taan7753hw4.c
 *
 *  Caustic motion effect Copyright (c) Mark J. Kilgard, 1997.
 *  https://www.opengl.org/archives/resources/code/samples/mjktips/caustics/
 *  This used a really old texture format of RGB (from SGI) in the 32 caustic textures
 *
 *  Bubbler starting point  "Particle Systems" lesson of the OpenGL tutorial on www.videotutorialsrock.com
 *  Tons of opengl tutorials helped with lighting controls (esp transparency stuff)
 *
 *  More time: I would have restructed the code big time, and made all C++.  I started out in
 *  C for everything, but all of the interesting particle generators leverage the Class capability
 *  of C++.  A bit kludgy to have both C and C++ source files in this program.
 *  I also would have figured out how to add a 'top of the tank water view'.
 *
 *  Known bug:  when program first startsup, something is wrong until you turn on or off
 *  a light, or move/change perspective, or go to first person mode.  Not sure what the problem is.
 *
 *  Techniques used:
 *  Caustic motion      Particle gun    Alpha blending/transparency
 *  Display lists       Animation       Waving opjects (grass)
 *  FPS optimization    Textures        Mip maps (for caustic performance increase)
 *
 *  Key bindings:
 *  FIRST PERSON MODE
 *  f          Toggle first person view on and off
 *  w/s        Move in first person forward and back  (also arrow up and down)
 *  a/d        Rotate camera in first person  (also arrow left and arrow right)
 *  PgDn/PgUp  Move first person up and down (after all, we are fish :-)
 *
 *  OTHER MODES
 *  p          Perspective view
 *  o          Othogonal view
 *  arrows     Change view angle of camera
 *  1/2        Changes field of view for perspective
 *  PgDn/PgUp  Zoom in and out
 *
 *  ALL MODES
 *  9          Toggle axes
 *  0          Reset a few params to initial conditions
 *  ESC        Exit
 *
 *  x          Start and Stop swarm  <== this is cool!!
 *  F1         Number of bubbles
 *  F3         Toggle light distance (1/3)
  * F6          Current Texture on Aquariam Decor
 *  F7         Starts/Stops sub motion
 *  F8         Light0 (orbiting light) toggle
 *  F9         Light1 (submarine) toggle
 *  F10        Light2 (corner) toggle
 *  F11        Light3 (caustic) toggle
    F12        Tank walls
 *  7/8        Lower/raise caustic/tank light
 *  [/]        Lower/raise submarine
 */

#define glBindTexture(A,B)
#define glGenTextures(A,B)
#define glDeleteTextures(A,B)

const float GRAVITY = .1f;
const int NUM_PARTICLES = 2000;  //max num of particles
const int NUM_GUNS = 7;
//The interval of time, in seconds, by which the particle engine periodically steps.
const float STEP_TIME = 0.05f;
//The length of the sides of the quadrilateral drawn for each particle.
const float PARTICLE_SIZE = 0.1f;
const float PARTICLE_SPEED = .7;  //controls how 'slanted' the stream is
const float PARTICLE_ALPHA = .8;
const float PARTICLE_LIFE = 1.1;
int bubs =100;  //iniital number of bubbles
int curgun;
float gunAngle;  //angle of the bubbler
Vec3f particleColor = Vec3f((float).7, (float) 1., (float)1.); //.878,1,1-lt blue
int DLwalls, DLwalls1,DLtuna, DLsmallball, DLlargeball,DLscrub, DLscrubfield;    //display prebuild walls

GLboolean HaveTexObj = GL_FALSE;
int causticIncrement = 1;


//Represents a single particle.
struct Particle {
	Vec3f pos;
	Vec3f velocity;
	Vec3f color;
	float timeAlive; //The amount of time that this particle has been alive.
	float lifespan;  //The total amount of time that this particle is to live.
};

//Returns a random float from a to b
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}

//Rotates the vector by the indicated number of degrees about the specified axis
Vec3f rotate(Vec3f v, Vec3f axis, float degrees) {
	axis = axis.normalize();
	float radians = degrees * pi / 180;
	float s = sin(radians);
	float c = cos(radians);
	return v * c + axis * axis.dot(v) * (1 - c) + v.cross(axis) * s;
}

//Returns the position of the particle, after rotating the camera
Vec3f adjParticlePos(Vec3f pos) {
	return rotate(pos, Vec3f(1, 0, 0), 0);
}

//Returns whether particle1 is in back of particle2
bool compareParticles(Particle* particle1, Particle* particle2) {
	return adjParticlePos(particle1->pos)[2] <
		adjParticlePos(particle2->pos)[2];
}


class ParticleEngine {
	private:
		GLuint textureId;
		Particle particles[NUM_GUNS][NUM_PARTICLES];
		//The amount of time until the next call to step().
		float timeUntilNextStep;
		//Returns the average velocity of particles produced by the fountain.
		Vec3f curVelocity() {
			//return Vec3f(PARTICLE_SPEED*2 * cos(gunAngle),2.f, PARTICLE_SPEED*2 * sin(gunAngle));
            return Vec3f(PARTICLE_SPEED*2*cos(gunAngle)-.2,2.f, .35);
		}

		//Alters p to be a particle newly produced by the fountain.
		void createParticle(Particle* p) {
			p->pos = Vec3f(-1.1, 0, .5);
			p->velocity = curVelocity() + Vec3f(PARTICLE_SPEED*0.5f * RandomFloat(-1,1),
												PARTICLE_SPEED*0.5f * RandomFloat(-1,1),
												PARTICLE_SPEED*0.5f * RandomFloat(-1,1) - 0.25f);
			p->color = particleColor;
			p->timeAlive = 0;
			p->lifespan = RandomFloat(-1,1)+PARTICLE_LIFE;
		}

		//Advances the particle fountain by STEP_TIME seconds for all guns
		void step() {
		    for (int curgun=0; curgun<NUM_GUNS; curgun++) {
                for(int i = 0; i < bubs; i++) {
                    Particle* p = particles[curgun] + i;
                    p->pos += p->velocity * STEP_TIME;
                    p->velocity += Vec3f(0.0f, -GRAVITY * STEP_TIME, 0.0f);
                    p->timeAlive += STEP_TIME;
                    if (p->timeAlive > p->lifespan) {
                        createParticle(p);
                    }
                }
			}
		}
	public:
		ParticleEngine(GLuint textureId1) {
			textureId = textureId1;
			timeUntilNextStep = 0;
//			colorTime = 0;
			gunAngle = -30;
			//load all the particle guns
		    for (int curgun=0; curgun<NUM_GUNS; curgun++) {
                for(int i = 0; i < bubs; i++) {
                    createParticle(particles[curgun] + i);
                }
			}
			for(int i = 0; i < 5 / STEP_TIME; i++) {
				step();
			}
		}

		//Advances the particle fountain by the specified amount of time.
		void advance(float dt) {
			while (dt > 0) {
				if (timeUntilNextStep < dt) {
					dt -= timeUntilNextStep;
					step();
					timeUntilNextStep = STEP_TIME;
				}
				else {
					timeUntilNextStep -= dt;
					dt = 0;
				}
			}
		}

		//Draws the particle fountain.
		void draw() {
            for (int curgun=0; curgun<NUM_GUNS; curgun++) {
                vector<Particle*> ps;
                for(int i = 0; i < bubs; i++) {
                    ps.push_back(particles[curgun] + i);
                }
                sort(ps.begin(), ps.end(), compareParticles);

                glEnable(GL_TEXTURE_2D);  //should be enable
                glBindTexture(GL_TEXTURE_2D, textureId);
 //               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                //glBegin(GL_QUADS);
                for(unsigned int i = 0; i < ps.size(); i++) {
                    Particle* p = ps[i];
                    GLdouble bubblecolor[] = {particleColor[0], particleColor[1],
                            particleColor[2], PARTICLE_ALPHA*(1 - p->timeAlive / p->lifespan)};
                    //glColor4f(p->color[0], p->color[1], p->color[2],
						 // PARTICLE_ALPHA*(1 - p->timeAlive / p->lifespan));
                    float size = PARTICLE_SIZE / 2;
                    Vec3f pos = adjParticlePos(p->pos);
                    //glEnable(GL_BLEND);
                    bubble(pos[0]+.3*curgun, pos[1], pos[2], size, bubblecolor, glass);
                }
            }
		}
};

ParticleEngine* _particleEngine;
const int TIMER_MS = 100; //The number of milliseconds to which the timer is set
GLuint _textureId;

void cleanup() {
	delete _particleEngine;
}

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
double zh=0;      //  Rotation
double tzh=0;     //  tail rotation
int axes=0;       //  Display axes
int swarm=1;  //  swarm mode
int mode=1;       //  Projection mode 1=perspective 0=orthogonal
int fp=0;         //  First person mode
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=4.0;   //  Size of world
double tlen= 2.;  //  Tank axis size
int talpha=0;     //  Tank wall texture/alpha
// Light values
int light     =   1;  // lighting mode
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
float emission=   1;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   5;  // Shininess (power of two)
float shiny   =  32;  // Shininess (value)
float ylight  = -1.75;  // Elevation of sub a start
int light0    =   1;    //Rotating over tank
int light1    =   1;    //Sub light
int light2    =   1;    //Corner light
int light3    =   1;    // Water movement light
float light1x =   0;
float light1y =   0;
float light1z =   0;
int lh        =  90;  // Light azimuth
double lzh    =   0;
int move      =   1;  // 1=moving, 0=stopped
//texture values
int texmode   =   1;  // Texture mode
int texparam  =   0;  // Texture param 0=modulate 1=replace
int texture[20];      //  Texture names
int curtex    =   jupiter;
// transparency values
int cmode     =   0;  //color mode for transparency
int aone      =   1;  // Alpha one
int rot       = 0.0;  //rotation var for first person
// Eye coords
double Ex = 0;
double Ey = 0;
double Ez = 5;
// Camera looking coords
double Cx = 0;
double Cz = 0;
double Cy = 0;
//frame rate stuff
int frame=0,time0,timebase=0;
double fps;
int fpsmode = 0;  //1=slow, 0= fast)
// other stuff
int globe = 1;
enum {   //assigns sequential numbers to these variables
  PASS_NORMAL, PASS_CAUSTIC
};

enum {
  M_POSITIONAL, M_DIRECTIONAL, M_GREENISH_LIGHT, M_WHITE_LIGHT,
  M_NO_CAUSTICS, M_WITH_CAUSTICS, M_SWITCH_MODEL,
  M_INCREASE_RIPPLE_SIZE, M_DECREASE_RIPPLE_SIZE
};

enum {
  MODEL_SPHERE
};


static int object = MODEL_SPHERE;
static GLfloat causticScale = 1.0*1.5*1.5*1.5*1.5*1.5;


static GLfloat light3Pos[4];
// XXX Diffuse light color component > 1.0 to brighten caustics.
static GLfloat lightDiffuseColor[] = {1.5, 1.5, 1.5, 1.0};  //XXX Green = 1.5 * White = 1,1.5,1

static int directionalLight = 1;
static int showCaustics = 1, causticMotion = 1;
static int currentCaustic = 0;
//static int causticIncrement = 1;

static float lightAngle = 0.0, light3Height = 5.0;
static GLfloat angle = 90;   /* in degrees -150 */
static GLfloat angle2 = 0;   /* in degrees 30 */

static int moving = 0, startx, starty;
static int lightMoving = 0, lightStartX, lightStartY;
float swo[1000];
static GLfloat floorVertices[4][3] = {
  {-2, -2, -2}, //1
  {-2, -2,  2}, //4
  { 2, -2,  2}, //3
  { 2, -2, -2},  //2
  };
GLdouble color[14][4] = { {0.0,0.0,1.0,1.0}, {0.0,1.0,0.0,1.0}, {1.0,0.0,0.0,1.0}, {0.0,1.0,1.0,1.0}, {1.0,0.0,1.0,1.0}, {1.0, 1.0, 0.0,1.0},
    {1.0,1.0,1.0,1.0}, {0.0,0.0,0.0,1.0}, {1.0,165.0/255.0,122.0/255.0,1.0}, {.82, .411, .117,1.0}, {.803, .648, .247,1.0}, {.5,0.0,0.0,1.0},
    {.3,.3,.3,1.0}, {0.0,.5,.5,1.0}
};

char texnames[16][15] = {"none", "gravel", "gold scales", "blue scales",
                         "earth", "jupiter", "shark", "clownfish", "tang","tuna" ,"glass", "leaf","sub metal", "water"};
unsigned int LoadTexBMP(const char* file);
void ErrCheck(const char* where);

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format, ...)
{
    char    buf[LEN];
    char*   ch=buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args,format);
    vsnprintf(buf,LEN,format,args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*ch++);
}

void reset() {
    th=0;          //  Azimuth of view angle
    ph=0;          //  Elevation of view angle
    zh=0;          //  Rotation
    axes=1;        //  Display axes
    swarm=0;   //  What to display
//mode=0;       //  Projection mode 1=perspective 0=orthogonal
    fp=0;          //  First person mode
    fov=55;        //  Field of view (for perspective)
    asp=1;         //  Aspect ratio
    dim=4.0;       //  Size of world
    light= 1;     // lighting on
}
void drawLightLocation(void) // specific handling for light 3
{
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  if (light3)
    glEnable(GL_LIGHT3);
  else
    glDisable(GL_LIGHT3);
  glColor3f(1.0, 1.0, 0.0);
  if (directionalLight) {
    glDisable(GL_CULL_FACE);
    glTranslatef(light3Pos[0], light3Pos[1], light3Pos[2]);
    glRotatef(lightAngle * -180.0 / M_PI, 0, 1, 0);
    glRotatef(atan(light3Height/(12*.16)) * 180.0 / M_PI, 0, 0, 1);
  } else {
    /* Draw a yellow ball at the light source. */
    glTranslatef(light3Pos[0], light3Pos[1], light3Pos[2]);
    //glutSolidSphere(1.0*.1, 10,10);
  }
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

/* Draw a floor (possibly textured). */
static void drawFloor(int pass)
{
  if (pass == PASS_NORMAL) {
    if (HaveTexObj)
      glBindTexture(GL_TEXTURE_2D, 100);
    else
      glCallList(100);
  }
  /* The glTexCoord2f calls get ignored when in texture generation
     mode (ie, when modulating in caustics). */
  glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0);   glVertex3fv(floorVertices[0]);
    glTexCoord2f(0.0, 2.0);   glVertex3fv(floorVertices[1]);
    glTexCoord2f(2.0, 2.0);   glVertex3fv(floorVertices[2]);
    glTexCoord2f(2.0, 0.0);   glVertex3fv(floorVertices[3]);
  glEnd();
}

void drawObject(int pass)
{
  if (pass == PASS_NORMAL) {
    /* The objects are not textured (they could be if someone
       wanted them to be) so disable texture in the normal pass.  In
       the caustic pass, we want to avoid disabling texture though. */
    glDisable(GL_TEXTURE_2D);
  }
  glPushMatrix();
  emission = 1;
  if (globe)
    ball(0,-(tlen-.5),0, 0.5, White, curtex, 1, LARGE_BALL);   // little globe at bottom of tank
  //glCallList(DLsphere);  // pre-rendered globe
  Plant(-tlen+.5,-tlen+.25,-1.5, 1.5);
  Plant(tlen-.5,-tlen+.25,-1.5, 1.5);
  Plant(tlen-.9,-tlen+.25,-1.5, 1.5);
  Plant(-1,-tlen,tlen-.2, 1);
  Plant(1.3,-tlen+.1,1.5, 1.2);
  Scrub(1,   -tlen, 1.1, 1.0);
  Scrub(-1.2,   -tlen, .3, 1.0);
  Scrub(-1., -tlen, 0, 1.0);
  Scrub(.75,  -tlen, -.7, 1.0);

    switch (swarm)
    {
    // Stationary fish
    case 0:
        Sub(light1x*1.2,  light1y, light1z*1.2,   1.3,  .1, .2,  Yellow, Yellow, Cyan, submetal);  //Submarine
        fishSwarm();
        break;
    // Moving fish
    case 1:
        //  Solid Fish
        //SolidFish(Cos(zh),Sin(zh), 0 ,-Sin(zh),Cos(zh),0 , Cos(4*zh),0,Sin(4*zh), .08, .2, Red, Yellow, Magenta);
        Sub(light1x,  light1y, light1z,  1.3,  .1, .2,  Yellow, Yellow, Yellow, submetal);  //Submarine

        SolidFish(Cos(zh),     1.4+Sin(zh)*.2,               0,  -Sin(zh),    0,.1,   0,1,0,     .8, .08, .2,  White,       White, Orange,  clownfish,     1,0); //clownfish
        SolidFish(-Cos(zh+45),   -.3,             -1,  Sin(zh+45), 0,.1,   0,1,0,    .8, .08, .3,  White,       White, Orange,  clownfish,     1,0); //clownfish
        SolidFish(Cos(zh+180), Sin(zh+180)*.4,   .6,  -Sin(zh+180),0,.1,   0,1,0,     .8, .08, .2,  Orange,      White, Orange,  goldfish,      1,0);  //goldfish
        SolidFish(-Cos(zh+25),  -.9,             .6,   Sin(zh+25), 0,.1,   0,1,0,     .8, .08, .2,  Yellow,     White,  Steelblue,  bluescales, 1,0);  //bluefish

        SolidFish(Cos(zh),     -1.6+Sin(zh)*.25,        Sin(zh),            0, 0,.1,   0,1,0,     .6, .7,  .1,  Chocolate, Peru,   Maroon,   0,             1,0);  //flatfish
        SolidFish(Cos(zh),       .8-Sin(zh)*.4,        Sin(zh),            0, 0,.1,   0,1,0,    .85, .08, .13,  White,     White,  Yellow,  tuna,          0, 0-30);  //tuna
        SolidFish(Cos(zh+45),   .65+Sin(zh+45)*.4,        Sin(zh+45),         0, 0,.1,   0,1,0,    .85, .08, .13,  White,     White,  Yellow,  tuna,          0, Sin(45)-30); //tuna
        SolidFish(Cos(zh+15),   .50-Sin(zh+15)*.2,        Sin(zh+15),         0, 0,.1,   0,1,0,    .85, .08, .13,  White,     White,  Yellow,  tuna,          0, Sin(15)-30); //tuna
        SolidFish(Cos(zh+35),   .8, Sin(zh+35),         0, 0,.1,   0,1,0,    .85, .08, .13,  White,     White,  Yellow,  tuna,          0, Sin(35)-30);  //tuna

        SolidFish(Cos(zh+90),   Sin(zh+90)*.4,    0,  -Sin(zh+90), 0, 1,    0,1,0,      .8,  .1, .3,  Cyan,      White,  Yellow,  tang,         0,0);  //tang
        SolidFish(-Cos(zh+60),  -.6,             .6,   Sin(zh+60), 0,1,    0,1,0,     .3, .03, .1,   Yellow,    Orange, Magenta,  0,            1,0);
        SolidFish(-Cos(zh+150), -.75,            .7,   Sin(zh+150),0,1,    0,1,0,     .3, .03, .1,   Yellow,    Orange, Magenta,  0,            1,0);
        SolidFish(-Cos(zh+240), -.8,            -.8,   Sin(zh+240),0,1,    0,1,0,     .3, .03, .1,   Yellow,    Orange, Magenta,  0,            1,0);
        SolidFish(-Cos(zh+200), -.85,     -.8,   Sin(zh+200),0,1,    0,1,0,      .3, .03, .1,   Yellow,    Orange, Magenta,  0,            1,0);

        break;
    }
    glColor4dv(White);
    glPushMatrix();
        glTranslatef(0.0f,  -tlen, -tlen);
        _particleEngine->draw();  //bubbler
    glPopMatrix();

  Tank(tlen);
  glPopMatrix();

  if (pass == PASS_NORMAL) {
    glEnable(GL_TEXTURE_2D);
  }

}

void drawScene(int pass)
{
  /* The 0.03 in the Y column is just to shift the texture coordinates
     a little based on Y (depth in the water) so that vertical faces
     (like on the cube) do not get totally vertical caustics. */
  GLfloat sPlane[4] = { 0.05, 0.03, 0.0, 0.0 };
  GLfloat tPlane[4] = { 0.0, 0.03, 0.05, 0.0 };
  /* The causticScale determines how large the caustic "ripples" will
     be.  See the "Increate/Decrease ripple size" menu options. */

  sPlane[0] = 0.05 * causticScale;
  sPlane[1] = 0.03 * causticScale;

  tPlane[1] = 0.03 * causticScale;
  tPlane[2] = 0.05 * causticScale;

  if (pass == PASS_CAUSTIC) {
    /* Set current color to "white" and disable lighting
       to emulate OpenGL 1.1's GL_REPLACE texture environment. */
    glColor3f(1.0, 1.0, 1.0);
    glDisable(GL_LIGHTING);

    /* Generate the S & T coordinates for the caustic textures
       from the object coordinates. */
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    if (HaveTexObj) {
      glBindTexture(GL_TEXTURE_2D, currentCaustic+1);
    } else {
      glCallList(currentCaustic+101);
    }
  }
  drawFloor(pass);
  drawObject(pass);

  if (pass == PASS_CAUSTIC) {
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
  }
}


/*
 *  Set projection
 */
static void Projection()
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective transformation
    if(fp)
    {
        gluPerspective(fov,asp,dim/4.,4.*dim);
    }
    else
    {
        if (mode)
           gluPerspective(fov,asp,dim/4,4*dim);
        else  //  Orthogonal projection
            glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    }
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();  //<-- why does did this kill the program
}


void display(void)
{

  //glEnable(GL_LIGHT3);
  //glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
  //  Flat or smooth shading
  glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);
  //glEnable(GL_CULL_FACE);
  /* Clear depth and color buffer. */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  if (fp) {
    Cx = +2*dim*Sin(rot); //Adjust the camera vector based on rot
    Cz = -2*dim*Cos(rot);
    gluLookAt(Ex,Ey,Ez, Cx+Ex,Cy+Ey,Cz+Ez, 0,1,0); //  Use gluLookAt, y is the up-axis
  }
  else {
  //  Perspective - set eye position
    if (mode)   // code from example 9
    {
      double px = -2*dim*Sin(th)*Cos(ph);
      double py = +2*dim        *Sin(ph);
      double pz = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(px,py,pz, 0,0,0, 0,Cos(ph),0);
    }
    //  Orthogonal - set world orientation
    else
    {
        glRotatef(ph,1,0,0);
        glRotatef(th,0,1,0);
    }
  }
glPushMatrix();
glTranslated(0,.35,0);
glScaled(1.5, 1.2, 1);
  if (light)
  {
    if (light0)  //rotating light at top
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);
    //enable light 1 (sub light)
    if (light1)
        glEnable(GL_LIGHT1);
    else
        glDisable(GL_LIGHT1);
    //enable light 2 corner
    if (light2)
        glEnable(GL_LIGHT2);
    else
        glDisable(GL_LIGHT2);
    //light for water movement
    if (light3)
        glEnable(GL_LIGHT3);
    else
        glDisable(GL_LIGHT3);
    //  Translate intensity to color vectors
    float Ambient[]   = {(float) 0.01*ambient, (float) 0.01*ambient,(float) 0.01*ambient,1.0};
    float Diffuse[]   = {(float) 0.01*diffuse, (float) 0.01*diffuse,(float) 0.01*diffuse,1.0};
    float Specular[]  = {(float) 0.01*specular,(float) 0.01*specular,(float) 0.01*specular,1.0};
    float white[]     = {1,1,1,1};

    //  Enable lighting and normalization
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    //  Light0 - orbiting light at top of tanking
    //float Light0Pos[]  =  {(float) (.5*ldistance*Cos(lzh)),(float) (tlen+.2), (float) (.5* ldistance*Sin(lzh)),1.0};  //positional (close)
    float Light0Pos[]  =  {0,(float) (tlen+.2), 0};  //positional (close)
    glColor4f(1.0, 1.0, 1.0, 1.0);
    //ball(Light0Pos[0],Light0Pos[1],Light0Pos[2], 0.1, White, 1, 0);  // light0 is rotating outside the tank
    float light0dir[] = {0, -1, 0};  //orbit light
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0);  //45 degree spotlight
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0dir);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,1);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, .5);
    glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Light0Pos);

    // Light1 - submarine light
    float Light1Pos[] = {light1x,          light1y, light1z,          1.0};  //positional light (close)
    if (move)
        ylight = Sin(zh)*1.2;
    light1x = 1*Cos(lzh);
    light1y = ylight;
    light1z = 1.0*Sin(lzh);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    //ball(Light1Pos[0],Light1Pos[1],Light1Pos[2], 0.09, Green, 0);  // light1 is inside sub
    float light1dir[] = {(float) -Sin(lzh), 0,(float) Cos(lzh)};  //sub light
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20.0);  //45 degree spotlight
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1dir);
    glLightfv(GL_LIGHT1,GL_AMBIENT,Ambient);
    glLightfv(GL_LIGHT1,GL_DIFFUSE,Diffuse);
    ambient = 100;
    glLightfv(GL_LIGHT1,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT1,GL_POSITION,Light1Pos);

    // Light 2 - blue ball in corner lighting up the globe
    float Light2Pos[] = {(float)(-tlen+.2), -1.75,(float) (tlen-.2),  1.0};
    //Lamp(-tlen+.2, -1.7, tlen-.2,  .6, .05, .1, Steelblue, White, White, 0);
    ball(-tlen+.2, -1.75, tlen-.2,  .1, White, 0,0,LARGE_BALL);
    float light2dir[] = {(float)tlen, (float) (tlen*.2), (float)-tlen}; // point toward center of tank
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 12.0);  //20 degree spotlight
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light2dir);
    glLightfv(GL_LIGHT2,GL_AMBIENT,Ambient);
    glLightfv(GL_LIGHT2,GL_DIFFUSE,Diffuse);
    glLightfv(GL_LIGHT2,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT2,GL_POSITION,Light2Pos);

    // Reposition light 3
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuseColor);
    light3Pos[0] = 12*.16*cos(lightAngle);
    light3Pos[1] = light3Height;
    light3Pos[2] = 12*.16*sin(lightAngle);
    if (directionalLight) {
        light3Pos[3] = 0.0;
    }
    else {
        light3Pos[3] = 1.0;
    }
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);  // not sure we need this anymore - materials called per object created now
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);  // ditto
  }
  else
    glDisable(GL_LIGHTING);

  glPushMatrix();
    /* Perform scene rotations based on user mouse input. */
    glRotatef(angle2, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);

    /* Position the light again, after viewing rotation. */
    glLightfv(GL_LIGHT3, GL_POSITION, light3Pos);

    /* Draw the light location. */
    drawLightLocation();

    /* Normal pass rendering the scene (caustics get added
       after this pass). */
    drawScene(PASS_NORMAL);

    if (showCaustics) {
      /* Disable depth buffer update and exactly match depth
	 buffer values for slightly faster rendering. */
      glDepthMask(GL_FALSE);
      glDepthFunc(GL_EQUAL);

      /* Multiply the source color (from the caustic luminance
	 texture) with the previous color from the normal pass.  The
	 caustics are modulated into the scene. */
      glBlendFunc(GL_ZERO, GL_SRC_COLOR);
      glEnable(GL_BLEND);

      drawScene(PASS_CAUSTIC);

      /* Restore fragment operations to normal. */
      glDepthMask(GL_TRUE);
      glDepthFunc(GL_LESS);
      glDisable(GL_BLEND);
    }
  glPopMatrix();
  //Turn off lighting for static screen stuff
  glDisable(GL_LIGHTING);
  glColor4f(1.0, 1.0, 1.0, 1.0);
  //display axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(tlen,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,tlen,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,tlen);
      glEnd();
      //  Label axes
      glRasterPos3d(tlen,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,tlen,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,tlen);
      Print("Z");
   }

    //  Display parameters
    glWindowPos2i(5,790);
    Print("Framerate FPS=%.0f",fps) ;
    glWindowPos2i(5,70);
    Print("(F8)Top Light=%-3s(F9)Sub Light=%-3s  (F10)Floor Light=%-3s(F11)Tank Light=%-3s  (F12)Tank walls=%-3s", light0?"On": "Off", light1?"On": "Off",light2?"On": "Off", light3?"On": "Off",talpha?"On":"Off" );
    glWindowPos2i(5,50);
    Print("(F7)Sub=%-7s  ([/])Sub Elev=%-4.1f (F6)Globe=%-12s", move?"Moving": "Stopped",ylight,texnames[curtex]);
    glWindowPos2i(5,30);
    Print("(F1)Bubbles=%-4d (x)Swarm=%-3s       (9)Axis: %-3s",bubs*NUM_GUNS,  swarm?"Off":"On", axes?"On": "Off");
       if (fp) {
        glWindowPos2i(5,10);
        Print("(f)FP=On         View Angle=%d          Lights:%s",rot, light?"On":"Off");
    }
    else {
        glWindowPos2i(5,10);
        Print("(f)FP=Off        Angle=%-4d,%-4d    (1/2)FOV=%-3d        (o/p)Proj=%s (PgUp/PgDn)Dim=%.1f ",th,ph,fov,mode?"Perpective":"Orthogonal",dim);
    }
    if (light)
        glEnable(GL_LIGHTING);
    glPopMatrix();
    glFlush();
  glutSwapBuffers();

}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    //  Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Set projection
    Projection();
}

void idle(void) {
    double t1 = glutGet(GLUT_ELAPSED_TIME)/100; //1/10 sec
    zh  = fmod(1*t1,360); //fmod(10*t,360);  //36 sec/rev
    if (move)  // for the light and the moving sub
        lzh=zh*2;
  /* Advance the caustic pattern. */
  currentCaustic = (currentCaustic + causticIncrement) % NUM_PATTERNS;
  glutPostRedisplay();

  frame++;
	time0=glutGet(GLUT_ELAPSED_TIME);
	if (time0 - timebase > 1000) {
        fps = frame*1000.0/(time0-timebase);
		timebase = time0;
		frame = 0;
	}
}

void updateIdleFunc(void) {
  /* Must be both displaying the caustic patterns and have the
     caustics in rippling motion to need an idle callback. */
  if (showCaustics && causticMotion) {
    glutIdleFunc(idle);
  } else {
    glutIdleFunc(NULL);
  }
}

void visible(int vis) {
  /* Stop the animation when the window is not visible. */
  if (vis == GLUT_VISIBLE)
    updateIdleFunc();
  else
    glutIdleFunc(NULL);
}

static void mouse(int button, int state, int x, int y) {
  /* Rotate the scene with the left mouse button. */
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      moving = 1;
      startx = x;
      starty = y;
    }
    if (state == GLUT_UP) {
      moving = 0;
    }
  }
  /* Rotate the light position with the middle mouse button. */
  if (button == GLUT_MIDDLE_BUTTON) {
    if (state == GLUT_DOWN) {
      lightMoving = 1;
      lightStartX = x;
      lightStartY = y;
    }
    if (state == GLUT_UP) {
      lightMoving = 0;
    }
  }
}

void menuSelect(int value) {
  switch (value) {
  case M_POSITIONAL:
    directionalLight = 0;
    break;
  case M_DIRECTIONAL:
    directionalLight = 1;
    break;
  case M_GREENISH_LIGHT:
    lightDiffuseColor[0] = 1.0;
    lightDiffuseColor[1] = 1.5;  /* XXX Green = 1.5 */
    lightDiffuseColor[2] = 1.0;
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuseColor);
    break;
  case M_WHITE_LIGHT:
    lightDiffuseColor[0] = 1.5;  /* XXX Red = 1.5 */
    lightDiffuseColor[1] = 1.5;  /* XXX Green = 1.5 */
    lightDiffuseColor[2] = 1.5;  /* XXX Blue = 1.5 */
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuseColor);
    break;
  case M_WITH_CAUSTICS:
    showCaustics = 1;
    updateIdleFunc();
    break;
  case M_NO_CAUSTICS:
    showCaustics = 0;
    updateIdleFunc();
    break;
  case M_SWITCH_MODEL:
    object = (object + 1) % 2;
    break;
  case M_INCREASE_RIPPLE_SIZE:
    causticScale /= 1.5;
    break;
  case M_DECREASE_RIPPLE_SIZE:
    causticScale *= 1.5;
    break;
  }
  glutPostRedisplay();
}



/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
    if (!fp)
    {
        //  Right arrow key - increase angle by 5 degrees
        if (key == GLUT_KEY_RIGHT)
            th += 5;
        //  Left arrow key - decrease angle by 5 degrees
        else if (key == GLUT_KEY_LEFT)
            th -= 5;
        //  Up arrow key - increase elevation by 5 degrees
        else if (key == GLUT_KEY_UP)
            ph += 5;
        //  Down arrow key - decrease elevation by 5 degrees
        else if (key == GLUT_KEY_DOWN)
            ph -= 5;
        //  PageUp key - increase dim
        else if (key == GLUT_KEY_PAGE_UP && dim>1)
            dim -= 0.1;
        //  PageDown key - decrease dim
        else if (key == GLUT_KEY_PAGE_DOWN)
            dim += 0.1;
        else if (key==GLUT_KEY_F1)
            bubs = (bubs==400?10:  (bubs==10?50:  (bubs==50?100:    (bubs==100?200:400  ))         )        );
        //  texture mode parameters
        else if (key==GLUT_KEY_F6)
            curtex = ((curtex+1)%MAXTEX);
        //light motion
        else if (key==GLUT_KEY_F7)
            move = (1-move);
        //  Toggle light 0
        else if (key == GLUT_KEY_F8)
            light0 = (1-light0);
        //  Toggle light 0
        else if (key == GLUT_KEY_F9)
            light1 = (1-light1);
        //light motion
        else if (key==GLUT_KEY_F10)
            light2 = 1-light2;
        else if (key==GLUT_KEY_F11)
            light3= 1-light3;
        // tank wall texture/alpha
        else if (key == GLUT_KEY_F12)
            talpha = 1-talpha;
    }
    else  // arrow keys move you around in fp model
    {
        double dt = 0.05;

        //  Right arrow key - rotate camera
        if (key == GLUT_KEY_RIGHT)
        {
            rot += 3;
        }
        //  Left arrow key - rotate camera
        else if (key == GLUT_KEY_LEFT)
        {
            rot -= 3;
        }
        //  Up arrow key - move forward in direction of camera
        else if (key == GLUT_KEY_UP)
        {
            Ex += Cx*dt;
            Ez += Cz*dt;
        }
        //  Down arrow key - move backward in direction of camera
        else if (key == GLUT_KEY_DOWN)
        {
            Ex -= Cx*dt;
            Ez -= Cz*dt;
        }
        //  PageUp key - move up
        else if (key == GLUT_KEY_PAGE_UP)
            Ey += dt;
        //  PageDown key - move down
        else if (key == GLUT_KEY_PAGE_DOWN)
            Ey -= dt;
        else if (key == GLUT_KEY_F12)
          talpha = 1-talpha;
        //light motion
        else if (key==GLUT_KEY_F7)
            move = (1-move);
        //  Toggle light0
        else if (key == GLUT_KEY_F8)
            light0 = (1-light0);
        //  Toggle light 0
        else if (key == GLUT_KEY_F9)
            light1 = (1-light1);
        else if (key == GLUT_KEY_F12)
          talpha = 1-talpha;
    }
    //  Keep angles to +/-360 degrees
    th %= 360;
    th = ((th) ? th : 1);  //hack to prevent antialiasing at 0 degrees
    th = ((th==180) ? 179 : th);  //hack to prevent antialiasing at 180 degrees
    ph %= 360;
    ph = ((ph) ? ph : 1);
    ph = ((ph==180) ? 179 : ph);
    angle=th;
    angle2=ph;
    Projection();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
static void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);

    else if (ch == ' ') {
    // Spacebar toggles caustic rippling motion
      causticMotion = !causticMotion;
      updateIdleFunc();
    }
    //  Reset everything but mode
    else if (ch == '0')
        reset();
    //th = ph = 0;
    //  Toggle axes
    else if (ch == '9')
        axes = 1-axes;
            //  Toggle axes
    else if (ch == 'b')
        globe = 1-globe;
    //  swarm mode
    else if (ch == 'x')
        swarm = 1-swarm;
    //  swarm mode
    //else if (ch == 'l')
        //light = 1-light;
    else if (ch=='a' && ambient>0)
        ambient -= 5;
    else if (ch=='A' && ambient<100)
        ambient += 5;
    else if (ch==' ') { /* Spacebar toggles caustic rippling motion. */
        causticMotion = !causticMotion;
        updateIdleFunc();
        }
    //  Switch display mode
    else if (ch == 'o')
    {
        mode = 0;
        fp = 0;
    }
    else if (ch == 'p')
    {
        mode = 1;
        fp = 0;
    }
    //  Toggle first person
    else if (ch == 'f')
    {
        fp = 1-fp;
    }
    //  Shininess level
   else if (ch=='n') {
      if (shininess > 0)
        shininess -= 1;
   }
   else if (ch=='N' && shininess<7) {
      shininess += 1;
      //  Translate shininess power to value (-1 => 0)
      shiny = shininess<0 ? 0 : pow(2.0,shininess);
   }
    //  Light elevation
    else if (ch=='[')
        ylight -= 0.1;
    else if (ch==']')
        ylight += 0.1;
    else if (ch=='7')
        light3Height -= 0.1;
    else if (ch=='8')
        light3Height += 0.1;
    if (fp){
        double dt = 0.05;
        if (ch == 'w') {
            Ex += Cx*dt; //Update the eye vector based on the camera vector
            Ez += Cz*dt;
        }
        else if (ch == 'a') {
            rot -= 3;
        }
        else if (ch == 's') {
            Ex -= Cx*dt;
            Ez -= Cz*dt;
        }
        else if (ch == 'd')
        {
            rot += 3;
        }
        //  Keep angles to +/-360 degrees
        rot %= 360;
    }
    else {
        //  Change field of view angle
        if (ch == '1')
            fov--;
        else if (ch == '2')
            fov++;

    }
    //  Reproject
    Projection();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

void update(int value) {
	_particleEngine->advance(TIMER_MS / 1000.0f);
	glutPostRedisplay();
	glutTimerFunc(TIMER_MS, update, 0);
}

void BuildLists() {
    DLwalls = glGenLists(135);   //two lists so far
    glNewList(DLwalls, GL_COMPILE);
        talpha = 0;  // alpha off
        sidewalls();   //tank walls
    DLwalls1 = glGenLists(DLwalls+1);
        glNewList(DLwalls, GL_COMPILE);
        talpha = 0;  // alpha off
        sidewalls();   //tank walls
    glEndList();
    DLtuna = glGenLists(DLwalls1+1);
        glNewList(DLtuna, GL_COMPILE);
            SolidFish(0,   0,       0,         0, 0,.1,   0,1,0,    .85, .08, .13,  White,     White,  Yellow,  tuna,          0, 0); //tuna
        glEndList();
    DLsmallball = glGenLists(DLtuna+1);
        glNewList(DLsmallball, GL_COMPILE);
            fastBall(SMALL_BALL);
        glEndList();
    DLlargeball = glGenLists(DLsmallball+1);
        glNewList(DLlargeball, GL_COMPILE);
            fastBall(LARGE_BALL);
        glEndList();
    DLscrub = glGenLists(DLlargeball+1);
        glNewList(DLscrub, GL_COMPILE);
            fastscrub(0);
        glEndList();
    DLscrubfield = glGenLists(DLscrub+1);
        glNewList(DLscrubfield, GL_COMPILE);
            fastscrubfield();
        glEndList();
}

void LoadTextures() {
  //  Load textures
    texture[gravel]     = LoadTexBMP("gravel128.bmp");
    texture[goldfish]   = LoadTexBMP("goldfish128.bmp");
    texture[bluescales] = LoadTexBMP("bluescales128.bmp");  // goldfish or blue
    texture[earth]      = LoadTexBMP("earth.bmp");
    texture[jupiter]    = LoadTexBMP("jupiter.bmp");
    texture[shark]      = LoadTexBMP("shark128.bmp");
    texture[clownfish]  = LoadTexBMP("clownfish128.bmp");
    texture[tang]       = LoadTexBMP("tang128.bmp");
    texture[tuna]       = LoadTexBMP("tuna1128.bmp");
    texture[glass]      = LoadTexBMP("glass128.bmp"); //glass.bmp
    texture[leaf1]      = LoadTexBMP("leaf1128.bmp");
    texture[submetal]   = LoadTexBMP("submetal128.bmp");
    texture[water]      = LoadTexBMP("water128.bmp");
}
void initTank() {
    for (int i=0; i<1000; i++) {
        swo[i] = (((float)rand()/(float)RAND_MAX)-.5)*2;
    }
    LoadTextures();
    BuildLists();
}
int main(int argc, char **argv)
{

  srand((unsigned int)time(0)); //Seed the random number generator
  glutInit(&argc, argv);
  glutInitWindowSize(800,800);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutCreateWindow("Julia Heil CS4229 Final Project - Fish Tank");
  initTank();       // setup any build lists
  /* Register assorted GLUT callback routines. */
  glutIdleFunc(idle);
  glutDisplayFunc(display);
  glutVisibilityFunc(visible);
  glutMouseFunc(mouse);
  //glutMotionFunc(motion);
  glutReshapeFunc(reshape);
  glutSpecialFunc(special);
  glutKeyboardFunc(key);

  _particleEngine = new ParticleEngine(_textureId);
  loadFiles();  //load caustic maps
  glutTimerFunc(TIMER_MS, update, 0);
  /* Create a pop-up menu. */
  glutCreateMenu(menuSelect);
  ErrCheck("init");
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
