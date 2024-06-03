//--------------------------------------------------
// Author: Yue Chen
// Date: 02/28/2023
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include "plymesh.h"
#include "osutils.h"

using namespace std;
using namespace glm;
using namespace agl;

class MeshViewer : public Window {
public:
   MeshViewer() : Window() {
   }

   void setup() {

      filename = "../models/"+filenames[index];
      mesh.load(filename);
      //mesh.load("../models/cow-uvs.ply"); for debugging
   }

   void mouseMotion(int x, int y, int dx, int dy) {
      if (mouseIsDown(GLFW_MOUSE_BUTTON_LEFT)){
         azimuth+=(dx/2);
         elevation+=(dy/2);
         if(azimuth<-180){
            azimuth+=360;
         }
         if(elevation<-90){
            elevation = -90;
         }
         if(azimuth>180){
            azimuth-=360;
         }
         if(elevation>90){
            elevation = 90;
         }
         
         updateEyePos();
      }
   }

   void mouseDown(int button, int mods) {
   }

   void mouseUp(int button, int mods) {

   }

   void scroll(float dx, float dy) {
   }

   void keyUp(int key, int mods) {
      if(key==GLFW_KEY_N){
         index++;
         if(index>=filenames.size()){
            index = 0;
         }
         eyePos = vec3(10, 0, 0);
         x=0;y=0;z=0;
         _scale = 1.0f;
         string filename = "../models/"+filenames[index];
         mesh.clear();
         mesh = PLYMesh(filename);
      }      
      if(key==GLFW_KEY_P){
         index--;
         if(index<0){
            index = filenames.size()-1;
         }
         eyePos = vec3(10, 0, 0);
         elevation = 0; azimuth = 0;
         x=0;y=0;z=0;
         _scale = 1.0f;
         string filename = "../uv-models/"+filenames[index];
         mesh.clear();
         mesh = PLYMesh(filename);
      }
      
      if(key==GLFW_KEY_UP){
         _scale = _scale +0.1f;
      }
      if(key==GLFW_KEY_DOWN){
         _scale = _scale/1.5f;
         if(_scale <0.01f){
            _scale = 0.01f;
         }
      }
      if(key==GLFW_KEY_Q){
         y++;
      }
      if(key==GLFW_KEY_E){
         y--;
      }
      if(key==GLFW_KEY_A){
         x++;
      }
      if(key==GLFW_KEY_D){
         x--;
      }
      if(key==GLFW_KEY_S){
         shader++;
         if(shader>=shaders.size()){
            shader = 0;
         }
      }
      if(key==GLFW_KEY_W){
         texture++;
         if(texture>=textures.size()){
            texture = 0;
         }
      }
   }

   void updateEyePos(){
         float pie = 3.1415926;
         float eyeX = 10* cos(azimuth/360.0*2*pie)*cos(elevation/360.0*2*pie);
         float eyeY = 10* sin(elevation/360.0*2*pie);
         float eyeZ = 10* sin(azimuth/360.0*2*pie)*cos(elevation/360.0*2*pie);
         eyePos = vec3(eyeX,eyeY,eyeZ);
   }


   void draw() {
      //renderer.beginShader(shaders[shader]);
      renderer.beginShader("phong-vertex"); 
     // renderer.texture("diffuseTexture",textures[texture]);

      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.lookAt(eyePos, lookPos, up);

      GLfloat maxX = mesh.maxBounds()[0];
      GLfloat maxY = mesh.maxBounds()[1];
      GLfloat maxZ = mesh.maxBounds()[2];
      GLfloat minX = mesh.minBounds()[0];
      GLfloat minY = mesh.minBounds()[1];
      GLfloat minZ = mesh.minBounds()[2];
      GLfloat dx = maxX - minX;
      GLfloat dy = maxY - minY;
      GLfloat dz = maxZ - minZ;
      GLfloat maxd = std::max(dz,dy);
      maxd = std::max(dx/2,maxd);
      GLfloat scale =  6.0f/maxd;
      renderer.rotate(vec3(0,0,0));   
      renderer.scale(vec3(_scale*scale,_scale*scale,_scale*scale)); 
      GLfloat mx = -(maxX+minX)/2.0f;
      GLfloat my = -(maxY+minY)/2.0f;
      GLfloat mz = -(maxZ+minZ)/2.0f;
      renderer.translate(vec3(mx+x,my+y,mz+z));

      renderer.mesh(mesh);
      //renderer.cube(); // for debugging!
      renderer.endShader();
   }

   
protected:
   PLYMesh mesh;
   vec3 eyePos = vec3(10, 0, 0);
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
   int index = 0;
   int shader = 0;
   int texture = 0;
   float _scale = 1.0f;
   float x=0.0f;
   float y=0.0f;
   float z=0.0f;
   int azimuth = 0;
   int elevation = 0;
   string filename;
   std::vector<string> filenames = GetFilenamesInDir("../models", "ply");
   std::vector<string> shaders = {"normals","phong-vertex","phong-pixel","phong-textures","toon"};
   //std::vector<string> textures = {"bacteria","circles","daisy_1","lines"};
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

