# Connect to gdbserver
target remote :8080

# number of instructions to execute
set $count = 10000

set logging file work/gdb/qemu.log
set logging on
set $i = 0
while $i < $count
    info reg
    echo ---\n
    stepi
    set $i = $i + 1
end
