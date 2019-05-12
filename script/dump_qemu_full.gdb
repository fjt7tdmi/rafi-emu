# Connect to gdbserver
target remote :8080

# number of instructions to execute
set $count = 20000000

set logging file work/gdb/qemu_full.log
set $i = 0
while $i < $count
    set logging on
    echo GDB [\n
    info reg
    echo ]\n
    set logging off
    stepi
    set $i = $i + 1
end
