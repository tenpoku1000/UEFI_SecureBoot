
REM [PK]
makecert -a sha256 -e "12/31/2100" -n "CN=PKself" -r -sv keys\PKself.pvk keys\PKself.cer
del /q /f keys\PKself.pvk

REM [KEK]
makecert -a sha256 -e "12/31/2100" -n "CN=KEKself" -r -sv keys\KEKself.pvk keys\KEKself.cer
del /q /f keys\KEKself.pvk

REM [DB]
makecert -a sha256 -e "12/31/2100" -n "CN=DBself" -r -sv keys\DBself.pvk keys\DBself.cer

REM [DB] PKCS#12
pvk2pfx -pvk keys\DBself.pvk -spc keys\DBself.cer -pfx keys\DBself.pfx -f
del /q /f keys\DBself.pvk

REM [MOK]
makecert -a sha256 -e "12/31/2100" -n "CN=MOKself" -r -sv keys\MOKself.pvk keys\MOKself.cer

REM [MOK] PKCS#12
pvk2pfx -pvk keys\MOKself.pvk -spc keys\MOKself.cer -pfx keys\MOKself.pfx -f
del /q /f keys\MOKself.pvk

