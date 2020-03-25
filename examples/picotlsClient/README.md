# 概要：
picotlsClientは簡単なTLSクライアントのサンプルプログラムです。
GR-PEACH, mbed + TOPPERS環境で動作します。
プログラムをターゲットボードにダウンロードして起動すると、プログラム内で指定されているTLSサーバに
対してTLS接続し、簡単なメッセージを送信します。その後、TLSサーバからのメッセージを受信し、プログラムを
終了します。受信したメッセージはCOMポートを通じてPC上のターミナルに表示されます。
デフォルトでは、簡単なHTTP GETリクエストを送信するようになっていて、同サーバのwebページを受信します。
TLSライブラリとしてpicotls、暗号ライブラリとしてwolfSSLの暗号モジュールを利用しています。

### 動作環境：
GR-PEACH, 有線ネットワーク接続、ダウンロード、デバッグメッセージ用COMポート

## インストール方法：
### １）基本部のダウンロード
GITHUBからGR-PEACH, mbed＋TOPPERSのプロジェクト一式をPC上にダウンロードします。
https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed

### ２）submoduleのダウンロード
以下のコマンドを実行し、サブモジュール（picoquic、picotls、wolfSSL）をダウンロードします。
```
% git submodule init
% git submodule update
```
wolfSSLは実験、社内評価などの目的で無償GPLv2ライセンスで利用できます。商用利用の場合は
別途商用ライセンスを取得していください。
###３）ビルド、ダウンロード、実行
#### macでの実行方法：
examples/picotlsClientに移動し、下記のコマンドを実行
```
% make depend
% make
```
生成された picotlsClient.binをGR-PEACHにドラッグ&ドロップし、実行します。

接続先、送信メッセージなどはpicotlsClient.cppで定義してあるので、適宜変更することができます。