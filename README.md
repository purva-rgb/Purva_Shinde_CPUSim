# Purva_Shinde_CPUSim
TASK 2

Your Objective
Build a CPU scheduler and memory cache simulator in C++ or Java (Python not allowed). You'll simulate how tasks get allocated to a CPU and how memory blocks move through a cache hierarchy.

scheduling algorithm : Round Robin
I chose Round Robin scheduling because it models fairness in a very natural way for systems where multiple tasks compete for the CPU. Instead of letting one process dominate 
the CPU for a long time, each process gets a fixed time quantum and is rotated in a cyclic queue. This makes it especially useful in interactive and time-sharing systems where
responsiveness matters more than strict efficiency. It also ensures that every task progresses steadily over time, maintaining a smooth and balanced execution flow even when 
the system is heavily loaded.

I have implemented LRU cache replacement.
It manages memory by always evicting the least recently accessed data when the cache is full, improving efficiency by keeping frequently used data readily available.
