/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code7_queue.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:48:34 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 13:24:29 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief: 2つのコーダーの優先度を判定・比較
 * @detail: "edf" の場合は deadline が近い方を、"fifo" の場合は request_time が早い方を優先
 * @detail: もし同値だった場合は、コーダーIDが小さい方を優先します（Tie-breaker）
 */
int	is_higher_priority(t_coder *a, t_coder *b, char *scheduler)
{
	int	res;

	pthread_mutex_lock(&a->config->state_mutex);
	if (strcmp(scheduler, "edf") == 0)
	{
		if (a->deadline != b->deadline)
			res = (a->deadline < b->deadline);
		else
			res = (a->id < b->id);
	}
	else
	{
		if (a->request_time != b->request_time)
			res = (a->request_time < b->request_time);
		else
			res = (a->id < b->id);
	}
	pthread_mutex_unlock(&a->config->state_mutex);
	return (res);
}

/**
 * @brief: ヒープ木に追加された要素を正しい優先度位置まで浮上
 * @detail: キューの末尾に追加された要素を、親要素と優先度を比較
 * @detail: 自分の方が優先度が高ければ入れ替える（Swap）処理を、根（Root）に達するまで繰り返します。
 */
static void	up_heap(t_dongle *dongle, t_coder *coders, int idx)
{
	int	parent;
	int	tmp;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (is_higher_priority(&coders[dongle->queue[idx] - 1],
				&coders[dongle->queue[parent] - 1],
				coders[0].config->scheduler))
		{
			tmp = dongle->queue[idx];
			dongle->queue[idx] = dongle->queue[parent];
			dongle->queue[parent] = tmp;
			idx = parent;
		}
		else
			break ;
	}
}

/**
 * @brief queueにコーダーを登録
 */
void	push_queue(t_dongle *dongle, t_coder *coders, int coder_id)
{
	dongle->queue[dongle->queue_size] = coder_id;
	up_heap(dongle, coders, dongle->queue_size);
	dongle->queue_size++;
}

/**
 * @brief: 先頭要素を取り出した後、末尾から持ってきた要素を正しい位置までdown
 * @detail: 左右の子要素のより優先度が高い方と自分を比較.
 * @detail: 自分の方が優先度が低ければ入れ替える処理を、葉（Leaf）に達するまで繰り返してヒープを再構築
 */
static void	down_heap(t_dongle *dongle, t_coder *coders, int idx)
{
	int	child;
	int	tmp;

	while (2 * idx + 1 < dongle->queue_size)
	{
		child = 2 * idx + 1;
		if (child + 1 < dongle->queue_size
			&& is_higher_priority(&coders[dongle->queue[child + 1] - 1],
				&coders[dongle->queue[child] - 1], coders[0].config->scheduler))
			child++;
		if (is_higher_priority(&coders[dongle->queue[child] - 1],
				&coders[dongle->queue[idx] - 1], coders[0].config->scheduler))
		{
			tmp = dongle->queue[idx];
			dongle->queue[idx] = dongle->queue[child];
			dongle->queue[child] = tmp;
			idx = child;
		}
		else
			break ;
	}
}

/**
 * @brief 最も優先度の高いコーダーIDを取り出す
 */
int	pop_queue(t_dongle *dongle, t_coder *coders)
{
	int	top_coder_id;

	if (dongle->queue_size == 0)
		return (0);
	top_coder_id = dongle->queue[0];
	dongle->queue_size--;
	dongle->queue[0] = dongle->queue[dongle->queue_size];
	if (dongle->queue_size > 0)
		down_heap(dongle, coders, 0);
	return (top_coder_id);
}
