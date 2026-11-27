"""Host checks for voltage scaling, full-scale overflow and block statistics.
These do not validate analog accuracy, DMA or timer behavior on hardware.
"""
from pathlib import Path
import subprocess,tempfile
root=Path(__file__).resolve().parents[2]
code=r'''
#include "adc_stats.h"
#include <assert.h>
#include <math.h>
int main(void){
 uint16_t a[32]={0};adc_stats_t s=adc_block_stats(a,3300);
 assert(s.mean==0 && s.millivolts==0 && s.volts==0);
 for(unsigned i=0;i<32;i++)a[i]=65535;
 s=adc_block_stats(a,3600);assert(s.mean==65535 && s.minimum==65535 && s.maximum==65535);
 assert(s.millivolts==3600 && fabsf(s.volts-3.6f)<0.000001f);
 for(unsigned i=0;i<32;i++)a[i]=(i&1)?65535:0;
 s=adc_block_stats(a,3300);assert(s.mean==32768 && s.minimum==0 && s.maximum==65535);
 assert(s.millivolts==1650 && fabsf(s.volts-1.65f)<0.000001f);
 for(unsigned i=0;i<32;i++)a[i]=19859;
 s=adc_block_stats(a,3300);assert(s.millivolts==1000 && s.minimum==s.maximum);
 return 0;
}
'''
with tempfile.TemporaryDirectory() as td:
 p=Path(td);(p/'test.c').write_text(code);exe=p/'test.exe'
 subprocess.run(['gcc','-std=c11','-Wall','-Wextra','-Werror','-I'+str(root/'Application'),str(p/'test.c'),str(root/'Application/adc_stats.c'),'-lm','-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: ADC zero/full scale, overflow, mixed block and nominal voltage conversion')
