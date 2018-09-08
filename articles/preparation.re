={preparation} Kotlinはいいぞ
== Kotlinのここがすごい！
 * ぬるぽに厳しい！
 * SAM変換ですっきり！
 * new不要！
 * スコープ関数が便利！
 * 型推論ですっきり！
 * スマートキャストがとてもスマート！
 * if,when等が式なので値を返せる！
 * Kotlin Festのロゴがかわいい！（@moyo_1994氏デザインだそうです）

=== ぬるぽ絶対踏まないマン（null許容/非許容型）
Kotlinの最大の特徴ともいえるのが、null許容/非許容型の宣言です。
Javaでは、変数にnullが入る可能性を考慮してnullチェック等をしなければなりませんが、
Kotlinではnull非許容で宣言すればnullを代入できないので、比較的安全に変数を参照することができます@<fn>{nullpo}。

また許容型で宣言しても、if等でnullチェックをすれば、ブロック内では非許容型として使うことができます！（@<list>{after_nullcheck}）

//footnote[nullpo][絶対踏まないって書いてるけど踏むときは踏みます。仕方なし。]

//listnum[after_nullcheck][nullチェック後の型の扱い]{
val s: Int?
if (s != null){
    //このブロック内ではInt型として扱える！
    s.compareTo(...)
}
//}

=== 本質じゃない記述絶対書かないマン（SAM変換）
SAM変換はもう@<list>{sam_change}を見ていただければ一目瞭然と思います。

//listnum[sam_change][SAM変換]{
//Java
hoge.setOnClickListener(new OnClickListener() //new以降本質じゃない {
    @Override //本質じゃない
    public void onClick(View v){ //本質じゃない
        //実装の記述。大事な部分
    }
}); //本質じゃない

//Kotlin
hoge.setOnClickListener { //ここに実装を記述！短い！ }
//}

overridableなメソッドをただ１つだけ持っている場合は、大概こんな感じに書けます。
ただし、複数のメソッドを持っている場合はobject式を利用して実装します。
また、自前で定義したインターフェースはSAM変換に対応していないようです。
おとなしくobject式でoverrideしましょう。

//listnum[sam_change2][複数メソッドのoverride]{
window.addMouseListener(object : MouseAdapter() {
  override fun mouseClicked(e: MouseEvent) { ... }
  override fun mouseEntered(e: MouseEvent) { ... }
})
//}

=== new不要、スコープ関数、型推論
new不要は言わずもがな、インスタンス生成にnew演算子が不要です。
スコープ関数はrun/let/with/apply/alsoの５つがあり、いろいろ使い分けがあるようですが、
もっぱら筆者はalsoしか使っていません。まだまだ覚えきれませんね……。

また、インスタンス生成や関数が返す型から自動で変数の型を推論してくれるので、
変数の型宣言をしなくてもよくなります。

//listnum[no_new][インスタンス生成にnewは不要]{
val hoge = Hoge().also { it.fuga() ... }
//}

=== スマートなスマートキャスト
Javaで型チェックといえばinstanceofですが、これはifブロック内で改めて明示的にキャストしないといけないものでした。
Kotlinではis演算子を使って型チェックをすると、ブロック内ではチェック後の型に自動的に変換されます。

//listnum[smart_cast][スマートキャスト]{
fun demo(x: Any) {
    if (x is String) {
        print(x.length) //xはこのブロック内でStringに変換される
    }
}
//}

findFragmentByTagで取得したFragmentを目的の型にキャストするときなど、
利用する場面はかなり多いと思います。

=== 文ではなく式という罠（if/when/try）
罠ではないです。とてもイイ仕様です。

Kotlinにおいて、if/when/tryは式なので、値を返すことができます。
中でもwhenが最強です（@<list>{if}、@<list>{when}）。

//listnum[if][if式を使用する]{
val hoge = if( fuga > 0 ) {
    //ブロック内の最後の行が評価される
} else { ... }
//}

//listnum[when][when式を使用する]{
val hoge = when(fuga) {
    0 -> "a"
    in 0..99 -> "b"
    else -> "c"
}

val hoge = when {
    fuga == 0 -> "a"
    fuga in 0..99 -> "b"
    else -> "c"
}
//}

whenはかなり自由に値を評価して返すことができます。最高か。

=== Kotlin Fest公式ロゴかわいい問題
筆者ビビリなので画像転載はしませんが、かわいいです。
ぜひ「Kotlin Fest 2018」でググって見てください。
ロゴのデザイナーはmoyo_1994氏（Twitter）のようです。

connpassのKotlin Fest 2018のページでは、そのときの
登壇資料がすべてアップされていますので是非ご覧になることをお勧めします。
特にCoroutinesのスライドは、本書を執筆するにあたってとても参考になりました。

//footnote[husanka][実はこれだけKotlin推しておきながら、Kotlin Festの存在を当日知りました。チケット制なので参加できず。次回こそは…！]

== Kotlinのここがだめ！
 * すっきりすぎてて、慣れないと読めないかも
 * Javaに戻れなくなる危険

完全に筆者の主観です。ifやwhenで変数宣言に続けて処理を書いたり、
スコープ関数使ってたりすると初見◯しになりかねません。
まあこの際、初見の人をKotlin沼に引きずり込むなどすれば万事OKと思います@<fn>{pennywise}。

あとJavaはKotlin変換機能で全置換してしまいましょう（過激派）。
Kotlin変換後はnull許容型やval/varの修正が必要になると思いますが、
Kotlinの恩恵を享受できると考えれば苦ではないですね@<fn>{slide}。

個人的にはここがだめ！という明確な点はまだ見えてません……Kotlinはいいぞ！

//footnote[pennywise][ハァイジョージ……Kotlinやらない？]
//footnote[slide][https://www.slideshare.net/kenichitatsuhama/java-kotlin]