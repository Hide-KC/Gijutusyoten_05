={create_color_picker} カラーピッカーを作成してみる
アプリを作成する中で、どうしてもカラーピッカーが欲しかったので作ってみました。
本章はそのカラーピッカーの実装の解説になります。

もともと付録として概要のみ記載する予定でしたが、予想以上に締切に余裕があったので
@<chap>{create_color_picker}としてめでたく昇華されました。なぜわざわざ自分を追い込むのか。

カラーピッカーは色の更新にObserverパターンを適用しています。
本書ではObserverパターン自体の解説はかなり軽いので、
あらかじめ調べておいていただければ分かりやすいかと思われます@<fn>{viewmodel}。

//footnote[viewmodel][ViewModel等を使えばもっと簡単に実装できるかも？]

== できるもの
最終的には@<img>{img_dialog}のように、DialogFragmentとして表示するようにします。

//image[img_dialog][ColorPickerDialogFragment.kt][scale=0.75]{
//}

カラーピッカーはHSB（HSV）形式で表現します。

 * Hue - 色相
 * Saturation - 彩度
 * Brightness（Value） - 明度

明度はValueと呼ぶことが多いようですが、コード上だと名称がぶつかってしまうので
Brightnessとしています。

== 作るクラス
DialogFragmentに表示するViewとして、次のものを作ります。

 * 彩度-明度平面（SBPlane.kt）
 ** 抽象クラスHSBView.ktを継承
 * 色相バー（HueBar.kt）
 ** Seekbarを継承
 * RGB値を表示するTextView（ObservableTextView.kt）
 ** TextViewを継承

HueBar及びObservableTextViewは、直接Viewを継承せずに簡易的に作成しました。
また上記３つのViewは、相互に連携して状態を変えるためにObserverパターンを適用します。

== Observerパターン
Observerパターンは通称GoF本@<fn>{gof_book}の振る舞いに関するデザインパターンのひとつで、
次のように説明されています。

//quote{
あるオブジェクトが状態を変えたときに、それに依存するすべてのオブジェクトに自動的にそのことが知らされ、
また、それらが更新されるように、オブジェクト間に一対多の依存関係を定義する。
//}

今回はSBPlane、HueBar、そしてObservableTextViewのすべてがイベントを受信して状態を更新するのでObserverにあたります。
それでは次節より実装を見ていきます。

== 実装
実装は次の順で見ていきます。

 1. データクラス（HSB.kt）、インターフェース定義
 2. Subjectクラス（abstract, Concrete）
 3. カスタムViewの抽象クラス（HSBView.kt）、彩度-明度平面（SBPlane.kt）
 4. 色相バー（HueBar.kt）、テキストビュー（ObservableTextView.kt）
 5. 表示するためのダイアログ（ColorPickerDialogFragment.kt）


=== データクラス、インターフェース定義
カラーピッカーはHSBの３値を相互にやり取りするため、
扱いやすいようにデータクラスを定義します（@<list>{hsb}）。

//listnum[hsb][HSB.kt]{
data class HSB(val hue: Float,
               val saturation: Float,
               val brightness: Float)
//}

また次のインターフェースを定義します。

//listnum[interfaces][interfaces.kt]{
interface IColorObserver{
    //Subject#notifyからの受信用
    fun colorUpdate(hsb: HSB)
}

interface ColorChangeListener{
    //変更発報用。最終的にSubject#notifyを叩く
    fun changed(hsb: HSB)
}
//}

IColorObserver#colorUpdateでイベントの受信、ColorChangeListener#changedでイベントの通知を行います。




そのため次のインターフェースを実装しています（@<list>{icolorobserver}）。

//listnum[icolorobserver][IColorObserver.kt]{
interface IColorObserver{
    //Subject#notifyからの受信用
    fun colorUpdate(hsb: HSB)
}
//}

HSBクラスは次のようなデータクラスとして定義します。




//footnote[gof_book][いわずと知れた「オブジェクト指向における再利用のためのデザインパターン」]


== 

== 要件定義



