={create_color_picker} カラーピッカーを作成してみる
もともと付録として概要のみ記載する予定でしたが、予想以上に締切に余裕があったので
@<chap>{create_color_picker}としてめでたく昇華されました。なぜわざわざ自分を追い込むのか。

カラーピッカーはViewの更新にObserverパターンを適用しています。
本書ではObserverパターン自体の解説はかなり軽いので、
あらかじめ調べておいていただければ分かりやすいかと思われます。

== できるもの
最終的には@<img>{img_dialog}のように、DialogFragmentとして表示するようにします。

//image[img_dialog][ColorPickerDialogFragment.kt][scale=0.5]{
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

今回はSBPlane、HueBar、そしてObservableTextViewのすべてが
イベントを受信して状態を更新するのでObserverにあたります。

//footnote[gof_book][いわずと知れた「オブジェクト指向における再利用のためのデザインパターン」]

== シーケンス図
カラーピッカーのシーケンス図は次のようになります（@<img>{sequence_diagram}）。

//image[sequence_diagram][シーケンス図]{
//}

のちほど記述していますが、これから作るViewはすべてIColorObserverインターフェースを実装しています。
ColorChangeListenerはSubject#notifyを叩くためのインターフェースであり、
View#getContextで取得できるcontext（今回はMainActivity）に実装します。
Viewから直接Subjectを参照するとめちゃくちゃになりますので、
発報用のインターフェースを介してSubjet#notifyを叩くようにしてみました@<fn>{getcontext}。
Subject自体はDialogFragmentのフィールドに保持するようにします。

//footnote[getcontext][getContextがColorPickerDialogFragmentであればMainActivityに実装する必要はないのですが、ViewからFragmentを取得するのは無理な模様。]

== 実装
実装は次の順で行っていきます。

 1. データクラス（HSB.kt）、インターフェース定義
 2. MainActivityへの実装
 2. Subjectクラス（abstract, Concrete）
 3. 抽象クラス（HSBView.kt）、彩度-明度平面（SBPlane.kt）
 4. 色相バー（HueBar.kt）、テキストビュー（ObservableTextView.kt）
 5. 表示するためのダイアログ（ColorPickerDialogFragment.kt）

=== データクラス、インターフェース定義
カラーピッカーはHue, Saturation, Brightnessの３値を相互にやり取りするため、
扱いやすいようにデータクラスを定義します（@<list>{hsb}）。

//listnum[hsb][HSB.kt]{
data class HSB(val hue: Float = 0f,
               val saturation: Float = 0f,
               val brightness: Float = 1f)
//}

また相互にやり取りするためのインターフェースを定義します。

//listnum[interfaces][interfaces.kt]{
interface IColorObserver{
    //Subject#notifyからの受信用
    fun colorUpdate(hsb: HSB)
}

interface ColorChangeListener{
    //更新用。最終的にSubject#notifyを叩く
    fun changed(hsb: HSB)
}
//}

IColorObserver#colorUpdateで更新イベントの受信、ColorChangeListener#changedでView更新の通知を行います。
Viewの更新は、基本的にHSBオブジェクトからhue, saturation, brightnessを取り出してセットし再描画することで行います。

=== MainActivityへのColorChangeListenerの実装
先にMainActivityにColorChangeListenerを実装したコードを掲載しておきます（@<list>{activity_implement}）。

//listnum[activity_implement][ColorChangeListenerの実装]{
//MainActivity.kt
override fun changed(hsb: HSB) {
    ...
    if (fragment is ColorPickerDialogFragment){
        fragment.changed(hsb)
    }
}

//ColorPickerDialogFragment.kt
fun changed(hsb: HSB) {
    subject.notify(hsb) //Observerの状態を更新する
}
//}

ColorChangeListener#changed -> ColorPickerDialogFragment#changedをコールします。
Fragment側のchangedは単にnotifyを叩くために用意したメソッドなので、名称はなんでも構いません。
これで、イベント発報 -> MainActivity -> DialogFragmentのSubject#notify -> View更新、の流れができました。

=== Subjectクラス
今回はSubjectに特別抽象クラスが必要なわけではないですが、
本来のパターンに即して抽象および具象クラスを定義していきます（@<list>{abstract_subject}、@<list>{concrete_subject}）。

//listnum[abstract_subject][Subject.kt]{
abstract class Subject<T, E> {
    protected val observers = mutableListOf<T>()
    abstract fun notify(parameter: E)

    fun attach(observer: T){
        observers.add(observer)
    }

    fun detachAll(){
        observers.clear()
    }    
    ...
}
//}

//listnum[concrete_subject][ColorSubject.kt]{
class ColorSubject: Subject<IColorObserver, HSB>() {
    private var hsb = HSB()

    override fun notify(parameter: HSB) {
        hsb = parameter
        for (observer in observers){
            observer.colorUpdate(hsb)
        }
    }
}
//}

無駄にジェネリクス使って抽象化度を高めてみましたがいかがでしょう。
これで何らかの手段でSubject#notifyを叩けば、mutableListに集約されている
IColorObserverのcolorUpdateが実行されます。

=== 抽象ViewクラスとSBPlane
次にSBPlaneのスーパークラスとなる抽象クラスHSBViewを作成します（@<list>{abstract_view}）。

//listnum[abstract_view][HSBView.kt]{
abstract class HSBView : View {
    ...
    var mAlpha: Float = 0f
        set(value){ field = value.coerceIn(0f..1f) }

    var hue: Float = 0f
        set(value){
            var degree = value
            ... //360度以内に丸め込んでセット
            field = degree
        }

    var saturation: Float = 1f
        set(value){ field = value.coerceIn(0f..1f) }

    var brightness: Float = 1f
        set(value){ field = value.coerceIn(0f..1f) }
}
//}

コンストラクタは@<chap>{implements}を参考に、３つ定義してください。
抽象クラスといっても、やっているのは共通のプロパティを定義しているだけです。
Hueは0～360、SaturationおよびBrightnessは0～1に収めてセットします。
KotlinではcoerceIn関数があり、指定範囲内に値を強制することができます@<fn>{coercein}。

次にこのHSBViewを継承してSBPlaneを作成します（@<list>{concrete_view}）。

//footnote[coercein][執筆中に存在を知りました。やはりKotlin最強か。]

//listnum[concrete_view][SBPlane.kt]{
class SBPlane : HSBView, IColorObserver {
    ...
    private lateinit var paint: Paint
    private lateinit var lg: LinearGradient

    @SuppressLint("DrawAllocation")
    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        ...
        //ShaderはonDrawで逐次生成
        paint = Paint(Paint.ANTI_ALIAS_FLAG)
        paint.style = Paint.Style.FILL
        paint.strokeWidth =
          Math.ceil(viewSize.width / 100.0).toFloat()
           * context.resources.displayMetrics.density
    }

    @SuppressLint("DrawAllocation")
    override fun onDraw(canvas: Canvas?) {
        super.onDraw(canvas)
        val unit = viewSize.width / 100f
        for (x_i in 0 until 100){
            val startColor
             = Color.HSVToColor(floatArrayOf(hue, x_i / 100f, 1f))
            val endColor
             = Color.HSVToColor(floatArrayOf(hue, x_i / 100f, 0f))

            lg = LinearGradient(x_i*unit , 0f,
                                x_i*unit , viewSize.height*1f,
                                startColor, endColor,
                                Shader.TileMode.CLAMP)
            paint.shader = lg
            canvas?.drawLine(x_i * unit, 0f, x_i * unit,
                             viewSize.height * 1f, paint)
        }
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        ...
        if (context is ColorChangeListener){
            //イベントの発報
            val listener = context as ColorChangeListener
            listener.changed(HSB(hue, saturation, brightness))
        }
        ...
    }

    override fun colorUpdate(hsb: HSB) {
        //HSBをセットして再描画
        hue = hsb.hue
        saturation = hsb.saturation
        brightness = hsb.brightness
        postInvalidateOnAnimation()
    }
}
//}

ちょっと長いですが、ざっくりと次のように処理をしています。

 * onMeasure - SBPlaneのサイズからグラデーション線の幅を決定する
 * onDraw - 開始色と終端色をHSBから決定し、View上に100本線を描画する
 * onTouchEvent - タッチ座標からHSBを作成し、ColorChangeListener#changedを発報する
 * colorUpdate - HSBをセットして再描画する

onTouchEventがトリガーとなってObserverの更新イベントを発報させます。

もうお察しいただいているかもしれませんが、発報させたView自身も
Observerであるため、めぐりめぐってcolorUpdateが実行されます。
この辺は明らかに無駄な処理となってしまいますので、
hsb.equalsなどでpostInvaridateOnAnimationの実行を避けた方が軽くなりそうです。

=== 色相バー、ObservableなTextView
HueBarはSeekbarを継承、ObservableTextViewはTextViewを継承した上でIColorObserverを実装します（@<list>{extend_view}）。

//listnum[extend_view][既存のViewをIColorObserverで拡張]{
class HueBar(context: Context, attrs: AttributeSet)
      : SeekBar(context, attrs), IColorObserver {
    //プロパティをHSBViewと同様に定義
    ...
    override fun colorUpdate(hsb: HSB) {
        progress = hsb.hue.toInt()
        hue = hsb.hue
        saturation = hsb.saturation
        brightness = hsb.brightness
        postInvalidateOnAnimation()
    }

    fun progressChanged(){
        val hsb = HSB(hue, saturation, brightness)
        if (context is ColorChangeListener){
            val listener = context as ColorChangeListener
            listener.changed(hsb)
        }
    }
    ...
}

class ObservableTextView(context: Context, attrs: AttributeSet)
       : TextView(context, attrs), IColorObserver {
    
    private var observer: IColorObserver? = null
    fun setObserver(observer: IColorObserver){
        this.observer = observer
    }

    override fun colorUpdate(hsb: HSB) {
        observer?.colorUpdate(hsb)
    }
}
//}

少し話は逸れますが、既存のView（Seekbar、TextView等）を拡張する場合
コンストラクタに注意する必要があります。
@<chap>{implements}で説明したとおりViewを直接継承した場合は
３つのコンストラクタを定義しなければなりませんが、
既存のViewを継承した場合は@<list>{extend_view}のとおり2番めのコンストラクタのみで大丈夫です。
逆に３つコンストラクタを定義すると、レイアウトエディタ上で表示されなくなります。

おそらくdefStyleAttrあたりが関係していると思いますが、原因について著者あまり分かっていません。
そのうち調べたいと思います。

HueBarはバーのつまみを動かしたとき（onProgressChanged）にColorChangeListener#changedを実行するようにします。
onProgressChangedはDialogFragment#onCreateDialog内で実装するため、
progressChangedを間に挟んでchangedを実行するようにしました。

ObservableTextViewはRGBそれぞれの値を表示する必要があるため、
次節にてsetObserverで匿名メソッドを渡してやります。

これでHueBarとObservableTextViewができました。

==={hd_dialogfragment} 表示するためのダイアログフラグメント
やっと終わりに差し掛かってきました。カラーピッカーに関係するViewを
レイアウトファイルに配置し、DialogFragmentを作成します（@<list>{colorpicker_dialog}）。

//list[colorpicker_dialog][ColorPickerDialogFragment.kt]{
private val subject
    : Subject<IColorObserver, HSB> = ColorSubject()

override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
    ...
    if (inflater != null){
        ...
        //RGBのTextViewそれぞれについてObserverをセット
        val rValue = view.r_value.also {
            it.setObserver(object : IColorObserver {
                override fun colorUpdate(hsb: HSB) {
                    val color = Color.HSVToColor
                        (floatArrayOf
                         (hsb.hue, hsb.saturation, hsb.brightness))
                    
                    it.text = "Red " + Color.red(color).toString()
                }
            })
        }
        ...

        val hueBar = view.hue_bar.also {
            it.setOnSeekBarChangeListener
            (object : SeekBar.OnSeekBarChangeListener{
                ...
                override fun onProgressChanged(...)
                {
                    subject.notify(HSB(it.getFloatProgress(),
                                        it.saturation,
                                        it.brightness))
                }
            })
        }

        //SubjectにObserverを追加
        subject.attach(view.sb_plane)
        subject.attach(rValue)
        subject.attach(gValue)
        subject.attach(bValue)
        subject.attach(hueBar)

        val builder = AlertDialog.Builder(context)
        ...
        return builder.create()
    } else { ... }
}
...
fun changed(hsb: HSB) {
    subject.notify(hsb)
}

companion object { ... }
//}

onCreateDialogでは次のことを行っています。

 * ObservableTextView#setObserverにて、colorUpdateを実装する
 * HueBar#onProgressChangedを実装する
 * SubjectにObserverを追加する

これでカラーピッカーの作成が完了しました。
DialogFragment#showで各Viewの操作ができれば完成です。

== まとめ
長々と実装を書きましたが、まとめると次のとおりです。

 1. IObserverを実装したViewを作る
 2. Subjectを作る
 3. DialogFragmentでnotifyを叩く処理を書く
 4. colorUpdateの処理を定義する

本当はActivityを経由せずにDialogFragment内だけで完結できれば
よかったのですが、執筆中にいい案が浮かばなかったのでこのような実装になりました。

本書で作成したファイルはTwitterClientAppのリポジトリに同梱されているので、
そちらも参照いただければと思います。
