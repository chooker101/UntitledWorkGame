#include <precompiled.hpp>
#include <Buffer/halcyonic_buffer.hpp>
#include <Pipeline/halcyonic_pipeline.hpp>
#include <DrawInfo/halcyonic_draw_info.hpp>

using namespace hal;

hal::DrawInfo::DrawInfo(const Pipeline* pipeline) : mPipeline(pipeline)
{
}

void hal::DrawInfo::SetPipeline(const Pipeline* pipeline)
{
	if (pipeline)
	{
		mPipeline = pipeline;
	}
}

uint32_t hal::DrawInfo::AddBuffer(Buffer* buffer)
{
	auto& vector = m_Buffers[buffer->GetBufferType()];
	vector.push_back(buffer);
	return static_cast<uint32_t>(vector.size() - 1);
}

void hal::DrawInfo::UpdateBuffer(BufferType bufferType, uint32_t index, uint8_t* pData)
{
	m_Buffers[bufferType][index]->UpdateBuffer(pData);
}

const Buffer* hal::DrawInfo::GetBuffer(BufferType bufferType, uint32_t index) const
{
	return m_Buffers.at(bufferType)[index];
}
