#include "../src/emb_rb.h"
#include "benchmark/benchmark.h"

static void BM_emb_rb_queue(benchmark::State& state)
{
   uint8_t  buf[512];
   emb_rb_t rb;

   emb_rb_init(&rb, buf, sizeof(buf));

   uint8_t data[128];
   for (int i = 0; i < sizeof(data); i++)
   {
      data[i] = i;
   }

   for (auto _ : state)
   {
      emb_rb_queue(&rb, data, sizeof(data));
   }
}

BENCHMARK(BM_emb_rb_queue);

int main(int argc, char **argv)
{
   ::benchmark::Initialize(&argc, argv);
   if (::benchmark::ReportUnrecognizedArguments(argc, argv))
   {
      return(1);
   }
   ::benchmark::RunSpecifiedBenchmarks();
}
