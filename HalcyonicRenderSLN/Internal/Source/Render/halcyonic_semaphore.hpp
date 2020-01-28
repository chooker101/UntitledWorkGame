#pragma once

namespace hal
{
	class Semaphore
	{
	private:
		VkSemaphore mSemaphore;
		std::vector<VkSemaphore> vWaitOn;
		std::vector<VkSemaphore> vSignalTo;
	public:
		Semaphore() = default;
		Semaphore(std::vector<Semaphore*> waitOn, std::vector<Semaphore*> signalTo);
		void AddWaitSemaphore(Semaphore* waitOn);
		void AddSignalSemaphore(Semaphore* signalTo);
		const std::vector<VkSemaphore>& GetWaitSemaphores() const;
		const std::vector<VkSemaphore>& GetSignalSemaphores() const;
	};
}