={whats_api} Twitter APIとTwitter4J
この章では、Twitter利用に必要な「Twitter API」と、それらを使いやすくライブラリ化した
「Twitter4J」を紹介します。

== APIとは
Twitterでは、ツイートの送信やフォローの取得等のサービスを利用するための機能を提供しており、
これらの機能を利用するための命令セットを「API（Application Programming Interface）」と呼びます。
Twitterクライアントアプリ開発や、自作アプリ内でツイートの利用をする場合、このAPIを通して
Twitterのサービスを利用することができます。

Twitter APIは大きく次のように分けられます@<fn>{developerUrl}。
//footnote[developerUrl][https://developer.twitter.com/en.html]

 * REST API（Tweetの送信、DMの受信等）
 * 検索API（Tweetの検索）
 * ストリーミングAPI（ステータスの逐次配信）
 * Webサイト向け用API（Webサイト内へのTweetの埋め込み）

これらのAPIを個人で直接利用するのは少々ハードルが高いと思いますので、今回は
Yusuke Yamamoto氏の開発した「Twitter4J」というライブラリを
利用します@<fn>{twitter4jUrl}。
//footnote[twitter4jUrl][http://twitter4j.org/ja/index.html]
#@# //footnote[twitter4jGitHub][GitHub: https://github.com/yusuke/twitter4j]

== Twitter4Jとは
HPでは次のように説明されています。
//quote{
Twitter4J は Twitter API の Java ラッパです。
Twitter4J を使うと JSON や HTTP に詳しくなくても容易に Twitter とインタラクトするアプリケーションを
書くことが出来ます。Twitter4J は Twitter 非公式のライブラリです。 
//}

Twitter4Jを利用すると、面倒なHTTP通信や認証に関する処理をライブラリ側で担ってくれるため、
簡単にTweetの送信やフォローの取得等の操作をすることができます。