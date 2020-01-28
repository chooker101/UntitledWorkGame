#pragma once

namespace hal
{
	enum class BufferType;

	class Pipeline;
	class Buffer;
	class DrawInfo
	{
	private:
		const Pipeline* mPipeline;

		std::unordered_map<BufferType, std::vector<Buffer*>> m_Buffers = {};
	public:
		DrawInfo(const Pipeline* pipeline);

		void SetPipeline(const Pipeline* pipeline);

		uint32_t AddBuffer(Buffer* buffer);

		void UpdateBuffer(BufferType bufferType, uint32_t index, uint8_t* pData);

		const Buffer* GetBuffer(BufferType bufferType, uint32_t index) const;
	};
}