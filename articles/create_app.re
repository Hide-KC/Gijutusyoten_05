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

//image[app_image][アプリ完成イメージ]{
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
しかし悲しいかな、執筆中にアカウントの審査が厳しくなってしまいました。
正直ここが一番の鬼門です@<fn>{guchi}。

登録はTwitter Application Management@<fn>{app_management_url}を開いて、
Twitterアカウントでサインインしてください。開発者アカウントは事前に携帯電話番号の入力が必要なので、
開発者用に別アカウント用意してもいいですね。

筆者はアカウント登録済みなので、登録の流れについては
次のQiitaを参考にしてください。

//quote{
新しくなった Twitter Developer ポータルに登録してみる

https://qiita.com/tdkn/items/521686c240b0c5bc6207
//}

申請後審査完了メールが届けば、無事開発者アカウントの登録完了です。

//footnote[app_management_url][https://apps.twitter.com/]
//footnote[guchi][なお筆者は申請から２週間くらいかかりました。無理ゲー]

=== アプリの登録
アカウント登録が完了したら、次に作成するアプリを登録し、Consumer KeyとConsumer Secretを取得します。
KeyとSecretは、Twitter APIを通じて情報を取得するときに使用します。

Twitter Application Managementを開くと、次のような画面が表示されます（@<img>{app_management_top}）。

//image[app_management_top][Twitter Application Managementトップ]{
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

アプリをクリックし、Keys and Access TokensタブをクリックするとConsumerKeyとConsumerSecretが表示されています。
あとからでも確認できるので、場所だけ覚えておきましょう。

//image[circlefinder][Keyの取得][scale=0.75]{
//}

これでキーの取得は完了です。

== プロジェクトを作る
次にAndroid Studioを起動し、プロジェクトを作ります。

//image[create_project][プロジェクトの作成][scale=0.75]{
//}

Kotlinを使用するので、「Include Kotlin Support」にチェックを入れます。
ActivityはEmptyActivityから作っていきましょう。

== ライブラリをインポートする
無事にプロジェクトが作れたら、次にGradleを開きライブラリをインポートします。

//listnum[gradle_code][ライブラリの依存関係の追加 app.gradle]{
repositories {
    jcentor()
}

kotlin {
    experimental {
        coroutines 'enable'
    }
}

dependencies {
    /..
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

Twitterサーバとの非同期処理はAsyncTaskでもAsyncTaskLoaderでもいいのですが、せっかくのKotlinなのでCoroutinesで実装してみます。

//footnote[twitter4j_git][https://github.com/yusuke/Twitter4J]
//footnote[glide_hp][https://github.com/bumptech/glide]

== レイアウトファイルを作る
=== MainActivity
MainActivityのイメージは@<img>{activity_main}のとおりです。

//image[activity_main][MainActivityレイアウト][scale=0.5]{
//}

//listnum[main_xml][MainActivityレイアウト.xml]{
<android.support.constraint.ConstraintLayout
    ...>
    <android.support.v7.widget.Toolbar
        android:id="@+id/toolbar"
        ...>
    </android.support.v7.widget.Toolbar>

    <FrameLayout
        android:id="@+id/participant_frame"
        ...>
    </FrameLayout>
</android.support.constraint.ConstraintLayout>
//}

//listnum[menu_xml][menu_main.xml]{
<menu ...>
    <item
        android:id="@+id/update_follows"
        app:showAsAction="always"
        ... />
</menu>
//}

FrameLayoutは参加者一覧を表示するParticipantsFragment用に配置しています（@<list>{main_xml}）。
Toolbarにはフォローを取得するためのアップデートボタンを配置します（@<list>{menu_xml}）。

=== 参加者一覧用のFragment
参加者一覧はListViewを使って表示します。
単純にFrameLayoutにListViewをmatch parentで配置してるだけなので、
レイアウトファイル作らなくてもたぶん大丈夫だと思います。

//listnum[fragment_participants][参加者一覧用Fragment.xml]{
<FrameLayout ...>
    <ListView
        android:id="@+id/participant_list"
        ...>
    </ListView>
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

//listnum[custom_adapter_xml][表示するフォローアカウントの情報]{
<android.support.constraint.ConstraintLayout ...
    android:layout_height="wrap_content">

    <ImageView
        android:id="@+id/profile_icon"
        ... />

    <TextView
        android:id="@+id/user_name"
        ... />

    <TextView
        android:id="@+id/screen_name"
        ... />

    <TextView
        android:id="@+id/circle_space"
        ... />

</android.support.constraint.ConstraintLayout>
//}

=== DialogFragment
参加者取得中はダイアログを表示して待機させたいので、
待機用のDialogFragmentを作成します。
また待機中に何もレスポンスが無いといつ取得が終わるか分からないので、
最低限取得したフォローアカウントの数（counter）を表示するようにしています。

//listnum[fragment_dialog][待機用Dialog.xml]{
<android.support.constraint.ConstraintLayout ...>
    <TextView
        android:id="@+id/counter"
        .../>
</android.support.constraint.ConstraintLayout>
//}

== 文字列リソースを用意する
strings.xmlに@<list>{strings}のとおり追加します。

//listnum[strings][追加する文字列リソース]{
    .../
    <string name="update">更新</string>
    <string name="accesstoken_success">認証成功</string>
    <string name="accesstoken_error">認証失敗</string>
    <string name="profile_url">"twitter://user?screen_name="</string>

    <string name="consumer_key">ここにConsumer Keyを入力</string>
    <string name="consumer_secret">ここにConsumer Secretを入力</string>
    <string name="callback_url">ここにCallbackURLを入力</string>
//}

Consumer KeyとConsumer Secretは本来ユーザから見えないように組み込むべきですが、
今回はサンプルアプリなので平文で記述してしまいます。

== 実装
ここまでで必要なレイアウトの生成が終わったので、やっと実装に入っていけます。書き下すと長かった。
実装は次の順番で行います。

 1. OAuth認証関係クラス（TwitterUtils, ConfirmOAuthActivity, TwitterOAuth）
 4. 参加者一覧抽出クラス（TwitterTask, StringMatcher）
 5. 参加者一覧表示フラグメント（ParticipantsFragment, ParticipantsAdapter）
 6. タスク進捗確認ダイアログ（ProgressDialogFragment）
 7. MainActivity

コード全容はGitHubに置いておきますので、ポイントを絞って解説していきます。

=== OAuth認証関係クラス
Twitterを自作アプリから操作する場合、操作するアカウント固有のAccessTokenを取得しなければなりません。
このAccessTokenを取得するための操作がOAuth認証となります。
AccessTokenが流出すると第三者からアカウントの乗っ取りが可能になってしまいますので、絶対に漏らさないようにしましょう。

さて、まずはTwitterオブジェクトを生成するためのSingletonなTwitterUtilsクラスを作成します。

//listnum[twitterutils][TwitterUtils.kt]{
//object宣言することでSingletonなクラスが作れる。すごーい！
object TwitterUtils {
    private enum class KeyEnum{
        TOKEN, TOKEN_SECRET
    }

    fun getTwitter(context: Context): Twitter {
        //Twitterオブジェクトの生成
        val twitter = TwitterFactory.getSingleton()

        //ConsumerKeyとConsumerSecretを入力
        twitter.setOAuthConsumer(context.getString(R.string.consumer_key), 
                                 context.getString(R.string.consumer_secret))

        //AccessTokenを生成
        val accessToken = loadAccessToken(context)
        if (accessToken != null){
            twitter.oAuthAccessToken = accessToken
        } else {
            Log.d(javaClass.simpleName, "AccessToken is not seved")
        }

        //AccessTokenをセットしたTwitterオブジェクトを返す
        return twitter
    }

    fun storeMyAccount(context: Context, accessToken: AccessToken){
        //AccessTokenの保持
        storeAccessToken(context, accessToken.token, accessToken.tokenSecret)
    }

    fun loadAccessToken(context: Context): AccessToken?{
        //DefaultSharedPreferencesからAccessTokenを取得
        ...
        return if (token != null && tokenSecret != null){
            AccessToken(token, tokenSecret)
        } else {
            null
        }
    }

    fun deleteAccessToken(context: Context){
        storeAccessToken(context, null, null)
    }

    private fun storeAccessToken(context: Context, token: String?,
                                             tokenSecret: String?){
        //DefaultSharedPreferencesに保持
        ...
    }
}
//}

TwitterUtils.getTwitterでは、ConsumerKey、ConsumerSecret、
（保存されていれば）AccessToken及びAccessTokenSecretを詰めた状態のTwitterオブジェクトを返します。
ツイートの取得・送信等のTwitterの操作は、すべてここで生成したTwitterオブジェクトにより行います。

次に、OAuth認証をするためのConfirmOAuthActivity及びTwitterOAuthクラスを作成します。
ConfirmOAuthActivityは、OAuth認証により起動したWebブラウザからのコールバックを受け取るだけのActivityであり、
MainActivity内で実装することも不可能ではありませんが、認証とアプリ制御でコードがめちゃくちゃになりますので、
別々に分けることをオススメします。

//listnum[oauth_activity][ConfirmOAuthActivity.kt]{
class ConfirmOAuthActivity : AppCompatActivity() {
    private val oauth: TwitterOAuth = TwitterOAuth(this) 
    companion object {
        val REQUEST_CODE = 1000 //Javaでいうpublic static finalな変数
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_confirm_oauth)

        //即承認画面（Webページ）に飛ばす。
        //実際は確認ボタンやダイアログ表示など。
        oauth.startAuthorize()
    }

    //Webページでアプリを承認するとここに戻ってくる
    override fun onNewIntent(intent: Intent?) {
        super.onNewIntent(intent)
        oauth.oAuthApproval(this, intent)
        setResult(Activity.RESULT_OK, intent)
        finish()
    }

    //戻るボタンでActivityを閉じた場合はRESULT_CANCELE
    override fun onBackPressed() {
        super.onBackPressed()
        val intent = Intent()
        intent.putExtra("msg", "finish")
        setResult(Activity.RESULT_CANCELED, intent)
        finish()
    }
}
//}

実際のOAuth認証を処理するTwitterOAuthクラスを、@<list>{oauth_core}に示します。
やっとこさCoroutinesを使う場面が出てきました。

//listnum[oauth_core][TwitterOAuth.kt]{
class TwitterOAuth(private val context: Context) {
    private val twitter: Twitter = TwitterUtils.getTwitter(context)
    private val callbackUrl = context.getString(R.string.callback_url)
    private lateinit var requestToken: RequestToken

    //認証開始。Coroutinesで処理してみる。
    fun startAuthorize(){
        launch(UI) {
            val requestToken: String? = async {
                try {
                    requestToken = twitter.getOAuthRequestToken(callbackUrl)
                    return@async requestToken.authenticationURL
                } catch (e: IllegalStateException) {
                    e.printStackTrace()
                } catch (e: TwitterException) {
                    e.printStackTrace()
                }
                return@async null
            }.await()

            if (requestToken != null){
                //Webブラウザを起動
                val intent = Intent(Intent.ACTION_VIEW, Uri.parse(requestToken))
                context.startActivity(intent)
            } else {
                Log.d(javaClass.simpleName, "Authorize失敗")
            }
        }
    }

    //ConfirmOAuthActivity#onNewIntentからコールされる
    fun oAuthApproval(context: Context, intent: Intent?){
        if (intent == null || intent.data == null ||
            !intent.data.toString().startsWith(callbackUrl)){
            //callbackUrlでない場合はreturn。startsWith使わなくてもいい気がする。
            return
        }

        launch(UI) {
            //Deferred<AccessToken?>が返る
            val deferred = async {
                try {
                    //RequestTokenにはoauth_verifierというパラメータが含まれている。
                    //AccessTokenを取得するにあたってこのパラメータを与えてやる。
                    return@async twitter.getOAuthAccessToken
                                            (requestToken, "oauth_verifier")
                } catch (e: TwitterException) {
                    e.printStackTrace()
                }
                return@async null
            }

            val accessToken = deferred.await() //ここで待機
            if (accessToken.await() != null) {
                //認証成功。AccessTokenを保存して終了。
                TwitterUtils.storeMyAccount(context, accessToken)
                Log.d("Comiketter", "認証成功！")
            } else {
                Log.d("Comiketter", "認証失敗。")
            }
        }
    }
}
//}

oAuthApprovalでは、あえてasync関数とawait関数を離してみました。
AccessTokenが正常に返ってくればOAuth認証完了です！

=== 参加者一覧抽出クラス
ここで、このアプリの要件は「フォロー一覧から技術書典の参加者を抽出して表示する」ことだったので、
まずは「フォロー一覧」を取得します。
取得はTwitter#getFriendsIDsメソッド及びlookupUsersメソッドを組み合わせて行います。

 * getFriendsIDs：１回の呼び出しで最大5000件のユーザIDを取得。RateLimit=15
 * lookupUsers：１回の呼び出しで最大100件のUserオブジェクトを取得。RateLimit=900

getFriendsIDsが15分間で15回まで呼び出せるので最大7.5万アカウント、lookupUsersは
900回までなので最大9万アカウントまで対応できます。

//listnum[tasks][TwitterTask.kt]{
class TwitterTask(private val context:Context,
                    private val rootJob: Job?) {
    interface UpdateListener{
        fun update(count: Int) //DialogFragmentに値を通知する用
    }

    fun cancel(){
        //ダイアログからのキャンセル受信用
        rootJob?.cancel()
        rootJob = null
    }

    fun getFollow(): List<User>{
        val follows = mutableListOf<User>()
        val twitter = TwitterUtils.getTwitter(context)
        var ids: IDs
        val idsList = mutableListOf<Long>()
        var cursor: Long = -1L //ページング処理

        launch(UI, parent = rootJob) {
            try {
                do {
                    //自身のIDを引数として渡す
                    ids = async{ twitter.getFriendsIDs(twitter.id, cursor) }
                                .await()
                    for (id: Long in ids.iDs){
                        idsList.add(id)
                    }
                    //次のページ
                    cursor = ids.nextCursor
                } while (ids.hasNext())

                if (idsList.size <= 0){
                    //取得IDsが0個だった場合return
                    return@launch
                }

                //IDリストをぶん回しUserオブジェクトを取得
                ...
                for (user_i: Int in 0 until max){
                    ...
                    val userResponseList = async {
                        twitter.lookupUsers(*mlist.toLongArray())
                        }.await()
                    for (user in userResponseList){
                        if (StringMatcher.getCircleSpace(user.name) != ""){
                            follows.add(user)
                        }
                    }

                    if (context is UpdateListener){
                        //MainActivityに経過を通知
                        context.update(follows.size)
                    }
                }
            } catch (e: CancellationException){
                //cancel
                follows.clear()
                return@launch
            } catch (e: TwitterException){
                e.printStackTrace()
            }
        }
        return follows
    }
}
//}

=== 参加者一覧表示フラグメント

=== タスク進捗確認ダイアログ

