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

extern "C" GLubyte *read_alpha_texture(char *name, int *width, int *height);
extern "C" GLubyte * read_rgb_texture(char *name, int *width, int *height);

#define FLOOR_FILE "floor.rgb"

static int useMipmaps = 1;

void loadFiles() {
  /* used code from https://www.opengl.org/archives/resources/code/samples/mjktips/caustics/ */
  /* Load the caustic ripple textures. */
  int width, height;
  int i;
  GLubyte *imageData;
  //printf("loading caustics:");
  for (i=0; i<NUM_PATTERNS; i += causticIncrement) {
    char filename[80];
    sprintf(filename, "caust%02d.bw", i);
    //printf(" %d", i);
    fflush(stdout);
    imageData = read_alpha_texture((char *)filename, &width, &height);
    if (imageData == NULL) {
      fprintf(stderr, "\n%s: could not load image file\n", filename);
      exit(1);
    }
    if (HaveTexObj)
      glBindTexture(GL_TEXTURE_2D, i+1);
    else
      glNewList(i+101, GL_COMPILE);
    if (useMipmaps) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      gluBuild2DMipmaps(GL_TEXTURE_2D, GL_LUMINANCE, width, height,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);
    } else {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, height, width, 0,
        GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);
    }
    free(imageData);
    if (!HaveTexObj)
      glEndList();
  }

  /* Load an RGB file for the floor texture. */
  //printf("loading RGB textures: floor");
  fflush(stdout);
  imageData = read_rgb_texture((char *)FLOOR_FILE, &width, &height);
  if (imageData == NULL) {
    fprintf(stderr, "%s: could not load image file\n", FLOOR_FILE);
    exit(1);
  }
  //printf(".\n");

  if (HaveTexObj)
    glBindTexture(GL_TEXTURE_2D, 100);
  else
    glNewList(100, GL_COMPILE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  if (useMipmaps) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 256, 256, GL_RGB,
      GL_UNSIGNED_BYTE, imageData);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0,
      GL_RGB, GL_UNSIGNED_BYTE, imageData);
  }
  //free(imageData);
  if (!HaveTexObj)
    glEndList();
}
