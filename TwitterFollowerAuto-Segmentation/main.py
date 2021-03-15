# モジュールの読み込み
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