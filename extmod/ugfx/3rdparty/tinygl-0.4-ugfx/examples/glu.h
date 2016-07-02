
void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar );

typedef struct {
  int draw_style;
} GLUquadricObj;

#define GLU_LINE 0

GLUquadricObj *gluNewQuadric(void);
void gluQuadricDrawStyle(GLUquadricObj *obj, int style);

void gluSphere(GLUquadricObj *qobj,
               float radius,int slices,int stacks);
void gluCylinder( GLUquadricObj *qobj,
                  GLdouble baseRadius, GLdouble topRadius, GLdouble height,
                  GLint slices, GLint stacks );
void gluDisk( GLUquadricObj *qobj,
              GLdouble innerRadius, GLdouble outerRadius,
              GLint slices, GLint loops );

void drawTorus(float rc, int numc, float rt, int numt);
