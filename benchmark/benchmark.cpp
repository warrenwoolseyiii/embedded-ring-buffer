#include <benchmark/benchmark.h>
#include <cstring>
#include "../src/emb_rb.h"

// Pattern to be copied
uint8_t pattern[] = {
   0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0xba, 0xbe,
   0xfe, 0xed, 0xfa, 0xce, 0xfe, 0xed, 0xba, 0xbe
};

uint8_t buffer[1024];
uint8_t dummy[1024];

// Ring buffer to be used for benchmarking
static emb_rb_t rb;

// Fill the buffer with dummy data
static void fill()
{
   emb_rb_queue(&rb, dummy, sizeof(dummy), NULL);
}

// Empty the buffer
static void empty()
{
   emb_rb_flush(&rb);
}

// Benchmark remove
static void BM_remove(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   fill();

   for (auto _ : state)
   {
      n += emb_rb_remove(&rb, 0, pattern, len, 1);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_remove)->Range(8, 512);

// Benchmark queue operation
static void BM_queue(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   empty();

   for (auto _ : state)
   {
      n += emb_rb_queue(&rb, pattern, len, NULL);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_queue)->Range(8, 512);

// Benchmark dequeue operation
static void BM_dequeue(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   fill();

   for (auto _ : state)
   {
      n += emb_rb_dequeue(&rb, pattern, len, NULL);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_dequeue)->Range(8, 512);

// Benchmark peek operation
static void BM_peek(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   fill();

   for (auto _ : state)
   {
      n += emb_rb_peek(&rb, 0, pattern, len);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_peek)->Range(8, 512);

// Benchmark standard for loop copying bytes
static void BM_for_loop_copy(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   for (auto _ : state)
   {
      for (uint32_t i = 0; i < len; i++)
      {
         pattern[i] = pattern[(i + 1) % len];
      }
      n += len;
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_for_loop_copy)->Range(8, 512);

// Benchmark memcpy copying bytes
static void BM_memcpy(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   for (auto _ : state)
   {
      memcpy(pattern, pattern + 1, len - 1);
      n += len;
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_memcpy)->Range(8, 512);

// Benchmark insert into an existing buffer
static void BM_insert(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   emb_rb_queue(&rb, pattern, len, NULL);

   for (auto _ : state)
   {
      n += emb_rb_insert(&rb, 0, pattern, len, 1);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_insert)->Range(8, 512);

// Benchmark single queue
static void BM_single_queue(benchmark::State& state)
{
   uint32_t len = state.range(0);
   uint32_t n   = 0;

   empty();

   for (auto _ : state)
   {
      n += emb_rb_queue_single(&rb, pattern[n], NULL);
      n %= sizeof(pattern);
   }
   benchmark::DoNotOptimize(n);
   state.SetBytesProcessed(len * state.iterations());
}

BENCHMARK(BM_single_queue)->Range(8, 512);

// Main function to initialize the ring buffer and run benchmarks
int main(int argc, char **argv)
{
   emb_rb_init(&rb, buffer, sizeof(buffer));
   benchmark::Initialize(&argc, argv);
   benchmark::RunSpecifiedBenchmarks();
}
