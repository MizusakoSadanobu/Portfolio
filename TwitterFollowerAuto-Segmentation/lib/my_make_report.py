from my_common import *

def make_report(df_potential_followers_status_master, str_today, file_path):
    # セグメント別の集計
    ## 人数
    df_count_user_by_seg = df_potential_followers_status_master.pipe(
        lambda df: df[ pd.to_datetime(df['date']) <= pd.to_datetime(str_today) ]
    ).pivot_table(
        index='segment_yesterday',
        columns='date',
        values='screen_name',
        aggfunc='count'
    ).fillna(0)


    ## アクション対象人数（ユニーク ）
    df_count_actions_by_seg = df_potential_followers_status_master.pipe(
        lambda df: df[ pd.to_datetime(df['date']) <= pd.to_datetime(str_today) ]
    ).pivot_table(
        index='segment_yesterday',
        columns='date',
        values='count_I_act_today',
        aggfunc=lambda s: (s>0).sum()
    ).fillna(0)

    ## アクションカバー率
    df_count_action_coverage_by_seg = df_count_actions_by_seg/df_count_user_by_seg


    # 図表作成
    ig, axes = plt.subplots(nrows=1, ncols=3, figsize=(15, 7))
    df_fig = df_count_user_by_seg
    df_fig.T.plot(kind='area', stacked=True, ax=axes[0], legend=False, title='①セグメント別ユーザー数')

    df_fig = df_count_actions_by_seg
    df_fig.T.plot(kind='area', stacked=True, ax=axes[1], title='②セグメント別アクション対象ユーザー数（ユニーク）')
    axes[1].legend(bbox_to_anchor=(1, 1), loc='upper right', borderaxespad=0, fontsize=8, ncol=2)

    df_fig = df_count_action_coverage_by_seg.fillna(0)
    df_fig.T.plot(ax=axes[2], title='③セグメント別アクションカバー率（＝②÷①）')
    axes[2].legend(bbox_to_anchor=(1, 1), loc='upper right', borderaxespad=0, fontsize=8, ncol=2)

    plt.savefig(file_path + '/base_summary_'+ str_today +'.png', bbox_inches="tight", pad_inches=0.1)

    # 流出入の図表作成
    def comp(a, b):
        if(a<b): return 'upsell'
        elif(a==b): return 'stay'
        else: return 'downsell'

    df_potential_followers_status_master = df_potential_followers_status_master.assign(
        segment_change = lambda df: df.segment_yesterday + '->' + df.segment_today,
        up_stay_down = lambda df: np.vectorize(comp)(df.segment_today.astype(str), df.segment_yesterday.astype(str))
    )

    def temp(seg):
        return df_potential_followers_status_master.pipe(
            lambda df: df[(df.segment_yesterday==seg)]
        ).pivot_table(
            index='up_stay_down', columns='date',
            values='screen_name', aggfunc='count'
        ).T.pipe(
            lambda df: df[pd.to_datetime(df['date']) <= pd.to_datetime(str_today)]
        ).pipe(
            lambda df: DataFrame(columns=['upsell', 'stay', 'downsell']).append(df)
        ).fillna(0).plot()