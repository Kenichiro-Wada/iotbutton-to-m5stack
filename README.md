# Iot Button To m5stack
## Description
[あのボタンを押したらM5Stackに今何してるを表示させる](https://qiita.com/keni_w/items/e5e49f05ff72f14cda8a)
で記載しているソースコードの公開場所です。


## Requirement
- Lambda
    - Serverless Framework
- arduino
    - ArduinoJson
    - Adafruit MQTT Library

## Usage
### Lambda
- ソースは、`python/` 以下にあります。
- デプロイ用のserverless.ymlもあるので、Serverless Frameworkでのデプロイも可能です。
- 手動でデプロイする場合は、`handler.py`の中身をLambdaのコンソールのソースコードエディタに貼り付けてください。
    - 実行権限にAWS IoT CoreのPublishする権限を忘れずに `iot:publish`
### M5Stack
- M5Stack側のソースは `arduino/` 以下
- Arduino IDEを使って、`sketch_m5stack_publisher_message.ino`の中身をM5Stackに適用してください。
    - Requirementに記載のライブラリを読み込んでください。

その他諸々は Qiitaの記事を参照のこと


## 今後の予定
- M5Stack側変えるかも。
    - `1時間ぐらい変わらなかったら、Deep Sleepするようにした方がいいのかな？`

## Licence

[MIT](https://github.com/tcnksm/tool/blob/master/LICENCE)

## Author

[Kenichiro-Wada](https://github.com/Kenichiro-Wada)
