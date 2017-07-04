#include "fishtank.h"
/*
 *  Draw a bubble
 *     at (x,y,z)
 *     radius r
 *     color
 *     tmode = apply texture
 */
void bubble(double x,double y,double z,double r, const GLdouble color[], int tmode) {
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,1.0};
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
        glEnable(GL_BLEND);
        //  Offset, scale and rotate
        glTranslated(x,y,z);
        glScaled(r,r,r);
        glColor4dv(color);
        glMaterialf(GL_FRONT,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
        glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
        glCallList(DLsmallball);  // pre-rendered bubbles
    //  Undo transformations
    if (tmode)
        glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glPopMatrix();
}

void fastBall(int inc) {  // single bubble for display list  - very smooth
    //int inc = 36;
    int th, ph;
    for (ph=-90; ph<90; ph+=inc){
        glBegin(GL_QUAD_STRIP);
            for (th=0; th<=360; th+=2*inc)
            {
                Vertex(th,ph);
                Vertex(th,ph+inc);
            }
        glEnd();
    }
}
