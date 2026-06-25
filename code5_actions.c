/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code5_actions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:54:24 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 02:15:37 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief: ドングルが取得可能になるまでスレッドを待機させるループ
 * @detail: ドングルの「クールダウンが明けているか」、そして「待機キューの先頭が自分か」をチェック
 * @detail: 条件を満たさない場合はpthread_cond_waitで無駄なCPU消費を避けて待機.条件を満たせばループを抜けてドングルを取得します。
 **/
static void	lock_loop(t_coder *coder, t_dongle *dongle)
{
	long long	cooldown;

	while (!check_stop_flag(coder->config))
	{
		cooldown = dongle->cooldown_end - get_time_ms();
		if (dongle->holder_id == 0 && dongle->queue[0] == coder->id)
		{
			if (cooldown <= 0)
				break ;
			pthread_mutex_unlock(&dongle->mutex);
			precise_usleep(cooldown);
			pthread_mutex_lock(&dongle->mutex);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

/**
 * @brief: ドングル1本を優先度キューに登録し、順番を待って確実に確保
 * @detail: リクエスト時刻（request_time）を記録してからドングルの待機キューに自身をpush.
 * @detail: その後lock_loopで待機,取得できたらキューからpopし,holder_idに自分のIDを書き込みます。
 */
static void	acquire_one_dongle(t_coder *coder, t_dongle *dongle)
{
	t_coder		*head;

	pthread_mutex_lock(&dongle->mutex);
	head = coder - (coder->id - 1);
	pthread_mutex_lock(&coder->config->state_mutex);
	coder->request_time = get_time_ms();
	pthread_mutex_unlock(&coder->config->state_mutex);
	push_queue(dongle, head, coder->id);
	lock_loop(coder, dongle);
	if (!check_stop_flag(coder->config))
	{
		pop_queue(dongle, head);
		dongle->holder_id = coder->id;
	}
	pthread_mutex_unlock(&dongle->mutex);
}

/**
 * @brief: ドングル1本を解放し、クールダウンを設定して待機スレッドに通知
 * @detail: holder_idを0に戻し現在時刻＋dongle_cooldownを設定.
 * @detail: その後pthread_cond_broadcastを呼び出し,このドングルを待っている他のコーダーを起動。
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
 * @brief: 左右のドングルをロック・確保する（デッドロック回避対応）
 * @detail: Coffmanの条件である「環状待機」を破るため,リソース階層（Resource Hierarchy）アルゴリズムを採用
 * @detail: 左右のドングルの「IDが小さい方」から必ず先に取得することで、全コーダーが同時に左を取ってデッドロックに陥る事態を防止
 */
void	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left_dongle == coder->right_dongle)
	{
		acquire_one_dongle(coder, coder->left_dongle);
		print_state(coder, "has taken a dongle");
		while (!check_stop_flag(coder->config))
			precise_usleep(1);
		return ;
	}
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	acquire_one_dongle(coder, first);
	print_state(coder, "has taken a dongle");
	acquire_one_dongle(coder, second);
	print_state(coder, "has taken a dongle");
}

/**
 * @brief 使用し終えた左右のドングルを解放
 */
void	release_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
		return ;
	release_one_dongle(coder, coder->right_dongle);
	release_one_dongle(coder, coder->left_dongle);
}
