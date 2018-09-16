={implements} 実装集
カスタムAdapter、カスタムView、カスタムLayout（ConstraintLayoutの拡張）、
カスタムPreference、そして非同期処理の実装テンプレを集めてみました。
個人的なまとめという側面が強いですが、開発の役に立てば幸いです。

== カスタムAdapter
ArrayAdapter<T>を拡張して、ListViewに表示する項目の見た目を変更します。
@<img>{image_adp}のようなレイアウトファイルを作り、カスタムAdapter内でInflateし、
ListViewにセットします。

//image[image_adp][作るListViewのイメージ][scale=0.75]{
//}

//listnum[dto][SampleDTO.kt]{
//Kotlinではdata classが使える
data class SampleDTO{
    val name: String,
    val resId: Int
}
//}

//listnum[adp_kotlin][カスタムAdapter.kt]{
class MyAdapter(context: Context):
      ArrayAdapter<SampleDTO>(context, android.R.layout.simple_list_item_1) {
    private val inflater = LayoutInflater.from(context)

    override fun getView(position: Int, convertView: View?,
                         parent: ViewGroup?): View {
        val cv = convertView?:
                 inflater.inflate(R.layout.item_layout, null).also {
            it.tag = ItemViewHolder(it)
        }

        val holder = cv.tag as ItemViewHolder
        //DTOから各値を格納します。
        holder.name.setText(sampleDTO.name)
        holder.image.setImageResource(sampleDTO.resId)
        return cv
    }

    private class ItemViewHolder(view: View){
        val name: TextView = view.findViewById(R.id.user_name)
        val image: ImageView = view.findViewById(R.id.user_image)
    }
}
//}

いわゆるViewHolderパターンによる実装です。

Kotlinではエルビス演算子（?:）@<fn>{elbis}とスコープ関数（also）により、
初期化処理を一気に記述することができます。
また、データを保持するだけのData Transfer Objectを、Kotlinではdata classとして定義できます。
data classは自動で次のものを作ってくれます。

 * equals()/hashCode()
 * toString()
 * componentN()関数 - プロパティ宣言順に内容を取り出す
 * copy() - プロパティのいくつかを変更し新しいオブジェクトを生成

めんどうなequals()/hashCode()/toString()のoverrideを自動でやってくれます。
data classにはふつうにメソッド定義もできるので、データやり取りのためのクラスを作るときは
ぜひ使っていきたいところですね。

//footnote[elbis][右に倒すとエルビス・プレスリーのリーゼント（ポンパドール）に見えることから、だそうです。]

#@# === StickyListHeadersListView

=={custom_view} カスタムView
カスタムViewはViewを継承したクラスを作り、最低限onDrawをoverrideすればOKです。

//listnum[view_kotlin][カスタムView.kt]{
class MyView: View {
    constructor(context: Context): this(context, null) //1
    constructor(context: Context, attrs: AttributeSet?)
                                 : this(context, attrs, 0) //2
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int)
                                 : super(context, attrs, defStyleAttr) { //3
        
        //３つめのsuperの後に初期化処理を記述する
        //obtainStyledAttributesメソッドで独自の識別子の値を取得する場合は、
        //このブロックの中で取得する
        val typedArray = context.obtainStyledAttributes
                                 (attrs, R.styleable.Hoge, defStyleAttr, 0)
        try{
            //xmlで静的にセットされている値の取出し
            fuga = typedArray.getDimension(R.styleable.Hoge_fuga, 0f)
        } finally {
            typedArray.recycle() //必ず呼ぶ
        }
    }

    override fun onDraw(canvas: Canvas?){
        super.onDraw(canvas)
        //円を書いたり塗りつぶしたり
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        //event.x, event.yからローカル座標取得
    }
}
//}

カスタムViewのコンストラクタは、少なくとも次の１～３を必ずoverrideしてください。
overrideしないとなぜか描画されません（待て）。

Qiitaの記事@<fn>{view_fn}によると次のとおり呼び出されるようです。

//quote{
1. コードでViewを生成したときのコンストラクタ

2. XMLからViewをinflateした際のコンストラクタ

3. XMLからinflateして、Themeからクラス固有の基本スタイルを適用します

4. XMLからinflateして、Themeからクラス固有の基本スタイルまたは指定のスタイルを適用します
//}

//footnote[view_fn][Viewの4つのコンストラクタ　https://qiita.com/alzybaad/items/aca3049b6a13ab78f945]

４つめのコンストラクタはLollipopから追加されたもので、Lollipop未満では例外を発報するため
バージョン分岐等の措置が必要になります……が、実際overrideしなくても動きますね。
このため@<list>{view_kotlin}にも記載はしていません。すみません。

また、@JvmOverloadsアノテーションを付与すればもっと簡潔に記述できますが、
既存のView（EditText等）を継承する場合は、必ずしもうまくいかない場合があるため注意してください@<fn>{extends_view}。

Viewの定義が終わったら、Android Studioのメニューバーから Build→Rebuild Project
を実行することで、LayoutEditorのPallet→ProjectにカスタムViewが表示されます。
あとは他のViewと同じようにxmlで配置してください。

なお、地味にネットでは見つけにくい情報ですが、Kotlinでセカンダリコンストラクタを定義し、
さらにobtainStyledAttributesメソッドで静的にセットした独自定義の識別子の値を取得する場合、
３つめのコンストラクタの後に初期化ブロックを続けて記述することで、attrsを参照することができます。
別途initブロックを作っても、attrsはinitブロックの中からは参照はできません。
日本語圏では明確な答えが見つからず、少し詰みました@<fn>{attrs_sof}。

@<chap>{create_color_picker}にて自作カラーピッカーの実装を載せておきます。よしなに。

//footnote[extends_view][https://qiita.com/kwhrstr1206/items/93827190a535b11bd064]
//footnote[attrs_sof][https://stackoverflow.com/questions/36716794/kotlin-how-to-access-the-attrs-for-a-customview]

=={custom_layout} カスタムLayout（ConstraintLayoutの拡張）
あまり需要はないかもしれませんが、カスタムLayoutについても少し記述します。

基本はカスタムViewと同様で、コンストラクタのoverrideも同様に行ってください。
ConstraintLayout特有の制約（Constraint）の付け方を@<list>{constraint}に示します。
ついでに何かと便利なGuidelineも生成します。

//listnum[constraint][カスタムLayout.kt]{
class MyLayout: ConstraintLayout{
    //Guideline用のId。フィールドに保持
    val leftId = View.generateViewId()
    val topId = View.generateViewId()
    //ImageView用のId
    val resId = View.generateViewId()

    //コンストラクタは、カスタムViewと同じく３つoverride
    constructor... {
        //ImageViewの生成、追加
        val image = ImageView(context).also{ it.id = resId }
        this.addView(image)
        
        //ConstraintSetの生成
        val constraintSet = ConstraintSet()
        constraintSet.clone(this)

        //Vertical Guidelineの生成
        constraintSet.create(leftId, ConstraintSet.VERTICAL_GUIDELINE)
        constraintSet.setGuidelinePercent(leftId, 0f)

        //Horizontal Guidelineの生成
        constraintSet.create(topId, ConstraintSet.HORIZONTAL_GUIDELINE)
        constraintSet.setGuidelinePercent(topId, 0f)

        //Guidelineへの制約付けと親コンテナへの制約付け
        //resIdはLayoutに配置されているViewのID
        constraintSet.connect(resId, ConstraintSet.START,
                            leftId, ConstraintSet.START)
        constraintSet.connect(resId, ConstraintSet.TOP,
                            topId, ConstraintSet.TOP)
        constraintSet.connect(resId, ConstraintSet.END,
                            ConstraintSet.PARENT_ID, ConstraintSet.END)
        constraintSet.connect(resId, ConstraintSet.BOTTOM,
                            ConstraintSet.PARENT_ID, ConstraintSet.BOTTOM)

        //MatchConstarintの設定
        constraintSet.constrainWidth(resId, ConstraintSet.MATCH_CONSTRAINT)
        constraintSet.constrainHeight(resId, ConstraintSet.MATCH_CONSTRAINT)

        //設定の適用
        constraintSet.applyTo(this)
    }
}
//}

constraintSetにcloneして、各種制約を付けた（connect）後、applyToで適用します。
@<list>{constraint}では、コードで生成したImageViewのTopとLeft（START）をGuidelineに、BottomとRight（END）を親コンテナに紐づけてます。
親コンテナのIDはConstraintSet.PARENT_IDで取得できます。

ところでこの節を執筆中、新たにMotionLayoutが発表されました。
ConstraintLayoutの子クラスであり、ConstraintSetからモーションを設定できるようです。
今はリリースされたばかりで解説が少ないですが、技術書典が開催される10月ごろには
もっとQiitaとかで多くなってますかね。楽しみです（自分で書け）。

== カスタムPreference
PreferenceはCheckBoxPreferenceやEditTextPreferenceなどが標準で用意されていますが、
本項では、１つのPreferenceにCheckBoxとEditTextの両方の機能をもたせたPreferenceを作ります（@<img>{preference}）。

//image[preference][カスタムPreferenceのイメージ][scale=0.6]{
//}

//listnum[preference_xml][カスタムPreference.xml]{
<android.support.constraint.ConstraintLayout
    android:id="@android:id/widget_frame">

    <TextView android:id="@android:id/title"
        ... />

    <TextView android:id="@android:id/summary"
        ... />

    <CheckBox android:id="@+id/checkBox"
        ... />
</android.support.constraint.ConstraintLayout>
//}

title要素とsummary要素になるTextViewには、idを "@android:id/title（summary）" として付与してください。
SharedPreferences#getStringでtitle（summary）要素が取得できるようになります。

またrootは "@android:id/widget_frame" でないと正常にInflateできないので、こちらも付与してください@<fn>{pref_reference}。

//footnote[pref_reference][onCreateViewの項参照 https://developer.android.com/reference/android/preference/Preference]

コンストラクタはカスタムViewと同様に３つoverrideします。

//listnum[pref_kotlin][カスタムPreference.kt]{
class MyPreference: Preference{
    //コンストラクタのoverride
    override fun onCreateView(parent: ViewGroup?): View {
        ...
    }

    override fun onBindView(view: View?) {
        super.onBindView(view)
        val prefs = context.getSharedPreferences(
                            key, Context.MODE_PRIVATE)
        
        view?.setOnClickListener {
            //ダイアログの表示処理
        }
        
        val titleView = view?.findViewById(android.R.id.title)
        titleView?.text = prefs.getString("title", "COMITIA")

        val checkBox = view?.findViewById(R.id.checkBox)
        checkBox?.setOnCheckedChangeListener {
            compoundButton: CompoundButton, b: Boolean ->
            //チェック状態監視
        }
    }
}
//}

Fragmentでは初期化処理をonCreateViewで行っていましたが、リファレンスにも記載されているとおり
onBindViewでプロパティ設定等の処理を行っています。onCreateViewで初期化をしていると、
項目が入れ替わったり変な挙動になるようです@<fn>{onbindview}。

view.setOnClickListenerでは@<img>{preference}のダイアログ表示を仕込みます。
なお、カスタムPreferenceの内容からは離れてしまいますので詳細は省きますが、
ダイアログのOKボタンを押した後、onBindViewが走らないことがある@<fn>{dialog_result}ので、
コールバックを実装するなどにより確実に値更新をしてやったほうが安心です。

//footnote[onbindview][http://ksoichiro.blogspot.com/2011/05/android-preference.html]
//footnote[dialog_result][ソフトキーボードの表示・非表示時にonBindViewが走る模様。]

== 非同期処理
@<chap>{create_app}ではTwitterアプリを作りますが、Webサーバと通信する場合、
通信処理中にアプリの応答不可を回避するための非同期処理が必要になります。
ここでは従来のAsyncTaskによる実装、そしてKotlinのCoroutinesによる実装を行います。

=== AsyncTask
AsyncTaskクラスは、非同期処理を実現する方法の中でもかなり簡単に実装できるクラスです。
ただし、非同期処理の結果をUIスレッドに返すときに少し注意が必要です。

AsyncTaskクラスを継承して書くなら@<list>{task_in_other}、
匿名クラスで書くなら@<list>{task_in_noname}のようになります。

//listnum[task_in_other][継承してSampleTaskを定義]{
open class SampleTask: AsyncTask<Void, Void, String>() {
    //SampleTaskクラスではworkerスレッドでの処理に集中
    override fun doInBackground(vararg params: Void?): String {
        //workerスレッドでの処理
        return "a"
    }
}

class SampleClass{
    //UI側の処理に集中するため、doInBackground以外を必要に応じoverride
    //SampleTaskをopenにしないとoverrideできないので注意！
    fun taskRun(){
        val task = object : SampleTask(){
            override fun onPreExecute(...){ }
            override fun onProgressUpdate(...){ }
            override fun onPostExecute(result: String?) {
                super.onPostExecute(result)
            }
        }
    }
}
//}

//listnum[task_in_noname][匿名クラスでの定義]{
class SampleClass{
    fun taskRun(){
        val task = object : AsyncTask<Void, Int, String>() {
            override fun onPreExecute() {
                super.onPreExecute()
                //UIの操作が可能。処理中のDialogFragmentを表示など
            }
            
            //doInBackgroudだけoverride必須
            override fun doInBackground(vararg params: Void?): String {
                //workerスレッドで行う処理。UIは操作不可！
                publishProgress(...) //onProgressUpdateのコール
                return "a"
            }

            override fun onProgressUpdate(vararg values: Int?) {
                super.onProgressUpdate(*values)
                //ここもUIの操作が可能
            }

            override fun onPostExecute(result: String?) {
                super.onPostExecute(result)
                //UIに結果を反映したりいろいろ
            }
        }
        task.execute()
    }
}
//}

@<list>{task_in_other}だと、引数の型や処理が変わるたびにクラスが増えていくので、ちょっとした処理なら
匿名クラスでいい気がします。リファレンスではAsyncTaskは数秒程度で終わる処理に対して適用すべきものと
記載されており、もし長時間バックグラウンドで処理するならばExecutorやThreadPoolExecutor、FutureTaskの検討を
強く推奨しています@<fn>{asynctask_ref}。

また、Kotlinで複数のメソッドをoverrideする場合は、object : AsyncTask<~>のように書く必要があります@<fn>{kotlin_object}。
さらに@<list>{task_in_other}のように別クラスで記述する場合は、openを付けて継承可能にしないとoverrideできません。
Kotlinは、classのみだとfinalとして扱われてしまうためです。

=== Coroutines
執筆中にKotlin Fest 2018がありましたが、ちょうどCoroutinesに関する神登壇@<fn>{coroutines}があったので記述します。

次の@<list>{coroutines_code}は、TwitterアプリのOAuth認証の抜粋です。
サーバと通信してトークンを取得していますが、通信処理はUIスレッド上で実行できない@<fn>{exception}ので、
async関数の中に記述します。

//listnum[coroutines_gradle][依存関係の追加 app/build.gradle]{
kotlin {
    //書かないとlaunch関数で警告が出る。無くても実行には影響なし
    experimental {
        coroutines 'enable'
    }
}

dependencies {
    ...
    implementation "org.jetbrains.kotlin:kotlin-stdlib-jdk7:$kotlin_version"
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-core:0.24.0'
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-android:0.24.0'
}
//}

//listnum[coroutines_code][Coroutineの実装]{
class SampleClass(){
    fun taskRun(){
        launch(UI) {　//UIスレッド上で実行
            //サーバとの通信処理や時間のかかる処理
            val requestToken: Deferred<String?> = async {
                try {
                    requestToken = twitter.getOAuthRequestToken(callbackUrl)
                    return@async requestToken.authenticationURL
                } ...
                return@async null
            }
            ... //asyncの処理と並行して処理を継続
            
            //asyncの結果が返るまでCoroutinesは中断される
            if (requestToken.await() != null){
                ...
            }
        }
        ...
    }
}
//}

launch関数は引数で実行するスレッドを選択でき、UIを指定するとUIスレッド上で
タスクを実行します（＝UIの変更が可能）。何も指定しなかった場合、あるいはCommonPoolを指定した場合は
スレッドプール上でタスクを実行します（＝UIの変更が不可能）。

@<list>{coroutines_code}では、launch関数の引数にUIを指定しているので、
CoroutinesはUIスレッド上で実行されます。
処理がasync関数まで到達すると、async関数の匿名メソッドがスレッドプール上で実行され、
並行してawait関数まで処理が進行します。
await関数に到達するとasync関数の結果が返るまでCoroutinesは中断され、
結果を受け取ったらUIスレッド上で処理を再開します@<fn>{asyncawait}。

//footnote[asyncawait][「async関数の匿名メソッドがスレッドプール上で実行」は厳密に言えば違うようなのですが、動作的にはこのような動きになります。もう少し勉強します。]

次にCoroutinesのキャンセルです（@<list>{coroutines_cancel}）。

//listnum[coroutines_cancel][Coroutinesのキャンセル]{
class SampleActivity: AppCompatActivity(){
    //一括キャンセル用のJob
    private var rootJob: Job? = null
    
    fun start(){
        val job = launch(UI, parent = rootJob){ ... }
        ...　//個別キャンセル
        job.cancel()
    }

    override fun onResume(){
        super.onResume()
        rootJob = Job()
    }

    override fun onPause(){
        super.onPause()
        //一括キャンセル
        rootJob?.cancel()
        rootJob = null
    }
}
//}

個別のCoroutinesのキャンセルは、launch関数が返すJobオブジェクトのcancelメソッドを使用します。

一方rootJobをlaunch関数に渡すと、rootJob.cancelをコールすることで一括キャンセルすることもできます。
Activity等のライフサイクルに応じてrootJobを生成・キャンセルすることで、
Activity破棄後でもバックグラウンドでCoroutinesが動いてしまうことを防止できます。

//footnote[asynctask_ref][https://developer.android.com/reference/android/os/AsyncTask]
//footnote[kotlin_object][オブジェクト式 https://dogwood008.github.io/kotlin-web-site-ja/docs/reference/object-declarations.html]
//footnote[coroutines][https://speakerdeck.com/sys1yagi/kotlin-korutinwo-li-jie-siyou]
//footnote[exception][UIスレッド上で通信処理を実行すると、NetworkOnMainThreadExceptionが発報します。]


