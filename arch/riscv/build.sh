#! /bin/sh

riscv64-unknown-elf-as -mabi=ilp32 -march=rv32imac --defsym TEST_IRQ=1 --defsym TEST_SWI=1 -o entry.o entry.s
riscv64-unknown-elf-gcc -mabi=ilp32 -march=rv32imac -c -o main.o main.c
riscv64-unknown-elf-ld -march=rv32imac -melf32lriscv -Tlink.lds -o hello.elf entry.o main.o
riscv64-unknown-elf-size hello.elf
riscv64-unknown-elf-objcopy -O ihex hello.elf hello.hex
riscv64-unknown-elf-objcopy -O binary hello.elf hello.bin

# riscv64-unknown-elf-gcc -mabi=ilp32 -march=rv32imac -mcmodel=medany -nostartfiles -Tlink.lds -o entry.elf entry.c
# riscv64-unknown-elf-objcopy -O ihex entry.elf entry.hex
# riscv64-unknown-elf-objcopy -O binary entry.elf entry.bin
