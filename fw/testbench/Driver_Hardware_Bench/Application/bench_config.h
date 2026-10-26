#ifndef BENCH_CONFIG_H
#define BENCH_CONFIG_H
/* Driver validation only: no sensor-based control of outputs. */
/* Disable absent peripherals to test one interface at a time. */
#ifndef BENCH_TEST_OLED
#define BENCH_TEST_OLED 1
#endif
#ifndef BENCH_TEST_MAX31855
#define BENCH_TEST_MAX31855 1
#endif
#ifndef BENCH_TEST_ADC
#define BENCH_TEST_ADC 1
#endif
#ifndef BENCH_TEST_SD
#define BENCH_TEST_SD 1
#endif
#ifndef BENCH_TEST_ENCODER
#define BENCH_TEST_ENCODER 1
#endif
#endif
