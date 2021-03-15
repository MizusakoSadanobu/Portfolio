from my_common import *

def get_tup_X(X, df_potential_followers_status_master, str_today):
    sr_X = df_potential_followers_status_master.pipe(
        lambda df: df[df.date == str_today]
    ).pipe(
        lambda df: df[df.list_name_today == 'LIST_' + X]
    ).screen_name

    tup_X = tuple(sr_X)
    
    return tup_X

def my_make_list(df_potential_followers_status_master, my_screen_name, str_today):
    tup_A = get_tup_X('A', df_potential_followers_status_master, str_today)
    tup_B = get_tup_X('B', df_potential_followers_status_master, str_today)
    tup_C = get_tup_X('C', df_potential_followers_status_master, str_today)
    my_lists = api.GetLists(screen_name=my_screen_name)
    for name, member_name_tup_new in zip(['全確認', '全返信・QT', 'いいね・RT'], [tup_A, tup_B, tup_C]):
        i = 0
        for my_list in my_lists:
            if((my_list.name == name)&(i == 0)):
                list_id = my_list.id
                while(1):
                    try:
                        members = api.GetListMembers(list_id)
                        member_name_list_old = list([member.screen_name for member in members])
                        member_name_tup_add = tuple(set(member_name_tup_new) - set(member_name_list_old))
                        member_name_tup_destroy = tuple(set(member_name_list_old) - set(member_name_tup_new))
                        if(len(member_name_tup_add)>0): api.CreateListsMember(list_id=my_list.id, screen_name=member_name_tup_add)
                        if(len(member_name_tup_destroy)>0): api.DestroyListsMember(list_id=my_list.id, screen_name=member_name_tup_destroy)
                        i+=1
                        print('find & add')
                        print(name)
                        print(member_name_tup_add)
                        print(member_name_tup_destroy)
                        break
                    except:
                        print('sleep in make_list. time = %s' % (pd.to_datetime(datetime.today())))
                        time.sleep(3*60)
        if(i == 0):
            while(1):
                try:
                    print('create')
                    print(name)
                    my_list = api.CreateList(name=name, mode='private')
                    if(len(member_name_tup_new)): api.CreateListsMember(list_id=my_list.id, screen_name=member_name_tup_new)
                    print(member_name_tup_new)
                    break
                except:
                    print('sleep in make_list. time = %s' % (pd.to_datetime(datetime.today())))
                    time.sleep(3*60)