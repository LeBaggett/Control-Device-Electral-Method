from pathlib import Path
import os,sys,shutil,subprocess,concurrent.futures
root=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else Path(__file__).resolve().parents[1]
compiler=os.environ.get('ARM_GCC') or shutil.which('arm-none-eabi-gcc')
if not compiler:
 found=list(Path('C:/ST').glob('STM32CubeIDE*/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/arm-none-eabi-gcc.exe'))
 if not found:found=list(Path('C:/ST').rglob('arm-none-eabi-gcc.exe'))
 if not found:raise SystemExit('Set ARM_GCC to the full arm-none-eabi-gcc path.')
 compiler=str(found[0])
compiler=Path(compiler);build=root/'Build';build.mkdir(exist_ok=True)
include=['Core/Inc','Application','Application/board','Application/ui','LWIP/Target','LWIP/App','Drivers/STM32H7xx_HAL_Driver/Inc','Drivers/STM32H7xx_HAL_Driver/Inc/Legacy','Drivers/CMSIS/Include','Drivers/CMSIS/Device/ST/STM32H7xx/Include','Drivers/BSP/Components/lan8742','Middlewares/Third_Party/LwIP/src/include']
flags=['-mcpu=cortex-m7','-mthumb','-mfpu=fpv5-d16','-mfloat-abi=hard','-DSTM32H753xx','-DUSE_HAL_DRIVER','-Og','-g3','-ffunction-sections','-fdata-sections','-fno-common']+['-I'+str(root/p) for p in include]
hal=['hal','hal_cortex','hal_dma','hal_dma_ex','hal_eth','hal_exti','hal_flash','hal_flash_ex','hal_gpio','hal_pwr','hal_pwr_ex','hal_rcc','hal_rcc_ex']
sources=list((root/'Core/Src').glob('*.c'))+list((root/'Application').rglob('*.c'))+list((root/'LWIP').rglob('*.c'))
sources=[p for p in sources if 'tests' not in p.parts]
sources += [root/'Drivers/STM32H7xx_HAL_Driver/Src'/('stm32h7xx_'+x+'.c') for x in hal]
sources += [root/'Drivers/BSP/Components/lan8742/lan8742.c']
lw=root/'Middlewares/Third_Party/LwIP/src'
sources+=list((lw/'core').glob('*.c'))+list((lw/'core/ipv4').glob('*.c'))
sources += [lw/'netif/ethernet.c',lw/'apps/http/fs.c',lw/'apps/http/httpd.c']
sources += list((root/'Core/Startup').glob('*.s'))
def compile_one(source):
 obj=build/('_'.join(source.relative_to(root).parts)+'.o')
 own=source.relative_to(root).parts[0] in ('Application','LWIP') and source.name!='ethernetif.c'
 opts=flags+(['-std=c11','-Wall','-Wextra'] if source.suffix=='.c' else [])+(['-Werror'] if own else [])
 res=subprocess.run([str(compiler)]+opts+['-c',str(source),'-o',str(obj)],capture_output=True,text=True)
 return res.returncode,res.stdout+res.stderr,obj
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:results=list(pool.map(compile_one,sources))
errors=[x for x in results if x[0]]
for _,message,_ in results:
 if message:print(message)
if errors:raise SystemExit(str(len(errors))+' compilation failures')
elf=build/'MVP_H753_Hardware_Bench.elf'
rsp=build/'objects.rsp';rsp.write_text('\n'.join('"'+str(x[2]).replace('\\','/')+'"' for x in results))
cmd=[str(compiler)]+flags[:4]+['--specs=nano.specs','--specs=nosys.specs','-T'+str(root/'STM32H753ZITX_FLASH.ld'),'-Wl,--gc-sections','-Wl,--print-memory-usage','-Wl,-Map='+str(build/'MVP_H753_Hardware_Bench.map'),'@'+str(rsp),'-Wl,--start-group','-lc','-lm','-lnosys','-Wl,--end-group','-o',str(elf)]
subprocess.run(cmd,check=True)
subprocess.run([str(compiler.with_name('arm-none-eabi-size'+compiler.suffix)),str(elf)],check=True)
subprocess.run([str(compiler.with_name('arm-none-eabi-objcopy'+compiler.suffix)),'-O','binary',str(elf),str(elf.with_suffix('.bin'))],check=True)
print('H753 ARM build/link passed:',elf)
