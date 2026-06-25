/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code4_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 14:22:15 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:40:34 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief 現在のシステム時刻をエポック秒からミリ秒精度(ms)に集約して算出・返却する
 * @return long long 型の現在ミリ秒時刻
 */
long long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

/**
 * @brief 通常のusleepのOS側揺らぎを排除し、ミリ秒単位待機
 * @param target_ms スリープして引き止めたい目標のミリ秒数
 */
void	precise_usleep(long long target_ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < target_ms)
		usleep(100);
}
