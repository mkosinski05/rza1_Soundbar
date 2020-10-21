//
// Forces J-Link to program, even if flash contents already match data to be programmed
//
r
h
//
// Load the QSPI Boot Loader in single SPI Mode
exec device = R7S721031
exec SetCompareMode = 0
exec SetVerifyDownload = 0

// Ensure WP# (P3.12 SPBIO21_0) to high
w2 0xfcfe300c,0x1c00
w2 0xfcfe340c,0x0000
w2 0xfcfe330c,0xc3ff
w2 0xfcfe720c,0x0000

//
// Download application into QSPI flash
//
loadbin StreamIt2_QSPI_Loader_Release.bin,0x18000000
r
g
q
exit
