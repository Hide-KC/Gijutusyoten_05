={create_app} Twitterアプリを作る
本章では、Kotlinにて簡単なTwitterクライアントアプリ（以下、単にアプリ）の作成を行います。
オールKotlinで記述し、@<chapref>{implements}で挙げた実装をできる限り使用してアプリを構築していきます。
なお、開発は@<chapref>{preface}に記載のとおりAndroid Studio 3.1.4で行いますので、
インストールがまだの方は最初にインストールをお願いします。

== 要件定義
アプリは次の要件を満たすものとします。

 * 最新のタイムラインを取得する（非同期的）
 * アプリからツイートする
 * アプリからふぁぼ＆RTを飛ばす
 * フォロー/フォロワーアカウントの一覧を取得する

== Twitter4Jの利用
今回は、Twitterとの連携が非常に簡単になる神ライブラリ「Twitter4J@<fn>{twitter4j_url}」を利用します。
ホームページでは次のように説明されています。

//footnote[twitter4j_url][http://twitter4j.org/ja/index.html]

//quote{
Twitter4J は Twitter API の Java ラッパです。

Twitter4J を使うと JSON や HTTP に詳しくなくても容易に Twitter とインタラクトするアプリケーションを書くことが出来ます。Twitter4J は Twitter 非公式のライブラリです。 

 * 100% Pure Java - works on any Java Platform version 5 or later

 * Android やGoogle App Engine上でも動作します

 * 依存なし : 追加の jar ファイルなどは必要ありません

 * OAuth に対応

 * デフォルトで gzip をサポートするため高速

 * Twitter API 1.1に完全対応
//}

SNSのクライアントとの連携でとても面倒くさい（と思う）OAuth認証の実装が
いとも簡単に行なえます。
それでは次節より、アプリを作っていきます。

== Consumer Keyを取得する
=== 開発者アカウントの申請
アプリを作るにあたって、Twitter社に「こんなアプリを作りますよ～」と申請をします@<fn>{app_management_url}。
しかし悲しいかな、執筆中にTwitterの開発者アカウントの審査が厳しくなり、
個人アカウントでもいろいろ聞かれるようになってしまったようです。
厳しいとは言っても、しっかり回答すれば大丈夫のようです。

筆者はアカウント登録済みなので、アカウント登録の流れについては
次のページを参考にしてください。
//quote{
新しくなった Twitter Developer ポータルに登録してみる

https://qiita.com/tdkn/items/521686c240b0c5bc6207
//}

申請後審査完了メールが届けば、無事開発者アカウントの登録完了です。

//footnote[app_management_url][https://apps.twitter.com/]

=== アプリの登録
Twitter Application Managementを開くと、次のような画面が表示されます（@<img>{app_management_top}）。

//image[app_management_top][Twitter Application Managementトップ]{
//}

（なんやかんや押してアプリを登録します。）

登録したアプリをクリックし、「Keys and Access Tokens」タブをクリックし、Consumer KeyとConsumer Secretを確認します。
これらの文字列はあとで使用するので、場所だけ覚えておいてください。

== プロジェクトを作る
次にAndroid Studioを起動し、プロジェクトを作ります。

//image[create_project_01][プロジェクトの作成1]{
//}

//image[create_project_02][プロジェクトの作成2]{
//}



== ライブラリをインポートする


=== Twitter4J
=== Glide
=== Android Support Library
== レイアウトファイルを作る
=== MainActivity
=== Fragment、DialogFragment
=== タイムライン用ListViewのレイアウト
== 文字列リソースを用意する
=== strings.xml
=== oauth_key.xml
== Fragmentを作る
== Activityを作る






