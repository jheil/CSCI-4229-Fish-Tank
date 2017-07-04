#include "fishtank.h"

void fVertex(double x, double y, double z, double sscale, const GLdouble fcolor[])  //fish vertex - simplifies code
{
    glColor4dv(fcolor);
    glTexCoord2d(-x*sscale+.4,y*sscale+.5);
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

void SolidFish(double x,double y,double z,
                      double dx,double dy,double dz,
                      double ux,double uy, double uz,
                      double fl, double fw, double fh,
                      const GLdouble color1[], const GLdouble color2[], const GLdouble color3[],
                      int tex, int face, int frot)
{
    // fw = fishwidth
    // fh = fishheight
    // Dimensions used to size fish
    double xslices = 30;  // number of slices in each fish
    double ss = 1.0;  // scaling for scale texture - smaller = larger scales
    double fthin = .001; //thickness of fins, tails, etc
    //double yslices =4;
    //  Unit vector in direction of flght
    double D0 = sqrt(dx*dx+dy*dy+dz*dz);
    double X0 = dx/D0;
    double Y0 = dy/D0;
    double Z0 = dz/D0;
    //  Unit vector in "up" direction
    double D1 = sqrt(ux*ux+uy*uy+uz*uz);
    double X1 = ux/D1;
    double Y1 = uy/D1;
    double Z1 = uz/D1;
    //  Cross product gives the third vector
    double X2 = Y0*Z1-Y1*Z0;
    double Y2 = Z0*X1-Z1*X0;
    double Z2 = X0*Y1-X1*Y0;
    //  Rotation matrix
    double mat[16];
    mat[0] = X0;    mat[4] = X1;    mat[ 8] = X2;   mat[12] = 0;
    mat[1] = Y0;    mat[5] = Y1;    mat[ 9] = Y2;   mat[13] = 0;
    mat[2] = Z0;    mat[6] = Z1;    mat[10] = Z2;   mat[14] = 0;
    mat[3] =  0;    mat[7] =  0;    mat[11] =  0;   mat[15] = 1;

    //  Save current transforms
    glPushMatrix();
    //  Offset, scale and rotate
    glScaled(.8, .8, .8);
    glTranslated(x,y,z);
    glMultMatrixd(mat);
    if (fw>fh)
        glRotated(-(zh-30),0,1,0);
    if (tex == tuna) {
        glRotated((frot-zh), 0,1,0);
    }

    // body
    //glBegin(GL_QUADS);
    double zh0; // calculate wiggle
    double p = -fl/2;
    double px = fl/xslices;
    double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    double fex, fey, fez, fer;
    fex = -fl/2+fl*.75;          // fish eye x-coord
    fey = sin(.75*pi)*fh*.7;     // fish eye y-coord
    fez = sin(.75*pi)*fw/2*.8;   // fish eye z-coord
    fer = fh*sin(.85*pi)*.5;     // fish eye radius
    if (fw>fh)
        fer=fer*3.5;  //bigger eyes for flat fish
    ball(fex,       fey,  fez,      fer,    White, 0,0,SMALL_BALL); // draw the eye
    ball(fex+fer*.8,fey,  fez,  fer*.45,    Black, 0,0,SMALL_BALL); // pupil
    ball(fex,       fey, -fez*1.1,  fer,    White, 0,0,SMALL_BALL); // draw the eyes
    ball(fex+fer*.8,fey, -fez,  fer*.45,    Black, 0,0,SMALL_BALL); // pupil


    //set fish material
    //  Set specular color to white - try emerald and coral material types later
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    float shiny = 32;
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
    //  Select texture if textures are on
    if (tex && texmode)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texparam?GL_REPLACE:GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[tex]);
    }
    else
        glDisable(GL_TEXTURE_2D);


    for (double i = 0; i<=.95*xslices; i++) {
        if ((i >= .7*xslices) && face) //no texture in front of eyes
            glDisable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        x1 = p;     y1 = sin((fl/2-p)*pi/fl)*fh;        z1 = 0;                         //1
        x2 = p;     y2 = 0;                             z2 = sin((fl/2-p)*pi/fl)*fw;           //2
        x3 = p+px;  y3 = 0;                             z3 = sin((fl/2-(p+px))*pi/fl)*fw;      //3
        x4 = p+px;  y4 = sin((fl/2-(p+px))*pi/fl)*fh;   z4 = 0;   //4
        zh0=0;
        //top of fish
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
        glEnd();
        p += px;
    }


    // we need to put one more quad on the mouth/nose on the fish
    glBegin(GL_QUADS);
        glNormal3f(1,0,0);
        glColor4dv(Grey);
        glVertex3d(x1, y1, -z1);
        glVertex3d(x2, y2, -z2);
        glVertex3d(x1, -y1,z1);
        glVertex3d(x2, y2, z2);
    glEnd();

    // tail
    double ydelta, zdelta;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_POLYGON);
        //front side of tail
        if (fw<fh) {
            zdelta = Sin(9*zh)*fw;
            ydelta = 0;
        }
        else {
            zdelta = 0;
            ydelta = Sin(9*zh)*fh;
        }
        glNormal3f(0, 0, 1) ;
        glColor4dv(color1);
        glTexCoord2f(0, 0.0);  glVertex3d(-fl/2,  0, fthin); //1
        glColor4dv(color3);
        glTexCoord2f(.3, .7);  glVertex3d(-fl/2-fl*.3, fh*.7+ydelta, fthin+zdelta); //2
        glColor4dv(color1);
        glTexCoord2f(.15, .7);  glVertex3d(-fl/2-fl*.15, 0, fthin+zdelta/2); //2
        glColor4dv(color3);
        glTexCoord2f(.3, .7);  glVertex3d(-fl/2-fl*.3, -fh*.7+ydelta, fthin+zdelta); //4
        glColor4dv(color1);
        glTexCoord2f(0, 0.0);  glVertex3d(-fl/2,  0, fthin); //1
    glEnd();
    //back side of tail
    glBegin(GL_POLYGON);
        glNormal3f(0, 0, -1);
        glColor4dv(color1);
        glTexCoord2f(0,0);      glVertex3d(-fl/2, 0.0, -fthin); //1
        glColor4dv(color3);
        glTexCoord2f(.3, .7);   glVertex3d(-fl/2-fl*.3, fh*.7+ydelta, -fthin+zdelta); //2
        glColor4dv(color1);
        glTexCoord2f(.15, .7);  glVertex3d(-fl/2-fl*.15, 0, -fthin+zdelta/2); //2
        glColor4dv(color3);
        glTexCoord2f(.3, .7);   glVertex3d(-fl/2-fl*.3, -fh*.7+ydelta,-fthin+zdelta); //4
        glColor4dv(color1);
        glTexCoord2f(0,0);      glVertex3d(-fl/2, 0.0, -fthin); //1
    glEnd();

    // top fin
    glBegin(GL_QUADS);
        //front side - top fin
        glNormal3f(0,          0,        1) ;
        glColor4dv(color1);
        glVertex3d(-fl/2+.75*fl, sin(.75*fl*pi/fl)*fh, fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.6*fl,  sin(.6*fl*pi/fl)*fh+(fh*.55), fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.4*fl,  sin(.4*fl*pi/fl)*fh+(fh*.30), fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.25*fl, sin(.25*fl*pi/fl)*fh, fthin);
        //back side - top fin
        glNormal3f(0,          0,        -1) ;
        glColor4dv(color1);
        glVertex3d(-fl/2+.75*fl, sin(.75*fl*pi/fl)*fh, -fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.6*fl,  sin(.6*fl*pi/fl)*fh+(fh*.55), -fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.4*fl,  sin(.4*fl*pi/fl)*fh+(fh*.30), -fthin);
        glColor4dv(color3);
        glVertex3d(-fl/2+.25*fl, sin(.25*fl*pi/fl)*fh, -fthin);
    glEnd();
    glPopMatrix();
}
/*
 * Draw a single fish in the swarm
 * f = arc offset degrees
 * d = arc size in degrees
 * ho = height offset in the tank
 * ro = radius offset in the swarm
 */
void swarmfish(int f, float ho, float ro, float swo) {
        glPushMatrix();
            glTranslated(ro*1.6*Cos(zh+f),ho+Sin(zh)*swo,ro*1.6*Sin(zh+f));
            glRotated(zh+f, 0,-1,0);
            glBindTexture(GL_TEXTURE_2D, texture[tuna]);
            glCallList(DLtuna);  // pre-rendered tank walls
        glPopMatrix();
}
void fishSwarm() {
    int d = 60;  // 60 degrees between pods
    int rings = 6; // numbers of rings of fish
    int ipod = 1;  //current pod
    float ko = 0; // ring offset

    //sine wave offset
    for (int j=0; j<rings; j++) {  //one for each ring of fish
        for (int i=0; i<= 360/d; i++) {  //single ring of fish
            for (int k=1; k<3; k++) {   //inner ring
                ko = ((k==1)?1. : .6);
                ipod++;
                swarmfish(i*d+(j-1)*d/2,        tlen-(.6+.5*j),         1*ko,   .6*swo[ipod]);
                swarmfish(i*d+(j-1)*d/2+10,     tlen-(.6+.5*j+.3),    1.1*ko,   .5*swo[ipod]);
                swarmfish(i*d+(j-1)*d/2+10,     tlen-(.6+.5*j+.2),     .9*ko,   .3*swo[ipod]);
            }
        }
    }
}
