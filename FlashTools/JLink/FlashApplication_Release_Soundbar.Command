r
h

exec device = R7S721031_DualSPI

loadbin bin\rza1lu_blinkytest.bin,0x18080000
verifybin bin\rza1lu_blinkytest.bin,0x18080000

r
g
q
