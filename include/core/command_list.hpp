#pragma once

#include "common.hpp"
#include "pipeline_state.hpp"

namespace vee
{
    class CommmandList
    {
    public:
        void SetPipeline(const RefPtr<PipelineState>& pipelineState) {m_pipelineState = pipelineState; }

        // Do all the render command bullshit here.
        void DrawIndexed() //todo

        RefPtr<PipelineState> m_pipelineState;
    }
}