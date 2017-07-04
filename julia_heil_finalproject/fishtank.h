
#ifndef fishtank_h
#define fishtank_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
//#include <SDL2/SDL.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
//#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))
//#define Tan(th) tan(3.1415926/180*(th))

#define pi        3.1415926
#define Blue      color[0]
#define Green     color[1]
#define Red       color[2]
#define Cyan      color[3]
#define Magenta   color[4]
#define Yellow    color[5]
#define White     color[6]
#define Black     color[7]
#define Orange    color[8]
#define Chocolate color[9]
#define Peru      color[10]
#define Maroon    color[11]
#define Grey      color[12]
#define Steelblue color[13]



#define notexture    0
#define gravel       1
#define goldfish     2
#define bluescales   3
#define earth        4
#define jupiter      5
#define shark        6
#define clownfish    7
#define tang         8
#define tuna         9
#define glass        10
#define leaf1        11
#define submetal     12
#define water        13
#define MAXTEX       14  // max textures to step through
#define NUM_PATTERNS 32//set to 8, 16 or 32 - caustic pattern

#define SMALL_BALL   30  //number of degrees in ball rendering per slice
#define LARGE_BALL    9
#ifdef __cplusplus
extern "C" {
#endif


//void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(const char* file);
//void Project(double fov,double asp,double dim);
extern void glWindowPos2i(int x, int y);
extern void ErrCheck(const char* where);
extern int  LoadOBJ(const char* file);
extern int texmode, texparam;
extern GLdouble color[14][4];
extern int texture[20];
extern double tlen;
extern double zh, lzh;
extern int light0, light1, light2;
extern float shiny;
extern float emission;
extern int alpha;
extern int aone;
extern int talpha;
extern int fpsmode;
extern int DLwalls;     //display list for tank walls
extern int DLwalls1;    //tank walls with texture
extern int DLtuna;      //tuna fish
extern int DLsmallball;    //single bubble
extern int DLlargeball;
extern int DLscrub;
extern int DLscrubfield;
extern float swo[1000];        // propoerties (wiggle) of fish pods in a swarm
extern void fishSwarm();
extern void Vertex(double th,double ph);
extern void fVertex(double x, double y, double z, double sscale, const GLdouble fcolor[]);
extern void bubble(double x,double y,double z,double r, const GLdouble color[], int tmode);
extern void cylinder(double x, double y, double z, double cl, double r, const GLdouble color[], int tmode);
extern void Plant(double x, double y, double z, double pscale);
extern void Scrub(double x, double y, double z, double pscale);
extern void Sub(double x,double y,double z,
                      double fl, double fw, double fh,
                      const GLdouble color1[], const GLdouble color2[], const GLdouble color3[], int tex);
extern void Lamp(double x,double y,double z,
                      double fl, double fw, double fh,
                      const GLdouble color1[], const GLdouble color2[], const GLdouble color3[], int tex);
extern void SolidFish(double x,double y,double z,
                      double dx,double dy,double dz,
                      double ux,double uy, double uz,
                      double fl, double fw, double fh,
                      const GLdouble color1[], const GLdouble color2[], const GLdouble color3[],
                      int tex, int face, int f);
extern void cylinder(double x, double y, double z, double cl, double r, const GLdouble color[], int tmode);
extern void ball(double x,double y,double z,double r, const GLdouble color[], int tmode, int spin, int ballsize);
extern void Tank(double len);
extern void fastBall(int ballsize);
extern void fastscrub(int col);
extern void fastscrubfield();
float RandomFloat(float a, float b);
extern void sidewalls();
extern void loadFiles();
extern GLboolean HaveTexObj;
extern int causticIncrement;

#ifdef __cplusplus
}
#endif

#endif

