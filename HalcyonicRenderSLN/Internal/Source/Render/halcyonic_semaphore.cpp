#include <precompiled.hpp>
#include <Render/halcyonic_render.hpp>
#include <Render/halcyonic_semaphore.hpp>

using namespace hal;

hal::Semaphore::Semaphore(std::vector<Semaphore*> waitOn, std::vector<Semaphore*> signalTo)
{
	vWaitOn.reserve(waitOn.size());
	for (const auto wait : waitOn)
	{
		AddWaitSemaphore(wait);
	}

	vSignalTo.reserve(signalTo.size());
	for (const auto signal : signalTo)
	{
		AddSignalSemaphore(signal);
	}
}

void hal::Semaphore::AddWaitSemaphore(Semaphore * waitOn)
{
	vWaitOn.push_back(waitOn->mSemaphore);
}

void hal::Semaphore::AddSignalSemaphore(Semaphore * signalTo)
{
	vSignalTo.push_back(signalTo->mSemaphore);
}

const std::vector<VkSemaphore>& hal::Semaphore::GetWaitSemaphores() const
{
	return vWaitOn;
}

const std::vector<VkSemaphore>& hal::Semaphore::GetSignalSemaphores() const
{
	return vSignalTo;
}
