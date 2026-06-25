/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code7_queue.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:48:34 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:48:36 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief コーダーの優先度
 * @return 1 (aの高) / 0 (bの高)
 * @note edfならdeadlineが近い順、fifoならrequext_timeが早い順。
 * 同値の場合はIDが小さい方を先
 */
int	is_higher_priority(t_coder *a, t_coder *b, char *scheduler)
{
	if (strcmp(scheduler, "edf") == 0)
	{
		if (a->deadline < b->deadline)
			return (1);
		if (a->deadline > b->deadline)
			return (0);
		return (a->id < b->id);
	}
	else
	{
		if (a->request_time < b->request_time)
			return (1);
		if (a->request_time > b->request_time)
			return (0);
		return (a->id < b->id);
	}
}

/**
 * @brief queueに追加された末尾の要素を正しい位置まで上昇
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
 * @brief 先頭に持ってきた要素を正しい位置まで下降
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
