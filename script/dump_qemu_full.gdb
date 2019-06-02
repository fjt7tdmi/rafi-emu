# Connect to gdbserver
target remote :8080

set riscv use_compressed_breakpoint off

# number of instructions to execute
set $count = 5 * 1000 * 1000

set logging file work/gdb/qemu.gdb.log
set $i = 0
while $i < $count
    set logging on
    info reg
    echo BREAK\n
    set logging off
    stepi
    set $i = $i + 1
end
