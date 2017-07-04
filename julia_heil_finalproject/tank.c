#include "fishtank.h"

void top() {
    double nstrips = 25.;
    double mul = 1/nstrips;

    //float gwhite[4] = {0., .58, 1.0, 1};  //70 = *.707
    float gwhite[4] = {1, 1, 1, 1};  //70 = *.707
    glColor4fv(gwhite);

    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, gwhite);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, gwhite);

    for (int i=0; i<nstrips; i++) {
        float yo = 0;
        glBegin(GL_QUADS);
            glNormal3f( 0, 1, 0);
            glTexCoord2d(0, 1-2*i*mul);       glVertex3d(-1,.95+yo,1-2*i*mul);
            glTexCoord2d(1, 1-2*i*mul);       glVertex3d(1, .95+yo,1-2*i*mul);
            glTexCoord2d(1, 1-2*(i+1)*mul);   glVertex3d(1, .95+yo,1-2*(i+1)*mul);
            glTexCoord2d(0, 1-2*(i+1)*mul);   glVertex3d(-1,.95+yo,1-2*(i+1)*mul);
        glEnd();
    }
}

void sidewalls() {
    glBindTexture(GL_TEXTURE_2D,texture[glass]);
    glDisable(GL_TEXTURE_2D);
    //float white[4] = {0., .58, 1.0, .00218};
    //glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);  // not sure we need this anymore - materials called per object created now
    //glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    //float alphaval = (talpha? .0021972656:0);
    float gwhite[4] = {0., .58, 1.0, .0021972656};  //70 = *.707 .0021972656 .0101*.97 .0021975, .0021973 .002197267
    float gwhite1[4] = {.5, .5, .5, 1};  //70 = *.707
    float gblack[4] = {0,0,0,0};
    glColor4fv(gwhite);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, gblack);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE, gwhite1);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR, gblack);

   int num = 40;  //40

   double mul = 1./num;
   for (int i=0;i<num;i++)
      for (int j=0;j<num;j++)
        for (int k=0; k<num; k++)
      {
        glBegin(GL_QUADS);
            // back wall

            glNormal3f( 0, 0, -1);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1,-1.);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1,-1.);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1,-1.);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1,-1.);

            // back wall - vector to middle of tank
            glNormal3f( 0, 0, 1);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1,-1.);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1,-1.);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1,-1.);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1,-1.);

            //left wall
            glNormal3f( -1, 0, 0);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(-1.,2*mul*(j+0)-1,2*mul*(k+0)-1);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(-1.,2*mul*(j+0)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(-1.,2*mul*(j+1)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(-1.,2*mul*(j+1)-1,2*mul*(k+0)-1);

            //right wall
            glNormal3f( 1, 0, 0);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(1.,2*mul*(j+0)-1,2*mul*(k+0)-1);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(1.,2*mul*(j+0)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(1.,2*mul*(j+1)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(1.,2*mul*(j+1)-1,2*mul*(k+0)-1);

            // now with vectors to inside


            //left wall
            glNormal3f( 1, 0, 0);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(-1.,2*mul*(j+0)-1,2*mul*(k+0)-1);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(-1.,2*mul*(j+0)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(-1.,2*mul*(j+1)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(-1.,2*mul*(j+1)-1,2*mul*(k+0)-1);

            //right wall
            glNormal3f( -1, 0, 0);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(1.,2*mul*(j+0)-1,2*mul*(k+0)-1);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(1.,2*mul*(j+0)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(1.,2*mul*(j+1)-1,2*mul*(k+1)-1);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(1.,2*mul*(j+1)-1,2*mul*(k+0)-1);
            // front wall
            glNormal3f( 0, 0, 1);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1,1.);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1,1.);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1,1.);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1,1.);

            // front wall
            glNormal3f( 0, 0, -1);
            glTexCoord2d(mul*(i+0),mul*(j+0)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+0)-1,1.);
            glTexCoord2d(mul*(i+1),mul*(j+0)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+0)-1,1.);
            glTexCoord2d(mul*(i+1),mul*(j+1)); glVertex3d(2*mul*(i+1)-1,2*mul*(j+1)-1,1.);
            glTexCoord2d(mul*(i+0),mul*(j+1)); glVertex3d(2*mul*(i+0)-1,2*mul*(j+1)-1,1.);
        glEnd();
        }
}

// Draw tank and texture it
//  Select texture if textures are on
void Tank(double len) {
     //  Save transformation
    glPushMatrix();
    //  Offset

    glScaled(len, len, len);

    glEnable(GL_BLEND);
    //glColor4f(1,1,1,alpha);
    if (aone)
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    else
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(0);
    glDisable(GL_LIGHTING);
    //  White
    glColor4dv(Steelblue);
    glBegin(GL_LINE_STRIP);
    glVertex3d(-1, -1, -1); //1
    glVertex3d(1,  -1, -1); //2
    glVertex3d(1,  1,  -1); //3
    glVertex3d(-1, 1,  -1); //4
    glVertex3d(-1, -1, -1); //1
    glVertex3d(-1, -1, 1);  //5
    glVertex3d(1,  -1, 1); //6
    glVertex3d(1,  1,  1); //7
    glVertex3d(-1, 1,  1); //8
    glVertex3d(-1, -1, 1);  //5
    glEnd();
    glBegin(GL_LINES);
    glVertex3d(1,  -1, -1); //2
    glVertex3d(1,  -1, 1); //6
    glVertex3d(1,  1,  -1); //3
    glVertex3d(1,  1,  1); //7
    glVertex3d(-1, 1,  -1); //4
    glVertex3d(-1, 1,  1); //8
    glEnd();

    glEnable(GL_LIGHTING);

    //top();  // draw water surface
    if (talpha) {
        glBindTexture(GL_TEXTURE_2D, texture[glass]);
        glCallList(DLwalls);  // pre-rendered tank walls
    }
    else {
        glBindTexture(GL_TEXTURE_2D, texture[glass]);
        glCallList(DLwalls1);  // pre-rendered tank walls with texture/alpha
    }
    //sidewalls();        // draw tank walls
    glDepthMask(1);
    glDisable(GL_BLEND);

    glPopMatrix();
}

