//
// Forces J-Link to program, even if flash contents already match data to be programmed
//
r
h
// Load the application in SINGLR BYTE SPI Mode
exec device = R7S721031
exec SetCompareMode = 0
exec SetVerifyDownload = 0


//
// Make sure that P7_0 (CS), P7_6 (WE), P7_8 (RD) are set as alternate function
// PMC (off 0x420): Configure PIOs as alternate function
// PFCAE (off 0xA20): Alternate function 0
// PFCE (off 0x620): Alternate function 0
// PFC (off 0x520): Alternate function 0
// PIPC (off 0x7220): I/O is controlled by alternate function
//
w2 0xFCFE341C,0xFF41
w2 0xFCFE3A1C,0x0000
w2 0xFCFE361C,0x0000
w2 0xFCFE351C,0x0000
w2 0xFCFE721C,0xFF41

// Ensure WP# (P3.12 SPBIO21_0) to high
w2 0xfcfe300c,0x1c00
w2 0xfcfe340c,0x0000
w2 0xfcfe330c,0xc3ff
w2 0xfcfe720c,0x0000

/*
    //Configure SW1 (P3_13) as an output
    PORT3.PMCn.BIT.PMCn13 = 0;
    PORT3.PMn.BIT.PMn13 = 0;
    PORT3.Pn.BIT.Pn13 = 1;
*/

// Download loader into QSPI flash
loadbin StreamIt2_QSPI_Loader_Release.bin,0x18000000

r
h
// Load the application in DUAL SPI Mode
exec device = R7S721031_DUALSPI
exec SetCompareMode = 0
exec SetVerifyDownload = 0

//
// Make sure that P7_0 (CS), P7_6 (WE), P7_8 (RD) are set as alternate function
// PMC (off 0x420): Configure PIOs as alternate function
// PFCAE (off 0xA20): Alternate function 0
// PFCE (off 0x620): Alternate function 0
// PFC (off 0x520): Alternate function 0
// PIPC (off 0x7220): I/O is controlled by alternate function
//
w2 0xFCFE341C,0xFF41
w2 0xFCFE3A1C,0x0000
w2 0xFCFE361C,0x0000
w2 0xFCFE351C,0x0000
w2 0xFCFE721C,0xFF41


// Ensure WP# (P3.12 SPBIO21_0) to high
w2 0xfcfe300c,0x1c00
w2 0xfcfe340c,0x0000
w2 0xfcfe330c,0xc3ff
w2 0xfcfe720c,0x0000

/*
    PORT3.PMCn.BIT.PMCn13 = 0;
    PORT3.PMn.BIT.PMn13 = 1;
    PORT3.Pn.BIT.Pn13 = 0;
*/
// Download application into QSPI flash
loadbin StreamIt2_User_App.bin,0x18080000
r
g
q
exit
