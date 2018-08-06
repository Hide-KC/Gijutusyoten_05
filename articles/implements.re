={implements} 実装集
//lead{
本章は、JavaとKotlinにてよくあるAndroidアプリの実装を集めました。
本章単体でも読めますが、@<chapref>{create-app}もあわせてお読みいただければ、
より理解が深まるかと思います。
//}

== Fragment
staticなnewInstanceメソッドを定義して生成するのが定番ですね。
Kotlinではcompanion object内にメソッドを定義することで、クラス名.newInstance()としてコールできます。

//listnum[fragment_java][Fragmentの生成-Java]{
public void createFragment(){
    Fragment fragment = JavaFragment.newInstance("KC");
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
        fragment.setTargetFragment(target, 0);
        return fragment;
    }
}
//}

//listnum[fragment_kotlin][Fragmentの生成-Kotlin]{
fun createFragment(){
    val fragment = KotlinFragment.newInstance("KC")
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
            fragment.setTargetFragment(target, 0)
            return fragment
        }
    }
}
//}

new演算子で生成～コンストラクタで初期化処理みたいな実装をすると、Fragmentが自動で再生成された際に
onCreateから読み込まれるため、正常に初期化が行われない可能性があります。
Fragmentの初期化はonCreateView内でgetArgumentsしてBundleから初期化に必要な値を
取得するようにしましょう。なんかルー大柴っぽい。

一応呼び出し側（Activityなど）でsetArgumentsしても大丈夫ですが、呼び出し側が
どんどん肥大化するためお勧めできません。

=== Fragmentの親子間通信
子Fragmentから親、またFragment間でイベントを通知するにはinterfaceを使いましょう。
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

注意としては、子1→子2のように、直接の親子関係がないFragment間で参照するのはやめましょう。
Fragment同士の結びつきが強くなり、後々の追加・修正時に泣きを見ます。

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
class SampleDTO{
    String name; int resId;
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

Kotlinではエルビス演算子@<fn>{elbis}（?:）とスコープ関数（also）により、可読性を損なうことなく
簡潔に記述することができます。

//footnote[elbis][エルビス・プレスリーのリーゼント（ポンパドール）に見えることから、だそうです。]

#@# === StickyListHeadersListView

== カスタムView
カスタムViewはViewを継承したクラスを作り、最低限onDrawをoverrideすればOKです。

//listnum[view_java][カスタムView-Java]{
class MyView extends View{
    public MyViewJava(Context context){
        super(context, null);
    }

    public MyViewJava(Context context, AttributeSet attrs){
        super(context, attrs);
    }

    public MyViewJava(Context context, AttributeSet attrs, int defStyleAttr){
        super(context, attrs, defStyleAttr);
    }


}
//}

//listnum[view_kotlin][カスタムView-Kotlin]{
class MyView: View {
    constructor(context: Context): super(context, null)
    constructor(context: Context, attrs: AttributeSet?)
     : super(context, attrs)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int)
     : super(context, attrs, defStyleAttr)

    
}
//}

Kotlinのコンストラクタは誌面の都合上改行していますが、普通に書けば３行になります。


== カスタムPreference
== TabLayout
== Toolbar
== DrawerLayout
== 非同期処理（AsyncTask拡張）