={implements} 実装集
//lead{
本章は、JavaとKotlinにてよくあるAndroidアプリの実装を集めました。
本章単体でも読めますが、@<chapref>{create-app}もあわせてお読みいただければ、
より理解が深まるかと思います。
//}

== Fragment
staticなnewInstanceメソッドを定義して生成するのが定番ですね。
Kotlinではcompanion object内にメソッドを定義することで、Javaと同様にコールできます。

//listnum[fragment_java][Fragmentの生成.Java]{
public void createFragment(){
    Fragment fragment = JavaFragment.newInstance("KC");
    FragmentTransaction transaction =
        getSupportFragmentManager().beginTransaction();
    transaction.add(fragment, "sample");
    transaction.commit();
}

public class JavaFragment extends Fragment{
    public JavaFragment(){ }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        //ViewのInfrateなどなど
        Bundle args = getArguments()
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

//listnum[fragment_kotlin][Fragmentの生成.kot]{
fun createFragment(){
    val fragment = KotlinFragment.newInstance("KC")
    val transaction = supportFragmentManager.beginTransaction()
    transaction.add(fragment, "sample")
    transaction.commit()
}

public class KotlinFragment: Fragment(){
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

FragmentはActivityと同様にライフサイクルを持っているため、途中でDestroyされる可能性があります。
Fragmentが自動で再生成されるとコンストラクタを通らずにonCreateからコールされるため、
new演算子での生成（というよりコンストラクタ内で初期化）は避けたほうが無難ですね。
一応呼び出し側（Activityなど）でsetArgumentsしても大丈夫ですが、呼び出し側が
どんどん肥大化するためお勧めできません。


=== FragmentとActivity間、FragmentとFragment間の通信
== カスタムAdapter
== カスタムLayout
== カスタムView
== カスタムPreference
== TabLayout
== Toolbar
== DrawerLayout
== 非同期処理（AsyncTask拡張）