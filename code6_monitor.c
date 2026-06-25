/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code6_monitor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:05:49 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:05:51 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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
		if (now >= coders[i].deadline)
		{
			print_state(&coders[i], "burned out");
			config->stop_flag = 1;
			return (1);
		}
		if (coders[i].compile_count < config->num_compile_req)
			all_done = 0;
		i++;
	}
	if (config->num_compile_req > 0 && all_done)
		return (config->stop_flag = 1, 1);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_coder		*coders;
	t_config	*config;

	coders = (t_coder *)arg;
	config = coders[0].config;
	while (1)
	{
		if (check_coders_status(coders, config))
			break ;
		precise_usleep(1);
	}
	return (NULL);
}
