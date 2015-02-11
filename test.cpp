#define GL_GLEXT_PROTOTYPES
#include <stdio.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>
#include <unistd.h>

#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

float cameraAngleX=0, cameraAngleY=0, cameraDistance=0;
bool mouseLeftDown=false, mouseRightDown=false, mouseMiddleDown=false;
int mouseX=0, mouseY=0;

GLfloat *myVertices=NULL;
GLfloat *myNormals=NULL;
int verticeSize=0, normalSize=0, sides=0;

GLfloat VBObuff[1253340];

struct point{
   float x;
   float y;
   float z;
};

void viewVolume(){
   struct point eye;
   struct point view;
   struct point up;
   
   glEnable(GL_DEPTH_TEST);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, 1.0, 0.01, 1000.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   eye.x=0.0; eye.y=1.0; eye.z=7.0;
   view.x=0.0; view.y=0.0; view.z=0.0;
   up.x=0.0; up.y=1.0; up.z=0.0;

   gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void draw(){
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

/*  
   glTranslatef(0,0,-cameraDistance);
   glRotatef(cameraAngleX, 1, 0, 0);
   glRotatef(cameraAngleY, 0,1, 0);
*/

   glDrawArrays(GL_TRIANGLES,0,sides);
   glutSwapBuffers();
}

void update(){
   usleep(1000);
   glTranslatef(.5,0,.5);
   glRotatef(1.0,0.0,1.0,0.0);
   glTranslatef(-.5,0,-.5);
   glutPostRedisplay();
}

void lights(){
   float key_ambient[]={0,0,0,0};
   float key_diffuse[]={1,1,1,0};
   float key_specular[]={1,1,1,0};
   float key_position[]={-3,5,6,1};
   float key_direction[]={3,-5,-6,1};
   
   float fill_ambient[]={0,0,0,0};
   float fill_diffuse[]={.2,.2,.2,0};
   float fill_specular[]={.2,.2,.2,0};
   float fill_position[]={2,4.5,7.5,1};
   float fill_direction[]={-2,-4.5,-7.5,1};
   
   float back_ambient[]={0,0,0,0};
   float back_diffuse[]={.5,.5,.5,0};
   float back_specular[]={.5,.5,.5,0};
   float back_position[]={0,5,-6.5,1};
   float back_direction[]={-3,-5,-2,1};

   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,key_ambient);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1);
   
   glLightfv(GL_LIGHT0,GL_AMBIENT,key_ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,key_diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,key_specular);
   glLightf(GL_LIGHT0,GL_SPOT_EXPONENT, 1.0);
   glLightf(GL_LIGHT0,GL_SPOT_CUTOFF, 180.0);
   glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION, .5);
   glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION, .1);
   glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION, .01);
   glLightfv(GL_LIGHT0,GL_POSITION,key_position);
   glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,key_direction);
   
   glLightfv(GL_LIGHT1,GL_AMBIENT,fill_ambient);
   glLightfv(GL_LIGHT1,GL_DIFFUSE,fill_diffuse);
   glLightfv(GL_LIGHT1,GL_SPECULAR,fill_specular);
   glLightf(GL_LIGHT1,GL_SPOT_EXPONENT, 1.0);
   glLightf(GL_LIGHT1,GL_SPOT_CUTOFF, 180.0);
   glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION, .5);
   glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION, .1);
   glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION, .01);
   glLightfv(GL_LIGHT1,GL_POSITION,fill_position);
   glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,fill_direction);
   
   glLightfv(GL_LIGHT2,GL_AMBIENT,back_ambient);
   glLightfv(GL_LIGHT2,GL_DIFFUSE,back_diffuse);
   glLightfv(GL_LIGHT2,GL_SPECULAR,back_specular);
   glLightf(GL_LIGHT2,GL_SPOT_EXPONENT, 1.0);
   glLightf(GL_LIGHT2,GL_SPOT_CUTOFF, 180.0);
   glLightf(GL_LIGHT2,GL_CONSTANT_ATTENUATION, .5);
   glLightf(GL_LIGHT2,GL_LINEAR_ATTENUATION, .1);
   glLightf(GL_LIGHT2,GL_QUADRATIC_ATTENUATION, .01);
   glLightfv(GL_LIGHT2,GL_POSITION,back_position);
   glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,back_direction);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);
   
}

void material(){
   float mat_ambient[]={0,0,0,1};
   float mat_diffuse[]={.9,.2,.2,1};
   float mat_specular[] = {1,1,1,1};
   float mat_shininess[]={2.5};

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

GLuint mybuf = 1;
void initOGL(int argc, char **argv){
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE);
   glutInitWindowSize(512,512);
   glutInitWindowPosition(100,50);
   glutCreateWindow("test");
   glClearColor(.35,.35,.35,0);

   viewVolume();
   lights();
   material();
   
   glBindBuffer(GL_ARRAY_BUFFER, mybuf);
   glBufferData(GL_ARRAY_BUFFER, sizeof(VBObuff), VBObuff, GL_STATIC_DRAW);
   glVertexPointer(3, GL_FLOAT, 3*sizeof(GLfloat), NULL+0);

   glNormalPointer(GL_FLOAT, 3*sizeof(GLfloat), (GLvoid*)(NULL+626670*sizeof(GLfloat)));
   
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
}

void mouse(int button, int state, int x, int y){
   mouseX=x; mouseY=y;
   bool isDown=false;
   if(state == GLUT_DOWN)isDown=true;
   if(button == GLUT_LEFT_BUTTON){
      mouseLeftDown=isDown;
   }else if(button == GLUT_RIGHT_BUTTON){
      mouseRightDown=isDown;
   }else if(button == GLUT_MIDDLE_BUTTON){
      mouseMiddleDown=isDown;
   }
}

void mouseMotion(int x, int y){
   if(mouseLeftDown){
      cameraAngleY+=(x-mouseX);
      cameraAngleX+=(y-mouseY);
      mouseX=x;
      mouseY=y;
   }
   if(mouseRightDown){
      cameraDistance -=(y-mouseY)*.2f;
      mouseY=y;
   }
}

void keyboard(unsigned char key, int x, int y){
   switch(key){
      case 'q': 
        //glDeleteBuffers(2, mybuf);
        glDeleteBuffers(1, &mybuf);
        exit(1);
      default: break;  
   }
}

void timer(int millisec){
   glutTimerFunc(millisec, timer, millisec);
   glutPostRedisplay();
}

class vec2{
public:
   vec2() : x(0), y(0) {}
   vec2(const double inx, const double iny) : x(inx), y(iny) {}
   vec2(const vec2& in) : x(in.x), y(in.y) {}
   vec2& operator=(const vec2& in){
      if(this == &in) return *this;
      x = in.x;
      y = in.y;
      return *this;
   }
   double x, y;
};

class vec3{
public:
   vec3() : x(0), y(0), z(0) {}
   vec3(const double inx, const double iny, const double inz) : x(inx), y(iny), z(inz) {}
   vec3(const vec3& in) : x(in.x), y(in.y), z(in.z) {}
   vec3& operator=(const vec3& in){
      if(this == &in) return *this;
      x = in.x;
      y = in.y;
      z = in.z;
      return *this;
   }
   double x, y, z;
};

bool loadObj(string filename,
             vector <vec3> &vertices,
             vector <vec2> &uvs,
             vector <vec3> &normals)
{
   vector<unsigned int> vertexIndices, uvIndices, normalIndices;
   vector<vec3> temp_vertices;
   vector<vec2> temp_uvs;
   vector<vec3> temp_normals;

   ifstream ifs(filename.c_str());
   if(!ifs){
      cout << "Error opening file: " << filename << endl;
      return false;
   }
   
   string lineIn;
   while(ifs >> lineIn){
      if(lineIn.compare("v") == 0){
         vec3 vertex;
         ifs >> vertex.x >> vertex.y >> vertex.z;
         temp_vertices.push_back(vertex);
         //std::cout << "vert " << vertex << std::endl;
      }
      else if(lineIn.compare("vt") == 0){
         vec2 uv;
         ifs >> uv.x >> uv.y;
         temp_uvs.push_back(uv);
         //std::cout << "uv " << uv << std::endl;
      }
      else if(lineIn.compare("vn") == 0){
         vec3 normal;
         ifs >> normal.x >> normal.y >> normal.z;
         temp_normals.push_back(normal);
         //std::cout << "normal " << normal << std::endl;
      }
      //scan vertex//normal
      else if(lineIn.compare("f") == 0){
         unsigned int vertIndex[3], normIndex[3];
         ifs >> vertIndex[0];
         ifs.ignore(1,'/'); ifs.ignore(1,'/');
         ifs >> normIndex[0] >> vertIndex[1];
         ifs.ignore(1,'/'); ifs.ignore(1,'/');
         ifs >> normIndex[1] >> vertIndex[2];
         ifs.ignore(1,'/'); ifs.ignore(1,'/');
         ifs >> normIndex[2];
         vertexIndices.push_back(vertIndex[0]);
         vertexIndices.push_back(vertIndex[1]);
         vertexIndices.push_back(vertIndex[2]);
         normalIndices.push_back(normIndex[0]);
         normalIndices.push_back(normIndex[1]);
         normalIndices.push_back(normIndex[2]);
      }
   }

   for(unsigned int i=0; i<vertexIndices.size(); i++){
      unsigned int vertexIndex = vertexIndices[i];
      vec3 vertex = temp_vertices[vertexIndex-1];
      vertices.push_back(vertex);
   }
   for(unsigned int i=0; i<uvIndices.size(); i++){
      unsigned int uvIndex = uvIndices[i];
      vec2 uv = temp_uvs[uvIndex-1];
      uvs.push_back(uv);
   }
   for(unsigned int i=0; i<normalIndices.size(); i++){
      unsigned int normalIndex = normalIndices[i];
      vec3 normal = temp_normals[normalIndex-1];
      normals.push_back(normal);
   }
   
   return 1;
}

int main(int argc, char **argv){
   string filename= "bunny.obj";
   vector <vec3> vertices2;
   vector <vec2> uvs2;
   vector <vec3> normals2;
   if(!loadObj(filename, vertices2, uvs2, normals2)){
      cout << "Could not read object.\n";
      return 0;
   }

   for(unsigned int i = 0; i<vertices2.size(); i++){
      VBObuff[(i*3)]=vertices2[i].x;
      VBObuff[(i*3)+1]=vertices2[i].y;
      VBObuff[(i*3)+2]=vertices2[i].z;
      verticeSize+=3;
   }
   for(unsigned int i = 0; i<normals2.size(); i++){
      VBObuff[verticeSize+(i*3)]=normals2[i].x;
      VBObuff[verticeSize+(i*3)+1]=normals2[i].y;
      VBObuff[verticeSize+(i*3)+2]=normals2[i].z;
      normalSize+=3;
   }
   
   /*myVertices = new GLfloat[vertices2.size()*3] ;
   for(unsigned int i = 0; i<vertices2.size(); i++){
      myVertices[(i*3)]=vertices2[i].x;
      myVertices[(i*3)+1]=vertices2[i].y;
      myVertices[(i*3)+2]=vertices2[i].z;
      //verticeSize+=3;
   }

   myNormals = new GLfloat[normals2.size()*3];
   for(unsigned int i = 0; i<normals2.size(); i++){
      myNormals[(i*3)]=normals2[i].x;
      myNormals[(i*3)+1]=normals2[i].y;
      myNormals[(i*3)+2]=normals2[i].z;
      //normalSize+=3;
   }*/
   sides=verticeSize/3;

/*
  normalSize=0;verticeSize=0;
  myVertices=new GLfloat[12];
  for(int i=0; i<12; i++){verticeSize++;myVertices[i]=vertices[i];}
  myNormals=new GLfloat[12];
  for(int i=0; i<12; i++){normalSize++;myNormals[i]=normals[i];}
*/

   initOGL(argc, argv);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(mouseMotion);

   glutDisplayFunc(draw);
   glutIdleFunc(update);
   glutMainLoop();
   return 0;
}

