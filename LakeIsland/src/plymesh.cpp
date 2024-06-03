//--------------------------------------------------
// Author: Yue Chen
// Date: 02/28/2023
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#include "plymesh.h"
#include <fstream>

using namespace std;
using namespace glm;

namespace agl {

   PLYMesh::PLYMesh(const std::string& filename) {
      load(filename);
   }

   PLYMesh::PLYMesh() {
   }

   void PLYMesh::init() {
      assert(_positions.size() != 0);
      initBuffers(&_faces, &_positions, &_normals, &_uvs);
   }

   PLYMesh::~PLYMesh() {
   }

   bool PLYMesh::load(const std::string& filename) {
      if (_positions.size() != 0) {
         std::cout << "WARNING: Cannot load different files with the same PLY mesh\n";
         return false;
      }
      std::ifstream myfile (filename);

      if(myfile.is_open()){
         string word;
         std::string line;
         //check whether is a ply file
         myfile >> word;
         if(word!="ply"){
            std::cout << "Cannot load files other than Ply\n";
            return false;
         }
         //find the number of vertices
         int property = 0;
         while(word!="vertex"){
            myfile >> word;
            
         }
         myfile >> word;
         _vertexNum = stoi(word);
         //find the number of faces
         while(word!="face"){
            if(word=="property"){
               property++;
            }
            myfile >> word;
         }
         myfile >> word;
         _faceNum = stoi(word);
         //find the end of the header
         while(word!="end_header"){
            myfile >> word;
         }
         for(int i = 0; i<_vertexNum;i++){
            myfile>>word;
            _positions.push_back(stof(word));
            if(stof(word)>_maxBound[0]){
               _maxBound[0]=stof(word);
            }if(stof(word)<_minBound[0]){
               _minBound[0]=stof(word);
            }
            myfile>>word;
            _positions.push_back(stof(word));
            if(stof(word)>_maxBound[1]){
               _maxBound[1]=stof(word);
            }if(stof(word)<_minBound[1]){
               _minBound[1]=stof(word);
            }
            myfile>>word;
            _positions.push_back(stof(word));
            if(stof(word)>_maxBound[2]){
               _maxBound[2]=stof(word);
            }if(stof(word)<_minBound[2]){
               _minBound[2]=stof(word);
            }
            myfile>>word;
            _normals.push_back(stof(word));
            myfile>>word;
            _normals.push_back(stof(word));
            myfile>>word;
            _normals.push_back(stof(word));

            if(property > 6){
               myfile>>word;
               _uvs.push_back(stof(word));
               myfile>>word;
               _uvs.push_back(stof(word));
            }
            std::getline(myfile,line);
         }         
         while(myfile>>word){
            myfile>>word;
            _faces.push_back(stoi(word));
            myfile>>word;
            _faces.push_back(stoi(word));
            myfile>>word;
            _faces.push_back(stoi(word));
         }
      }
      myfile.close();
      return true;
   }

   glm::vec3 PLYMesh::minBounds() const {
      return _minBound;
   }

   glm::vec3 PLYMesh::maxBounds() const {
      return _maxBound;
   }

   int PLYMesh::numVertices() const {
      return _vertexNum;
   }

   int PLYMesh::numTriangles() const {
      return _faceNum;
   }

   const std::vector<GLfloat>& PLYMesh::positions() const {
      return _positions;
   }

   const std::vector<GLfloat>& PLYMesh::normals() const {
      return _normals;
   }

   const std::vector<GLuint>& PLYMesh::indices() const {
      return _faces;
   }

   const std::vector<GLfloat>& PLYMesh::uvs() const {
      return _uvs;
   }

   void PLYMesh::clear() {
      _positions.clear();
      _normals.clear();
      _faces.clear();
      _uvs.clear();
      _maxBound = glm::vec3{FLT_MIN,FLT_MIN,FLT_MIN};
      _minBound = glm::vec3 {FLT_MAX,FLT_MAX,FLT_MAX};
   }
}
