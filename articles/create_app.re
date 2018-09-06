={create_app} Twitterアプリを作る
本章では、Kotlinにて簡単なTwitterクライアントアプリ（以下、単にアプリ）の作成を行います。
オールKotlinで記述し、@<chapref>{implements}で挙げた実装をできる限り使用してアプリを構築していきます。
なお、開発は@<chapref>{preface}に記載のとおりAndroid Studio 3.1.4で行いますので、
インストールがまだの方は最初にインストールをお願いします。

== 要件定義
アプリの要件は次の１件だけ。

 * フォロー一覧から技術書典の参加者を抽出して表示する。

その他ふぁぼRTやリプライ機能は今回は実装しません。
OAuth認証と非同期処理ができれば、他の処理の実装もそんなに難しくないと思います。

== できるアプリ
こんな感じのアプリができます。

//image[app_image][アプリ完成イメージ][scale=0.5]{
//}

ActivityはMainActivityとOAuth認証用（View無し）の２つ、DialogFragment１つの
超シンプル構成です。

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
アプリを作るにあたって、Twitter社に開発者アカウントの申請をします。
しかし悲しいことに、執筆中にアカウントの審査が厳しくなってしまいました。
正直ここが一番の鬼門です@<fn>{guchi}。

登録はTwitter Application Management@<fn>{app_management_url}を開いて、
Twitterアカウントでサインインしてください。開発者アカウントは事前に携帯電話番号の入力が必要なので、
開発者用に別アカウントを用意してもいいですね。

筆者はアカウント登録済みなので、登録の流れについては
次のQiitaを参考にしてください。

//quote{
新しくなった Twitter Developer ポータルに登録してみる

https://qiita.com/tdkn/items/521686c240b0c5bc6207
//}

申請後審査完了メールが届けば、無事開発者アカウントの登録完了です。

//footnote[app_management_url][https://apps.twitter.com/]
//footnote[guchi][なお筆者は申請から３週間弱かかりました。無理ゲー]

=== アプリの登録
アカウント登録が完了したら、次に作成するアプリを登録し、Consumer KeyとConsumer Secretを取得します。
KeyとSecretは、Twitter APIを通じて情報を取得するときに使用します。

Twitter Application Managementを開くと、次のような画面が表示されます（@<img>{app_management_top}）。

//image[app_management_top][Twitter Application Managementトップ][scale=0.75]{
//}

Create New Appをクリックし、アプリの情報を入力します。

//image[create_app][アプリの情報入力][scale=0.75]{
//}

アスタリスクが付いている項目は必須項目ですが、
CallbackURLはOAuth認証の際に必要になるので、実質必須項目です。

CallbackURLは「https://」から始まる適当な（ただし存在しない）URLを入力します。存在しなければ適当でかまいません。
以前のTwitterAPIではカスタムスキーマを設定することで、「twitter://callback」などが
使用できましたが、現在はカスタムスキーマが禁止されてしまいました。

OAuth認証が完了すると、ここで設定したURLにTwitterがリダイレクト→アプリ側でフック→アプリに戻ってくるという流れです。
実際の実装は後述します。

ここまで入力したら、Create Your Twitter Applicationをクリックすれば登録完了です。
Application Managementのトップに戻ると、@<img>{registered_top}のように登録したアプリが表示されていると思います。
真ん中の枠は気にしないでください@<fn>{kiiroi_waku}。

//footnote[kiiroi_waku][筆者アカウント審査中だったため]

//image[registered_top][アプリ登録後のApplication Management][scale=0.75]{
//}

アプリをクリックし、Keys and Access TokensタブをクリックするとConsumer KeyとConsumer Secretが表示されています。
あとからでも確認できるので、場所だけ覚えておけば大丈夫です。

//image[circlefinder][Keyの取得][scale=0.75]{
//}

これでキーの取得が完了しました。

== プロジェクトを作る
次にAndroid Studioを起動し、プロジェクトを作ります。

//image[create_project][プロジェクトの作成][scale=0.75]{
//}

Kotlinを使用するので、「Include Kotlin Support」にチェックを入れます。
ActivityはEmptyActivityから作っていきます。

== ライブラリをインポートする
無事にプロジェクトが作れたら、次にGradleを開きライブラリをインポートします。

//listnum[gradle_code][app.gradle]{
repositories {
    jcentor()
}

kotlin {
    experimental {
        coroutines 'enable'
    }
}

dependencies {
    ...
    implementation 'com.android.support:design:27.+'
    implementation 'org.twitter4j:twitter4j-core:4.0.+'
    implementation 'com.github.bumptech.glide:glide:4.7.1'
    annotationProcessor 'com.github.bumptech.glide:compiler:4.7.1'
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-core:0.24.0'
    implementation 'org.jetbrains.kotlinx:kotlinx-coroutines-android:0.24.0'
}
//}

執筆時点のTwitter4Jの最新バージョンは4.0.7でしたが、4.0.+として微修正等に追従するようにしています@<fn>{twitter4j_git}。

また、GlideはURLをセットするといい感じに画像をキャッシュしてくれるライブラリで、
アカウントアイコンの表示用に使用します。

Twitterサーバとの非同期処理はAsyncTaskでもAsyncTaskLoaderでもいいのですが、
せっかくのKotlinなのでCoroutinesで実装してみます。

//footnote[twitter4j_git][https://github.com/yusuke/Twitter4J]
//footnote[glide_hp][https://github.com/bumptech/glide]

== レイアウトファイルを作る
=== MainActivityのレイアウト
MainActivityのイメージは@<img>{activity_main}のとおりです。

//image[activity_main][MainActivityレイアウト][scale=0.5]{
//}

//listnum[main_xml][activity_main.xml]{
<android.support.constraint.ConstraintLayout
    ...>
    <android.support.v7.widget.Toolbar android:id="@+id/toolbar"
        ... >
    </android.support.v7.widget.Toolbar>
    <FrameLayout android:id="@+id/participant_frame"
        ... >
    </FrameLayout>
</android.support.constraint.ConstraintLayout>
//}

//listnum[menu_xml][menu_main.xml]{
<menu ...>
    <item android:id="@+id/update_follows"
        app:showAsAction="always"
        ... />
</menu>
//}

FrameLayoutは参加者一覧を表示するParticipantsFragment用に配置しています（@<list>{main_xml}）。
Toolbarにはフォローを取得するためのアップデートボタンを配置します（@<list>{menu_xml}）。

=== 参加者一覧用Fragmentのレイアウト
参加者一覧はListViewを使って表示します。
単純にFrameLayoutにListViewをmatch_parentで配置してるだけなので、
コード上で生成しても大丈夫だと思います。

//listnum[fragment_participants][fragment_participants.xml]{
<FrameLayout ...>
    <ListView android:id="@+id/participant_list"
        ... />
</FrameLayout>
//}

ListViewの項目は、カスタムアダプターを使って@<img>{adapter_image}のようにします。
項目には次の要素を表示します。

 * アイコン
 * 名前
 * スクリーンネーム
 * サークルスペース（技術書典）

//image[adapter_image][表示する情報][scale=0.75]{
//}

//listnum[custom_adapter_xml][adapter_item.xml]{
<android.support.constraint.ConstraintLayout ...
    android:layout_height="wrap_content">
    <ImageView android:id="@+id/profile_icon"
        ... />
    <TextView android:id="@+id/user_name"
        ... />
    <TextView android:id="@+id/screen_name"
        ... />
    <TextView android:id="@+id/circle_space"
        ... />
</android.support.constraint.ConstraintLayout>
//}

=== DialogFragmentのレイアウト
参加者取得中はダイアログを表示して待機させたいので、
待機用のDialogFragmentを作成します。
また待機中に何もレスポンスが無いといつ取得が終わるか分からないので、
最低限取得したフォローアカウントの数（counter）を表示するようにしています。

//listnum[fragment_dialog][fragment_dialog.xml]{
<android.support.constraint.ConstraintLayout ...>
    <TextView android:id="@+id/counter"
        ... />
    <TextView android:id="@+id/init_count"
        ... />
</android.support.constraint.ConstraintLayout>
//}

== 文字列リソースを用意する
strings.xmlに@<list>{strings}を追加します。

//listnum[strings][追加する文字列リソース]{
    <string name="consumer_key">ここにConsumer Keyを入力</string>
    <string name="consumer_secret">ここにConsumer Secretを入力</string>
    <string name="callback_url">ここにCallbackURLを入力</string>
//}

Consumer KeyとConsumer Secretは本来ユーザから見えないように組み込むべきですが、
今回はサンプルアプリなので平文で記述してしまいます。

== 実装
ここまでで必要なレイアウトの定義が終わったので、やっと実装に入っていけます。
実装は次の順番で行います。

 1. OAuth認証関係クラス（TwitterUtils, ConfirmOAuthActivity, TwitterOAuth）
 4. 参加者一覧抽出クラス（TwitterTask, StringMatcher）
 5. 参加者一覧表示フラグメント（ParticipantsFragment, ParticipantsAdapter）
 6. タスク進捗確認ダイアログ（ProgressDialogFragment）
 7. MainActivity

コード全容はGitHubにありますので、できる限りポイントを絞って解説していきます。

=== OAuth認証関係クラス
Twitterを自作アプリから操作する場合、操作するアカウント固有のAccessTokenを取得しなければなりません。
このAccessTokenを取得するための操作がOAuth認証となります。

さて、まずはTwitterオブジェクトを生成するためのSingletonなTwitterUtilsクラスを作成します。

//listnum[twitterutils][TwitterUtils.kt]{
//object宣言することでSingletonなクラスが作れる。すごーい！
object TwitterUtils {
    private enum class KeyEnum{ TOKEN, TOKEN_SECRET }

    fun getTwitter(context: Context): Twitter {
        //Twitterオブジェクトの生成
        val twitter = TwitterFactory().getInstance()
        //ConsumerKeyとConsumerSecretを入力
        twitter.setOAuthConsumer(context.getString(R.string.consumer_key), 
                                 context.getString(R.string.consumer_secret))
        //AccessTokenを生成
        val accessToken = loadAccessToken(context)
        if (accessToken != null){
            twitter.oAuthAccessToken = accessToken
        }
        return twitter
    }

    fun storeAccessToken(context: Context, accessToken: AccessToken){
        //AccessTokenの保持
        store(context, accessToken.token, accessToken.tokenSecret)
    }

    fun loadAccessToken(context: Context): AccessToken?{
        //DefaultSharedPreferencesからAccessTokenを取得
    }
    
    private fun store(context: Context, token: String?,
                                             tokenSecret: String?){
        //DefaultSharedPreferencesに保持
    }
}
//}

TwitterUtils.getTwitterでは、ConsumerKey、ConsumerSecret、
（保存されていれば）AccessToken及びAccessTokenSecretを詰めた状態のTwitterオブジェクトを返します。
ツイートの取得・送信等のTwitterの操作は、すべてTwitterUtils.getTwitterで生成したTwitterオブジェクトにより行います。

次に、OAuth認証をするためのConfirmOAuthActivity及びTwitterOAuthクラスを作成します。
ConfirmOAuthActivityは、OAuth認証により起動したWebブラウザからのコールバックを受け取るだけのActivityであり、
MainActivity内で実装することも不可能ではありませんが、認証とアプリ制御でコードがめちゃくちゃになりますので、
別々に分けることをオススメします。

//listnum[oauth_activity][ConfirmOAuthActivity.kt]{
class ConfirmOAuthActivity : AppCompatActivity() {
    private lateinit var oauth: TwitterOAuth
    companion object { val REQUEST_CODE = 1000 }

    override fun onCreate(savedInstanceState: Bundle?) {
        ...
        //即承認画面（Webページ）に飛ばす
        oauth = TwitterOAuth(this)
        oauth.startAuthorize()
    }

    //Webページでアプリを承認するとここに戻ってくる
    override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        oauth.oAuthApproval(intent)
        setResult(Activity.RESULT_OK, intent)
        finish()
    }

    //戻るボタンでActivityを閉じた場合はRESULT_CANCELED
    override fun onBackPressed() {
        super.onBackPressed()
        val intent = Intent().also { it.putExtra("msg", "finish") }
        //RESULT_CANCELEDセットしてfinish
        setResult(Activity.RESULT_CANCELED, intent)
        finish()
    }
}
//}

@<list>{oauth_core}はOAuth認証を処理するTwitterOAuthクラスです。
やっとCoroutinesを使う場面が出てきました。

//listnum[oauth_core][TwitterOAuth.kt]{
class TwitterOAuth(private val context: Context) {
    ...
    //認証開始。Coroutinesで処理してみる。
    fun startAuthorize(){
        launch(UI) {
            val requestToken: String? = async {
                try {
                    requestToken = twitter.getOAuthRequestToken(callbackUrl)
                    return@async requestToken.authenticationURL
                } ...
                return@async null
            }.await()

            if (requestToken != null){
                //Webブラウザを起動して認証ページを表示
                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(requestToken))
                context.startActivity(intent)
            } else {
                Log.d(javaClass.simpleName, "RequestToken取得失敗")
            }
        }
    }

    //ConfirmOAuthActivity#onNewIntentからコールされる
    fun oAuthApproval(intent: Intent?){
        ...
        launch(UI) {
            //Deferred<AccessToken?>が返る
            val deferred = async {
                try {
                    //RequestTokenはoauth_verifierというパラメータを所持。
                    //AccessTokenを取得するにあたってこのパラメータを与えてやる。
                    val verifier = intent.data
                                    .getQueryParameter("oauth_verifier")
                    return@async twitter
                                  .getOAuthAccessToken(requestToken, verifier)
                } ...
                return@async null
            }

            val accessToken = deferred.await() //ここで待機
            if (accessToken != null) {
                //認証成功。AccessTokenを保存して終了。
                TwitterUtils.storeAccessToken(context, accessToken)
            } else {
                Log.d(javaClass.simpleName, "認証失敗")
            }
        }
    }
}
//}

oAuthApprovalでは、あえてasync関数とawait関数を離してみました。
Deferred#await()でgetOAuthAccessTokenの結果を待機しています。
AccessTokenが正常に返ってくればOAuth認証完了です！

==={extract_participants} 参加者一覧抽出クラス
ここで、このアプリの要件は「フォロー一覧から技術書典の参加者を抽出して表示する」ことだったので、
まず「フォロー一覧」を取得し、次に「参加者を抽出」という手順で実装します。
フォロー一覧はgetFriendsIDsメソッド及びlookupUsersメソッドを組み合わせて行います。

 * getFriendsIDs：１回の呼び出しで最大5000件のユーザIDを取得。RateLimit=15
 * lookupUsers：１回の呼び出しで最大100件のUserオブジェクトを取得。RateLimit=900

getFriendsIDsが15分間で15回まで呼び出せるので最大7.5万アカウント、lookupUsersは
900回までなので最大9万アカウントまで対応できます。

//listnum[tasks][TwitterTask.kt - コールバックインターフェース定義]{
class TwitterTask(private val context:Context) {
    //MainActivityに実装するインターフェース
    interface TwitterTaskListener{
        fun setInitCount(count: Int)
        fun update(count: Int)
        fun complete(participants: List<User>?)
    }
}
//}

次の@<list>{task_method}は、フォロー一覧取得と参加者抽出のコードです。

//listnum[task_method][TwitterTask.kt - メソッド]{
fun createRootJob() { rootJob = Job() }
fun cancelAll() {
    rootJob?.cancel()
    rootJob = null
}

fun getParticipants(){
    val idsList = mutableListOf<Long>()
    var cursor: Long = -1L //ページング処理用

    launch(UI, parent = rootJob) {
        try {
            var ids: IDs
            do { //自身のIDを引数として渡す
                ids = async{ twitter.getFriendsIDs(twitter.id, cursor) }.await()
                val max = Math.ceil(ids.iDs.size / 100.0).toInt()
                for (count in 0 until max){ //100個ずつ区切って格納
                    idsList.add(ids.iDs
                        .sliceArray(count*100 until (count+1)*100))
                }
                cursor = ids.nextCursor //次のページ
            } while (ids.hasNext())

            if (idsList.size <= 0){
                ...
            } else {
                ...
                val participants = mutableListOf<User>()
                try {
                    //IDリストをぶん回しUserオブジェクトを取得
                    for (array in idsList){
                        listener?.update(cnt * 100)
                        val userResponseList = 
                            async { twitter.lookupUsers(*array) }.await()
                        for (user in userResponseList){
                            if (StringMatcher.getCircleSpace(user.name) != ""){
                                participants.add(user)
                            }
                        }
                    }
                    listener?.complete(participants)
                    ...
            }
        }
    }
}
//}

キャンセル処理はrootJob.cancelの一括キャンセルのみ記述していますが、
たとえばMap<Key, Job>を作って個別に管理することも可能です。
TwitterTask内でキャンセルを行っていますが、呼び出し側でsetRootJobを忘れると
一括キャンセルが効かないので、そこだけ注意が必要ですね。もっとよい実装がある気がします。

基本的には、Activityのライフサイクルに合わせてonResume/onPauseで
createRootJob及びcancelAllを呼べば大丈夫です。

サークルスペースは正規表現を使用してマッチングしています（@<list>{string_matcher}）。

//listnum[string_matcher][StringMatcher.kt]{
object StringMatcher {
    private val EVENT_PATTERN = "書[典展]"
    private val SPACE_PATTERN =
             ".*([a-zA-Zａ-ｚＡ-Ｚあ-んア-ン]).?([0-9０-９][0-9０-９])"
    
    fun getCircleSpace(name: String): String{
        //EVENT_PATTERNがマッチすれば、次にサークルスペースを抽出
        if (!hasEventPattern(name)) return ""
        val pattern = Pattern.compile(SPACE_PATTERN)
        val matcher = pattern.matcher(name)
        ...
    }

    private fun hasEventPattern(name: String): Boolean{
        val pattern = Pattern.compile(EVENT_PATTERN)
        val matcher = pattern.matcher(name)
        return matcher.find()
    }
}
//}

多くの即売会系イベントではサークルスペースのパターンがほぼ同じなので、
イベント名とスペース名で分けることで他イベントの場合でも適用しやすいようにしてみました。
a/b/abの区分けを付ける場合はもうひと工夫必要ですが、技術書典では無かったため省略しています。

=== 参加者一覧表示フラグメント
@<hd>{extract_participants}にて参加者を抽出できたので、
次はリストビューに一覧を表示します。
表示する項目は@<img>{adapter_image}のとおりです。

//listnum[participants_adapter][ParticipantsAdapter.kt]{
class ParticipantsAdapter(context: Context):
         ArrayAdapter<User>(context, android.R.layout.simple_list_item_1) {
    private val inflater = LayoutInflater.from(context)

    override fun getView(position: Int, convertView: View?,
                                         parent: ViewGroup?): View {
        val cv = convertView?:
                    inflater.inflate(R.layout.adapter_item, null).also {
            it.tag = ItemViewHolder(it)
        }

        val holder = cv.tag as ItemViewHolder
        val item = getItem(position)
        holder.name.text = item.name
        holder.screenName.text = "@" + item.screenName
        holder.circleSpace.text = StringMatcher.getCircleSpace(item.name)
        holder.icon.setOnClickListener{
            //プロフィールページを公式アプリで起動するなど。
            val profileUrl = "twitter://user?screen_name=" + item.screenName
            ...
        }
        Glide.with(context).load(item.profileImageURL).into(holder.icon)
        return cv
    }
    ...
    private class ItemViewHolder(view: View){ ... }
}
//}

ViewHolderパターンにて、各ViewにUserの情報を詰めています。
地味にエルビス演算子とスコープ関数により初期化を行ったりしています。
次にこのリストビューを保持するFragmentを作成します。

//listnum[participant_fragment][ParticipantsFragment.kt]{
class ParticipantsFragment: Fragment() {
    private lateinit var participants: ListView
    ...
    fun setAdapter(users: List<User>){
        if (context != null){
            val adapter = if (participants.adapter != null){
                (participants.adapter as ParticipantsAdapter).clear()
            } else {
                ParticipantsAdapter(context!!)
            }

            for (user in users){
                adapter.add(user)
            }
            participants.adapter = adapter
        }
    }

    companion object{ ... }
}
//}

Adapterの生成にcontext!!を使うのはちょっと臭う気もしますが、
しかして代替案も見つけられなかったのでこれで行ってみます。
また、Kotlinではif文は式なので、変数に代入するような形で記述できるのがとても便利ですね。

これでリストビューに参加者一覧が表示されるようになりました！

=== タスク進捗確認ダイアログ
ダイアログはキャンセル動作を確実に行うため、ICancelインターフェースを定義します。
MainActivityにICancelを実装しますが、DialogFragmentとのヒモづけは
onAttachをoverrideしてフィールドに参照をもつようにしています。

//listnum[dialog][ProgressDialogFragment.kt]{
class ProgressDialogFragment: DialogFragment() {
    interface ICancel{
        fun cancel()
    }
    private var cancellable: ICancel? = null
    ...
    override fun onAttach(context: Context?) {
        super.onAttach(context)
        if (context is ICancel){
            cancellable = context
        }
    }

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        ...
        val builder = AlertDialog.Builder(context)
        builder.setView(view).setTitle(title)
                .setNegativeButton("cancel") {
                 dialogInterface: DialogInterface, i: Int ->
                    dialogInterface.cancel()
                }
        return builder.create()
    }

    override fun onCancel(dialog: DialogInterface?) {
        cancellable?.cancel()
        ...
    }

    fun setCount(count: Int) { ... }
    ...
}
//}

キャンセル処理は今回インターフェースを定義して行っていますが、
onAttachでコールバック先を保持し、cancelメソッドを呼び出す方が
個人的には安心のような気がします。
ここはいろいろとやりようがあると思いますので、お好きな方法で実装してみてください。

ただし、無いとは思いますが、コールバック先をコンストラクタで保持して、
クラス固有のメソッドをコールするような真似はいけません…それはいけない……。@<fn>{ikenai}

//footnote[ikenai][オブジェクト指向もカプセル化もよく知らないまま作ってた頃はよくやってました（遠い目）]

=== MainActivityの実装
やっと最後のMainActivityの実装です。
@<list>{main}にコードを示しますが、ライフサイクルに合わせてタスク関係の処理をoverrideしたら
けっこう長くなってしまいました。中身はそこまで凝ったことはしておりません。

//listnum[main][MainActivity.kt]{
class MainActivity : AppCompatActivity(),
                     ProgressDialogFragment.ICancel,
                     TwitterTask.TwitterTaskListener {
    override fun update(count: Int) {
        ...
        progressDialogFragment.setCount(count)
    }

    override fun onActivityResult(...) {
        ...　//認証をキャンセルした場合、アプリ終了
    }

    override fun onResume() {
        super.onResume()
        task?.setRootJob()
    }

    override fun onPause() {
        super.onPause()
        task?.cancelAll()
    }

    override fun cancel(){
        task?.cancelAll()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        ...
        if (TwitterUtils.loadAccessToken(this) == null){
            //AccessTokenが保存されていなかったら、すぐ認証処理開始
            val intent = Intent(this, ConfirmOAuthActivity::class.java)
            startActivityForResult(intent, ConfirmOAuthActivity.REQUEST_CODE)
        }
        ...
        toolbar.setOnMenuItemClickListener {
            ...
            //キャンセル付きダイアログ表示
            val dialog = ProgressDialogFragment.newInstance()
            dialog.show(supportFragmentManager, FragmentTag.PROGRESS.name)
            ...
            //フォロー一覧を取得
            //コールバックでアダプターにセット
            task.getParticipants()
        }
        ...
    }
}
//}

TwitterTaskをフィールドに保持して、ライフサイクルイベントで
TwitterTaskのメソッドを実行しています。
setRootJobの呼び出しを忘れるとcancelAllが効かなくなってしまうので、
そこだけ要注意ですね。

== AndroidManifestを編集する
最後に、本アプリではネット通信により各種データを取得するため、
AndroidManifest.xmlにパーミッションを追加します。
また、ConfirmOAuthActivityではコールバックを受けるため、
intent-filterを設定しdataタグを付与します。
schemeに「https」を、hostにアプリの登録で設定したCallbackURLを記述することで、
認証画面からのコールバックを受け取ることができます。

//list[permission][AndroidManifest.xml]{
<uses-permission android:name="android.permission.INTERNET"/>
...
<activity
    android:name=".ConfirmOAuthActivity"
    android:launchMode="singleTask">
    <intent-filter>
        <action android:name="android.intent.action.VIEW" />
        <category android:name="android.intent.category.DEFAULT" />
        <category android:name="android.intent.category.BROWSABLE" />

        <data
            android:scheme="https"
            android:host="callback" /> "https://以降の部分"
    </intent-filter>
</activity>
//}

== 動作確認
ここまで作り終わったら、いよいよアプリの起動です！
適当にNexus5、Android API 27あたりでエミュレータをデバッグモードで起動してみてください。

