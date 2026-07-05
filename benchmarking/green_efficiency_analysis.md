# Measurement Methodology

To evaluate the performance of the programs, I executed each one three times under the same conditions and recorded the execution times. Running each test multiple times helps reduce the impact of small variations caused by the operating system or other background processes. For the baseline benchmark, the program completed 100,000,000 iterations in **0.083895 s**, **0.083895 s**, and **0.085549 s**. The results were very close to each other, showing that the measurements were consistent.

For the algorithm comparison, I also performed three runs for each implementation. The naive algorithm took **1.034939 s**, **1.033312 s**, and **1.039411 s**, while the single-pass algorithm consistently completed in **0.000039 s**. The execution times were recorded directly from the program outputs instead of being estimated.

# Observed Performance Differences

The biggest difference in the experiment was between the naive algorithm and the single-pass algorithm. The naive version required a little over one second to finish in every run, while the single-pass version completed in only **0.000039 seconds** each time. This means the optimized implementation was dramatically faster.

The reason for this difference is the way the algorithms are written. The naive implementation performs much more repeated work, which increases the total number of operations the processor has to execute. In contrast, the single-pass version processes the data only once, avoiding unnecessary repeated calculations. This explains why its execution time is several orders of magnitude lower.

The baseline benchmark also showed stable performance. The difference between the fastest run (**0.083895 s**) and the slowest (**0.085549 s**) was very small, indicating that the timing measurements were reliable.

# Relation Between Runtime and Energy Consumption

This experiment did not measure energy consumption directly. Instead, execution time was used as an indirect indicator of energy usage. A program that keeps the CPU busy for a longer period will generally consume more energy than one that finishes almost immediately, assuming both run on the same hardware.

Based on the recorded results, the naive algorithm is expected to use more energy because it runs for about one second, whereas the single-pass implementation finishes in only **0.000039 seconds**. Although this does not tell us the exact amount of energy consumed, it provides a reasonable comparison between the two implementations.

# Limitations of the Experiment

One limitation of this experiment is that execution times can be affected by factors outside the program itself. Background processes, operating system scheduling, and CPU frequency changes can all introduce small timing differences between runs. This can be seen in the baseline benchmark, where one run took **0.085549 seconds** while the other two completed in **0.083895 seconds**.

Another limitation is that all measurements were collected on a single machine. Running the same programs on different hardware or operating systems could produce different execution times.

# Practical Engineering Takeaway

The measurements clearly show why algorithm design matters. Even when two programs solve the same type of problem, the amount of work they perform can have a huge impact on performance. In this experiment, the naive implementation consistently required about **1.03 seconds**, while the single-pass implementation finished in only **0.000039 seconds**. The baseline benchmark also demonstrated consistent timing across multiple runs, increasing confidence in the measurements.

From an engineering perspective, choosing an efficient algorithm is one of the most effective ways to improve performance. Faster programs not only reduce execution time but also keep the processor active for a shorter period, making them more efficient overall. These results demonstrate that optimizing the algorithm itself often provides much greater benefits than focusing only on hardware or compiler optimizations.
