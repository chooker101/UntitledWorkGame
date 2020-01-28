template<typename TFPTR, typename ...ARGS>
inline void hal::DrawBuffer::RecordVulkanCommands(TFPTR&& vkCmd, ARGS&& ...args)
{
	for (const DrawInfo* di : vDrawInfos)
	{
		DrawCommand::mCurrentDrawInfo = di;
		vkCmd(mCommandBuffer, args...);
	}
}

template<typename TFPTR, typename ...ARGS>
inline void hal::DrawBuffer::RecordSingleVulkanCommand(uint32_t index, TFPTR&& vkCmd, ARGS&& ...args)
{
	DrawCommand::mCurrentDrawInfo = vDrawInfos[index];
	vkCmd(mCommandBuffer, args...);
}