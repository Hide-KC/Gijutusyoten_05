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

Activityはタイムライン（以下、TL）表示用、PreferenceFragment用の２枚で構成します。
リプライ機能は今回は実装しません。

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

TODO: （なんやかんや押してアプリを登録します。）

登録したアプリをクリックし、「Keys and Access Tokens」タブをクリックし、Consumer KeyとConsumer Secretを確認します。
これらの文字列は後ほど使用します。

== プロジェクトを作る
次にAndroid Studioを起動し、プロジェクトを作ります。

//image[create_project_01][プロジェクトの作成1]{
//}

//image[create_project_02][プロジェクトの作成2]{
//}

Kotlinを使用するので、「Include Kotlin Support」にチェックを入れます。
ActivityはとりあえずEmpty Activityから作っていきます。

== ライブラリをインポートする
無事にプロジェクトが作れたら、次にGradleを開きライブラリをインポートします。

//listnum[gradle_code][ライブラリの依存関係の追加]{
dependencies {
    //...
    implementation 'com.android.support:design:27.+'
    implementation 'org.twitter4j:twitter4j-core:4.0.+'
    implementation 'com.github.bumptech.glide:glide:4.7.1'
    implementation 'com.android.support:design:27.+'
    annotationProcessor 'com.github.bumptech.glide:compiler:4.7.1'
}
//}

執筆時点のTwitter4Jの最新バージョンは4.0.4でしたが、4.0.+として微修正等に追従するようにしています。
また、GlideはURLをセットするといい感じに画像をキャッシュしてくれるライブラリで、
アカウントアイコンの表示用にインポートしています。
FIXME: デザインサポートライブラリも追加しています。

//footnote[glide_git][https://ほにゃらら]

#@# === Android Support Library

== レイアウトファイルを作る
アプリのレイアウトファイルを作っていきます。

=== MainActivity
MainActivityのイメージは次のとおりです。

//image[main_image][MainActivityレイアウト]{
//}

//listnum[main_code][MainActivityレイアウト.xml]{
<?xml version="1.0" encoding="utf-8"?>
<android.support.constraint.ConstraintLayout ...
    <FrameLayout
        android:id="@+id/timeline"
        ... />

    <FloatingActionButton
        android:id="@+id/show_dialog"
        ... />
</android.support.constraint.ConstraintLayout>
//}

FrameLayoutはTL用、FloatingActionButtonはツイートダイアログ呼び出し用に配置しています。

=== Preference用のActivity
PreferenceFragmentを表示するためのActivity


=== DialogFragment
次の動作にDialogFragmentを表示してサポートします。
 
 * ツイート送信
 * リツイート確認

//listnum[tweet_dialog][ツイート用DialogFragment.xml]{

//}

//listnum[rt_dialog][RT確認用DialogFragment.xml]{

//}

=== TL用ツイートのレイアウト
TLのツイートには次の要素を表示します。

 * アイコン
 * 名前
 * ツイート時間
 * ツイート
 * RTアイコン
 * ふぁぼアイコン



== 文字列リソースを用意する
=== strings.xml
=== oauth_key.xml
== Fragmentを作る
== Activityを作る






