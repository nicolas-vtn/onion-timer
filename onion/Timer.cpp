#include "Timer.hpp"

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>

namespace onion
{
	// --------------------------- Constructors and Destructor ------------------------------

	Timer::Timer() = default;

	Timer::Timer(std::chrono::duration<double> elapsedPeriod, std::function<void(void)> timeoutFunction, bool repeat)
		: m_elapsedPeriod(std::chrono::duration_cast<std::chrono::steady_clock::duration>(elapsedPeriod)),
		  m_timeoutFunction(std::move(timeoutFunction)), m_repeat(repeat)
	{
	}

	Timer::~Timer()
	{
		Stop();
	}

	// --------------------------- Public Methods ------------------------------

	void Timer::Start()
	{
		assert(m_elapsedPeriod.count() > 0 && "Timer period must be > 0");

		// Already Started.
		if (m_timerThread.joinable())
		{
			return;
		}

		// Starts the wait loop in a dedicated thread.
		m_timerThread = std::jthread{[this](std::stop_token st) { this->WaitLoop(st); }};
	};

	void Timer::Stop()
	{
		if (!m_timerThread.joinable())
			return;

		if (std::this_thread::get_id() == m_timerThread.get_id())
		{
			// If the timer is stopping itself, it cannot join its own thread, so it just requests the stop and returns.
			// It will be automatically stopped after the wait loop finishes.
			m_timerThread.request_stop();
			return; // Does not self-join
		}

		m_timerThread.request_stop();
		m_timerThread.join();
	}

	void Timer::Restart()
	{
		Stop();
		Start();
	}

	// --------------------------- Private Methods ------------------------------
	void Timer::WaitLoop(std::stop_token st)
	{
		std::unique_lock lock(m_mutex);

		m_startTime = std::chrono::steady_clock::now();
		m_nextElapsed = m_startTime + m_elapsedPeriod;

		while (!st.stop_requested())
		{
			if (m_cv.wait_until(lock, m_nextElapsed, [&] { return st.stop_requested(); }))
				return;

			// Copy timeoutFunction under mutex to it's not locked during it's execution.
			auto callback = m_timeoutFunction;
			bool repeat = m_repeat;

			lock.unlock();

			if (callback)
				callback();

			if (!repeat)
				return;

			lock.lock();

			while (m_elapsedPeriod.count() > 0 && std::chrono::steady_clock::now() >= m_nextElapsed)
				m_nextElapsed += m_elapsedPeriod;
		}
	}

	// --------------------------- Setters and Getters ------------------------------
	void Timer::setTimeoutFunction(std::function<void(void)> timeoutFunction)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_timeoutFunction = timeoutFunction;
	}

	void Timer::setElapsedPeriod(std::chrono::duration<double> elapsedPeriod)
	{
		Stop();

		{
			std::lock_guard lock(m_mutex);
			m_elapsedPeriod = std::chrono::duration_cast<std::chrono::steady_clock::duration>(elapsedPeriod);
		}

		Start();
	}

	void Timer::setRepeat(bool repeat)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_repeat = repeat;
	}

	bool Timer::isRunning()
	{
		return m_timerThread.joinable();
	}

	std::chrono::duration<double> Timer::getElapsedPeriod()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return std::chrono::duration_cast<std::chrono::duration<double>>(m_elapsedPeriod);
	}

	bool Timer::getRepeat()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_repeat;
	}

	std::chrono::duration<double> Timer::getRemainingTime()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if (!isRunning())
			return std::chrono::duration<double>(0);

		auto now = std::chrono::steady_clock::now();

		if (now >= m_nextElapsed)
			return std::chrono::duration<double>(0);

		return std::chrono::duration_cast<std::chrono::duration<double>>(m_nextElapsed - now);
	}

} // namespace onion
