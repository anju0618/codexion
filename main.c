/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amakino <amakino@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/20 00:26:33 by amakino           #+#    #+#             */
/*   Updated: 2026/06/25 17:41:08 by amakino          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_stop_flag(t_config *config)
{
	int	res;

	pthread_mutex_lock(&config->print_mutex);
	res = config->stop_flag;
	pthread_mutex_unlock(&config->print_mutex);
	return (res);
}

void	print_state(t_coder *coder, const char *state)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->config->print_mutex);
	if (!coder->config->stop_flag || strcmp(state, "burned out") == 0)
	{
		timestamp = get_time_ms() - coder->config->start_time;
		printf("%lld %d %s\n", timestamp, coder->id, state);
	}
	pthread_mutex_unlock(&coder->config->print_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;

	coder = (t_coder *)arg;
	while (!check_stop_flag(coder->config))
	{
		acquire_dongles(coder);
		pthread_mutex_lock(&coder->config->print_mutex);
		coder->deadline = get_time_ms() + coder->config->time_burnout;
		pthread_mutex_unlock(&coder->config->print_mutex);
		print_state(coder, "is compiling");
		precise_usleep(coder->config->time_compile);
		pthread_mutex_lock(&coder->config->print_mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->config->print_mutex);
		release_dongles(coder);
		print_state(coder, "is debugging");
		precise_usleep(coder->config->time_debug);
		print_state(coder, "is refactoring");
		precise_usleep(coder->config->time_refactor);
	}
	return (NULL);
}

int	run_simulation(t_config *config, t_coder *coders)
{
	pthread_t	monitor_id;
	int			i;

	if (pthread_create(&monitor_id, NULL, monitor_routine, coders))
		return (0);
	i = 0;
	while (i < config->num_coders)
	{
		if (pthread_create(&coders[i].thread_id, NULL, coder_routine,
				&coders[i]))
			return (0);
		i++;
	}
	i = 0;
	while (i < config->num_coders)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
	pthread_join(monitor_id, NULL);
	return (1);
}

int	main(int ac, char **av)
{
	t_config	config;
	t_dongle	*dongles;
	t_coder		*coders;
	int			i;

	if (ac != 9)
		return (fprintf(stderr, "Error: need 8 args.\n"), 1);
	if (!init(&config, &dongles, &coders, av))
		return (1);
	pthread_mutex_init(&config.print_mutex, NULL);
	run_simulation(&config, coders);
	pthread_mutex_destroy(&config.print_mutex);
	i = 0;
	while (i < config.num_coders)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		pthread_cond_destroy(&dongles[i].cond);
		free(dongles[i].queue);
		i++;
	}
	free(dongles);
	free(coders);
	return (0);
}
