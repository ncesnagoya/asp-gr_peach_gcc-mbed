# 概要：
quicClientは簡単なQUICクライアントのサンプルプログラムです。
GR-PEACH, mbed + TOPPERS環境で動作します。
プログラムをターゲットボードにダウンロードして起動すると、プログラム内で指定されているQUICサーバに
対してQUIC接続し、簡単なメッセージを送信します。その後、QUICサーバからのメッセージを受信し、プログラムを
終了します。受信したメッセージはCOMポートを通じてPC上のターミナルに表示されます。
デフォルトでは、簡単なHTTP GETリクエストを送信するようになっていて、同サーバのwebページを受信します。
QUICライブラリとしてpicoquic、TLSライブラリとしてpicotls、暗号ライブラリとしてwolfSSLの暗号モジュールを利用しています。

###動作環境：
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
examples/quicClientに移動し、下記のコマンドを実行
```
% make depend
% make
```
生成された quicClient.binをGR-PEACHにドラッグ&ドロップし、実行します。

## サンプルプログラム：
examples/quicClient.cppにサンプルプログラムが格納されています。
quicClient_mainがこのプログラムの入り口です。ネットワークの初期設定などを行なったのち、q_client関数を呼び出します。
q_client関数ではスタックの初期化、QUIC接続し、固定のメッセージを送信したのち、
サーバ側からのメッセージを受信します。

接続先、送信メッセージなどはquicClient.cppで定義してあるので、適宜変更することができます。
