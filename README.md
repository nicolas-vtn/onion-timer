# Onion Timer

A lightweight, thread-based C++20 timer utility.

It allows executing a function after a specified duration, either once or repeatedly, using a dedicated `std::jthread` and `std::chrono`.

---

## Features

* C++20 compliant
* Thread-safe
* Based on `std::jthread` and `std::chrono`
* One-shot or repeating mode
* Clean stop using `std::stop_token`
* No external dependencies

---

## Adding to Your CMake Project

If the library is included in your repository:

```cmake
add_subdirectory(libs/timer)

target_link_libraries(your_target
    PRIVATE
        onion::timer
)
```

---

## Basic Usage

```cpp
#include <onion/Timer.hpp>
#include <iostream>

onion::Timer timer(
    std::chrono::seconds(1),
    []()
    {
        std::cout << "Tick" << std::endl;
    },
    true // repeat
);

timer.Start();

// ...

timer.Stop();
```

### One-shot timer

```cpp
onion::Timer timer(
    std::chrono::milliseconds(500),
    []()
    {
        std::cout << "Executed once" << std::endl;
    },
    false // execute only once
);

timer.Start();
```

---

## Configuration

The timer can be reconfigured at runtime:

```cpp
timer.setElapsedPeriod(std::chrono::seconds(2));
timer.setRepeat(false);
timer.setTimeoutFunction([](){ /* new callback */ });
```

If the timer is running, changing the period automatically restarts it.

---

## Enable Demo

Enable demo:

```bash
cmake -DONION_BUILD_DEMO=ON ..
```

---

## Design Notes

* Internally uses a dedicated `std::jthread`.
* Synchronization is handled via `std::mutex` and `std::condition_variable_any`.
* Safe start/stop/restart semantics.
* The timer can be stack-allocated.
* Destruction cleanly stops the internal thread.
