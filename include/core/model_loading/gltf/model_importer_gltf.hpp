#pragma once
#include "core/common.hpp"
#include "core/model_loading/model_importer.hpp"

namespace vee
{
    class ModelImporterGltf : public ModelImporter
    {
    public:
        virtual void LoadImpl(const std::string& filePath) override;
    };
}