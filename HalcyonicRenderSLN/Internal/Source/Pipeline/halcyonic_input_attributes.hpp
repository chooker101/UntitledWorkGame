#pragma once

namespace hal
{
	class InputAttributes
	{
	public:
		uint32_t mOffset;
		VkFormat mFormat;

		InputAttributes(VkFormat inputFormat, uint32_t offset)
		{
			mFormat = inputFormat;
			mOffset = offset;
		}

		//A helper function to get the offset of an item in your vertex
		template<typename TRenderVertex, typename TInputMember>
		inline static uint32_t CalculateOffset();
	};

	template<typename TRenderVertex, typename TInputMember>
	uint32_t InputAttributes::CalculateOffset()
	{
		return offsetof(TRenderVertex, TInputMember);
	}
}