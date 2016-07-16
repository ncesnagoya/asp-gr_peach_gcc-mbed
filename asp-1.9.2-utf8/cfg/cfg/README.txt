
	TOPPERS新世代カーネル用コンフィギュレータ（Release 1.7.0）


TOPPERS新世代カーネル用コンフィギュレータは、カーネルやソフトウェア部品
の構成や初期状態を定義したシステムコンフィギュレーションファイルを解釈し、
システムを構築する上で必要なファイル類を生成するTOPPERS新世代カーネル用
のツールです。

コンフィギュレータは、コンパイラやアセンブラを初めとする開発ツールと同様、
PC等の開発用コンピュータ上で動作するコマンドラインプログラムです。したが
って、コンフィギュレータそのものが最終的な製品に組み込まれることは、原則
としてありません。コンフィギュレータはコマンドラインツールなので、コンパ
イラを初めとした他のコマンドラインツールと組み合わせて使用することを前提
としています。しかし、ユーザーが適切に設定を行うことで、多くの統合開発環
境から呼び出せます。 


【コンフィギュレータの構築方法】

カーネルを構築する前に、まず、コンフィギュレータをコンパイルする必要が
あります（コンフィギュレータをバイナリで入手した場合には、このステップ
は必要はありません）。
コンフィギュレータの構築にはBoost C++ Libraries 1.42.0以上が必要です。
また、開発時点でのBoost C Librariesの最新版は1.46.0であり、それ以降の
バージョンについては動作確認を行っていません（まだ無いので行えません）。
Boost C++ Librariesは、下記URLから入手することができます。

http://www.boost.org/

Windowsの場合、Visual C++用のバイナリが下記URLのサイトから入手可能です。

http://www.boostpro.com/

Boost C++ Librariesをバイナリで入手できない場合には、ソースからビルドし
てください。

・動作確認済みの環境
Windows 7 Home Premium(x64) SP1 + Cygwin 1.7.8-1 + GCC 4.3.4 + Boost 1.43.0
Windows 7 Home Premium(x64) SP1 + MinGW GCC 4.5.2 + Boost 1.46.0
Windows 7 Home Premium(x64) SP1 + MinGW64 GCC 4.5.1 + Boost 1.46.0
Windows Vista Home Premium SP2 + Cygwin 1.7.7-1 + GCC 4.3.4 + Boost 1.43.0
Windows Vista Home Premium SP2 + Visual Studio 2008 SP1 + Boost 1.44.0
Windows Vista Home Premium SP2 + Visual Studio 2010 + Boost 1.44.0
Windows XP Professional SP3 + Cygwin 1.7.7-1 + GCC 4.3.4 + Boost 1.43.0
Windows XP Professional SP3 + Visual C++ 2010 + Boost 1.44.0
Windows XP Professional SP3 + Visual C++ 2005 SP2 + Boost 1.44.0
Windows XP Professional SP3 + Visual C++.net 2003 SP1 + Boost 1.44.0
Windows XP Professional SP3 + Visual C++.net 2003 SP1 + Boost 1.43.0
Linux Ubuntu 10.10 + GCC 4.4.5 + Boost 1.42.0a
Mac OSX 10.6.6 + GCC 4.2.1 + Boost 1.45.0


=== GNU開発環境を用いる場合 ===

コンフィギュレータ（cfgプログラム）を構築するには、cfgディレクトリに移動
し、configureおよびmakeコマンドを実行します。

	% cd cfg
	% ./configure
	% make

ただし、Boostをインストールしたディレクトリおよび名称が、標準で想定して
いるものと違う場合には、configureに--with-headersおよび--with-libraries
オプションによりインクルードおよびライブラリのディレクトリを指定してくだ
さい。
また、ホストシステムによっては、最適化レベルを上げると正しくコンパイルで
きないことが知られています。そのような場合には、最適化レベルを下げるか、
最適化を抑止するように、Makefileを修正する必要があります。

=== Microsoft Visual Studioを用いる場合 ===

コンフィギュレータ（cfgプログラム）を構築するには、cfgディレクトリ中の
cfg.slnをVisual Studio（またはVisual C++ Express Edition）で開いてくだ
さい。cfg.slnはVisual Studio.NET 2003用のものですが、上位のバージョンを
使用する場合にはソリューションを変換して使用してください。
ソリューションを開いた後、Release版としてビルドを実行してださい。
cfg/cfg/Releaseディレクトリにcfg.exeが生成されますので、必要なフォルダ
にコピーまたは移動してください。

・注意事項

Boost C++ Librariesをインストールした後、インクルードおよびライブラリの
ディレクトリをVisual Studioに登録してからビルドを実行してください。
Visual Studio 2010からはプロジェクトごとにディレクトリを登録するように
仕様変更されているので注意してください。
Visual C++ 2005 Express Editionを使用する場合、別途Platform SDKをインス
トールする必要があります。
複数のバージョンのBoost C++ Librariesがインストールされている場合（中途
半端に案インストールされた場合を含む）、configureに失敗する可能性があり
ます。その場合は手作業でMakefile.configを修正してください。

【Mekefile.configの説明】

configureコマンドによって生成されるMakefile.configでは次のパラメータが
定義されます。

・LIBBOOST_SUFFIX
Boost C+ Librariesのライブラリファイルのサフィックス
たとえば、ライブラリファイルがlibboost_system-mt.aのような形式であれば、
LIBBOOST_SUFFIXには-mtが設定されます。

・BOOST_VERSION
Boost C++ Librariesのバージョン
バージョン1.43.0であれば、BOOST_VERSIONには1_43が設定されます。

・BOOST_DIR=/usr/include
Boost C++ Librariesのヘッダファイルがあるディレクトリ
自分でBoost C++ Librariesをインストールした場合、通常/usr/local/include
が設定されます。

・LIBBOOST_DIR
Boost C++ Librariesのライブラリファイルがあるディレクトリ
自分でBoost C++ Librariesをインストールした場合、通常/usr/local/libが設定
されます。

・OPTIONS
コンパイラの追加オプション
普通は何も設定されません。


【コンフィギュレータの使い方】

コンフィギュレータ（cfg）は、TOPPERS新世代カーネル統合仕様書の2.12.5節
の記述の通り、3つ（または4つ）のパスで構成されます。

コンフィギュレータは、システムコンフィギュレーションファイル名をパラメー
タに取り、以下のオプションを持ちます（重要なもののみ）。

	--help
		オプションのリストと説明を表示する。

	-v［--version］
		コンフィギュレータのバージョンを表示する。

	-k［--kernel］<カーネル名>
		カーネルの名称を指定する。デフォルトはasp。

	-p［--pass］<パス番号>
		パス番号を指定する。1〜3のいずれか。

	-I［--include-path］<インクルードパス名>
		INCLUDEディレクティブにおいて、インクルードするコンフィギュレー
		ションファイルを探すパス名を指定する。

	-T［--template-file］<テンプレートファイル名>
		テンプレートファイルの名称を指定する。パス2とパス3で有効。

	--api-table <静的APIテーブルファイル名>
		静的APIテーブルのファイル名を指定する。

	--cfg1-def-table <値取得シンボルテーブルファイル名>
		値取得シンボルテーブルのファイル名を指定する。

	-r［--rom-image］<ロードモジュール名>
		ロードモジュールをSレコードフォーマットの形に変換したファイルの
		名称を指定する。パス3でのみ有効。

	-s [--symbol-table] <シンボルファイル名>
		ロードモジュール中の各シンボルとアドレスの対応表を含むシンボル
		ファイルの名称を指定する。パス3でのみ有効。

	--id-output-file <ID番号出力ファイル名>
		オブジェクトのID番号の割付け結果を、指定した名前のファイルに書
		き出す。パス2でのみ有効。

	--id-input-file <ID番号入力ファイル名>
		オブジェクトのID番号の割付けを、指定した名前のファイルから取り
		込む。パス2でのみ有効。

	--external-id
		オブジェクトのID番号を保持する変数の定義を生成する。パス2でのみ
		有効。

	-M［--print-dependencies］<ターゲットシンボル>
		システムコンフィギュレーションファイルの依存関係を出力する。
		Makefile中の依存関係を生成する際に用いる。

コンフィギュレータの詳細仕様については、別途PDFファイルの形で配布してい
る「TOPPERS新世代カーネル用コンフィギュレータ仕様」を参照してください。
テンプレートファイルの作成にあたっては、「TOPPERS新世代カーネル用コンフ
ィギュレータ内蔵マクロプロセッサ仕様書」を参照してください。

