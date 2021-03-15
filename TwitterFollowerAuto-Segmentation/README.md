### これは何か
- Twitterのフォロワーさんのロイヤリティ向上を目的として、フォロワーさんから自分へのアクション${}^{(1)}$/自分からのアクション${}^{(1)}$の状況によってセグメント分けし、推奨アクション別にリストに振り分けるようなpythonプログラムです
	- (1) ここで、アクションとは、返信、いいね、RT（含む引用RT）を指しています。中でも、自分が運営するブログのリンクを含むツイートに対するアクションは特別なものと考えています
- セグメント分けの方法をもう少し具体的に説明すると、以下の図に示す通り、フォロワーさんを**「ロイヤリティ」**と**「思い思われのステータス」**の2つの指標でセグメント分けしました
	- フォロワーさんの**「ロイヤリティ」**を、「アカウントのブログ関連のTweetにアクションしてくれるかどうか、で計測しています
	- また、フォロワーさんの**「思い思われのステータス」**を、**「アカウント主からフォロワーさんへのアクションの有無」**と「フォロワーさんからアカウント主へのアクションの有無」、で計測しています

![](http://drive.google.com/uc?export=view&id=18NOlzZjUB4WEMuk6rME38fCXrMu9Ijdf)

- フォロワーさんをセグメント分けした結果、次のように3つのリストに振り分けます
	- A. 優先的に投稿内容をチェックしてレスポンスすべきフォロワーさんのリスト（下図：全確認）
	- B. 返信・QTなどの深いレスポンスを心がけるべきフォロワーさんのリスト（下図：全返信）
	- C. 親和性が高そうなツイートにはライトにいいねを押すべきフォロワーさんのリスト（下図：いいね）

![](http://drive.google.com/uc?export=view&id=1LQB6HtzjyvGGE0EVxKdTmKO2ftjWCoN2)

### なぜ作ったのか
- アカウント主は運営するブログへの集客を目的としてTwitterを活用しています（最近はほとんどノータッチになってしまいましたが。。）
- アカウント主は、特段発信力に長けているわけではないため、一方的に情報を発信するよりも、フォロワーさんと双方向的な関係を築くことが大事だと考えており、Twitterのリスト機能（フォロワーを管理する機能）を使い、フォロワーさんを管理していました
- そんな甲斐あってか、ありがたいことにフォロワーさんが1000名を超えるほどになったのですが、これまで管理していたリストが古くなり、全く使い物にならなくなりました。リストを使ったことがある方は分かると思いますが、Twitterのリストの更新はとても面倒なのです
- これによって、せっかくフォローしてくれたのに、相手のことをよく知らないままフォロー解除されたり、せっかくブログ記事を読んでくれたのにその人のツイートを見落としてしまったりと、これまで普通にできていたことができなくなったのです
- そこで、**動的にリストを更新するようなプログラムを開発したい**と思い至ったのでした
- 実際、このプログラム開発の過程でわかったことですが、**アカウント主のフォロワーの大半は、こちらからも相手からも一切アクション（いいね/返信/RT）がない遠距離フォロワー**であるようです（下図左の茶色のセグメント）。また、アカウント主からのアクションの大半は、相手からはアクションの無い片思いのフォロワーのようです（下図中央の紫のセグメント）が、片思いフォロワーの2~3割しかカバーできておらず（下図右の紫のセグメント）、**ラブコールを送ったところで返ってこない相手を思い続けている**ようにも思えます
- 私1人が1日にアクションできる数は限られているわけですので、**適切にアクション対象を選定することで、遠距離フォロワーを減らし、ロイヤルフォロワーを増やす余地がある**ように思いました

![](http://drive.google.com/uc?export=view&id=1LnandC5qeU6cmKWPOJ2pxQuSI88aIawj)

### どのように作ったのか
- Twitter APIには、データを取得するだけではなく、リストを作成したり、メンバーの更新をするような機能があります

![](http://drive.google.com/uc?export=view&id=1pEmP6-gQpQNG5CLUKPN3ncHOK8BUzCRU)

- そこで、アカウント主が何らかアクションをしたユーザーの一覧と、ユーザーからのアクションの情報とを取得し紐付けます。これらのデータに基づいて、ユーザーセグメンテーションを行い、リストに振り分けます

![](http://drive.google.com/uc?export=view&id=1aZsat1P50CfadhtYCpBau4OB0YmTZmzz)

- pythonコードとしては、次のような記述となりました。少し補足します
	- Twitter APIで取得できる情報には制限があり、自分含む対象ユーザーについて過去全てのツイートデータを取得するのには非常に時間がかかってしまいます。そのため、毎日投稿データを少しずつ取得して、マスタデータに格納していくことが必要となります
	- 自分の投稿のうち、自分のブログのリンクが貼られているものに対して、いいねやRT、返信をくれたユーザーを識別したいため、ブログのリンクをmy_urlsという変数に持たせています
```python
# モジュールの読み込み
## 共通で使うモジュール、変数の読み込み
from my_common import *
## 自分の投稿データ、フォロワーの投稿データの取得を取得するためのモジュール
from my_get_data import get_my_data, get_users_data
## 投稿データをデータフレーム型に変換するためのモジュール
from my_transform_to_df import (make_df_users, 
								make_df_posts, 
								make_df_favs, 						
								make_df_potential_followers)
## 取得した各種データからマスタデータ（日毎のウォッチ対象ユーザー、日毎×ユーザー毎のステータス、自分やユーザーの投稿情報）を作成するモジュール
from my_make_master import (make_df_potential_followers_master,
							make_df_user_log_master,
							make_df_potential_followers_status_master)
## マスターデータからフォロワーをセグメント分けしてリストを更新するためのモジュール
from my_make_list import my_make_list
## KPIを集計してグラフを出力するためのモジュール
from my_make_report import make_report

# 変数設定
my_screen_name = 'XXXXXX' # 自分のTwitterアカウントのスクリーンネーム
my_urls = ['XXXXXX.com', 'XXXXXX.com'] # 自分が運営するブログのURL
file_path = 'XXXXXXXXXX/' # マスターやログデータを格納するディレクトリ

# 自分の投稿データ（フォロー、フォロワー、投稿、いいね）の取得
(my_friends, my_followers,my_posts, my_favs) = get_my_data(my_screen_name)

# dfオブジェクトへの変換
df_friends = make_df_users(my_friends)
df_followers = make_df_users(my_followers)
df_posts = make_df_posts(my_posts, my_screen_name)
df_favs = make_df_favs(my_favs, my_screen_name)

# アクション済みユーザーの一覧化（フォローしている/されている、自分が返信/RT/いいねした人）
df_potential_followers = make_df_potential_followers(df_friends,
													df_followers,
													df_posts, 
													df_favs,
													my_screen_name)

# ユーザーデータ（投稿、いいね）の取得
(df_user_posts, df_user_favs) = get_users_data(df_potential_followers)

# 自分とユーザーのデータの保存
df_user_posts.to_csv(file_path+'/df_user_posts'+str_today+'.csv', index=False)
df_user_favs.to_csv(file_path+'/df_user_favs'+str_today+'.csv', index=False)
df_posts.to_csv(file_path+'/df_posts'+str_today+'.csv', index=False)
df_favs.to_csv(file_path+'/df_favs'+str_today+'.csv', index=False)
df_potential_followers.to_csv(file_path+'/df_potential_followers'+str_today+'.csv', index=False)

# マスターデータ作成及び保存
## 1. 日別のウォッチ対象ユーザーの一覧
df_potential_followers_master = make_df_potential_followers_master(str_today, file_path) 
## 2. 自分とユーザーのいいねや投稿単位の情報を記録したマスタデータ
(df_user_posts_master, df_user_favs_master, df_posts_master, df_favs_master) = make_df_user_log_master(str_today, file_path)
## 3. 日別×ユーザー毎のステータスを記録したマスタデータ
df_potential_followers_status_master = make_df_potential_followers_status_master(
    df_potential_followers_master, df_posts_master, df_favs_master,
    df_user_posts_master, df_user_favs_master, my_screen_name, my_urls, str_today)

# リスト作成
my_make_list(df_potential_followers_status_master, my_screen_name, str_today)

# レポート作成・保存
make_report(df_potential_followers_status_master, str_today, file_path)
```
- 上記で示したプログラムを毎日実行し、ログを取得し続けていった結果、冒頭でも示したセグメント別のKPI推移が可視化されるようになり、課題を把握しやすくなったのでした

![](http://drive.google.com/uc?export=view&id=1LnandC5qeU6cmKWPOJ2pxQuSI88aIawj)

### 最後に
- **大事なことは、「これがTwitter運用についおいて効果（=フォロワーさんのロイヤリティ向上）を生み出しているのか」ということですが、「残念ながらわかりません・・・」**
- 言い訳をすると、プログラム作成当初の構想では、以下のように2ヶ月スパンで効果検証する予定だったのですが、本業との兼ね合いで2ヶ月間も継続できませんでした。不甲斐ないです
	- 1ヶ月間、リストを使わず通常運用した際のログを取得し続けて、
	- その後1ヶ月で、リスト運用に切り替えて効果を計測する
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTIwNjA0ODQ4MjIsMjA1NDA4MTMzNiwxOD
gzODI5NTg0LDY0ODYyMTc3NiwtOTQzODQwNTY1LC0xOTM1OTA1
NjE0LDY4OTY1MzA1MSw3MzA5OTgxMTZdfQ==
-->