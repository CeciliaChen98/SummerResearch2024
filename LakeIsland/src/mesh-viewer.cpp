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

      //filename = "../models/"+filenames[index];

      renderer.loadTexture("bacteria", "../textures/bacteria.png",0);

      renderer.loadShader("phong-vertex", "../shaders/phong-vertex.vs", "../shaders/phong-vertex.fs");

      mesh_island_small.load("../models/island_small.ply");
      mesh_island_big.load("../models/island_big.ply");
      mesh_house.load("../models/house.ply");
      mesh_tree.load("../models/tree.ply");
      mesh_bridge.load("../models/bridge.ply");
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
         float eyeX = 10* cos(azimuth/360.0*2*pie)*cos(elevation/360.0*2*pie);
         float eyeY = 10* sin(elevation/360.0*2*pie);
         float eyeZ = 10* sin(azimuth/360.0*2*pie)*cos(elevation/360.0*2*pie);
         eyePos = vec3(eyeX,eyeY,eyeZ);
   }
   
   
   void draw() {
      renderer.beginShader(shaders[shader]);
      //renderer.beginShader("phong-vertex"); 
      renderer.texture("diffuseTexture",textures[texture]);

      float aspect = ((float)width()) / height();
      renderer.perspective(glm::radians(60.0f), aspect, 0.1f, 50.0f);
      renderer.lookAt(eyePos, lookPos, up);

      GLfloat maxX = mesh_island_big.maxBounds()[0];
      GLfloat maxY = mesh_island_big.maxBounds()[1];
      GLfloat maxZ = mesh_island_big.maxBounds()[2];
      GLfloat minX = mesh_island_big.minBounds()[0];
      GLfloat minY = mesh_island_big.minBounds()[1];
      GLfloat minZ = mesh_island_big.minBounds()[2];
      GLfloat dx = maxX - minX;
      GLfloat dy = maxY - minY;
      GLfloat dz = maxZ - minZ;
      GLfloat maxd = std::max(dz,dy);
      maxd = std::max(dx/2,maxd);
      GLfloat scale =  6.0f/maxd;
      renderer.rotate(vec3(-1,pie/2,0));   
      renderer.scale(vec3(_scale*scale,_scale*scale,_scale*scale)); 
      GLfloat mx = -(maxX+minX)/2.0f;
      GLfloat my = -(maxY+minY)/2.0f;
      GLfloat mz = -(maxZ+minZ)/2.0f;
      renderer.translate(vec3(mx+x,my+y,mz+z));

      vec3 color_island = vec3(0.71f, 0.96f, 0.46f);  
      vec3 color_tree = vec3(0.28f, 0.42f, 0.14f);    
      vec3 color_bridge = vec3(0.6f, 0.4f, 0.4f);  // Example color for bridge
      vec3 color_house = vec3(0.8f, 0.5f, 0.2f);   // Example color for house

      // Draw each model with its color
      renderer.setUniform("ModelColor", color_island);
      renderer.mesh(mesh_island_big);
      renderer.setUniform("ModelColor", color_island);
      renderer.mesh(mesh_island_small);
      renderer.setUniform("ModelColor", color_tree);
      renderer.mesh(mesh_tree);
      renderer.mesh(mesh_bridge);

      renderer.scale(vec3(0.8,0.8,0.8)); 
      renderer.mesh(mesh_house);

      //renderer.cube(); // for debugging!
      renderer.endShader();
   }

   
protected:
   PLYMesh mesh_island_small;
   PLYMesh mesh_island_big;
   PLYMesh mesh_house;
   PLYMesh mesh_tree;
   PLYMesh mesh_bridge;
   vec3 eyePos = vec3(10, 0, 0);
   vec3 lookPos = vec3(0, 0, 0);
   vec3 up = vec3(0, 1, 0);
   int index = 0;
   int shader = 0;
   int texture = 0;
   float _scale = 1.0f;
   float x=0.0f;
   float y=0.0f;
   float pie = 3.1415926;
   float z=0.0f;
   int azimuth = 0;
   int elevation = 0;
   string filename;
   /*vector<std::string> faces = {
      FileSystem::getPath("../textures/top.jpg"),
   };*/
   std::vector<string> filenames = GetFilenamesInDir("../models", "ply");
   std::vector<string> shaders = {"phong-vertex"};
   std::vector<string> textures = {"bacteria"};
};

int main(int argc, char** argv)
{
   MeshViewer viewer;
   viewer.run();
   return 0;
}

