# AetherSim

A high-performance, data-oriented simulation engine written in modern C++ utilizing an Entity Component System (ECS) architecture. This project serves as a technical demonstration of low-level systems engineering, cache-friendly architecture design, and scriptable simulation logic.

---

## 🚀 Core Features

* **Data-Oriented Design:** Built entirely around an ECS framework to maximize CPU cache locality, eliminate deep inheritance hierarchies, and ensure contiguous memory layout for high-throughput component processing.
* **Decoupled Simulation Systems:** Discrete systems handle independent simulation domains (spatial partitioning, physics/collision resolution, pathfinding, and state updates) running linearly over component pools.
* **Extensible Scripting Interface:** Deep integration with Lua via modern bindings, allowing runtime behavioral scripting, configuration hot-reloading, and rapid state testing without recompiling the core C++ engine.
* **Algorithmic Optimization:** Implementation of optimized spatial partitioning alongside high-efficiency pathfinding systems designed to scale to thousands of active agents.

---

## 🛠️ Architecture & Design Decisions

### Entity Component System (ECS)
The core architecture prioritizes data locality over traditional object-oriented hierarchies, leveraging **EnTT** for backend registry management.
* **Memory Layout:** Components are tightly packed in contiguous arrays (sparse sets), ensuring that system iterations achieve optimal CPU cache utilization and minimize cache misses during high-frequency updates.
* **Deterministic Pipeline:** The simulation loop enforces a strict, predictable execution order:  
  `Input Processing -> Scripting/AI Logic -> Spatial Partitioning -> Physics/Collision -> State Resolution`

### Scripting & Bindings
High-level simulation logic and agent behaviors are decoupled from the native engine using **sol2** to bind Lua 5.4.
* **Performance vs. Flexibility:** Performance-critical operations (like spatial grid updates and physics math) remain entirely in native C++. 
* **Runtime Interoperability:** State variables and event hooks are exposed to Lua scripts, enabling real-time behavior adjustments and configuration tweaking without requiring a full build cycle.

### Build & Package Management
The development environment is designed to be hermetic, reproducible, and cross-platform:
* **vcpkg (Manifest Mode):** All project dependencies (EnTT, sol2, Lua, etc.) are explicitly versioned and automatically resolved via a `vcpkg.json` manifest.
* **CMake:** Modern CMake targets manage compilation boundaries, build optimization flags, and include paths natively.

---

## 🏗️ Technical Stack

* **Language:** C++20 / C++23
* **Build System:** CMake (3.20+)
* **Package Management:** vcpkg
* **ECS Framework:** EnTT
* **Scripting:** Lua 5.4 / sol2

---

## ⚡ Getting Started

### Prerequisites
* A modern C++ compiler supporting C++20 (GCC 11+, Clang 13+, or MSVC 2022)
* CMake 3.20 or higher
* [vcpkg](https://github.com/microsoft/vcpkg) dependency manager installed and configured in your environment variables

### Installation & Compilation

```bash
# Clone the repository and its submodules
git clone --recursive [https://github.com/William-Halling/AetherSim.git](https://github.com/William-Halling/AetherSim.git)
cd AetherSim

# Configure the build directory using the vcpkg toolchain
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[path-to-your-vcpkg]/scripts/buildsystems/vcpkg.cmake

# Build the project in Release mode for performance evaluation
cmake --build build --config Release
