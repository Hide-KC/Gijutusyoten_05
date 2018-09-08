={preface} はじめに
はじめまして、KCと申します。この度は本書を手に取っていただきましてありがとうございます。
@<br>{}

本書は全編Kotlin推しの本になっています。
特に@<chap>{preparation}は、深夜のテンションで一気に書いたので無駄に熱いです@<fn>{nekonda}。

筆者自身Kotlinは2018年3月18日頃（Twitter調べ）から触り始めたので約半年程度ですが、
Javaの冗長な記述が削られ、ぬるぽの可能性が潰され、変換機能でJavaとの互換性もある程度保たれ、
いいことづくめの言語ですね！書いてて楽しいです。

本書の構成は次のようになっています。

 1. Kotlinはいいぞ（@<chap>{preparation}）
 2. カスタムView周り、非同期処理の実装（@<chap>{implements}）
 3. TwitterアプリをKotlinで作る（@<chap>{create_app}）

カスタムViewについては、カラーピッカーを自作したのでその実装の概要を載せています（@<chap>{appendix_a}）。
意外とググってもないんですView単体の実装の話。
本書に掲載のコード全容はGitHubにありますので、そちらも参照していただければと思います。

== 開発環境
本書の開発環境は次のとおりです。

 * OS - Windows 7,10
 * IDE - Android Studio 3.1.4
 * Kotlin - 1.2.61
 * minSdkVersion - 19（KitKat）

Sdkは一応KitKat以後を想定しています。
Lolipop以降で非推奨なメソッドは使用していないので、
ご自由に変更願います。

//footnote[nekonda][そのあと丸一日寝込みました。寝不足だめ絶対。]