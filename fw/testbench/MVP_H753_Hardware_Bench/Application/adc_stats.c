#include "adc_stats.h"
adc_stats_t adc_block_stats(const uint16_t samples[32],uint32_t reference_mv){
 uint32_t sum=0,minimum=65535,maximum=0;
 for(unsigned i=0;i<32;i++){uint32_t v=samples[i];sum+=v;if(v<minimum)minimum=v;if(v>maximum)maximum=v;}
 adc_stats_t result;
 result.mean=(sum+16U)/32U;result.minimum=minimum;result.maximum=maximum;
 /* 32 * 65535; 64-bit product prevents overflow at full scale. */
 result.millivolts=(uint32_t)(((uint64_t)sum*reference_mv+1048560U)/2097120U);
 result.volts=(float)sum*(float)reference_mv/(32.0f*65535.0f*1000.0f);
 return result;
}
