#include <precompiled.hpp>
#include <DrawInfo/halcyonic_draw_buffer.hpp>
#include <Render/halcyonic_semaphore.hpp>
#include <Render/halcyonic_render.hpp>
#include <Render/halcyonic_render_info.hpp>

using namespace hal;

hal::RenderInfo::RenderInfo(std::vector<DrawBuffer*> drawBuffers) : vDrawCommandBuffers(std::move(drawBuffers))
{
}

hal::RenderInfo::RenderInfo(std::vector<DrawBuffer*> drawBuffers, Semaphore * bufferSemaphore) : vDrawCommandBuffers(std::move(drawBuffers)), mSemaphore(bufferSemaphore)
{
}

uint32_t hal::RenderInfo::AddDrawBuffer(DrawBuffer* drawBuffers)
{
	vDrawCommandBuffers.push_back(drawBuffers);
	return static_cast<uint32_t>(vDrawCommandBuffers.size() - 1);
}

const DrawBuffer* hal::RenderInfo::GetDrawBuffer(uint32_t index) const
{
	return vDrawCommandBuffers[index];
}

const VkSubmitInfo& hal::RenderInfo::GetSubmitInfo() const
{
	return mSubmitInfo;
}

void hal::RenderInfo::SetSemaphore(Semaphore* bufferSemaphore)
{
	mSemaphore = bufferSemaphore;
}

void hal::RenderInfo::BuildRenderinfo()
{
	HALCYONIC_DEBUG((vDrawCommandBuffers.size() > 0), "RenderInfo: Draw Buffers are empty");

	if(vRawDrawBuffers.size() < vDrawCommandBuffers.size())
	{
		vRawDrawBuffers.reserve(vDrawCommandBuffers.size());
		for (uint32_t i = static_cast<uint32_t>(vRawDrawBuffers.size()); i < vDrawCommandBuffers.size(); ++i)
		{
			vRawDrawBuffers.push_back(vDrawCommandBuffers[i]->GetCommandBuffer());
		}
	}
	else if(vRawDrawBuffers.size() == vDrawCommandBuffers.size())
	{
		for (uint32_t i = static_cast<uint32_t>(vDrawCommandBuffers.size()); i < vDrawCommandBuffers.size(); ++i)
		{
			vRawDrawBuffers[i] = vDrawCommandBuffers[i]->GetCommandBuffer();
		}
	}
}

void hal::RenderInfo::BuildSubmitinfo()
{
	mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	mSubmitInfo.pWaitDstStageMask = &mWaitStageMask;
	mSubmitInfo.commandBufferCount = static_cast<uint32_t>(vRawDrawBuffers.size());
	mSubmitInfo.pCommandBuffers = vRawDrawBuffers.data();

	if (mSemaphore == nullptr)
	{
		mSubmitInfo.waitSemaphoreCount = 1;
		mSubmitInfo.pWaitSemaphores = &Render::Instance()->GetPresentComplete();
		mSubmitInfo.signalSemaphoreCount = 1;
		mSubmitInfo.pSignalSemaphores = &Render::Instance()->GetRenderComplete();
	}
	else
	{
		const auto& waitSemaphores = mSemaphore->GetWaitSemaphores();
		const auto& signalSemaphores = mSemaphore->GetSignalSemaphores();
		if (waitSemaphores.size() > 0)
		{
			mSubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
			mSubmitInfo.pWaitSemaphores = waitSemaphores.data();
		}
		else
		{
			mSubmitInfo.waitSemaphoreCount = 1;
			mSubmitInfo.pWaitSemaphores = &Render::Instance()->GetPresentComplete();
		}

		if (signalSemaphores.size() > 0)
		{
			mSubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
			mSubmitInfo.pSignalSemaphores = signalSemaphores.data();
		}
		else
		{
			mSubmitInfo.signalSemaphoreCount = 1;
			mSubmitInfo.pSignalSemaphores = &Render::Instance()->GetRenderComplete();
		}
	}
}
