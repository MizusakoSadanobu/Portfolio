from my_common import *
from my_transform_to_df import *

# 投稿やいいねを連続取得する関数
def get_posts_and_favs(screen_name, n_roop, flag):
    '''投稿またはいいねを取得する'''
    global timestamp2str
    global api
    global date_today
    global str_today
    
    if(flag == 'post'): get_func = api.GetUserTimeline
    elif(flag == 'fav'): get_func = api.GetFavorites
    last_id = None
    posts = []
    for i in range(n_roop):
        while(1):
            try:
                posts_temp = get_func(screen_name=screen_name, count=200, max_id=last_id)
                if(i>0): posts = posts + posts_temp[1:]
                if(i==0): posts = posts + posts_temp
                break
            except:
                print('sleep in post. time = %s' % (pd.to_datetime(datetime.today())))
                time.sleep(3*60)
        if(len(posts_temp) <2):
            break
        last_id = posts_temp[-1].id
    return posts

def get_my_data(my_screen_name):
    '''自分の投稿データの取得'''
    global timestamp2str
    global api
    global date_today
    global str_today
    
    while(1):
        try:
            friends = api.GetFriends() #フォロー先のユーザーリストを取得
            followers = api.GetFollowers() #フォロワーのユーザーリストを取得
            posts = get_posts_and_favs(my_screen_name, 5, 'post') #投稿の取得
            favs = get_posts_and_favs(my_screen_name, 5, 'fav') #いいねの取得
            break
        except:
            print('sleep in friends&followers. time = %s' % (pd.to_datetime(datetime.today())))
            time.sleep(3*60)
    return (friends, followers, posts, favs)

def get_user_data(user):
    '''1ユーザーの投稿/Favデータの取得'''
    if(user.protected):
        posts = []
        favs = []
    else:
        posts = get_posts_and_favs(user.screen_name, 1, 'post') #投稿の取得
        favs = get_posts_and_favs(user.screen_name, 1, 'fav') #いいねの取得
    return (posts, favs)

def get_users_data(df_potential_followers):
    '''複数のユーザーの投稿/Favデータを連続して取得する'''
    df_user_posts = DataFrame(
        columns=['screen_name', 'user_id', 'action_type', 'datetime_act', 'text', 'urls',
           'date', 'screen_name_who_act', 'date_act'])
    df_user_favs = DataFrame(
        columns=['screen_name', 'user_id', 'action_type', 'datetime_act', 'text', 'urls',
           'date', 'screen_name_who_act', 'date_act'])
    i = 0
    start = pd.to_datetime(datetime.today())
    # df_potential_followersの各レコードに対して以下実行する
    for user in list(df_potential_followers.iloc[i,:] for i in range(len(df_potential_followers))):
        print('%d th roop' % i)
        (user_posts, user_favs) = get_user_data(user)
        if(len(user_posts) != 0): #投稿数が0ではない場合のみ実行
            df_user_posts_ = make_df_posts(user_posts, user.screen_name)
            df_user_posts = df_user_posts.append(df_user_posts_)
        if(len(user_favs) != 0): #Fav数が0ではない場合のみ実行
            df_user_favs_ = make_df_favs(user_favs, user.screen_name)
            df_user_favs = df_user_favs.append(df_user_favs_)
        i=i+1
    print(pd.to_datetime(datetime.today()) - start)
    return (df_user_posts, df_user_favs)