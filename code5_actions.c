/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code5_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:24 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:40:43 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief ドングルを1本確実に仕留めるヘルパー関数
 * @note 自分がヒープキューの先頭になり、かつドングルが空くまで条件変数で眠ります。
 * 所有権を確定させてMutexを外した後にクールダウンを待つことで、並行性を最大化します。
 */
static void	acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
	t_coder		*coders_head;
	long long	cooldown_left;

	pthread_mutex_lock(&dongle->mutex);
	coders_head = coder - (coder->id - 1);
	coder->request_time = get_time_ms();
	push_queue(dongle, coders_head, coder->id);
	while (!check_stop_flag(coder->config)
		&& (dongle->holder_id != 0 || dongle->queue[0] != coder->id))
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	cooldown_left = 0;
	if (!check_stop_flag(coder->config))
	{
		pop_queue(dongle, coders_head);
		dongle->holder_id = coder->id;
		cooldown_left = dongle->cooldown_end - get_time_ms();
	}
	pthread_mutex_unlock(&dongle->mutex);
	if (!check_stop_flag(coder->config) && cooldown_left > 0)
		precise_usleep(cooldown_left);
}

/**
 * @brief 使用し終えたドングル1本を解放し次へ
 */
static void	release_one_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->holder_id = 0;
	dongle->cooldown_end = get_time_ms() + coder->config->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

/**
 * @brief 左右のドングルをロック
 */
void	acquire_dongles(t_coder *coder)
{
	if (coder->id % 2 != 0)
	{
		acquire_one_dongle(coder, coder->left_dongle);
		print_state(coder, "has taken a dongle");
		acquire_one_dongle(coder, coder->right_dongle);
		print_state(coder, "has taken a dongle");
	}
	else
	{
		acquire_one_dongle(coder, coder->right_dongle);
		print_state(coder, "has taken a dongle");
		acquire_one_dongle(coder, coder->left_dongle);
		print_state(coder, "has taken a dongle");
	}
}

/**
 * @brief 使用し終えた左右のドングルを解放
 */
void	release_dongles(t_coder *coder)
{
	if (coder->id % 2 != 0)
	{
		release_one_dongle(coder, coder->right_dongle);
		release_one_dongle(coder, coder->left_dongle);
	}
	else
	{
		release_one_dongle(coder, coder->left_dongle);
		release_one_dongle(coder, coder->right_dongle);
	}
}
