#include "fishtank.h"

/*
 * Draw a cylinder, using triangle strips (think pie slices...)
 * Cylinder is lying on side along x-axis
 */
void cylinder(double x, double y, double z, double cl, double r, const GLdouble color[], int tmode) {
   int slices = 36;
   if (fpsmode == 0)
        slices = 3;
   double czh = 360./(double) slices;
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
    if (texmode)
    {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texparam?GL_REPLACE:GL_MODULATE);
        glBindTexture(GL_TEXTURE_2D,texture[tmode]);
    }
    else
        glDisable(GL_TEXTURE_2D);

   glColor4dv(color);
   float a1, a2, b1, b2;
   cl=cl/2;
   for (double i=0; i<slices; i++) {
        a1 = Cos(i*czh);        a2=Sin(i*czh);
        b1 = Cos((i+1)*czh);    b2=Sin((i+1)*czh);
        //right side
        glBegin(GL_TRIANGLES);  //top
            glNormal3f(1.0, 0.0, 0.0);   // right side
            glTexCoord2f(0,  0);    glVertex3d(cl, 0.0, 0.0);
            glTexCoord2f(a1, a2);   glVertex3d(cl, r*a1, r*a2);
            glTexCoord2f(b1, b2);   glVertex3d(cl, r*b1, r*b2);
        //lefet side
        //glBegin(GL_TRIANGLES);  //left side
            glNormal3f(-1.0, 0.0, 0.0);   // right side
            glTexCoord2f(0,  0);    glVertex3d(-cl, 0.0, 0.0);
            glTexCoord2f(a1, a2);   glVertex3d(-cl, r*a1, r*a2);
            glTexCoord2f(b1, b2);   glVertex3d(-cl, r*b1, r*b2);
        glEnd();
        glBegin(GL_QUADS); //length of cylinder
            glNormal3f(cl, r*b1, r*b2); glTexCoord2f(1, 1);     glVertex3d( cl, r*b1, r*b2);
            glNormal3f(cl, r*a1, r*a2); glTexCoord2f(1, 0);     glVertex3d( cl, r*a1, r*a2);
            glNormal3f(-cl,r*a1, r*a2); glTexCoord2f(0,  0);    glVertex3d(-cl, r*a1, r*a2);
            glNormal3f(-cl,r*b1, r*b2); glTexCoord2f(0,  1);    glVertex3d(-cl, r*b1, r*b2);

        glEnd();

    }
    glPopMatrix();
}

