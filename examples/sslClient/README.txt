概要：
sslClientはSSL/TLSライブラリーwolfSSLを使った簡単なSSLクライアントのサンプルプログラムです。
GR-PEACH, mbed + TOPPERS環境で動作します。
プログラムをターゲットボードにダウンロードして起動すると、プログラム内で指定されているSSLサーバに
対てSSL接続し、簡単なメッセージを送信します。その後、SSLサーバからのメッセージを受信し、プログラムを
終了します。受信したメッセージはCOMポートを通じてPC上のターミナルに表示されます。
デフォルトでは、通信先は　www.wolfssl.comのポート443となっていて、簡単なHTTP GET
リクエストを送信するようになっていて、同サーバのwebページを受信します。

動作環境：
GR-PEACH, 有線ネットワーク接続、ダウンロード、デバッグメッセージ用COMポート

プログラミング環境：
ガジェットルネサスWebコンパイラー http://gadget.renesas.com/ja/

インストール方法：
１）基本部のダウンロード
GITHUBからGR-PEACH, mbed＋TOPPERSのプロジェクト一式をPC上にダウンロードします。
https://github.com/ncesnagoya/asp-gr_peach_gcc-mbed

２）wolfSSLの組み込み
以下のGitHubサイトからダウンロードして， wolfSSL-lib/wolfsslディレクトリ下に全ファイルとディレクトリをコピーしてください。
https://github.com/ncesnagoya/wolfssl/tree/minor-change

別の方法：
wolfSSLダウンロードページで wolfssl-3.xx.xx.zip をダウンロードします。
https://wolfssl.com/wolfSSL/download/downloadForm.php
unzipし、ルートディレクトリ直下のsrc, wolfssl, wolfcryptの三つのディレクトリを（ディレクトリ内の
全 *.[ch]ファイル及びサブディレクトリも含めて）， wolfSSL-lib ディレクトリ下にコピーしてください。

wolfSSLは実験、社内評価などの目的で無償GPLv2ライセンスで利用できます。商用利用の場合は
別途商用ライセンスを取得していください。

３）プロジェクトファイルのアップロード
3-1) ２）で準備したファイルをzip化
3-2) ガジェットルネサスwebコンパイラーを起動
3-3) プロジェクト欄の「+」をクリック
3-4) GR-PEACH用TOPPERS リアルタイムOSテンプレート（Ver V1.03)を選択し，好きなプロジェクト名を入力して，「プロジェクト作成」ボタンをクリック
3-5) エクスプローラ欄のトップディレクトリを右クリックして，「アップロード」を選択
3-6) 3-1)で作成したzipファイルをドラッグ＆ドロップ

４）ビルド、ダウンロード、実行
build_targetファイルの内容に ./examples/sslClient を指定して、通常のビルドを実行します。
エラーがないことを確認し、生成された sslClient.bin をGR-PEACHにダウンロード、実行します。

サンプルプログラム：
examples/sslClient.cppにサンプルプログラムが格納されています。
sslClient_mainがこのプログラムの入り口です。ネットワークの初期設定などを行い、呼び出し先
サーバにTCP接続したのち Security関数を呼び出します。Security関数ではSSLの初期化、SSL接続した
のち ClientGreet を呼び出します。ClientGreetでは固定のメッセージをSSL送信したのち、
サーバ側からのメッセージを受信します。

ClientGreet関数の直上に接続先、送信メッセージなどを定義してあるので、適宜変更することができます。

以上、
