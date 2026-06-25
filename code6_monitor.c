/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   code6_monitor.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 17:05:49 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:40:53 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_single_coder(t_coder *coder, long long now, int *all_done)
{
	pthread_mutex_lock(&coder->config->print_mutex);
	if (now >= coder->deadline)
	{
		print_state(coder, "burned out");
		coder->config->stop_flag = 1;
		pthread_mutex_unlock(&coder->config->print_mutex);
		return (1);
	}
	if (coder->compile_count < coder->config->num_compile_req)
		*all_done = 0;
	pthread_mutex_unlock(&coder->config->print_mutex);
	return (0);
}

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
	pthread_mutex_lock(&config->print_mutex);
	if (config->num_compile_req > 0 && all_done)
	{
		config->stop_flag = 1;
		pthread_mutex_unlock(&config->print_mutex);
		return (1);
	}
	pthread_mutex_unlock(&config->print_mutex);
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
