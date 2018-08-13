={implements} 実装集
//lead{
本章は、JavaとKotlinにてよくあるAndroidアプリの実装を集めました。
本章単体でも読めますが、@<chapref>{create_app}もあわせてお読みいただければ、
より理解が深まるかと思います。
//}

== Fragment
staticなnewInstanceメソッドを定義して生成するのが定番ですね。
Kotlinではcompanion object内にメソッドを定義することで、クラス名.newInstance()としてコールできます。

//listnum[fragment_java][Fragmentの生成-Java]{
//Activity内のメソッド
public void createFragment(){
    Fragment fragment = JavaFragment.newInstance(null, "Sample");
    FragmentTransaction transaction =
        getSupportFragmentManager().beginTransaction();
    transaction.add(fragment, "sample");
    transaction.commit();
}

class JavaFragment extends Fragment{
    public JavaFragment(){ }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        //ViewのInfrateなどなど
        Bundle args = getArguments();
    }

    static JavaFragment newInstance(Fragment target, String name){
        JavaFragment fragment = new JavaFragment();
        Bundle args = new Bundle();
        args.putString("name", name);
        fragment.setArguments(args);
        if (target != null){
            fragment.setTargetFragment(target, 0);
        }
        return fragment;
    }
}
//}

//listnum[fragment_kotlin][Fragmentの生成-Kotlin]{
//Activity内のメソッド
fun createFragment(){
    val fragment = KotlinFragment.newInstance(null, "Sample")
    val transaction = supportFragmentManager.beginTransaction()
    transaction.add(fragment, "sample")
    transaction.commit()
}

class KotlinFragment: Fragment(){
    override fun onCreateView(inflater: LayoutInflater,
                              container: ViewGroup?,
                              savedInstanceState: Bundle?): View? {
        //ViewのInflateなどなど
        val args: Bundle? = arguments
    }
    
    companion object{
        fun newInstance(target: Fragment?, name: String): KotlinFragment{
            val fragment = KotlinFragment()            
            val args = Bundle()
            args.putString("name", name)
            fragment.arguments = args
            if (target != null){
                fragment.setTargetFragment(target, 0)
            }
            return fragment
        }
    }
}
//}

new演算子で生成～コンストラクタで初期化処理みたいな実装をすると、Fragmentが自動で再生成された際に
onCreateから読み込まれるため、正常に初期化が行われない可能性があります。
Fragmentの初期化はonCreateView内でgetArgumentsしてBundleから初期化に必要な値を
取得するようにしましょう。なんかルー大柴っぽい。

一応呼び出し側（Activityなど）でsetArgumentsしても大丈夫ですが、呼び出し側に
本来子側でできるはずの処理が記述されるためお勧めできません。

=== Fragmentの親子間通信
子Fragmentから親、またFragment間でイベントを通知するにはinterfaceを使います。
@<list>{notify_Java}では、子1→親→子2の順でイベントの通知を行っています。

//listnum[notify_Java][親子間通信-Java]{
interface OnSampleChangeListener{
    void onSampleChange();
}

//親（Activity）
class SampleActivity extends AppCompatActivity
                     implements OnSampleChangeListener{
    @Override
    void onSampleChange(){
        MyFragment2 fragment = getSupportFragmentManager()
                               .findFragmentByTag("fragment2");
        if (fragment instanceof MyFragment2){
            ((MyFragment2)fragment).change();
        }
    }
}

//イベント発信側の子1(Fragment)
class MyFragment1 extends Fragment{
    private OnSampleChangeListener listener;

    //API Lollipop未満は、onAttach(Activity activity)もOverrideする。
    @Override
    public void onAttach(Context context){
        super.onAttach(context);
        if (context instanceof OnSampleChangeListener){
            listener = (OnSampleChangeListener)context;
        }
    }

    //onClickなどから呼び出す。
    private void fire(){
        if (listener != null){
            listener.onSampleChange();
        }
    }
}

//イベント受信側の子2(Fragment)
class MyFragment2 extends Fragment{
    public void change(){
        //処理を記述
    }
}

//}

//listnum[notify_Kotlin][親子間通信-Kotlin]{
interface OnSampleChangeListener{
    fun onSampleChange()
}

//親（Activity）
class SampleActivity: AppCompatActivity,
                      OnSampleChangeListener{
    override void onSampleChange(){
        val fragment = supportFragmentManager
                        .findFragmentByTag("fragment2");
        if (fragment is MyFragment2){
            fragment.change();
        }
    }
}

//イベント発信側の子1(Fragment)
class MyFragment1: Fragment{
    private var listener: OnSampleChangeListener? = null

    override fun onAttach(context: Context){
        super.onAttach(context)
        if (context is OnSampleChangeListener){
            listener = context
        }
    }

    //onClickなどから呼び出す。
    private fun fire(){
        listener?.onSampleChange()
    }
}

//イベント受信側の子2(Fragment)
class MyFragment2: Fragment{
    public fun change(){
        //処理を記述
    }
}
//}

注意としては、子1→子2のように、直接の親子関係がないFragment間で参照するのは避けたほうがいいようです。
受信側のFragmentが必ずしも生きているとは限りませんし、またFragment同士の結びつきが強くなり、
後々の追加・修正時に泣きを見ます。

Kotlinはスマートキャストにより、ifブロック内でいちいちキャストをしなくて済むため
だいぶスッキリ書けますね。Kotlinはいいぞ。

== カスタムAdapter
標準のAdapterでは基本型しか扱えないので、すぐに限界がくると思います。
@<img>{image_adp}のようなレイアウトファイルを作り、カスタムAdapter内でInflateし、
ListViewにセットします。

ここでSampleDTOは、フィールドにString nameおよびint resIdのみをもつオブジェクトです。

//image[image_adp][作るListViewのイメージ]{
//}

//listnum[dto][SampleDTO]{
//Java
class SampleDTO{
    String name; int resId;
}

//Kotlinではdata classが使える
data class SampleDTO{
    val name: String,
    val resId: Int
}
//}

//listnum[adp_java][カスタムAdapter-Java]{
class MyAdapter extends ArrayAdapter<SampleDTO>{
    private LayoutInflater inflater;

    public MyAdapter(Context context) {
        super(context, android.R.layout.simple_list_item_1);
        inflater = LayoutInflater.from(context);
    }

    @NonNull
    @Override
    public View getView(int position, @Nullable View convertView,
                        @NonNull ViewGroup parent) {
        final ItemViewHolder holder;

        if(convertView == null){
            convertView = inflater.inflate(R.layout.item_layout, null);
            holder = new ItemViewHolder(convertView);
            convertView.setTag(holder);
        } else {
            holder = (ItemViewHolder)convertView.getTag();
        }

        //DTOから各値を格納します。
        final SampleDTO sampleDTO = getItem(position);
        holder.name.setText(sampleDTO.name);
        holder.image.setImageResource(sampleDTO.image));
        return convertView;
    }

    static class ItemViewHolder{
        TextView name;
        ImageView image;
        public ItemViewHolder(View view){
            name = view.findViewById(R.id.user_name);
            image = view.findViewById(R.id.user_image);
        }
    }
}
//}

//listnum[adp_kotlin][カスタムAdapter-Kotlin]{
class HistoryListAdapter(context: Context):
      ArrayAdapter<HistoryDTO>(context, android.R.layout.simple_list_item_1) {
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
        holder.image.setImageResource(sampleDTO.image)
        return cv
    }

    private class ItemViewHolder(view: View){
        val name: TextView = view.findViewById(R.id.user_name)
        val image: ImageView = view.findViewById(R.id.user_image)
    }
}
//}

いわゆるViewHolderパターンによる実装です。

紙面の都合により変な場所で改行が入っていますが、Kotlinではエルビス演算子@<fn>{elbis}（?:）と
スコープ関数（also）により、初期化処理を簡潔に記述することができます。

//footnote[elbis][エルビス・プレスリーのリーゼント（ポンパドール）に見えることから、だそうです。]

#@# === StickyListHeadersListView

== カスタムView
カスタムViewはViewを継承したクラスを作り、最低限onDrawをoverrideすればOKです。

//listnum[view_java][カスタムView-Java]{
class MyView extends View{
    public MyViewJava(Context context){
        super(context, null); //1
    }

    public MyViewJava(Context context, AttributeSet attrs){
        super(context, attrs); //2
    }

    public MyViewJava(Context context, AttributeSet attrs, int defStyleAttr){
        super(context, attrs, defStyleAttr); //3
    }

    @Override
    public void onDraw(Canvas canvas){
        super.onDraw(canvas);
        //円を書いたり塗りつぶしたり
    }

    @Override
    public boolean onTouchEvent(MotionEvent event){
        //event.x, event.yからローカル座標取得
    }
}
//}

//listnum[view_kotlin][カスタムView-Kotlin]{
class MyView: View {
    constructor(context: Context): super(context, null) //1
    constructor(context: Context, attrs: AttributeSet?) 
     : super(context, attrs) //2
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int)
     : super(context, attrs, defStyleAttr) //3

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
このため@<list>{view_java}にも記載はしていません。すみません。

Viewの定義が終わったら、Android Studioのメニューバーから Build→Rebuild Project
をしてやることで、LayoutEditorのPallet→ProjectにカスタムViewが表示されます。
あとは他のViewと同じようにxmlで配置してください。

見返したらイマイチ薄味な内容になってしまったので、付録に自作カラーピッカーの実装載せておきます。よしなに。

=== カスタムLayout（ConstraintLayoutの拡張）
あまり需要はないかもしれませんが、カスタムLayoutについても少し記述します。
といっても、筆者はFrameLayoutとConstraintLayoutを拡張したカスタムLayoutしか作ったことがないので、
参考程度にお読みください。

基本はカスタムViewと同様で、コンストラクタのoverrideも同様に行ってください。
ConstraintLayout特有の制約（Constraint）の付け方を@<list>{constraint}に示します。
ついでに何かと便利なGuidelineも生成します。

//listnum[constraint][ConstraintLayoutの初期化-Kotlin]{
class MyLayout: ConstraintLayout{
    //コンストラクタは、カスタムViewと同じく３つoverride
    //Guideline用のId。フィールドに保持。
    val leftId = View.generateViewId()
    val topId = View.generateViewId()
    val resId = View.generateViewId()

    init{
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

        //Match Constarintの設定
        constraintSet.constrainWidth(resId, ConstraintSet.MATCH_CONSTRAINT)
        constraintSet.constrainHeight(resId, ConstraintSet.MATCH_CONSTRAINT)

        //設定の適用。
        constraintSet.applyTo(this)
    }
}
//}

constraintSetインスタンスにいったんcloneして、ごにょごにょ制約を付けた後、applyToで適用するという流れです。
コードで生成したImageViewのTopとLeft（START）をGuidelineに、BottomとRight（END）を親コンテナに紐づけてます。
親コンテナのIDはConstraintSet.PARENT_IDで取得できます。

この節を執筆中（8月上旬）、新たにMotionLayoutが発表されました。ConstraintLayoutの子クラスであり、
ConstraintSetからモーションを設定できるようです。
今はリリースされたばかりで解説が少ないですが、技術書典が開催される10月ごろには
もっとQiitaとかで多くなってますかね。楽しみです。（自分で書け）

== カスタムPreference
１つのPreferenceにいろいろな機能をもたせたい！ではカスタムしましょう。

作るのはEditTextPreferenceとCheckBoxPreferenceを足したような機能をもったPreferenceです（@<img>{preference}）。
チェックボックスの切替及びダイアログを表示（title要素とsummary要素の編集）する機能を持っています@<fn>{preference_properties}。

//footnote[preference_properties][正確には、titleとsummaryはSharedPreference#getStringから取得しています。]

//image[preference][カスタムPreference][scale=0.75]{
//}

//listnum[preference_xml][カスタムPreference-Layout xml]{
<android.support.constraint.ConstraintLayout
    //...
    android:id="@android:id/widget_frame">

    <TextView
        android:id="@android:id/title"
        //..他要素省略
        />

    <TextView
        android:id="@android:id/summary"
        //..他要素省略
        />

    <CheckBox
        android:id="@+id/checkBox"
        //..他要素省略
        />
</android.support.constraint.ConstraintLayout>
//}

title要素とsummary要素になるTextViewには、idを"@android:id/title（summary）"として付与してください。
SharedPreferences#getStringでtitle要素が取得できるようになります。

またrootのidが"@android:id/widget_frame"でないと正常にInflateできないので、こちらも付与してください@<fn>{pref_reference}。

//footnote[pref_reference][onCreateView参照 https://developer.android.com/reference/android/preference/Preference]

コンストラクタは、カスタムViewと同様に３つoverrideします。

//listnum[pref_java][カスタムPreference-Java]{
class MyPreference extends Preference{
//コンストラクタのoverride

@Override
    protected View onCreateView(ViewGroup parent) {
        //Layoutのinfrate
        super.onCreateView(parent);
        LayoutInflater inflater = LayoutInflater.from(getContext());
        //TextView × 2, CheckBoxを配したレイアウトを展開する
        return inflater.inflate(R.layout.my_preference, parent,false);
    }

@Override
protected View onBindView(View view)
    super.onBindeView(view);
    //初期化処理はココ
    view.setOnClickListener(
        //ダイアログの表示処理
    )

    SharedPreferences prefs = getContext().getSharedPreferences(
                                getKey(), Context.MODE_PRIVATE);
    TextView titleView = view.findViewById(android.R.id.title);
    titleView.setText(prefs.getString("title", "COMITIA"));
    
    CheckBox checkBox = view.findViewById(R.id.checkBox);
    checkBox.setOnCheckedChangeListener( //リスナーセットしてチェック状態監視 );
}
//}

//listnum[pref_kotlin][カスタムPreference-Kotlin]{
class MyPreference: Preference{
//コンストラクタのoverride

    override fun onCreateView(parent: ViewGroup?): View {
        super.onCreateView(parent)
        val inflater = LayoutInflater.from(context)
        return inflater.inflate(R.layout.my_preference, parent, false)
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
//}

Fragmentでは初期化処理をonCreateViewで行っていましたが、リファレンスにも書かれているとおり
onBindViewでプロパティ設定等の処理を行っています。onCreateViewで初期化をしていると、
項目が入れ替わったり変な挙動になるようです@<fn>{onbindview}。

view.setOnClickListenerでは@<img>{preference}のダイアログ表示を仕込みます。
なお、カスタムPreferenceの内容からは離れてしまいますので詳細は省きますが、
ダイアログのOKボタンを押した後、onBindViewが走らない@<b>{ことがある}ので、
コールバックインターフェースを実装するなどにより確実に値更新をしてやってください。
DialogFragmentとコールバックの実装は別途GitHubに上げておきます（あとがき参照）。

//footnote[onbindview][http://ksoichiro.blogspot.com/2011/05/android-preference.html]
//footnote[dialog_result][ソフトキーボードの表示・非表示時にonBindViewが走るよう。]

#@# == TabLayout
#@# == Toolbar
#@# == DrawerLayout
== 非同期処理（AsyncTask拡張）
AsyncTaskクラスは、非同期処理を実現する方法の中でもかなり簡単に実装できるクラスです。
ただし、非同期処理の結果をUIスレッドに返すときに少し注意が必要です。

AsyncTaskクラスを継承して書くなら@<list>{task_in_other}、匿名クラスで書くなら@<list>{task_in_noname}のようになるでしょうか。

//listnum[task_in_other][継承してSampleTaskを定義-Kotlin]{
open class SampleTask: AsyncTask<Void, Void, Int>() {
    //SampleTaskクラスではworkerスレッドでの処理に集中。
    override fun doInBackground(vararg params: Void?): Int {
        //workerスレッドでの処理
        return 0
    }
}

class SampleClass{
    //UI側の処理に集中するため、onPostExecuteのみoverride。
    //TestTaskをopenにしないとoverrideできないので注意。
    fun taskRun(){
        val task = object : TestTask(){
            override fun onPostExecute(result: Int?) {
                super.onPostExecute(result)
            }
        }
    }
}
//}

//listnum[task_in_noname][匿名クラスでの定義-Kotlin]{
class SampleClass{
    fun taskRun(){
        val task = object : AsyncTask<Int, Int, Int>() {
            override fun onPreExecute() {
                super.onPreExecute()
                //UIの操作が可能。処理中のDialogFragmentを表示したり。
            }
            
            //doInBackgroudだけoverride必須
            override fun doInBackground(vararg params: Int?): Int {
                //workerスレッドで行う処理。UIは操作不可！
                publishProgress() //onProgressUpdateのコール
                return 0
            }

            override fun onProgressUpdate(vararg values: Int?) {
                super.onProgressUpdate(*values)
                //ここもUIの操作が可能
            }

            override fun onPostExecute(result: Int?) {
                super.onPostExecute(result)
                //UIに結果を反映したりいろいろ
            }
        }
        task.execute()
    }
}
//}

@<list>{task_in_other}だと、引数の型や処理が変わるたびにクラスが増えていくので、匿名クラスでの記述のほうが
使い勝手いい気がします。ただしStrategyパターンとかでworkerスレッドの処理をごそっと入れ替える場合は、
クラスを別に作るほかありませんね。

Kotlinで複数のメソッドをoverrideする場合は、object : AsyncTask<~>のように書く必要があります（オブジェクト式@<fn>{kotlin_object}）。
また@<list>{task_in_other}のように別クラスで記述する場合は、openを付けて継承可能にしないとoverrideできません。

//footnote[kotlin_object][https://dogwood008.github.io/kotlin-web-site-ja/docs/reference/object-declarations.html]


