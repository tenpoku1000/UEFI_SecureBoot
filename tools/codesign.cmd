
mkdir ..\efi\boot\ms-shim-mok

copy /y ..\boot\external\BSD\shim\efi\shim.efi ..\efi\boot\ms-shim-mok\bootx64.efi
copy /y ..\boot\external\BSD\shim\efi\MokManager.efi ..\efi\boot\ms-shim-mok
copy /y ..\efi\boot\self-only\bootx64.efi ..\efi\boot\ms-shim-mok\grubx64.efi
copy /y cmd\cp.cmd ..\efi\boot\ms-shim-mok

REM [MOK]
copy /y keys\MOKself.cer ..\efi\boot\ms-shim-mok

REM [MOK] PKCS#12
copy /y keys\MOKself.pfx ..\efi\boot\ms-shim-mok

REM [PK]
copy /y keys\PKself.cer ..\efi\boot\self-only

REM [KEK]
copy /y keys\KEKself.cer ..\efi\boot\self-only

REM [DB]
copy /y keys\DBself.cer ..\efi\boot\self-only

REM [DB] PKCS#12
copy /y keys\DBself.pfx ..\efi\boot\self-only

copy /y cmd\cp.cmd ..\efi\boot\self-only

REM [DB] PKCS#12 Codesign
cd ..\efi\boot\self-only
signtool sign /f DBself.pfx /fd sha256 bootx64.efi
del /q /f DBself.pfx

REM [MOK] PKCS#12 Codesign
cd ..\ms-shim-mok
signtool sign /f MOKself.pfx /fd sha256 grubx64.efi
del /q /f MOKself.pfx

