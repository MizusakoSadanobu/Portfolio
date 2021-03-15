from my_common import *

# マスターを作成
## user_master
def make_df_potential_followers_master(str_today, file_path):
    '''make_df_potential_followers_masterを作成して保存する'''
    ### 本日のファイルを読み込む
    df_potential_followers_today = pd.read_csv(file_path+'/df_potential_followers'+str_today+'.csv')

    ### マスターを読み込んで、本日のファイルと結合する
    try: 
        df_potential_followers_master = pd.read_csv(file_path + '/df_potential_followers_master.csv')
        df_potential_followers_master = df_potential_followers_master.append(
            df_potential_followers_today.assign(date = str_today)
        )
    except:
        df_potential_followers_master = DataFrame(
        columns=['screen_name', 'user_id', 'protected', 'count_follower',
           'count_friends', 'description', 'created_at', 'date', 'following',
           'followed'])
        for i in range(8):
            date = timestamp2str(pd.to_datetime(str_today) - pd.Timedelta(days = 7 - i))
            df_potential_followers_master = df_potential_followers_master.append(
                df_potential_followers_today.assign(date = date)
            )
    
    df_potential_followers_master.to_csv(file_path + '/df_potential_followers_master.csv', index=False)
    
    return df_potential_followers_master

## log_master
def make_df_user_log_master(str_today, file_path):
    def get_update(df_master, df_update):
        '''df_updateのみに含まれるアップデート分だけをdf_masterに結合する'''
        df_master = df_master.append(df_update).drop_duplicates(subset=['screen_name_who_act', 'datetime_act'])
        return df_master
    
    def get_master(file_name, file_path):
        '''user & my / favs & postsのマスターを作成して保存する関数'''        
        df_today = pd.read_csv(file_path+file_name+str_today+'.csv')
        try: 
            df_master = pd.read_csv(file_path +file_name + '_master.csv')
            df_master = get_update(df_master, df_today)
        except:
            df_master = df_today
        df_master.to_csv(file_path +file_name + '_master.csv', index=False)
        return df_master
    
    df_user_posts_master = get_master('/df_user_posts', file_path)
    df_user_favs_master = get_master('/df_user_favs', file_path)
    df_posts_master = get_master('/df_posts', file_path)
    df_favs_master = get_master('/df_favs', file_path)
    
    return (df_user_posts_master, df_user_favs_master, df_posts_master, df_favs_master)

def is_includes_my_urls(urls, my_urls):
    includes = 0
    for my_url in my_urls:
        includes += (str(my_url) in str(urls))*1
    return (includes>0)

# セグメントへの分類
def make_df_potential_followers_status_master(
    df_potential_followers_master, df_posts_master, df_favs_master,
    df_user_posts_master, df_user_favs_master, my_screen_name, my_urls, str_today):
    '''セグメントに分類する関数'''
    
    def segment(loyal_blog_visitor, blog_visitor, followed, user_act_me, user_I_act, user_act):
        if(followed&loyal_blog_visitor&user_act_me&user_I_act): return '01_両思いロイヤル読者'
        elif(followed&blog_visitor&user_act_me&user_I_act): return '02_両思い読者'
        elif(followed&user_act_me&user_I_act): return '03_両思いフォロワー'
        elif(followed&user_act_me): return '04_片思い（思われ）フォロワー'
        elif(followed&user_I_act): return '05_片思い（思い）フォロワー'
        elif(followed&user_act): return '06_遠距離アクティブフォロワー'
        elif(followed): return '07_遠距離非アクティブフォロワー'
        else: return '08_見込みフォロワー'
        
    def list_name(segment, days_from_I_act_last, days_from_act_me_last):
        '''
        A:ちゃんと何しているのか把握すべき人
        B:積極的にいいねや返信すべき人
        C:いいねをおすべき人
        '''
        if(segment == '01_両思いロイヤル読者'): return 'LIST_A'
        if(segment == '02_両思い読者'): return 'LIST_A'
        if((segment == '03_両思いフォロワー') & (days_from_I_act_last > 3)): return 'LIST_C'
        if(segment == '04_片思い（思われ）フォロワー'): return 'LIST_B'
        if((segment == '06_遠距離アクティブフォロワー') & 
           ((days_from_act_me_last < 14) | (days_from_I_act_last > 14)) ): return 'LIST_C'

    def get_today_log(df, str_today): 
        return df[pd.to_datetime(df.datetime_act, errors='coerce') <= (pd.to_datetime(str_today) + pd.Timedelta(days=1))]

    # マスターファイルを加工
    df_potential_followers_status_master = DataFrame(
        columns=['screen_name', 'user_id', 'protected', 'count_follower',
           'count_friends', 'description', 'created_at', 'date', 'following',
           'followed', 'datetime_I_act_first', 'datetime_I_act_last',
           'count_I_act', 'days_from_I_act_first', 'days_from_I_act_last',
           'user_I_act', 'datetime_act_me_first', 'datetime_act_me_last',
           'days_from_act_me_first', 'days_from_act_me_last', 'user_act_me',
           'datetime_act_my_article_first', 'datetime_act_my_article_last',
           'count_cum_act_my_article_last', 'days_from_act_my_article_first',
           'days_from_act_my_article_last', 'blog_visitor', 'loyal_blog_visitor',
           'segment']
    )

    # 顧客マスターの一番古い日付から一番新しい日付まで下記の操作を実行
    
    for date in df_potential_followers_master.date.drop_duplicates():
        str_today = date
        date_today = pd.to_datetime(str_today)

        #今日に該当するユーザーマスター
        df_potential_followers_today = df_potential_followers_master.pipe(lambda df: df[df.date == str_today])

        #今日に該当するアクションマスター
        df_posts = get_today_log(df_posts_master, str_today)
        df_favs = get_today_log(df_favs_master, str_today)
        df_user_posts = get_today_log(df_user_posts_master, str_today)
        df_user_favs = get_today_log(df_user_favs_master, str_today)

        # 自分
        ## 最初にアクションした日・最後にアクションした日
        df_my_action = df_favs.append(df_posts).assign(
            datetime_act = lambda df: pd.to_datetime(df.datetime_act, errors='coerce'),
            I_act = lambda df: (df.datetime_act.apply(timestamp2str) == str_today)*1
        ).dropna(subset=['datetime_act']).groupby('screen_name').agg(
            datetime_I_act_first = ('datetime_act', 'min'),
            datetime_I_act_last = ('datetime_act', 'max'),
            count_I_act = ('I_act', 'sum')
        ).reset_index().assign(
            days_from_I_act_first = lambda df: pd.to_datetime(date_today) - df.datetime_I_act_first,
            days_from_I_act_last = lambda df: pd.to_datetime(date_today) - df.datetime_I_act_last,
            user_I_act = lambda df: (df.days_from_I_act_last < pd.Timedelta(days=7))
        )

        #ユーザー
        ## ユーザーが最初にアクションした日・最後にアクションした日
        df_user_action_log = df_user_favs.append(df_user_posts).assign(
            datetime_act = lambda df: pd.to_datetime(df.datetime_act, errors='coerce'),
            act = lambda df: (df.datetime_act.apply(timestamp2str) == str_today)*1 ##ユーザーがその日にアクションしたかを確かめるフラグ
        ).dropna(subset=['datetime_act'])#次の特徴量加工でも使うので一旦ここまででパイプ処理を止める

        df_user_action = df_user_action_log.groupby('screen_name_who_act').agg(
            datetime_act_first = ('datetime_act', 'min'),
            datetime_act_last = ('datetime_act', 'max'),
            count_act = ('act', 'sum')##ユーザーがその日にアクションした回数
        ).reset_index().assign(
            days_from_act_first = lambda df: pd.to_datetime(date_today) - df.datetime_act_first,
            days_from_act_last = lambda df: pd.to_datetime(date_today) - df.datetime_act_last,
            user_act = lambda df: (df.days_from_act_last <= pd.Timedelta(days=7))
        )

        ## 最初にアクションされた日・最後にアクションされた日
        df_user_action_to_me_log = df_user_action_log.pipe(
            lambda df: df[df.screen_name == my_screen_name]
        )#次の特徴量加工でも使うので一旦ここまででパイプ処理を止める

        df_user_action_to_me = df_user_action_to_me_log.groupby('screen_name_who_act').agg(
            datetime_act_me_first = ('datetime_act', 'min'),
            datetime_act_me_last = ('datetime_act', 'max'),
            count_act_me = ('act', 'sum'), ##ユーザーがその日自分にアクションした回数
        ).reset_index().assign(
            days_from_act_me_first = lambda df: pd.to_datetime(date_today) - df.datetime_act_me_first,
            days_from_act_me_last = lambda df: pd.to_datetime(date_today) - df.datetime_act_me_last,
            user_act_me = lambda df: (df.days_from_act_me_last <= pd.Timedelta(days=7))
        )

        ## 記事に最初にアクションされた日・最後にアクションされた日
        df_user_action_to_blog = df_user_action_to_me_log.pipe(
            lambda df: df[df.urls.apply(lambda s: is_includes_my_urls(s, my_urls))]
        ).groupby('screen_name_who_act').agg(
            datetime_act_my_article_first = ('datetime_act', 'min'),
            datetime_act_my_article_last = ('datetime_act', 'max'),
            count_act_my_article = ('act', 'sum'),##ユーザーがその日自分の記事にアクションした回数
            count_cum_act_my_article_last = ('datetime_act', 'count'),
        ).reset_index().assign(
            days_from_act_my_article_first = lambda df: pd.to_datetime(date_today) - df.datetime_act_my_article_first,
            days_from_act_my_article_last = lambda df: pd.to_datetime(date_today) - df.datetime_act_my_article_last,
            blog_visitor = True,
            loyal_blog_visitor = lambda df: (df.count_cum_act_my_article_last>1)
        )

        # 特徴量の紐付け
        df_potential_followers_status = df_potential_followers_today.assign(
            followed = lambda df: df.followed.fillna(False)
        ).pipe(
            lambda df: pd.merge(left=df, right=df_my_action, on='screen_name', how='left')
        ).assign(user_I_act = lambda df: df.user_I_act.fillna(False)).pipe(
            lambda df: pd.merge(left=df, right=df_user_action, left_on='screen_name', right_on='screen_name_who_act', how='left')
        ).drop('screen_name_who_act', axis=1).assign(user_act = lambda df: df.user_act.fillna(False)).pipe(
            lambda df: pd.merge(left=df, right=df_user_action_to_me, left_on='screen_name', right_on='screen_name_who_act', how='left')
        ).drop('screen_name_who_act', axis=1).assign(user_act_me = lambda df: df.user_act_me.fillna(False)).pipe(
            lambda df: pd.merge(left=df, right=df_user_action_to_blog, left_on='screen_name', right_on='screen_name_who_act', how='left')
        ).drop('screen_name_who_act', axis=1).assign(
            blog_visitor = lambda df: df.blog_visitor.fillna(False), 
            loyal_blog_visitor = lambda df: df.loyal_blog_visitor.fillna(False)
        )

        # セグメント情報の追加
        df_potential_followers_status = df_potential_followers_status.assign(
            segment = lambda df: np.vectorize(segment)(
                df.loyal_blog_visitor, 
                df.blog_visitor, 
                df.followed, 
                df.user_act_me, 
                df.user_I_act,
                df.user_act
            ),
            list_name = lambda df: np.vectorize(list_name)(
                df.segment, 
                df.days_from_I_act_last.dt.days, 
                df.days_from_act_me_last.dt.days
            ),
            date = str_today
        )
        
        

        df_potential_followers_status_master = df_potential_followers_status_master.append(df_potential_followers_status)
    
    
    # 今日の実績と昨日の実績を紐付ける
    df_potential_followers_status_master_yesterday = df_potential_followers_status_master.assign(
        date = lambda df: (pd.to_datetime(df.date, errors='coerce') + pd.Timedelta(days=1)).apply(timestamp2str)
    )

    df_potential_followers_status_master = pd.merge(
        left=df_potential_followers_status_master,
        right=df_potential_followers_status_master_yesterday,
        how='outer',
        on=['screen_name', 'date'],
        suffixes=['_today', '_yesterday']
    ).assign(
        date = lambda df: pd.to_datetime(df.date, errors='coerce')
    ).fillna({'segment_yesterday': '09_その他', 'segment_today': '09_その他'})
    
    return df_potential_followers_status_master