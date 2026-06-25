/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code4_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 14:22:15 by amakino           #+#    #+#             */
/*   Updated: 2026/06/26 02:15:35 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * @brief: 現在のシステム時刻をミリ秒精度(ms)で取得する。
 * @detail: gettimeofdayを使用し、秒tv_secとマイクロ秒tv_usecから現在の絶対的なミリ秒。
 */
long long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

/**
 * @brief: OS側による usleep の過剰なスリープ（揺らぎ）を排除し、正確なミリ秒単位の待機を実現する。
 * @detail: 100マイクロ秒刻みで現在時刻を確認しながら待機
 */
void	precise_usleep(long long target_ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < target_ms)
		usleep(100);
}
