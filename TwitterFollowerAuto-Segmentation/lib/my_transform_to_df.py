from my_common import *

# フォロー、投稿、いいねをdfオブジェクトに変換する関数
def val_for_users(users, attr):
    '''ユーザーオブジェクトリストからattrの要素をリストとして抽出する'''
    return np.vectorize(lambda user: getattr(user, attr))(users)

def val_for_acts(acts, attr):
    '''ステータス（act）オブジェクトリストからattrの要素をリストとして抽出する'''
    return np.vectorize(lambda act: getattr(act, attr))(acts)

def fix_time_lag(time_list):
    '''9時間の時差を考慮'''
    global timestamp2str
    return pd.to_datetime(time_list).tz_convert(None) + pd.Timedelta(hours=9)

def urls_for_act(post):
    '''ステータスオブジェクトに含まれる展開済みurlを抽出する'''
    num_urls = len(post.urls)
    url_sets = ''
    if(num_urls>0):
        url_sets = ''
        for url in post.urls:
            url_sets += url.expanded_url
            url_sets += ','#urlのセパレーター
    return url_sets

def make_df_users(users):
    '''ユーザーオブジェクトからscreen name, idを抽出する'''
    global timestamp2str
    df_users = DataFrame(
        {'screen_name': val_for_users(users, 'screen_name'),
         'user_id': val_for_users(users, 'id_str'),
         'protected':  val_for_users(users, 'protected'),
         'count_follower': val_for_users(users, 'followers_count'),
         'count_friends': val_for_users(users, 'friends_count'),
         'description': val_for_users(users, 'description'),
         'created_at': fix_time_lag(val_for_users(users, 'created_at')),
         'date': str_today
        }
    )
    return df_users

def make_df_posts(posts, screen_name_who_act):
    '''投稿オブジェクトから投稿日時、公式/RT/QT/Rep/FAVの種別、メンション先、アクション日を抽出する'''
    global timestamp2str
    
    def name_for_post(post):
        if post.in_reply_to_screen_name != None: return post.in_reply_to_screen_name
        elif post.quoted_status != None: return post.quoted_status.user.screen_name
        elif post.retweeted_status != None: return post.retweeted_status.user.screen_name
        else: return None

    def id_for_post(post):
        if post.in_reply_to_screen_name != None: return str(post.in_reply_to_user_id)
        elif post.quoted_status != None: return str(post.quoted_status.user.id_str)
        elif post.retweeted_status != None: return str(post.retweeted_status.user.id_str)
        else: return None

    def type_for_post(post):
        if post.in_reply_to_screen_name != None: return 'REP'
        elif post.quoted_status != None: return 'QT'
        elif post.retweeted_status != None: return 'RT'
        else: return 'OFFICIAL'
    
    df_posts = DataFrame(
        {'screen_name': np.vectorize(name_for_post)(posts),
         'user_id': np.vectorize(id_for_post)(posts),
         'action_type': np.vectorize(type_for_post)(posts),
         'datetime_act': fix_time_lag(val_for_acts(posts, 'created_at')),
         'text': val_for_acts(posts, 'text'),
         'urls': np.vectorize(urls_for_act)(posts),
         'date': str_today,
         'screen_name_who_act': screen_name_who_act
        }
    ).assign(
         date_act = lambda df: df.datetime_act.apply(timestamp2str)
    )
    return df_posts

def make_df_favs(favs, screen_name_who_act):
    '''いいねした投稿オブジェクトから投稿日時、公式/RT/QT/Rep/FAVの種別、メンション先、アクション日を抽出する'''
    def name_for_fav(fav): return fav.user.screen_name
    def id_for_fav(fav): return str(fav.user.id_str)

    df_favs = DataFrame(
        {'screen_name': np.vectorize(name_for_fav)(favs),
         'user_id': np.vectorize(id_for_fav)(favs),
         'action_type': 'FAV',
         'datetime_act': fix_time_lag(val_for_acts(favs, 'created_at')),
         'text': val_for_acts(favs, 'text'),
         'urls': np.vectorize(urls_for_act)(favs),
         'date': str_today,
         'screen_name_who_act': screen_name_who_act
        }
    ).assign(
         date_act = lambda df: df.datetime_act.apply(timestamp2str)
    )
    return df_favs

def make_df_potential_followers(df_friends, df_followers, df_posts, df_favs, my_screen_name):
    '''アクション済みユーザーのリストアップ'''
    #フォロー・フォロワー関係にあるユーザー
    df_friends = df_friends.assign(following = True)
    df_followers = df_friends.assign(followed = True)
    df_follow_or_followed = df_friends.append(df_followers).groupby('screen_name').\
        agg(
            user_id = ('user_id', max),
            protected = ('protected', max),
            count_follower = ('count_follower', max),
            count_friends = ('count_friends', max),
            description = ('description', max),
            created_at = ('created_at', max),
            date = ('date', max),
            following = ('following', max),
            followed = ('followed', max)
    ).reset_index()

    #いいね・返信・RT・QTだけの関係
    df_acts = df_favs.append(df_posts)
    df_acts_screan_names = df_acts[['screen_name', 'user_id']].dropna().drop_duplicates().\
        assign(FF = lambda df: df.screen_name.apply(lambda s: s in list(df_follow_or_followed.screen_name))).\
        pipe(lambda df: df[df.screen_name != my_screen_name])
    df_act_only = df_acts_screan_names[df_acts_screan_names.FF==False][['screen_name', 'user_id']]

    #マージ
    df_potential_followers = df_follow_or_followed.append(df_act_only)
    
    return df_potential_followers
