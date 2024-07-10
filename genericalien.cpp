#include "genericalien.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"


GenericAlien::GenericAlien()
{


}

void GenericAlien::loadAlien()
{
    std::string inputfile = /*"/Users/ismailkazmi/Qt/Examples/Qt-5.9.1/opengl/idea1_QtProject/alien1_OBJexport_1.obj"*/"/Users/ismailkazmi/Desktop/Research Project/Research/Other Ideas/Idea 1/models/male_model_4.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) { // `err` may contain warning message.
      qDebug() << err.c_str();
    }

    qDebug() << ret;
    if (!ret) {
      exit(1);
    }

    qDebug() << shapes.size(); // 0

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
      // Loop over faces(polygon)
      size_t index_offset = 0;
      for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
        int fv = shapes[s].mesh.num_face_vertices[f];

        // Loop over vertices in the face.
        for (size_t v = 0; v < fv; v++) {
          // access to vertex
          tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
          tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
          tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
          tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
//          tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
//          tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
//          tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
//          tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
//          tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
          // Optional: vertex colors
          // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
          // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
          // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

//          qDebug() << vx << "," << vy << "," << vz/* << "," << nx << "," << ny << "," << nz*/;
        }
        index_offset += fv;

        // per-face material
        shapes[s].mesh.material_ids[f];
      }
    }
}
