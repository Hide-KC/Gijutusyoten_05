={appendix_a} カラーピッカーを作成してみた
カラーピッカーって結構需要がありそうで、意外とView単体で配布されているものってないんですよね。
だいたいDialogFragmentとして、まるごとパッケージングされて配布されている感じで……。

個人的に縦軸に明度（Brightness）、横軸に彩度（Saturation）を配した矩形のビュー（SBPlane）が欲しかったので作ってみました（@<img>{img_sbplane}）。

//image[img_sbplane][SBPlaneイメージ][scale=0.75]{
//}

色相のバー（Huebar）は脚注のページを参考に作りました@<fn>{huebar}。

SBPlaneのコードを@<list>{code_sbplane}、@<list>{code_sbplane2}に示します。

//footnote[huebar][https://medium.com/@stepango/kotlin-color-picker-part-1-427983cbcd66]

//listnum[code_sbplane][SBPlane.kt]{
...
private val viewSize: FloatArray = floatArrayOf(0f, 0f)
private lateinit var paint: Paint
private lateinit var lg: LinearGradient

//onDrawの前に発生
@SuppressLint("DrawAllocation")
override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
    super.onMeasure(widthMeasureSpec, heightMeasureSpec)
    viewSize[0] = this.measuredWidth.toFloat()
    viewSize[1] = this.measuredHeight.toFloat()

    //ShaderはonDrawで逐次生成
    paint = Paint(Paint.ANTI_ALIAS_FLAG)
    paint.style = Paint.Style.FILL
    //displayMetrics.densityでdp単位に変換
    paint.strokeWidth = Math.ceil((viewSize[0] / 100).toDouble()).toFloat()
                                     * context.resources.displayMetrics.density
}

override fun onDraw(canvas: Canvas?) {
    ...    
    //グラデーション線を縦方向に描画
    val unit = viewSize[0] / 100f
    for (ix in 0..100){
        val startColor = Color.HSVToColor(floatArrayOf(getHue(), ix / 100f, 1f))
        val endColor = Color.HSVToColor(floatArrayOf(getHue(), ix / 100f, 0f))

        //ここの座標系の数値の入力はあまり意味がない？startX,endX=0fでも動作確認。
        lg = LinearGradient(unit * ix, 0f, unit * ix, viewSize[1],
                             startColor, endColor, Shader.TileMode.CLAMP)
        paint.shader = lg
        canvas?.drawLine(unit * ix, 0f, unit * ix, viewSize[1], paint)
    }
}
//}

SBPlaneは、縦方向に明度のグラデーションを付けた直線を100本描写することで表現しています。
直線の幅はonMeasureで算出し、paint.strokeWidthにセットしています。

//listnum[code_sbplane2][SBPlane.kt-続き]{
override fun onTouchEvent(event: MotionEvent?): Boolean {
    val action = event?.action
    ...
    //タッチ座標から明度、彩度を計算
    when {
        ...
        else -> setSaturation(x)
    }

    when {
        ...
        else -> setBrightness(1 - y)
    }
    ...
    mListener?.onHSBChanged(getHue(), getSaturation(), getBrightness())
    return true
}

//他のオブジェクトの変更を受信するためのインターフェース（Observerパターン）
override fun colorUpdate(hue: Float, saturation: Float, brightness: Float) {
    setHSB(hue, saturation, brightness)
    postInvalidateOnAnimation()
}
//}

SBPlaneは、Huebarの変更やEditTextでRGBを直接入力等、外部からHue/Saturation/Brightnessの変更を受信することができる構成にしてみました。
受信にはObserverパターンを採用し、IObsever#colorUpdateを実装しています。

