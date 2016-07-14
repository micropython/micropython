#include <stdlib.h>
#include <math.h>
#include <GL/gl.h>
#include "glu.h"


void drawTorus(float rc, int numc, float rt, int numt)
{
    int i, j, k;
    double s, t;
    double x, y, z;
    double pi, twopi;

    pi = 3.14159265358979323846;
    twopi = 2 * pi;
 
    for (i = 0; i < numc; i++) {
	glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= numt; j++) {
	    for (k = 1; k >= 0; k--) {
		s = (i + k) % numc + 0.5;
		t = j % numt;

		x = cos(t*twopi/numt) * cos(s*twopi/numc);
		y = sin(t*twopi/numt) * cos(s*twopi/numc);
		z = sin(s*twopi/numc);
		glNormal3f(x, y, z);

		x = (rt + rc * cos(s*twopi/numc)) * cos(t*twopi/numt);
		y = (rt + rc * cos(s*twopi/numc)) * sin(t*twopi/numt);
		z = rc * sin(s*twopi/numc);
		glVertex3f(x, y, z);
	    }
        }
	glEnd();
    }
}

static void normal3f( GLfloat x, GLfloat y, GLfloat z )
{
   GLdouble mag;

   mag = sqrt( x*x + y*y + z*z );
   if (mag>0.00001F) {
      x /= mag;
      y /= mag;
      z /= mag;
   }
   glNormal3f( x, y, z );
}

void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

GLUquadricObj *gluNewQuadric(void)
{
  return NULL;
}

void gluQuadricDrawStyle(GLUquadricObj *obj, int style)
{
}

void gluCylinder( GLUquadricObj *qobj,
                  GLdouble baseRadius, GLdouble topRadius, GLdouble height,
                  GLint slices, GLint stacks )
{
   GLdouble da, r, dr, dz;
   GLfloat z, nz, nsign;
   GLint i, j;
   GLfloat du = 1.0 / slices;
   GLfloat dv = 1.0 / stacks;
   GLfloat tcx = 0.0, tcy = 0.0;
   
   nsign = 1.0;

   da = 2.0*M_PI / slices;
   dr = (topRadius-baseRadius) / stacks;
   dz = height / stacks;
   nz = (baseRadius-topRadius) / height;  /* Z component of normal vectors */

   for (i=0;i<slices;i++) {
	 GLfloat x1 = -sin(i*da);
	 GLfloat y1 = cos(i*da);
	 GLfloat x2 = -sin((i+1)*da);
	 GLfloat y2 = cos((i+1)*da);
	 z = 0.0;
	 r = baseRadius;
	 tcy = 0.0;
	 glBegin( GL_QUAD_STRIP );
	 for (j=0;j<=stacks;j++) {
	    if (nsign==1.0) {
	       normal3f( x1*nsign, y1*nsign, nz*nsign );
	       glTexCoord2f(tcx, tcy);
	       glVertex3f( x1*r, y1*r, z );
	       normal3f( x2*nsign, y2*nsign, nz*nsign );
	       glTexCoord2f(tcx+du, tcy);
	       glVertex3f( x2*r, y2*r, z );
	    }
	    else {
	       normal3f( x2*nsign, y2*nsign, nz*nsign );
	       glTexCoord2f(tcx, tcy);
	       glVertex3f( x2*r, y2*r, z );
	       normal3f( x1*nsign, y1*nsign, nz*nsign );
	       glTexCoord2f(tcx+du, tcy);
	       glVertex3f( x1*r, y1*r, z );
	    }
	    z += dz;
	    r += dr;
	    tcy += dv;
	 }
	 glEnd();
	 tcx += du;
      }
}

/* Disk (adapted from Mesa) */

void gluDisk( GLUquadricObj *qobj,
              GLdouble innerRadius, GLdouble outerRadius,
              GLint slices, GLint loops )
{
   GLdouble a, da;
   GLfloat dr;
   GLfloat r1, r2, dtc;
   GLint s, l;
   GLfloat sa,ca;

   /* Normal vectors */
	 glNormal3f( 0.0, 0.0, +1.0 );

   da = 2.0*M_PI / slices;
   dr = (outerRadius-innerRadius) / (GLfloat) loops;

   /* texture of a gluDisk is a cut out of the texture unit square */
   /* x, y in [-outerRadius, +outerRadius]; s, t in [0, 1] (linear mapping) */
   dtc = 2.0f * outerRadius;

   r1 = innerRadius;
   for (l=0;l<loops;l++) {
	    r2 = r1 + dr;
	       glBegin( GL_QUAD_STRIP );
	       for (s=0;s<=slices;s++) {
		  if (s==slices) a = 0.0;
		  else  a = s * da;
		  sa = sin(a); ca = cos(a);
                  glTexCoord2f(0.5+sa*r2/dtc,0.5+ca*r2/dtc);
                  glVertex2f( r2*sa, r2*ca );
                  glTexCoord2f(0.5+sa*r1/dtc,0.5+ca*r1/dtc);
                  glVertex2f( r1*sa, r1*ca );
	       }
	       glEnd();
	    r1 = r2;
	 }

}

/*
 * Sphère (adapted from Mesa)
 */

void gluSphere(GLUquadricObj *qobj,
               float radius,int slices,int stacks)
{
   float rho, drho, theta, dtheta;
   float x, y, z;
   float s, t, ds, dt;
   int i, j, imin, imax;
   int normals;
   float nsign;
  
   normals=1;
   nsign=1;

   drho = M_PI / (float) stacks;
   dtheta = 2.0 * M_PI / (float) slices;

  /* draw +Z end as a triangle fan */
  glBegin( GL_TRIANGLE_FAN );
  glNormal3f( 0.0, 0.0, 1.0 );
  glTexCoord2f(0.5,0.0);
  glVertex3f( 0.0, 0.0, nsign * radius );
  for (j=0;j<=slices;j++) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = -sin(theta) * sin(drho);
	 y = cos(theta) * sin(drho);
	 z = nsign * cos(drho);
	 if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	 glVertex3f( x*radius, y*radius, z*radius );
      }
   glEnd();


      ds = 1.0 / slices;
      dt = 1.0 / stacks;
      t = 1.0;  /* because loop now runs from 0 */
      if (1) {
        imin = 0;
        imax = stacks;
      }
      else {
        imin = 1;
        imax = stacks-1;
      }

      /* draw intermediate stacks as quad strips */
      for (i=imin;i<imax;i++) {
	 rho = i * drho;
	 glBegin( GL_QUAD_STRIP );
         s = 0.0;
	 for (j=0;j<=slices;j++) {
	    theta = (j==slices) ? 0.0 : j * dtheta;
	    x = -sin(theta) * sin(rho);
	    y = cos(theta) * sin(rho);
	    z = nsign * cos(rho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glTexCoord2f(s,1-t);
	    glVertex3f( x*radius, y*radius, z*radius );
	    x = -sin(theta) * sin(rho+drho);
	    y = cos(theta) * sin(rho+drho);
	    z = nsign * cos(rho+drho);
	    if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	    glTexCoord2f(s,1-(t-dt));
            s += ds;
	    glVertex3f( x*radius, y*radius, z*radius );
	 }
	 glEnd();
	 t -= dt;
      }

/* draw -Z end as a triangle fan */
    glBegin( GL_TRIANGLE_FAN );
    glNormal3f( 0.0, 0.0, -1.0 );
      glTexCoord2f(0.5,1.0);
      glVertex3f( 0.0, 0.0, -radius*nsign );
      rho = M_PI - drho;
      s = 1.0;
      t = dt;
      for (j=slices;j>=0;j--) {
	 theta = (j==slices) ? 0.0 : j * dtheta;
	 x = -sin(theta) * sin(rho);
	 y = cos(theta) * sin(rho);
	 z = nsign * cos(rho);
	 if (normals)  glNormal3f( x*nsign, y*nsign, z*nsign );
	 glTexCoord2f(s,1-t);
         s -= ds;
	 glVertex3f( x*radius, y*radius, z*radius );
      }
      glEnd();
}
