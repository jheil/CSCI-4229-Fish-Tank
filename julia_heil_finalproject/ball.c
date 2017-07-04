#include "fishtank.h"

/*
 *  Draw vertex in polar coordinates with normal
 */
void Vertex(double th,double ph)
{
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glTexCoord2d(th/360.0,ph/180.0+0.5);  //Texture
    glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 *     color
 *     tmode = apply texture
 */
void ball(double x,double y,double z,double r, const GLdouble color[], int tmode, int spin, int ballsize) {
 //   int th,ph;
 //   int inc       =   12;  // Ball increment (could be 3-10)
 //   if (r < .15)
  //      inc = 20;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0*emission,0.0*emission,1.0*emission};
    if (texmode && tmode)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texparam?GL_REPLACE:GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[tmode]);
    }
    else
        glDisable(GL_TEXTURE_2D);
    //  Save transformation
    glPushMatrix();
        //  Offset, scale and rotate
        glTranslated(x,y,z);
        glScaled(r,r,r);
        glColor4dv(color);
        glRotated(-90,1,0,0);           //  Declination
        if (spin)
            glRotated(zh,0,0,1);  // Spin around axes
        glMaterialf(GL_FRONT,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
        glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
        //try display list for the balls
        glCallList(ballsize==SMALL_BALL?DLsmallball:DLlargeball);  // pre-rendered bubbles
        /*
        //  Bands of latitude
        for (ph=-90; ph<90; ph+=inc){
            glBegin(GL_QUAD_STRIP);
            for (th=0; th<=360; th+=2*inc)
            {
                Vertex(th,ph);
                Vertex(th,ph+inc);
            }
            glEnd();
        }
*/
    //  Undo transformations
    if (tmode)
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


