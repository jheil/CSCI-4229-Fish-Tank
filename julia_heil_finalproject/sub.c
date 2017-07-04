#include "fishtank.h"

void Sub(double x,double y,double z,
         double fl, double fw, double fh,
         const GLdouble color1[], const GLdouble color2[],const GLdouble color3[], int tex)  // tex=texture
{
    double xslices = 30;  // number of slices in each sub
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(lzh+90, 0,-1,0);

    // body
    double zh0; // calculate wiggle
    double p = -fl/2;
    double px = fl/xslices;
    double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
    double fex, fey, fez, fer;

    //set fsub material
    //  Set specular color to white - try emerald and coral material types later
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    float shiny = 128;
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

    for (double i = 0; i<=.7*xslices; i++)
    {
        x1 = p;     y1 = sin((fl/2-p)*pi/fl)*fh;        z1 = 0;                         //1
        x2 = p;     y2 = 0;                             z2 = sin((fl/2-p)*pi/fl)*fw;           //2
        x3 = p+px;  y3 = 0;                             z3 = sin((fl/2-(p+px))*pi/fl)*fw;      //3
        x4 = p+px;  y4 = sin((fl/2-(p+px))*pi/fl)*fh;   z4 = 0;   //4
        zh0=0;
        //top front
        glBegin(GL_QUADS);
            fVertex(x1, y1, zh0+z1, 1, color1);
            fVertex(x2, y2, zh0+z2, 1, color2);
            fVertex(x3, y3, zh0+z3, 1, color2);
            fVertex(x4, y4, zh0+z4, 1, color1);
        //glEnd();
        // now bottom half
        //glBegin(GL_QUADS);
            fVertex(x2, y2, zh0+z2, 1, color2);
            fVertex(x1,-y1, zh0+z1, 1, color1);
            fVertex(x4,-y4, zh0+z4, 1, color1);
            fVertex(x3, y3, zh0+z3, 1, color2);
        //glEnd();
        //back side top
        //glBegin(GL_QUADS);
            fVertex(x1, y1, zh0+z1, 1, color1);
            fVertex(x2, y2,zh0-z2,  1, color2);
            fVertex(x3, y3,zh0-z3,  1, color2);
            fVertex(x4, y4, zh0+z4, 1, color1);
        //glEnd();
        // back side bottom
        //glBegin(GL_QUADS);
            fVertex(x2, y2,zh0-z2,  1, color2);
            fVertex(x1,-y1, zh0+z1, 1, color1);
            fVertex(x4,-y4, zh0+z4, 1, color1);
            fVertex(x3, y3,zh0-z3,  1, color2);
        glEnd();
        p += px;
    }

    //main light
    fex = -fl/2+fl*.68; // sub light x-coord
    fey = 0;            // sub light y-coord
    fez = 0;            // sub light z-coord
    fer = fw*1.1;       // sub light radius
    ball(fex,             fey,  fez,      fer,       White, 0,0,LARGE_BALL ); // draw Light
    ball(fex-fer*1.8*1,   fey,  fez,      fer*1.1,   Black, 0,0,SMALL_BALL); // window 1
    ball(fex-fer*1.8*2,   fey,  fez,      fer*1.05,  Black, 0,0,SMALL_BALL); // window 2
    ball(fex-fer*1.8*3,   fey,  fez,      fer*.85,   Black, 0,0,SMALL_BALL); // window 2

    // now landing pontoons
    cylinder(-.1*fl,-fh*.9, fw*.6, (fl/2)*.8, fh*.2, color3, 0);
    cylinder(-.1*fl,-fh*.9,-fw*.6, (fl/2)*.8, fh*.2, color3, 0);

    // now propeller
    glPushMatrix();
        glColor4dv(color1);
        glRotated(27*zh, 1,0,0);
        glBegin(GL_TRIANGLES);
            glVertex3d(-fl/2,      0,  0);
            glVertex3d(-fl/2,  -fh*.7,  fw*.4);
            glVertex3d(-fl/2 , -fh*.7, -fw*.4);
            //glEnd();
            //glBegin(GL_TRIANGLES);
            // second blade
            glVertex3d(-fl/2,      0,  0);
            glVertex3d(-fl/2,  fh*.7,  fw*.4);
            glVertex3d(-fl/2 , fh*.7, -fw*.4);
        glEnd();
    glPopMatrix();
glPopMatrix();
}
