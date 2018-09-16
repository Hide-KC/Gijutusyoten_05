={preparation} Kotlinはいいぞ
KotlinはJavaのいいとこどりみたいな言語ですが、本章ではその特徴を見ていきます。
Javaに慣れ親しんだ方ですと、Null-safetyやSAM変換の仕様に少し戸惑うかもしれません。

== Kotlinのここがすごい！
 * ぬるぽに厳しい！
 * SAM変換ですっきり！
 * new不要！
 * スコープ関数が便利！
 * 型推論ですっきり！
 * スマートキャストがとてもスマート！
 * if,when等が式なので値を返せる！
 * Kotlin Festのロゴがかわいい！

=== ぬるぽ絶対踏まないマン（null許容/非許容型）
Kotlinの最大の特徴ともいえるのが、Null-Safetyの仕様です（@<list>{after_nullcheck}）。

//listnum[after_nullcheck][nullチェック後の型の扱い]{
var s: Int? = 1
//s.compareTo(...) ←コンパイルエラー
s?.compareTo(...) //sがnullだったら何もしない
if (s != null){
    //このブロック内ではInt型として扱える！
    s.compareTo(...)
}
//}

Null-Safetyの仕様の何が嬉しいかというと、コンパイル時点でnullを参照してしまう可能性を潰せることにあります。
Javaでは変数にnullが入る可能性を考慮してnullチェック等をしなければなりませんが、
Kotlinではnull非許容で宣言すればnullを代入できないので、比較的安全に変数を参照することができます@<fn>{nullpo}。

null許容型の宣言は、型の接尾辞として「?」を付けるだけ（var s: T?）です。
従来どおりの「var s: T」は、Kotlinではすべてnullを入れられない非許容型として扱われます。

気を付けなければならないのは、「T」と「T?」はまったく別の型として扱われることです。
継承関係は次のとおりです。

 * Any? <- Any <- Non-null-Type
 *  └ Nullable-Type
 * AnyとNullable-Type間に継承関係はない。

AnyはJavaでいうところのObjectにあたります。
このような関係にあるため、T?型の変数からT型のメソッドを
呼ぼうとしても呼び出すことはできません。
@<br>{}

許容型で宣言しても通常どおりメソッドを呼ぶ方法が２種類あります。

 1. s?.compareTo(...)として、変数の接尾に「?」を付ける
 2. if等でnullチェックをする

ひとつめの方法は、s == nullだった場合に何もせず次の行へ移ります。
ふたつめの方法はifでnullチェックをするものです。チェック通過後のifのブロック内では
T? → Tにスマートキャスト（@<hd>{smart_cast}）されるため、T型のメソッドを呼ぶことができます。
nullだった場合は例外投げて処理したり、好きにやっちゃってください。

//footnote[nullpo][絶対踏まないって書いてるけど踏むときは踏みます。仕方なし。]


=== 本質じゃない記述絶対書かないマン（SAM変換）
SAM（SAMインターフェース）とはSingle Abstract Methodのことであり、
その名のとおり単一の抽象メソッドを持ったインターフェースを指します。
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
ただし、次の場合にはobject式でoverrideします。

 * 複数のメソッドを持っている場合
 * 自前で定義したインターフェース

@<list>{sam_change2}のMouseListenerは、ClickedとEnteredの
２つのoverridableなメソッドを持っているのでobjectが必要になります。

//listnum[sam_change2][複数メソッドのoverride]{
window.addMouseListener(object : MouseAdapter() {
  override fun mouseClicked(e: MouseEvent) { ... }
  override fun mouseEntered(e: MouseEvent) { ... }
})
//}

=== new不要、スコープ関数、型推論
Kotlinではインスタンス生成にnew演算子が不要です。
スコープ関数はrun/let/with/apply/alsoの５つがあり、いろいろ使い分けがあるようですが、
筆者はもっぱらalso（ぎりぎりletも）しか使っていません。まだまだ覚えきれませんね……。


また、インスタンス生成や関数が返す型から自動で変数の型を推論してくれるので、
変数の型宣言をしなくてもよくなります。

//listnum[no_new][インスタンス生成にnewは不要]{
val foo = Foo().also { it.fuga() ... } //Foo型になる
vai i = 1 //iはInt型
val i: Int? = null //明示的に宣言
//}

==={smart_cast} スマートキャスト
Javaで型チェックといえばinstanceofですが、これはチェック通過後もブロック内で明示的にキャストしないといけないものでした。
Kotlinではis演算子を使って型チェックをすると、ブロック内ではチェック後の型に自動的に変換されます。

//listnum[smart_cast][スマートキャスト]{
fun bar(foo: Any) {
    if (foo is String) {
        print(foo.length) //xはこのブロック内でStringに変換される
    }
}
//}

findFragmentByTagで取得したFragmentを目的の型にキャストするときなど、
利用する場面はかなり多いと思います。

=== 文ではなく式という罠（if/when/try）
罠ではないです。とてもイカした仕様です。

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
ロゴのデザイナーはmoyo_1994氏のようです。

connpassのKotlin Fest 2018のページでは、そのときの
登壇資料がすべてアップされていますのでご覧になることをお勧めします。
特にCoroutinesのスライドは、本書を執筆するにあたってとても参考になりました@<fn>{husanka}。

//footnote[husanka][実はこれだけKotlin推しておきながら、Kotlin Festの存在を当日知りました。チケット制なので参加できず。次回こそは…！]

== Kotlinのここがだめ！
 * すっきりすぎてて、慣れないと読めないかも
 * Javaに戻れなくなる危険

完全に筆者の主観です。Null-Safetyだったり、ifやwhenで変数宣言に続けて処理を書いたり、
スコープ関数使ってたりすると初見◯しになりかねません。
まあこの際、初見の人をKotlin沼に引きずり込むなどすれば万事OKと思います@<fn>{pennywise}。

あとJavaはKotlin変換機能で全置換してしまいましょう（過激派）。
Kotlin変換後はnull許容型やval/varの修正が必要になると思いますが、
Kotlinの恩恵を享受できると考えれば苦ではないですね。はい@<fn>{slide}。

個人的にはここがだめ！という明確な点はまだ見えてません。
強いて言えば、プライマリコンストラクタとセカンダリコンストラクタの
記述が分かりづらい部分でしょうか。
カスタムViewを作るときにちょっとごちゃごちゃしますし、
@JvmOverloadsアノテーションを付与して実装すると微妙に動作がおかしくなる件もあります（@<chap>{implements}参照）。

しかしそれ以上にJavaの冗長な部分が解消されている感が大きいので、
Kotlinはいいぞ！と筆者は推していきたい所存です。

//footnote[pennywise][ハァイジョージィ……Kotlinやらない？]
//footnote[slide][Java → Kotlin 変換　そのあとに。 https://www.slideshare.net/kenichitatsuhama/java-kotlin]