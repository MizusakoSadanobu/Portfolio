### これは何か
- Google Analytics APIを活用した、ブログなどのWebサイトのKPIを取得するためのpythonモジュールと、モジュールを使った可視化例の紹介です。
	- ./MyGoogleAnalyticsFunctions.py データ取得のためのpythonモジュール
	- ./GoogleAnalyticsKPI.ipynb モジュールを使用した可視化例
- 以下は、ページ閲覧時間をKGIとして、PV数、PVあたりの閲覧時間を可視化した例です。10月以降、閲覧時間の伸びが低減していること（これは作成者の本業が忙しくなったため・・・）、閲覧時間のシェアが特定の記事に依存していること（人気がない記事は・・・）、が分かります。

	- 作成者運営するブログの日別KPI：
![daily](http://drive.google.com/uc?export=view&id=1WJ9FAHDOrXFmUhUJAJFBd1PDExLa33G0)

	- 作成者運営するブログの記事別KPI：
![daily](http://drive.google.com/uc?export=view&id=1btghJTt5eB5nKRtTNi230uuvb66ecqQy)

### どのように使うのか
- Google Analyticsに対象Webサイトを登録していない方、APIの利用申請をしていない方は、登録、申請をお願いします（アカウント主は以下を参考にしました）。以降では、アカウントの設定が完了しており、①対象WebのビューIDが取得できている、②サービスアカウント用のJSON形式の鍵が生成できている、ことを前提にpythonモジュールの使い方を説明します
	- [Googleアナリティクス登録・設定手順【これさえ読めばOK！】](https://wacul-ai.com/blog/access-analysis/google-analytics-setting/ga-register/)
	- [Google Analytics API を叩いてデータを取得するまでの流れ（Ruby）](https://qiita.com/ryota-sasabe/items/a5efd2aac244cfcce5c7)	
	- [アナリティクス Reporting API v4を使ってGoogle Analyticsのデータを取得する](https://dev.classmethod.jp/articles/ga-api-v4/)
- まず、モジュールへのパスを設定した上で（割愛）、MyGoogleAnalyticsFunctionsモジュールからExtractGoogleAnalyticsDataクラスを読み込みます。このクラスには以下のメソッドが実装されています
	- \_\_init\_\_(self, params)
		- params変数の設定を読み込みます（詳細後述）
	- initialize_analyticsreporting(self)
		- Analytics Reporting API V4 service objectの初期化を行います
		- self.analyticsに初期化されたservice objectが格納されます
	- get_report(self)
		- 指定した日付から本日までのGoogle Analyticsデータを取得します
		- self.dfに取得したデータが格納されます
	- plot_data(self)
		- self.dfのデータを、日別とページ別のデータに集計して可視化します

```python
from MyGoogleAnalyticsFunctions import ExtractGoogleAnalyticsData
```

- 次に、クラス初期化に必要なparams変数の設定を行います

```python
params = {
    'scopes': ['https://www.googleapis.com/auth/analytics.readonly'],
    'key_file_location': '/content/drive/MyDrive/XXXX/XXXXX.json',# keyファイルへのパス
    'view_id': 'XXXXXXXXX', #viewのID
    'page_size': 100000, #抽出するデータ数。デフォルトは1000なので最大値の100000に設定
    'm_list': ['users', 'sessions', 'pageviews', 'bounceRate', 'exitRate', 'avgTimeOnPage'], #抽出するmetricsのリスト
    'd_list': ['pagePath', 'pageTitle',  'date'], #抽出するdimensionのリスト。最終要素は時間軸の集計に用いる
    'startDate':'today',#抽出開始日（'yyyy-mm-dd', 'today'などのフォーマットに対応）
}
```

- 最後に、クラスをインスタンス化して、以下のように実行すればデータ取得〜可視化が行えます

```python:
egad_1week = ExtractGoogleAnalyticsData(params)
egad_1week.initialize_analyticsreporting()
egad_1week.get_report()
egad_1week.plot_data()
```

<!--stackedit_data:
eyJoaXN0b3J5IjpbNDc1MTU4NzI4LDgzMzM4OTU3MiwtMjYzMz
YxMzQsLTUyNDQ4NzMxNCwxMjk2OTAzNDc2LC04ODE5NDg0Njks
MTcxNTY4MDQ5MSwtMTQ4OTg1MjY3Niw3MzA5OTgxMTZdfQ==
-->