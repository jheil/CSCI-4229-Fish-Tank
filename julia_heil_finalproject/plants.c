#include "fishtank.h"


void Grass(double x,double y,double z,
                  double gl, double gh, double gw, //grass len, height and width
                  double sf,
                  double sth, //angle of leaf
                  double lth, //rotation of leaf
                  const GLdouble color1[], const GLdouble color2[],
                  int tex)
{
    float shiny = 32;
    // Dimensions used to size fish
    double xslices = 40;  // number of slices in each fish
    double ss = 1.0;  // scaling for scale texture - smaller = larger scales
//    double zho = Sin(zh);  //z axis offset for wiggle
    double zh0;
    sth = (sth==0) ? .01 : sth;  // sth =0 correct for that case
    double ydx = (gl/2)*Sin(sth);
    double xdx =(ydx/(Cos(90-sth)/Sin(90-sth)));
    double sx=1;  //scrub factor
    if (gl<=.4)
        sx = .2;
    //scale factor
    gl = gl*sf;
    gh = gh*sf;
    gw = gw*sf;
    //  Save current transforms
    glPushMatrix();
    //  Offset, scale and rotate
    //glRotatef(90+sth,0,0,-1);
    glTranslated(x,y,z);
    glScaled(1/sf,1,1);
    glRotated(90,0,0,1);
    glTranslated(xdx,ydx,0);
    glRotated(sth,0,0,1);
    glRotatef(lth,1,0,0);
    // main leafS);
    double p = -gl/2;
    double px = gl/xslices;
    double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    //set leaf material
    //  Set specular color to white - try emerald and coral material types later
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    //  Select texture if textures are on
    if (tex && texmode) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texparam?GL_REPLACE:GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[tex]);
    }
    else
        glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    for (double i = 0; i<=xslices; i++)
    {
        x1 = p;     y1 = sin((gl/2-p)*pi/gl)*gh;        z1 = 0;                         //1
        x2 = p;     y2 = 0;                             z2 = sin((gl/2-p)*pi/gl)*gw*sx;           //2
        x3 = p+px;  y3 = 0;                             z3 = sin((gl/2-(p+px))*pi/gl)*gw*sx;      //3
        x4 = p+px;  y4 = sin((gl/2-(p+px))*pi/gl)*gh;   z4 = 0;   //4

        //top of fish
        zh0=sx*.06*Sin((i*360/xslices)+2*zh);
        if (gl<.3) {
            zh0=0;
        }
        fVertex(x1, y1, zh0+z1, ss, color1);
        fVertex(x2, y2, zh0+z2, ss, color2);
        fVertex(x3, y3, zh0+z3, ss, color2);
        fVertex(x4, y4, zh0+z4, ss, color1);
        // now bottom half
        fVertex(x2, y2, zh0+z2, ss, color2);
        fVertex(x1,-y1, zh0+z1, ss, color1);
        fVertex(x4,-y4, zh0+z4, ss, color1);
        fVertex(x3, y3, zh0+z3, ss, color2);
        //back side top
        fVertex(x1, y1, zh0+z1, ss, color1);
        fVertex(x2, y2,zh0-z2, ss, color2);
        fVertex(x3, y3,zh0-z3, ss, color2);
        fVertex(x4, y4, zh0+z4, ss, color1);
        // back side bottom
        fVertex(x2, y2,zh0-z2, ss, color2);
        fVertex(x1,-y1, zh0+z1, ss, color1);
        fVertex(x4,-y4, zh0+z4, ss, color1);
        fVertex(x3, y3,zh0-z3, ss, color2);

        p += px;
    }
    glEnd();
    glPopMatrix();
}

void Plant(double x, double y, double z, double sf) {
    Grass(x,y,z-.15,  1.2,  .15, .02, sf,   0,  0, White, White, leaf1);
    Grass(x,y,z,       .8, .05, .02, sf,   0,  0, White, Steelblue, 0);
    Grass(x,y,z,        1, .05, .02, sf,  15,  30, Green, Blue, 0);
    Grass(x,y,z,       .7, .05, .02, sf, -15,  -30, Blue, Green, 0);
    Grass(x,y,z,       .7, .05, .02, sf, -25,  20, Blue, Green, 0);
}

void fastscrub (int col) {
    if  (col==0) {
    Grass(0,0,0,        .1, .02, .005,  1,   15,  30, Red, White, 0);
    Grass(0,0,0,        .2, .02, .005,  1,  10,  30, Orange, White, 0);
    Grass(0,0,0,        .2, .02, .005,  1,  -10,  30, Red, White, 0);
    Grass(0,0,0,        .1, .02, .005,  1,  -15,  30, Orange,White, 0);
    }
    else {
    Grass(0,0,0,        .1, .02, .005,  1,   15,  30, Green, Yellow, 0);
    Grass(0,0,0,        .2, .02, .005,  1,  10,  30, Steelblue, Blue, 0);
    Grass(0,0,0,        .2, .02, .005,  1,  -10,  30, Green,Yellow, 0);
    Grass(0,0,0,        .1, .02, .005,  1,  -15,  30, Steelblue,Blue, 0);
    }
}

void fastscrubfield () {
    for (int i=0; i<15; i++) {
        glPushMatrix();
            glTranslated(RandomFloat(-.6,.6),0,RandomFloat(-.6, .6));
            fastscrub(i%2);
        glPopMatrix();
    }
}

void Scrub(double x, double y, double z, double sf) {
    glPushMatrix();
        glScaled(sf,sf,sf);
        glTranslated(x,y,z);
        glCallList(DLscrubfield);
    glPopMatrix();
}
