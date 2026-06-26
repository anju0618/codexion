/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code6_monitor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:05:49 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 13:24:23 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief: 単一のコーダーがBurnoutしていないか、およびコンパイル規定回数に達しているかを確認
 * @detail: 現在時刻とdeadlineを比較します。Burnoutを検知した場合は即座にstop_flagを立て、他のスレッドを停止へ。
 **/
static int	check_single_coder(t_coder *coder, long long now, int *all_done)
{
	pthread_mutex_lock(&coder->config->state_mutex);
	if (now >= coder->deadline)
	{
		coder->config->stop_flag = 1;
		pthread_mutex_unlock(&coder->config->state_mutex);
		print_state(coder, "burned out");
		return (1);
	}
	if (coder->compile_count < coder->config->num_compile_req)
		*all_done = 0;
	pthread_mutex_unlock(&coder->config->state_mutex);
	return (0);
}

/**
 * @brief: 全コーダーのステータスを確認し、シミュレーションの終了判定
 * @detail: 誰か一人がBurnoutしたか、全員が規定のコンパイル回数を達成した場合にstop_flagを有効化して 1 を返す
 **/
static int	check_coders_status(t_coder *coders, t_config *config)
{
	int			i;
	int			all_done;
	long long	now;

	i = 0;
	all_done = 1;
	now = get_time_ms();
	while (i < config->num_coders)
	{
		if (check_single_coder(&coders[i], now, &all_done))
			return (1);
		i++;
	}
	pthread_mutex_lock(&config->state_mutex);
	if (config->num_compile_req > 0 && all_done)
	{
		config->stop_flag = 1;
		pthread_mutex_unlock(&config->state_mutex);
		return (1);
	}
	pthread_mutex_unlock(&config->state_mutex);
	return (0);
}

/**
 * @brief: シミュレーション終了条件を常時監視し、終了時には全スレッドを安全に終わらせる監視スレッド
 * @detail: precise_usleep(1)で1ミリ秒ごとに高頻度で監視.
 * @detail: 終了判定が出た直後、全ドングルに対して pthread_cond_broadcast を送りpthread_joinできるようにします。
 **/
void	*monitor_routine(void *arg)
{
	t_coder		*coders;
	t_config	*config;
	int			i;

	coders = (t_coder *)arg;
	config = coders[0].config;
	while (1)
	{
		if (check_coders_status(coders, config))
			break ;
		precise_usleep(1);
	}
	i = 0;
	while (i < config->num_coders)
	{
		pthread_mutex_lock(&(coders[i].left_dongle->mutex));
		pthread_cond_broadcast(&(coders[i].left_dongle->cond));
		pthread_mutex_unlock(&(coders[i].left_dongle->mutex));
		i++;
	}
	return (NULL);
}
