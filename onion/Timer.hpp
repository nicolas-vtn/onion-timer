#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace onion
{
	/// @brief A Timer class that allows executing a function after a specified period of time, optionally repeating.
	class Timer
	{
	  public:
		/// @brief Creates a timer.
		Timer();

		/// @brief Creates a timer with it's configuration
		/// @param elapsedPeriod The period of time that has to elapse before the timeout function is called. If executeOnlyOnce is false, the timeout function will be called every elapsedPeriod.
		/// @param timeoutFunction The function to be called when the timer elapses.
		/// @param executeOnlyOnce If true, the timer will only execute the timeout function once. If false, it will repeat.
		Timer(std::chrono::duration<double> elapsedPeriod,
			  std::function<void(void)> timeoutFunction,
			  bool repeat = true);

		~Timer();

	  public:
		/// @brief Starts the timer. If the timer is already running, this function does nothing.
		void Start();
		/// @brief Stops the timer. If the timer is not running, this function does nothing.
		void Stop();
		/// @brief Restarts the timer. If the timer is not running, this function starts it.
		void Restart();

		/// @brief Sets the function to be called when the timer elapses.
		/// @param timeoutFunction The function to be called when the timer elapses.
		void setTimeoutFunction(std::function<void(void)> timeoutFunction);

		/// @brief Sets the period of time that has to elapse before the timeout function is called. If executeOnlyOnce is false, the timeout function will be called every elapsedPeriod. If the timer is already running, it restarts the timer with the new elapsed period.
		/// @param elapsedPeriod The period of time that has to elapse before the timeout function is called.
		void setElapsedPeriod(std::chrono::duration<double> elapsedPeriod);

		/// @brief Sets whether the timer should repeat or execute only once.
		/// @param repeat If true, the timer will repeat. If false, it will execute only once.
		void setRepeat(bool repeat);

		/// @brief Checks if the timer is currently running.
		/// @return True if the timer is running, false otherwise.
		bool isRunning();
		/// @brief Gets the period of time that has to elapse before the timeout function is called.
		/// @return The period of time that has to elapse before the timeout function is called.
		std::chrono::duration<double> getElapsedPeriod();
		/// @brief Gets whether the timer is set to repeat or execute only once.
		/// @return True if the timer is set to repeat, false if it is set to execute only once.
		bool getRepeat();
		/// @brief Gets the remaining time before the timer elapses.
		/// @return The remaining time before the timer elapses.
		std::chrono::duration<double> getRemainingTime();

	  private:
		/// @brief Mutex to protect access to the timer's internal state.
		std::mutex m_mutex;
		/// @brief Condition variable used to wake up the timer thread when the timer is stopped or restarted.
		std::condition_variable_any m_cv;
		/// @brief Elapsed period before the timer elapses.
		std::chrono::steady_clock::duration m_elapsedPeriod = std::chrono::steady_clock::duration::max();
		/// @brief The time point when the timer was started.
		std::chrono::time_point<std::chrono::steady_clock> m_startTime;
		/// @brief The time point when the timer will next elapse.
		std::chrono::time_point<std::chrono::steady_clock> m_nextElapsed;
		/// @brief Whether the timer should repeat after elapsing or execute only once.
		bool m_repeat = true;

		/// @brief The function to be called when the timer elapses.
		std::function<void(void)> m_timeoutFunction;

	  private:
		/// @brief The thread that runs the timer's wait loop.
		std::jthread m_timerThread;

		/// @brief The main loop that waits for the timer to elapse and calls the timeout function. This function runs in a dedicated thread.
		void WaitLoop(std::stop_token st);
	};

} // namespace onion
