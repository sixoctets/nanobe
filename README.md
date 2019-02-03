nanobe
======

# nanobe
* nanobe is co-operative context switching.
* The experiments with context switching and programmers model.
* Goals being to have deterministic ISR latencies and program injection support.

# Why
* CPU is leading consumer of power/current in a SoC.
* To design to optimise use of multi-CPU system architectures.
* Have a scalable programmers model for embedded software development.
* Have inter-component resource utilization independence.
* Ensure that component resource requirement don’t influence other independent components.

# Goals
* Deterministic-latency ISR.
* Bare-metal programming model.
* Soft Real Time execution contexts, Race to Idle.
* Flexible call stack configuration.
