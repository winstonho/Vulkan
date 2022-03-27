#pragma once

#include <vector>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
//#include "TextureLoader.h"

namespace VkHelper {
    class model_loader
    {
    public:

        bool Load(std::string Filename) noexcept;
        //void Draw( xgpu::cmd_buffer& CommandBuffer ) noexcept;
        Mesh* GetFirstMesh();
    private:

        Mesh					ProcessMesh(const aiMesh& Mesh, const aiScene& Scene) noexcept;
        void					ProcessNode(const aiNode& Node, const aiScene& scene) noexcept;

        std::vector<Mesh>		m_Meshes{};
    };
}




