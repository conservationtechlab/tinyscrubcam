# tiny_scrubcam
A Google Coral Micro based edge-AI device for wildlife detection

# Hardware wiring detail
![image](CircuitDiagram.jpg)

## Description

## Installation
1. git clone this repo
2. Run the CMake, delete the CmakeCache inside of the out
3. Build the project using command cmake -B out -S .
4. build the application using command make -C out -j4
   * If using Linux, use command make -C out -j$(nproc)
   * If using mac, use command make -C out -j$(sysctl -n hw.ncpu)
5. flash the application into the board using the command: python3 coralmicro/scripts/flashtool.py --build_dir out --elf_path out/detect_objects_http_usb