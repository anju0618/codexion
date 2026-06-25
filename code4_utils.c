/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code4_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 14:22:15 by amakino           #+#    #+#             */
/*   Updated: 2026/06/24 14:22:17 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
		return (0);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}
void	precise_usleep(long long target_ms)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < target_ms)
	{
		// 100マイクロ秒（0.1ミリ秒）ずつ細かく刻んで待つ
		usleep(100);
	}
}
