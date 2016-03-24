# UEFI_SecureBoot

UEFI セキュアブートのサンプル UEFI アプリケーションです。

Microsoft 鍵を利用してマシンを起動する方法、
自前の鍵でマシンを起動する方法と、
セキュアブート未対応の環境での実行を提供します。

今後の検討課題としては、Microsoft 鍵を利用してマシンを起動する場合の
マシン所有者鍵(MOK: Machine Owner Keys)の自動登録の実装があります。

## 開発環境

* Visual Studio Community 2015 + Windows SDK for Windows 10
https://msdn.microsoft.com/ja-jp/windows/hardware/hh852365.aspx
* 64 ビット版 Windows 10
* 64 ビット UEFI 対応 PC
* USB メモリ

## 実行方法(Microsoft 鍵を利用してマシンを起動する方法)

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* USB メモリ上の efi\boot\ms-shim-mok\cp.cmd をダブルクリックして実行します。
  USB メモリの efi\boot フォルダに実行用ファイルがコピーされます。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。セキュアブートを有効に設定します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。

ここから、初回実行時のみ必要な手順です。

* Shim UEFI key management メニュー画面が表示されます。Enroll key from disk を選択して⏎キーを押下します。
* 文字列 Acpi で始まる行で、文字列 Usb を含む行を選択して⏎キーを押下します。
* efi を選択して⏎キーを押下します。
* boot を選択して⏎キーを押下します。
* MOKself.cer を選択して⏎キーを押下します。
* 以下のように表示されます。  
  Input the key number to show the details of the key or type '0' to continue  
  1 key(s) in the new key list  
  Key Number:
* 1⏎と入力すると、マシン所有者鍵(MOK: Machine Owner Keys)の内容が表示されます。
* 以下のように表示されます。  
  Key Number:
* 0⏎と入力すると、Enroll the key(s)? (y/n):と表示されます。 y⏎と入力すると MOK が登録されます。
* .. を選択して⏎キーを押下します。
* .. を選択して⏎キーを押下します。
* Return to file system list を選択して⏎キーを押下します。
* Exit を選択して⏎キーを押下します。
* Continue boot を選択して⏎キーを 2 回、押下します。

ここまで、初回実行時のみ必要な手順です。

* 以下のメッセージが表示され、任意のキーを押下するとマシンが再起動されます。無事にセキュアブートされました。  
  After pressing any key, the system is reset in the case of a successful execution of ExitBootServices().

正常に動作しない場合は、UEFI BIOS 画面でセキュアブート
または UEFI BIOS を工場出荷時の状態に戻してから試してください。

PC の機種によっては、Boot 順序設定変更を恒久的な設定変更ではなく、
次回起動時のみ有効の設定とした方が好ましい場合があります。恒久的な
設定変更で、  PC リセット後の Windows 起動がスムーズに行えないか、
起動しない場合があるためです。ご注意ください。  

## 実行方法(自前の鍵でマシンを起動する方法)

UEFI BIOS 画面でセキュアブートの無効化と鍵のクリアが可能な PC で実行可能な方法です。

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* USB メモリ上の efi\boot\self-only\cp.cmd をダブルクリックして実行します。
  USB メモリの efi\boot フォルダに実行用ファイルがコピーされます。
  読み取り専用に設定出来る USB メモリでも書き込み可能な設定を維持します。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。セキュアブートを無効に設定します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* 以下のメッセージが表示され、任意のキーを押下すると UEFI BIOS 画面に遷移します。  
  When you press any key, The system will transition to the UEFI setup screen.  
  After please clear the UEFI key.
* UEFI BIOS 画面でセキュアブート鍵のクリアを実行します。セキュアブート鍵のクリア後はセキュアブート無効化を維持します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* セキュアブート鍵登録成功に成功すると、以下のメッセージが表示され、任意のキーを押下すると UEFI BIOS 画面に遷移します。
  鍵の登録先は、プラットフォーム鍵(PK: Platform Key)、鍵交換鍵(KEK: Key Exchange Key)、署名データベース(DB)です。  
  Successful registration of PK, KEK and DB.  
  When you press any key, The system will transition to the UEFI setup screen.  
  After please activate the UEFI secure boot.
* UEFI BIOS 画面でセキュアブートを有効に設定します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* 以下のメッセージが表示され、任意のキーを押下するとマシンが再起動されます。無事にセキュアブートされました。  
  After pressing any key, the system is reset in the case of a successful execution of ExitBootServices().

Windows が起動しなくなりますので、UEFI BIOS 画面でセキュアブート
または UEFI BIOS を工場出荷時の状態に戻すことで Windows の起動が可能になります。

## 実行方法(セキュアブート未対応の環境での実行)

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* USB メモリ上の efi\boot\ms-shim-mok\cp.cmd をダブルクリックして実行します。
  USB メモリの efi\boot フォルダに実行用ファイルがコピーされます。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* 以下のメッセージが表示され、任意のキーを押下するとマシンが再起動されます。  
  After pressing any key, the system is reset in the case of a successful execution of ExitBootServices().

## ビルド方法

* UEFI_SecureBoot.sln をダブルクリックします。
* Visual Studio のセキュリティ警告を回避してプロジェクトを開きます。
![warning](https://raw.githubusercontent.com/tenpoku1000/UEFI_SecureBoot/master/images/MSVC.PNG)
* Visual Studio の起動後に F7 キーを押下します。efi フォルダ以下に UEFI アプリケーションが生成されます。
* UEFI アプリケーションのビルド時にセキュアブート鍵で自動的に署名されます。

セキュアブート鍵や MOK を新たに生成したい場合は、VS2015 x64 Native Tools コマンドプロンプトを起動し、
tools フォルダに cd してから keygen.cmd を実行します。生成された鍵は、tools\keys フォルダに出力されます。
新しい鍵で署名された UEFI アプリケーションのセキュアブート対応 PC での実行時には、鍵の登録が再度必要になります。

## 謝辞

本ソフトウェアは、以下の外部プロジェクトの成果物を利用しています。感謝いたします。

### BSD License
* shim https://mjg59.dreamwidth.org/20303.html
* gnu-efi http://sourceforge.net/projects/gnu-efi/
* UDK2014 http://www.tianocore.org/udk2014/
* EDK II http://www.tianocore.org/edk2/

## ライセンス

[MIT license](https://raw.githubusercontent.com/tenpoku1000/UEFI_SecureBoot/master/LICENSE)

## 作者

市川 真一 <suigun1000@gmail.com>

## 参考資料

* Play with UEFI http://www.slideshare.net/syuu1228/play-with-uefi
* 技術者見習いの独り言: SecureBootとLinux
http://orumin.blogspot.jp/2015/05/securebootlinux.html
* mjg59 | Secure Boot bootloader for distributions available now
https://mjg59.dreamwidth.org/20303.html
* openSUSE:UEFI - openSUSE https://ja.opensuse.org/openSUSE:UEFI
* Playing with UEFI Secure Boot – Part 2: Basic Authenticode signing with MS Tools | Al's Website
http://al.robotfuzz.com/playing-with-uefi-secure-boot-part-2-basic-authenticode-signing-with-ms-tools/
* UEFI - PhoenixWiki http://wiki.phoenix.com/wiki/index.php/UEFI
* Unified Extensible Firmware Interface Specification Version 2.5 April, 2015
http://www.uefi.org/sites/default/files/resources/UEFI%202_5.pdf

