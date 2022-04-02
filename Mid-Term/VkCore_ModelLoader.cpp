#include "VkCore_ModelLoader.h"



    //----------------------------------------------------------------------------------------------------
namespace VkHelper {
    bool model_loader::Load(std::string Filename) noexcept
    {
        auto Importer = std::make_unique<Assimp::Importer>();

        const aiScene* pScene = Importer->ReadFile(Filename
            , aiProcess_Triangulate                // Make sure we get triangles rather than nvert polygons
            | aiProcess_LimitBoneWeights           // 4 weights for skin model max
            | aiProcess_GenUVCoords                // Convert any type of mapping to uv mapping
            | aiProcess_TransformUVCoords          // preprocess UV transformations (scaling, translation ...)
            | aiProcess_FindInstances              // search for instanced meshes and remove them by references to one master
            | aiProcess_CalcTangentSpace           // calculate tangents and bitangents if possible
            | aiProcess_JoinIdenticalVertices      // join identical vertices/ optimize indexing
            | aiProcess_RemoveRedundantMaterials   // remove redundant materials
            | aiProcess_FindInvalidData            // detect invalid model data, such as invalid normal vectors
        );

        if (pScene == nullptr)
            return true;

        ProcessNode(*pScene->mRootNode, *pScene);

        return false;
    }

    Mesh* model_loader::GetFirstMesh()
    {
        if (this->m_Meshes.size() == 0)
            return nullptr;

        return &m_Meshes[0];
    }

    //----------------------------------------------------------------------------------------------------

    Mesh model_loader::ProcessMesh(const aiMesh& mesh, const aiScene& Scene) noexcept
    {
        // Data to fill
        std::vector<Vertex>			Vertices;
        std::vector<unsigned int>	Indices;

        // Walk through each of the mesh's vertices
        for (auto i = 0u; i < mesh.mNumVertices; ++i)
        {
            Vertex Vertex;

            Vertex.pos = glm::vec3
            (static_cast<float>(mesh.mVertices[i].x)
                , static_cast<float>(mesh.mVertices[i].y)
                , static_cast<float>(mesh.mVertices[i].z)
            );

            if (mesh.mTextureCoords[0])
            {
                Vertex.uv = glm::vec2
                (static_cast<float>(mesh.mTextureCoords[0][i].x)
                    , static_cast<float>(mesh.mTextureCoords[0][i].y)
                );
            }

            Vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);

            if (mesh.mTangents)
            {
                Vertex.tangents = glm::vec3
                (static_cast<float>(mesh.mTangents->x)
                    , static_cast<float>(mesh.mTangents->y),
                    static_cast<float>(mesh.mTangents->z)
                );
            }


            if (mesh.mNormals)
            {
                Vertex.normal = glm::vec3
                (static_cast<float>(mesh.mNormals->x)
                    , static_cast<float>(mesh.mNormals->y),
                    static_cast<float>(mesh.mNormals->z)
                );
            }

            Vertices.push_back(Vertex);
        }

        // Walk thourgh the faces
        for (auto i = 0u; i < mesh.mNumFaces; ++i)
        {
            const auto& Face = mesh.mFaces[i];

            for (auto j = 0u; j < Face.mNumIndices; ++j)
                Indices.push_back(Face.mIndices[j]);
        }

        // Handle materials

        /*
        if( Mesh.mMaterialIndex >= 0 )
        {
            const aiMaterial& Material = *Scene.mMaterials[ Mesh.mMaterialIndex ];

            std::vector<texture> DiffuseMaps = LoadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse", Scene );

            Textures.insert( Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end() );
        }
        */

        return Mesh{ Vertices, Indices };
    }

    //----------------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------------

    void model_loader::ProcessNode(const aiNode& Node, const aiScene& Scene) noexcept
    {
        for (auto i = 0u, end = Node.mNumMeshes; i < end; ++i)
        {
            aiMesh* pMesh = Scene.mMeshes[Node.mMeshes[i]];
            m_Meshes.push_back(ProcessMesh(*pMesh, Scene));
        }

        for (auto i = 0u; i < Node.mNumChildren; ++i)
        {
            ProcessNode(*Node.mChildren[i], Scene);
        }
    }

    //----------------------------------------------------------------------------------------------------
}