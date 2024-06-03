//--------------------------------------------------
// Author: Yue Chen
// Date: 02/28/2023
// Description: Loads PLY files in ASCII format
//--------------------------------------------------

#ifndef plymeshmodel_H_
#define plymeshmodel_H_

#include "agl/aglm.h"
#include "agl/mesh/triangle_mesh.h"

namespace agl {
   class PLYMesh : public TriangleMesh
   {
   public:

      PLYMesh(const std::string& filename);
      PLYMesh();

      virtual ~PLYMesh();

      // Initialize this object with the given file
      // Returns true if successfull. false otherwise.
      bool load(const std::string& filename);

      // Return the minimum point of the axis-aligned bounding box
      glm::vec3 minBounds() const;

      // Return the maximum point of the axis-aligned bounding box
      glm::vec3 maxBounds() const;

      // Return number of vertices in this model
      int numVertices() const;

      // Positions in this model
      const std::vector<GLfloat>& positions() const;

      // Positions in this model
      const std::vector<GLfloat>& normals() const;

      // Uvs in this model
      const std::vector<GLfloat>& uvs() const;

      // Return number of faces in this model
      int numTriangles() const;

      // face indices in this model
      const std::vector<GLuint>& indices() const;

      void clear();
      

   protected:
      void init();

   protected:
      int _faceNum;
      int _vertexNum;
      glm::vec3 _maxBound = glm::vec3{FLT_MIN,FLT_MIN,FLT_MIN};
      glm::vec3 _minBound = glm::vec3 {FLT_MAX,FLT_MAX,FLT_MAX};
      std::vector<GLfloat> _positions;
      std::vector<GLfloat> _normals;
      std::vector<GLuint> _faces;
      std::vector<GLfloat> _uvs;
   };
}

#endif
