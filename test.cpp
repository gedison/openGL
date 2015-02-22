#define GL_GLEXT_PROTOTYPES
#include <stdio.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>

using namespace std;

#define VPASSES 10
#define JITTER 0.01

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

float eye[3]={0.0,1.0,6.0};

double genRand(){
   return (((double)(random()+1))/2147483649.);
}

struct point cross(const struct point& u, const struct point& v){
   struct point w;
   w.x = u.y*v.z - u.z*v.y;
   w.y = -(u.x*v.z - u.z*v.x);
   w.z = u.x*v.y - u.y*v.x;
   return(w);
}

struct point unit_length(const struct point& u){
   double length;
   struct point v;
   length = sqrt(u.x*u.x+u.y*u.y+u.z*u.z);
   v.x = u.x/length;
   v.y = u.y/length;
   v.z = u.z/length;
   return(v);
}

void viewVolume(){
   glEnable(GL_DEPTH_TEST);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60.0, 1.0, 0.01, 1000.0);
   glMatrixMode(GL_MODELVIEW);
}

void jitter_view(){
   glLoadIdentity();

   struct point jEye, view, up, vdir, utemp, vtemp;
   jEye.x = eye[0]; jEye.y = eye[1]; jEye.z = eye[2];
   view.x=JITTER*genRand(); view.y=JITTER*genRand(); view.z=JITTER*genRand();
   up.x=0.0; up.y=1.0; up.z=0.0;
   vdir.x = view.x - jEye.x; vdir.y = view.y - jEye.y; vdir.z = view.z - jEye.z;
   //up vector orthogonal to vdir, in plane of vdir and (0,1,0)
   vtemp = cross(vdir, up);
   utemp = cross(vtemp, vdir);
   up = unit_length(utemp);

   gluLookAt(jEye.x,jEye.y,jEye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}
/*
void draw(){
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glDrawArrays(GL_TRIANGLES,0,sides);
   glutSwapBuffers();
}
*/

void draw(){
   int view_pass;
   glClear(GL_ACCUM_BUFFER_BIT);
   for(view_pass=0; view_pass < VPASSES; view_pass++){
      jitter_view();
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLES,0,sides);
      //glFlush();
      glAccum(GL_ACCUM, 1.0/(float)(VPASSES));
   }
   glAccum(GL_RETURN, 1.0);
   glFlush();
}

void update(){
   //usleep(1000);
   glTranslatef(.5,0,.5);
   glRotatef(1.0,0.0,1.0,0.0);
   glTranslatef(-.5,0,-.5);
   glutPostRedisplay();
}

float light_position[3]={-3,5,-6};

void lights(){
   float key_ambient[]={0,0,0,0};
   float key_diffuse[]={.6,.6,.6,0};
   float key_specular[]={.2,.2,.2,0};
   float key_position[]={-3,5,6,1};
   float key_direction[]={3,-5,-6,1};
   
   float fill_ambient[]={0,0,0,0};
   float fill_diffuse[]={.4,.4,.4,0};
   float fill_specular[]={.2,.2,.2,0};
   float fill_position[]={2,4.5,7.5,1};
   float fill_direction[]={-2,-4.5,-7.5,1};
   
   float back_ambient[]={0,0,0,0};
   float back_diffuse[]={.3,.3,.3,0};
   float back_specular[]={.1,.1,.1,0};
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

   
   /*glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_LIGHT2);*/
   
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
   glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_ACCUM);
   glutInitWindowSize(512,512);
   glutInitWindowPosition(100,50);
   glutCreateWindow("test");
   glClearColor(.35,.35,.35,0);
   glClearAccum(0.0,0.0,0.0,0.0);

   viewVolume();
   jitter_view();
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

char *read_shader_program(char *filename) 
{
   FILE *fp;
   char *content = NULL;
   int fd, count;
   fd = open(filename,O_RDONLY);
   count = lseek(fd,0,SEEK_END);
   close(fd);
   content = (char *)calloc(1,(count+1));
   fp = fopen(filename,"r");
   count = fread(content,sizeof(char),count,fp);
   content[count] = '\0';
   fclose(fp);
   return content;
}

unsigned int set_shaders(){
   GLint vertCompiled, fragCompiled;
   char *vs, *fs;
   GLuint v, f, p;

   v = glCreateShader(GL_VERTEX_SHADER);
   f = glCreateShader(GL_FRAGMENT_SHADER);
   vs = read_shader_program("phong.vert");
   fs = read_shader_program("phong.frag");
   glShaderSource(v,1,(const char **)&vs,NULL);
   glShaderSource(f,1,(const char **)&fs,NULL);
   free(vs);
   free(fs); 

   glCompileShader(v);
   GLint shaderCompiled;
   glGetShaderiv(v, GL_COMPILE_STATUS, &shaderCompiled);
   if(shaderCompiled == GL_FALSE){
      cout<<"ShaderCasher.cpp"<<"loadShader(s)"<<"Shader did not compile"<<endl;
         int infologLength = 0;

         int charsWritten  = 0;
         char *infoLog;
         glGetShaderiv(v, GL_INFO_LOG_LENGTH,&infologLength);

         if (infologLength > 0){
            infoLog = (char *)malloc(infologLength);
            glGetShaderInfoLog(v, infologLength, &charsWritten, infoLog);
            string log = infoLog;
            cout<<log<<endl;
         }
   }

   glCompileShader(f);
   glGetShaderiv(f, GL_COMPILE_STATUS, &shaderCompiled);
   if(shaderCompiled == GL_FALSE){
      cout<<"ShaderCasher.cpp"<<"loadShader(f)"<<"Shader did not compile"<<endl;
         int infologLength = 0;

         int charsWritten  = 0;
         char *infoLog;
         glGetShaderiv(f, GL_INFO_LOG_LENGTH,&infologLength);

         if (infologLength > 0){
            infoLog = (char *)malloc(infologLength);
            glGetShaderInfoLog(f, infologLength, &charsWritten, infoLog);
            string log = infoLog;
            cout<<log<<endl;
         }
   }

   p = glCreateProgram();
   glAttachShader(p,f);
   glAttachShader(p,v);
   glLinkProgram(p);
   glUseProgram(p);
   return(p);
}

void set_uniform_parameters(unsigned int p)
{
   int location;
   location = glGetUniformLocation(p,"eye_position");
   glUniform3fv(location,1,eye);
   location = glGetUniformLocation(p,"light_position_1");
   glUniform3fv(location,1,light_position);
}


int main(int argc, char **argv){
   srandom(123456789);
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
   }sides=verticeSize/3;

   initOGL(argc, argv);

   glutKeyboardFunc(keyboard);
   glutMouseFunc(mouse);
   glutMotionFunc(mouseMotion);

   set_shaders();
   glutDisplayFunc(draw);
   glutIdleFunc(update);
   glutMainLoop();
   return 0;
}

