#!/bin/sh
vim `find . -name '*.c' -exec grep -le '\<mem\>' {} \;`
vim `find . -name '*.h' -exec grep -le '\<mem\>' {} \;`

vim `find . -name '*.c' -exec grep -le '\<Mem\>' {} \;`
vim `find . -name '*.h' -exec grep -le '\<Mem\>' {} \;`

vim `find . -name '*.c' -exec grep -le '\<MEM\>' {} \;`
vim `find . -name '*.h' -exec grep -le '\<MEM\>' {} \;`
